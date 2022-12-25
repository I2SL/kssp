#pragma once

#include <algorithm>
#include <cmath>
#include <ctime>
#include <thread>
#include <tuple>

#include <ceres/ceres.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "../libkessler/stage.h"

// Use Ceres to solve system of equations for calibration
using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solve;
using ceres::Solver;
struct F1 {
    double phinorm1;
    double sep;
    double r1;
    double thetanorm1;
    double phi1p;
    template <typename T>
    bool operator()(const T* const theta0, const T* const phi0, const T* const phi0p, T* residual) const {
        residual[0] = phinorm1 + phi0[0] - (sep * cos(phinorm1 + phi0[0]) / r1 / sin(thetanorm1 + theta0[0])) + phi0p[0] - phi1p;
        return true;
    }
};
struct F2 {
    double thetanorm1;
    double sep;
    double phinorm1;
    double r1;
    double theta1p;
    template <typename T>
    bool operator()(const T* const theta0, const T* const phi0, const T* const theta0p, T* residual) const {
        residual[0] = thetanorm1 + theta0[0] - (sep * cos(thetanorm1 + theta0[0]) * sin(phinorm1 + phi0[0]) / r1) - theta0p[0] - theta1p;
        return true;
    }
};
struct F3 {
    double phinorm2;
    double sep;
    double r2;
    double thetanorm2;
    double phi2p;
    template <typename T>
    bool operator()(const T* const theta0, const T* const phi0, const T* const phi0p, T* residual) const {
        residual[0] = phinorm2 + phi0[0] - (sep * cos(phinorm2 + phi0[0]) / r2 / sin(thetanorm2 + theta0[0])) + phi0p[0] - phi2p;
        return true;
    }
};
struct F4 {
    double thetanorm2;
    double sep;
    double phinorm2;
    double r2;
    double theta2p;
    template <typename T>
    bool operator()(const T* const theta0, const T* const phi0, const T* const theta0p, T* residual) const {
        residual[0] = thetanorm2 + theta0[0] - (sep * cos(thetanorm2 + theta0[0]) * sin(phinorm2 + phi0[0]) / r2) - theta0p[0] - theta2p;
        return true;
    }
};
DEFINE_string(minimizer,
              "trust_region",
              "Minimizer type to use, choices are: line_search & trust_region");
std::tuple<float, float, float, float> solve(double yhfov, double xhfov, int Nx, int Ny, double sep, double panstart, double panend, double tiltstart, double tiltend, double r1, int x1, int y1, double theta1m, double phi1m, double r2, int x2, int y2, double theta2m, double phi2m) {
    double theta0 = 0;
    double phi0 = 0;
    double theta0p = 0;
    double phi0p = 0;
    Problem problem;
    // Add residual terms to the problem using the autodiff
    // wrapper to get the derivatives automatically. The parameters, x1 through
    // x4, are modified in place.
    double phinorm1 = atan(2 * x1 * tan(xhfov) / Nx);
    double thetanorm1 = atan((float)Ny / 2 / (float)y1 / tan(yhfov));
    double phinorm2 = atan(2 * x2 * tan(xhfov) / Nx);
    double thetanorm2 = atan((float)Ny / 2 / (float)y2 / tan(yhfov));
    double theta1p = EIGEN_PI * 2 * theta1m / 3 / (tiltend - tiltstart);
    double phi1p = EIGEN_PI * ((phi1m / (panend - panstart)) - 0.5);
    double theta2p = EIGEN_PI * 2 * theta2m / 3 / (tiltend - tiltstart);
    double phi2p = EIGEN_PI * ((phi2m / (panend - panstart)) - 0.5);

    F1 * f1 = new F1;
    f1->phinorm1 = phinorm1;
    f1->sep = sep;
    f1->r1 = r1;
    f1->thetanorm1 = thetanorm1;
    f1->phi1p = phi1p;

    F2 * f2 = new F2;
    f2->phinorm1 = phinorm1;
    f2->sep = sep;
    f2->r1 = r1;
    f2->thetanorm1 = thetanorm1;
    f2->theta1p = theta1p;

    F3 * f3 = new F3;
    f3->phinorm2 = phinorm2;
    f3->sep = sep;
    f3->r2 = r2;
    f3->thetanorm2 = thetanorm2;
    f3->phi2p = phi2p;

    F4 * f4 = new F4;
    f4->phinorm2 = phinorm2;
    f4->sep = sep;
    f4->r2 = r2;
    f4->thetanorm2 = thetanorm2;
    f4->theta2p = theta2p;

    problem.AddResidualBlock(
            new AutoDiffCostFunction<F1, 1, 1, 1, 1>(f1), nullptr, &theta0, &phi0, &phi0p);
    problem.AddResidualBlock(
            new AutoDiffCostFunction<F2, 1, 1, 1, 1>(f2), nullptr, &theta0, &phi0, &theta0p);
    problem.AddResidualBlock(
            new AutoDiffCostFunction<F3, 1, 1, 1, 1>(f3), nullptr, &theta0, &phi0, &phi0p);
    problem.AddResidualBlock(
            new AutoDiffCostFunction<F4, 1, 1, 1, 1>(f4), nullptr, &theta0, &phi0, &theta0p);
    Solver::Options options;
    LOG_IF(FATAL,
           !ceres::StringToMinimizerType(CERES_GET_FLAG(FLAGS_minimizer),
                                         &options.minimizer_type))
    << "Invalid minimizer: " << CERES_GET_FLAG(FLAGS_minimizer)
    << ", valid options are: trust_region and line_search.";
    options.max_num_iterations = 100;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    // clang-format off
    std::cout << "Initial theta0 = " << theta0
              << ", phi0 = " << phi0
              << ", theta0p = " << theta0p
              << ", phi0p = " << phi0p
              << "\n";
    // clang-format on
    // Run the solver!
    Solver::Summary summary;
    Solve(options, &problem, &summary);
    std::cout << summary.FullReport() << "\n";
    // clang-format off
    std::cout << "Final theta0 = " << theta0
              << ", phi0 = " << phi0
              << ", theta0p = " << theta0p
              << ", phi0p = " << phi0p
              << "\n";
    // clang-format on
    std::tuple<float, float, float, float> errors((float)theta0, (float)phi0, (float)theta0p, (float)phi0p);
    return errors;
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

std::tuple<double, double, double, int, int> get_calibration_point(Stage& kessler, std::mutex& mtx) {
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
    std::tuple<double, double, double, int, int> ret{(double)tilt_position, (double)pan_position, r, x, y};
    return ret;
}

double get_phi(int x, int nx, double hfov, double phi0) {
    return atan(2 * x * tan(hfov) / nx) + phi0;
}

double get_theta(int y, int ny, double hfov, double theta0) {
    return atan((float)ny / 2 / (float)y / tan(hfov)) + theta0;
}

double get_phi_prime(double phi, double theta, double y0, double r, double phi0p) {
    return phi - (y0 / r) * (cos(phi) / sin(theta)) + phi0p;
}

double get_theta_prime(double phi, double theta, double y0, double r, double theta0p) {
    return theta - (y0 / r) * cos(theta) * sin(phi) + theta0p;
}

float get_pan_position(float begin_pan, float end_pan, double phi_prime) {
    float true_end = end_pan - begin_pan;
    float nom_position = true_end * (float)(phi_prime / EIGEN_PI) + (float)0.5 * true_end;
    if (true_end < 0) {
        float first_pass = std::min<float>(0, nom_position);
        return std::max<float>(true_end, first_pass);
    }
    float first_pass = std::max<float>(0, nom_position);
    return std::min<float>(true_end, first_pass);
}

float get_tilt_position(float begin_tilt, float end_tilt, double theta_prime) {
    float true_end = end_tilt - begin_tilt;
    float nom_position = 3 * true_end * (float)(theta_prime / EIGEN_PI) / 2;
    if (true_end < 0) {
        float first_pass = std::min<float>(0, nom_position);
        return std::max<float>(true_end, first_pass);
    }
    float first_pass = std::max<float>(0, nom_position);
    return std::min<float>(true_end, first_pass);
}