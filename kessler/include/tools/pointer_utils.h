#pragma once

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <thread>
#include <tuple>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "../stage.h"

double get_hfov(double focal_len, double dist, int npx, double px_size) {
    return atan((1/dist + 1/focal_len) * npx * px_size / 2);
}

double get_phi(double x, int nx, double hfov) {
    return atan(2 * x * tan(hfov) / nx);
}

double get_theta(double y, int ny, double hfov) {
    return (M_PI / 2) - atan(2 * y * tan(hfov) / ny);
}

double get_phi_prime(double phi, double theta, double sep, double r, double error) {
    return phi - (sep / r) * (cos(phi) / sin(theta)) + error;
}

double get_theta_prime(double phi, double theta, double sep, double r, double error) {
    return theta - (sep / r) * cos(theta) * sin(phi) + error;
}

float get_motor_position(float motor_begin, float motor_end, float ref_begin, float ref_end, double ref_target) {
    float slope = (motor_end - motor_begin) / (ref_end - ref_begin);
    float target =  slope*((float)ref_target-ref_begin) + motor_begin;
    if (abs(target) < abs(motor_end - motor_begin))
        return target;
    return motor_end - motor_begin;
}

bool key_is_pressed(KeySym ks) {
    Display *dpy = XOpenDisplay(":0");
    char keys_return[32];
    XQueryKeymap(dpy, keys_return);
    KeyCode kc2 = XKeysymToKeycode(dpy, ks);
    bool isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
    XCloseDisplay(dpy);
    return isPressed;
}

// Drive stage with manual controls
void controller (Stage* kessler)
{
    printf("\n");
    printf("CONTROLS:\n");
    printf("---------\n");
    printf("Tilt Up: 'W'\n");
    printf("Tilt Down: 'S'\n");
    printf("Pan Left: 'A'\n");
    printf("Pan Right: 'D'\n");
    printf("Slide Left: Left Arrow\n");
    printf("Slide Right: Right Arrow\n");
    printf("Speed Up: Up Arrow\n");
    printf("Speed Down: Down Arrow\n");
    printf("Stop: Space\n");

    bool running = true;
    int speed = 0;

    int prev_tilt_dir = 0;
    int prev_pan_dir = 0;
    int prev_slide_dir = 0;

    while (running)
    {
        const bool w_pressed = key_is_pressed(XK_W);
        const bool s_pressed = key_is_pressed(XK_S);
        const bool a_pressed = key_is_pressed(XK_A);
        const bool d_pressed = key_is_pressed(XK_D);
        const bool up_pressed = key_is_pressed(XK_Up);
        const bool down_pressed = key_is_pressed(XK_Down);
        const bool left_pressed = key_is_pressed(XK_Left);
        const bool right_pressed = key_is_pressed(XK_Right);
        const float speed_p = (float)speed / 100;

        int tilt_dir = 0;
        if (w_pressed) tilt_dir = 1;
        if (s_pressed) tilt_dir = -1;

        if (prev_tilt_dir != tilt_dir) {
            kessler->set_position_speed_acceleration(3, 25000 * (float)tilt_dir, TILT_MAX_SPEED * speed_p * (float)abs(tilt_dir), TILT_MAX_ACC);
            prev_tilt_dir = tilt_dir;
        }

        int pan_dir = 0;
        if (d_pressed) pan_dir = 1;
        if (a_pressed) pan_dir = -1;

        if (prev_pan_dir != pan_dir) {
            kessler->set_position_speed_acceleration(2, 25000 * (float)pan_dir, PAN_MAX_SPEED * speed_p * (float)abs(pan_dir), PAN_MAX_ACC);
            prev_pan_dir = pan_dir;
        }

        int slide_dir = 0;
        if (right_pressed) slide_dir = 1;
        if (left_pressed) slide_dir = -1;

        if (prev_slide_dir != slide_dir) {
            kessler->set_position_speed_acceleration(1, 25000 * (float)slide_dir, PAN_MAX_SPEED * speed_p * (float)abs(slide_dir), PAN_MAX_ACC);
            prev_slide_dir = slide_dir;
        }

        int speed_dir = 0;
        if (up_pressed) speed_dir = 1;
        if (down_pressed) speed_dir = -1;

        speed = std::clamp(speed + 1 * speed_dir, 0, 100);
        if (speed_dir != 0) printf("Speed: %d\n", speed);

        if (key_is_pressed(XK_space)) {
            running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Calibrate a given motor
void calibrate(boost::uint8_t motor_address, Stage* kessler, std::mutex& mtx) {
    bool start = false;
    bool end = false;
    while(!kessler->MotorCalibratedQueue.empty()) kessler->MotorCalibratedQueue.pop();
    printf("Press `Q` to mark Motor %hd start position.\n", motor_address);
    while (!start) {
        bool q_pressed = key_is_pressed(XK_Q);
        if (q_pressed) {
            mtx.lock();
            kessler->mark_begin_position(motor_address);
            printf("Start position marked.\n");
            start = true;
            mtx.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    printf("Press `Q` to mark Motor %hd end position.\n", motor_address);
    while (!end) {
        bool q_pressed = key_is_pressed(XK_Q);
        if (q_pressed) {
            mtx.lock();
            kessler->mark_end_position(motor_address);
            printf("End position marked.\n");
            end = true;
            mtx.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while(kessler->MotorCalibratedQueue.empty()){
        //do nothing until message received
    }

    class MotorCalibrated info = kessler->MotorCalibratedQueue.front();
    boost::uint8_t addr = info.motor_address;
    float start_pos = info.begin_position;
    float end_pos = info.end_position;
    printf("Calibration complete. Motor: %hd, Start: %.2f, End: %.2f\n", addr, start_pos, end_pos);
    kessler->MotorCalibratedQueue.pop();
}

// Ping device every 10 seconds
void ping(Stage* kessler, std::mutex& mtx, const bool& active) {
    clock_t last_ping = clock();
    while (active) {
        float since_last_ping = (float)(clock() - last_ping)/CLOCKS_PER_SEC;
        if (since_last_ping > 10) {
            mtx.lock();
            kessler->get_network_info();
            last_ping = clock();
            mtx.unlock();
        }
    }
}

std::tuple<float, float> find_errors(double hfovx, double hfovy, int nx, int ny, double sep, float begin_pan, float end_pan, float begin_tilt, float end_tilt, float begin_pan_angle, float end_pan_angle, float begin_tilt_angle, float end_tilt_angle, double r1, double x1, double y1, float theta1m, float phi1m) {
    double phi = get_phi(x1, nx, hfovx);
    double theta = get_theta(y1, ny, hfovy);
    double phi_prime_estimate = get_phi_prime(phi, theta, sep, r1, 0);
    double theta_prime_estimate = get_theta_prime(phi, theta, sep, r1, 0);
    double phi_prime_actual = begin_pan_angle + (phi1m - begin_pan)*(end_pan_angle - begin_pan_angle)/(end_pan - begin_pan);
    double theta_prime_actual = begin_tilt_angle + (theta1m - begin_tilt)*(end_tilt_angle - begin_tilt_angle)/(end_tilt - begin_tilt);
    auto phi_prime_error = (float)(phi_prime_actual - phi_prime_estimate);
    auto theta_prime_error = (float)(theta_prime_actual - theta_prime_estimate);
    printf("Estimated theta_p/phi_p: (%.2f, %.2f)\n", theta_prime_estimate * 180 / M_PI, phi_prime_estimate * 180 / M_PI);
    printf("Actual theta_p/phi_p: (%.2f, %.2f)\n", theta_prime_actual * 180 / M_PI, phi_prime_actual * 180 / M_PI);
    std::tuple<float, float> ret = {theta_prime_error, phi_prime_error};
    return ret;
}

std::tuple<float, float, double, double, double> get_calibration_point(Stage* kessler, std::mutex& mtx) {
    float pan_position;
    float tilt_position;
    double r;
    double x;
    double y;
    printf("Center camera on known target and press `Q`.\n");
    while (true) {
        bool q_pressed = key_is_pressed(XK_Q);
        if (q_pressed) {
            mtx.lock();
            class MotorInfo info = kessler->get_motor_info();
            std::cout << info.to_string();
            pan_position = info.motors[1].position;
            tilt_position = info.motors[2].position;
            mtx.unlock();
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printf("Enter target distance (m):\n");
    std::cin >> r;
    printf("Enter target x coordinate:\n");
    std::cin >> x;
    printf("Enter target y coordinate:\n");
    std::cin >> y;
    std::tuple<float, float, double, double, double> ret{tilt_position, pan_position, r, x, y};
    return ret;
}

