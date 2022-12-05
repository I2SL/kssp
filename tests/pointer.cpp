#include <iostream>
#include <ctime>
#include <thread>

#include "../libkessler/stage.h"

std::mutex mtx;
bool active = true;

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
            kessler.set_position_speed_acceleration(3, 25000 * (float)tilt_dir, TILT_MAX_SPEED * speed_p * abs(tilt_dir), TILT_MAX_ACC);
            prev_tilt_dir = tilt_dir;
        }

        int pan_dir = 0;
        if (d_pressed) pan_dir = 1;
        if (a_pressed) pan_dir = -1;

        if (prev_pan_dir != pan_dir) {
            kessler.set_position_speed_acceleration(2, 25000 * (float)pan_dir, PAN_MAX_SPEED * speed_p * abs(pan_dir), PAN_MAX_ACC);
            prev_pan_dir = pan_dir;
        }

        int slide_dir = 0;
        if (right_pressed) slide_dir = 1;
        if (left_pressed) slide_dir = -1;

        if (prev_slide_dir != slide_dir) {
            kessler.set_position_speed_acceleration(1, 25000 * (float)slide_dir, PAN_MAX_SPEED * speed_p * abs(slide_dir), PAN_MAX_ACC);
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

void calibrate(boost::uint8_t motor_address, Stage& kessler) {
    bool start = false;
    bool end = false;
    bool q_pressed = false;
    while(!kessler.MotorCalibratedQueue.empty()) kessler.MotorCalibratedQueue.pop();
    printf("Press `Q` to mark Motor %hd start position.\n", motor_address);
    while (!start) {
        q_pressed = GetAsyncKeyState(0x51);
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
        q_pressed = GetAsyncKeyState(0x51);
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
        continue;
    }

    class MotorCalibrated info = kessler.MotorCalibratedQueue.front();
    boost::uint8_t addr = info.motor_address;
    float start_pos = info.begin_position;
    float end_pos = info.end_position;
    printf("Calibration complete. Motor: %hd, Start: %.2f, End: %.2f\n", addr, start_pos, end_pos);
    kessler.MotorCalibratedQueue.pop();
}

void ping(Stage& kessler) {
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

int main () {
    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();
    kessler.reset_axis(0);
    std::thread driver(controller, std::ref(kessler));
    std::thread pinger(ping, std::ref(kessler));

    printf("Move Slide motor to start position and press 'Q'. Then move Slide motor to end position and press `Q`.\n");
    calibrate(1, kessler);
    printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `Q`.\n");
    calibrate(2, kessler);
    printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `Q`.\n");
    calibrate(3, kessler);
    printf("Calibration complete. Press space to exit manual control.\n");
    driver.join();

    class MotorInfo info = kessler.get_motor_info();
    std::cout << info.to_string();
    float begin_pan = info.motors[1].begin_position;
    float end_pan = info.motors[1].end_position;
    float begin_tilt = info.motors[2].begin_position;
    float end_tilt = info.motors[2].end_position;
    end_pan -= begin_pan;
    end_tilt -= begin_tilt;

    double hfovx;
    double hfovy;
    double px;
    double f;
    double y0;
    double r;
    int ny;
    int nx;
    int y;
    int x;
    printf("Enter focal length in millimeters:\n");
    std::cin >> f;
    f *= pow(10, -3);
    printf("Enter sensor separation in meters:\n");
    std::cin >> y0;
    printf("Enter number of vertical pixels:\n");
    std::cin >> ny;
    printf("Enter number of horizontal pixels:\n");
    std::cin >> nx;
    printf("Enter pixel size in microns:\n");
    std::cin >> px;
    px *= pow(10, -6);

    hfovx = atan(px * nx / f / 2);
    hfovy = atan(px * ny / f / 2);

    while (true) {
        printf("Enter target distance (or 0 to quit):\n");
        std::cin >> r;
        if (r == 0) {
            break;
        }
        printf("Enter target x coordinate:\n");
        std::cin >> x;
        printf("Enter target y coordinate:\n");
        std::cin >> y;

        double theta = get_theta(y, ny, hfovy);
        double phi = get_phi(x, nx, hfovx);
        double theta_prime = get_theta_prime(phi, theta, y0, r);
        double phi_prime = get_phi_prime(phi, theta, y0, r);

        printf("Calculated Coordinates in degrees (theta, phi):\n");
        printf("EBS: (%.2f, %.2f)\n", theta * 180 / M_PI, phi * 180 / M_PI);
        printf("FBC: (%.2f, %.2f)\n", theta_prime * 180 / M_PI, phi_prime * 180 / M_PI);
        printf("\n");

        float pan_position = end_pan * (float)(phi_prime / M_PI) + (float)0.5 * end_pan;
        float tilt_position = 3 * end_tilt * (float)(theta_prime / M_PI) / 2;

        printf("Target motor positions:\n");
        printf("Pan: %.2f (Range: 0 - %.2f)\n", pan_position, end_pan);
        printf("Tilt: %.2f (Range: 0 - %.2f)\n", tilt_position, end_tilt);

        std::string input;
        printf("Move to target? (y/n)\n");
        std::cin >> input;
        if (input == std::string("y")) {
            mtx.lock();
            kessler.set_position_speed_acceleration(2, pan_position, PAN_MAX_SPEED, PAN_MAX_ACC);
            kessler.set_position_speed_acceleration(3, tilt_position, TILT_MAX_SPEED, TILT_MAX_ACC);
            mtx.unlock();
        }
    }
    active = false;
    pinger.join();
}
