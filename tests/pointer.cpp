#pragma once

#include <iostream>
#include <cmath>
#include <thread>

#include "../libkessler/stage.h"
#include "pointer_utils.hpp"

int main () {
    std::mutex mtx;
    bool active = true;
    std::string correction("n");
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

    class MotorInfo info = kessler.get_motor_info();
    std::cout << info.to_string();
    float begin_pan = info.motors[1].begin_position;
    float end_pan = info.motors[1].end_position;
    float begin_tilt = info.motors[2].begin_position;
    float end_tilt = info.motors[2].end_position;

    printf("Calibrate for systematic errors? (y/n)\n");
    std::cin >> correction;
    float theta0, phi0, theta0p, phi0p = 0;
    if (correction == "y") {
        double r1, theta1m, phi1m, r2, theta2m, phi2m;
        int x1, y1, x2, y2;
        std::tie(theta1m, phi1m, r1, x1, y1) = get_calibration_point(kessler, mtx);
        std::tie(theta2m, phi2m, r2, x2, y2) = get_calibration_point(kessler, mtx);
        std::tie(theta0, phi0, theta0p, phi0p) = solve(hfovy, hfovx, nx, ny, y0, begin_pan, end_pan, begin_tilt, end_tilt, r1, x1, y1, theta1m, phi1m, r2, x2, y2, theta2m, phi2m);
    }
    printf("Calibration complete. Press space to exit manual control.\n");
    driver.join();

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

        double theta = get_theta(y, ny, hfovy, theta0);
        double phi = get_phi(x, nx, hfovx, phi0);
        double theta_prime = get_theta_prime(phi, theta, y0, r, theta0p);
        double phi_prime = get_phi_prime(phi, theta, y0, r, phi0p);

        printf("Calculated Coordinates in degrees (theta, phi):\n");
        printf("EBS: (%.2Lf, %.2Lf)\n", theta * 180 / EIGEN_PI, phi * 180 / EIGEN_PI);
        printf("FBC: (%.2Lf, %.2Lf)\n", theta_prime * 180 / EIGEN_PI, phi_prime * 180 / EIGEN_PI);
        printf("\n");

        float pan_position = get_pan_position(begin_pan, end_pan, phi_prime);
        float tilt_position = get_tilt_position(begin_tilt, end_tilt, theta_prime);

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
