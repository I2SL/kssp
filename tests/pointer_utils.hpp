#pragma once

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <thread>
#include <tuple>

#include "../libkessler/stage.h"

double get_hfov(double focal_len, double dist, int npx, double px_size) {
    return atan((1/dist + 1/focal_len) * npx * px_size / 2);
}

double get_phi(int x, int nx, double hfov) {
    return atan(2 * x * tan(hfov) / nx);
}

double get_theta(int y, int ny, double hfov) {
    return atan((float)ny / 2 / (float)y / tan(hfov));
}

double get_phi_prime(double phi, double theta, double y0, double r) {
    return phi - (y0 / r) * (cos(phi) / sin(theta));
}

double get_theta_prime(double phi, double theta, double y0, double r) {
    return theta - (y0 / r) * cos(theta) * sin(phi);
}

float get_pan_position(float begin_pan, float end_pan, double phi_prime, float phi0pm) {
    float true_end = end_pan - begin_pan;
    float nom_position = true_end * ((float)((phi_prime / PI) + 0.5)) + phi0pm;
    if (true_end < 0) {
        float first_pass = std::min<float>(0, nom_position);
        return std::max<float>(true_end, first_pass);
    }
    float first_pass = std::max<float>(0, nom_position);
    return std::min<float>(true_end, first_pass);
}

float get_tilt_position(float begin_tilt, float end_tilt, double theta_prime, float theta0pm) {
    float true_end = end_tilt - begin_tilt;
    float nom_position = 3 * true_end * (float)(theta_prime / PI) / 2 + theta0pm;
    if (true_end < 0) {
        float first_pass = std::min<float>(0, nom_position);
        return std::max<float>(true_end, first_pass);
    }
    float first_pass = std::max<float>(0, nom_position);
    return std::min<float>(true_end, first_pass);
}

// Drive stage with manual controls
void controller (Stage& kessler)
{
    printf("\n");
    printf("CONTROLS:\n");
    printf("---------\n");
    printf("Flashlight: 'F'\n");
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
        const bool w_pressed = GetAsyncKeyState(0x57);
        const bool s_pressed = GetAsyncKeyState(0x53);
        const bool a_pressed = GetAsyncKeyState(0x41);
        const bool d_pressed = GetAsyncKeyState(0x44);
        const bool up_pressed = GetAsyncKeyState(0x26);
        const bool down_pressed = GetAsyncKeyState(0x28);
        const bool left_pressed = GetAsyncKeyState(0x25);
        const bool right_pressed = GetAsyncKeyState(0x27);
        const float speed_p = (float)speed / 100;

        int tilt_dir = 0;
        if (w_pressed) tilt_dir = 1;
        if (s_pressed) tilt_dir = -1;

        if (prev_tilt_dir != tilt_dir) {
            kessler.set_position_speed_acceleration(3, 25000 * (float)tilt_dir, TILT_MAX_SPEED * speed_p * (float)abs(tilt_dir), TILT_MAX_ACC);
            prev_tilt_dir = tilt_dir;
        }

        int pan_dir = 0;
        if (d_pressed) pan_dir = 1;
        if (a_pressed) pan_dir = -1;

        if (prev_pan_dir != pan_dir) {
            kessler.set_position_speed_acceleration(2, 25000 * (float)pan_dir, PAN_MAX_SPEED * speed_p * (float)abs(pan_dir), PAN_MAX_ACC);
            prev_pan_dir = pan_dir;
        }

        int slide_dir = 0;
        if (right_pressed) slide_dir = 1;
        if (left_pressed) slide_dir = -1;

        if (prev_slide_dir != slide_dir) {
            kessler.set_position_speed_acceleration(1, 25000 * (float)slide_dir, PAN_MAX_SPEED * speed_p * (float)abs(slide_dir), PAN_MAX_ACC);
            prev_slide_dir = slide_dir;
        }

        int speed_dir = 0;
        if (up_pressed) speed_dir = 1;
        if (down_pressed) speed_dir = -1;

        speed = std::clamp(speed + 1 * speed_dir, 0, 100);
        if (speed_dir != 0) printf("Speed: %d\n", speed);

        if (GetAsyncKeyState(VK_SPACE)) {
            running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Calibrate a given motor
void calibrate(boost::uint8_t motor_address, Stage& kessler, std::mutex& mtx) {
    bool start = false;
    bool end = false;
    while(!kessler.MotorCalibratedQueue.empty()) kessler.MotorCalibratedQueue.pop();
    printf("Press `Q` to mark Motor %hd start position.\n", motor_address);
    while (!start) {
        bool q_pressed = GetAsyncKeyState(0x51);
        if (q_pressed) {
            mtx.lock();
            kessler.mark_begin_position(motor_address);
            printf("Start position marked.\n");
            start = true;
            mtx.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    printf("Press `Q` to mark Motor %hd end position.\n", motor_address);
    while (!end) {
        bool q_pressed = GetAsyncKeyState(0x51);
        if (q_pressed) {
            mtx.lock();
            kessler.mark_end_position(motor_address);
            printf("End position marked.\n");
            end = true;
            mtx.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while(kessler.MotorCalibratedQueue.empty()){
        //do nothing until message received
    }

    class MotorCalibrated info = kessler.MotorCalibratedQueue.front();
    boost::uint8_t addr = info.motor_address;
    float start_pos = info.begin_position;
    float end_pos = info.end_position;
    printf("Calibration complete. Motor: %hd, Start: %.2f, End: %.2f\n", addr, start_pos, end_pos);
    kessler.MotorCalibratedQueue.pop();
}

// Ping device every 10 seconds
void ping(Stage& kessler, std::mutex& mtx, bool& active) {
    clock_t last_ping = clock();
    float since_last_ping;
    while (active) {
        since_last_ping = (float)(clock() - last_ping)/CLOCKS_PER_SEC;
        if (since_last_ping > 10) {
            mtx.lock();
            kessler.get_network_info();
            last_ping = clock();
            mtx.unlock();
        }
    }
}

std::tuple<float, float> find_errors(double hfovx, double hfovy, int nx, int ny, double y0, float begin_pan, float end_pan, float begin_tilt, float end_tilt, double r1, int x1, int y1, float theta1m, float phi1m) {
    double phi = get_phi(x1, nx, hfovx);
    double theta = get_theta(y1, ny, hfovy);
    double phi_prime = get_phi_prime(phi, theta, y0, r1);
    double theta_prime = get_theta_prime(phi, theta, y0, r1);
    float est_pan = get_pan_position(begin_pan, end_pan, phi_prime, 0);
    float est_tilt = get_tilt_position(begin_tilt, end_tilt, theta_prime, 0);
    float pan_error = phi1m - est_pan;
    float tilt_error = theta1m - est_tilt;
    std::tuple<float, float> ret = {tilt_error, pan_error};
    return ret;
}

std::tuple<float, float, double, int, int> get_calibration_point(Stage& kessler, std::mutex& mtx) {
    float pan_position;
    float tilt_position;
    double r;
    int x;
    int y;
    printf("Center camera on known target and press `Q`.\n");
    while (true) {
        bool q_pressed = GetAsyncKeyState(0x51);
        if (q_pressed) {
            mtx.lock();
            class MotorInfo info = kessler.get_motor_info();
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
    std::tuple<float, float, double, int, int> ret{tilt_position, pan_position, r, x, y};
    return ret;
}

