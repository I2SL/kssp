#include <iostream>
#include <cmath>
#include <thread>

#include "../libkessler/stage.h"
#include "pointer_utils.hpp"

int main () {
    std::mutex mtx;
    bool active = true;

    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();
    kessler.reset_axis(0);
    std::thread driver(controller, std::ref(kessler));
    std::thread pinger(ping, std::ref(kessler), std::ref(mtx), std::ref(active));

    printf("Move Slide motor to start position and press 'Q'. Then move Slide motor to end position and press `Q`.\n");
    calibrate(1, kessler, mtx);
    printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `Q`.\n");
    calibrate(2, kessler, mtx);
    printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `Q`.\n");
    calibrate(3, kessler, mtx);
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
        printf("EBS: (%.2Lf, %.2Lf)\n", theta * 180 / EIGEN_PI, phi * 180 / EIGEN_PI);
        printf("FBC: (%.2Lf, %.2Lf)\n", theta_prime * 180 / EIGEN_PI, phi_prime * 180 / EIGEN_PI);
        printf("\n");

        float pan_position = end_pan * (float)(phi_prime / EIGEN_PI) + (float)0.5 * end_pan;
        float tilt_position = 3 * end_tilt * (float)(theta_prime / EIGEN_PI) / 2;

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
