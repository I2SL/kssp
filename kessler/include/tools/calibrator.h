#include "pointer_utils.h"

std::tuple<double, double, float, float, float, float, float, float> calibrate_stage (Stage* kessler, double focal_len, double sep, double dist, double px_size, int num_x, int num_y, bool correction, bool prev_cal, float begin_pan_angle, float end_pan_angle, float begin_tilt_angle, float end_tilt_angle) {
    std::mutex mtx;
    double hfovx = get_hfov(focal_len, dist, num_x, px_size);
    double hfovy = get_hfov(focal_len, dist, num_y, px_size);
    std::thread driver(controller, kessler);

    if (!prev_cal) {
        kessler->reset_axis(0);
        printf("Move Slide motor to start position and press 'Q'. Then move Slide motor to end position and press `Q`.\n");
        calibrate(1, kessler, mtx);
        printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `Q`.\n");
        calibrate(2, kessler, mtx);
        printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `Q`.\n");
        calibrate(3, kessler, mtx);
    }

    class MotorInfo info = kessler->get_motor_info();
    std::cout << info.to_string();
    float begin_pan = info.motors[1].begin_position;
    float end_pan = info.motors[1].end_position;
    float begin_tilt = info.motors[2].begin_position;
    float end_tilt = info.motors[2].end_position;

    printf("Calibrate for systematic errors? (y/n)\n");
    std::cin >> correction;
    float theta_prime_error = 0;
    float phi_prime_error = 0;
    if (correction) {
        double r1;
        float theta1m, phi1m;
        double x1, y1;
        std::tie(theta1m, phi1m, r1, x1, y1) = get_calibration_point(kessler, mtx);
        std::tie(theta_prime_error, phi_prime_error) = find_errors(hfovx, hfovy, num_x, num_y, sep, begin_pan, end_pan, begin_tilt, end_tilt, begin_pan_angle, end_pan_angle, begin_tilt_angle, end_tilt_angle, r1, x1, y1, theta1m, phi1m);
    }
    printf("Calibration complete. Press space to exit manual control.\n");
    driver.join();

    std::tuple<double, double, float, float, float, float, float, float> cal_params(hfovx, hfovy, begin_pan, end_pan, begin_tilt, end_tilt, theta_prime_error, phi_prime_error);
    return cal_params;
}