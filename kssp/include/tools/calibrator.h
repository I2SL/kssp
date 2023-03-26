#include "pointer_utils.h"

std::tuple<double, double, float, float, float, float, float, float> calibrate_stage (Stage* stage, double focal_len, double sep, double dist, double px_size, int num_x, int num_y, bool correction, bool prev_cal, float begin_pan_angle, float end_pan_angle, float begin_tilt_angle, float end_tilt_angle, KeySym ks) {
    /*
    This function calibrates the pan and tilt motors to a user-defined angle range when the scene is being observed
    with a camera.

    Args:
        stage: pointer to the Stage object
        focal_len: focal length in meters
        sep: camera/stage separation in meters
        dist: distance to the focal plane in meters
        px_size: camera pixel size in meters
        num_x: number of pixels along the horizontal
        num_y: number of pixels along the vertical
        correction: true or false whether to perform correction for systematic error
        prev_cal: true or false whether to use the stage's currently loaded calibration
        begin_pan_angle: reference point for the pan motor's lower limit (e.g. 0 degrees)
        end_pan_angle: reference point for the pan motor's upper limit (e.g. 180 degrees)
        begin_tilt_angle: reference point for the tilt motor's lower limit (e.g. 0 degrees)
        end_tilt_angle: reference point for the tilt motor's upper limit (e.g. 180 degrees)
        ks: key to exit manual control after calibration. Should not be an already used key.

    Ret:
        cal_params: tuple containing results of calibration
            hfovx: horizontal field of view in radians
            hfovy: vertical field of view in radians
            begin_pan: pan motor position at begin_pan_angle
            end_pan: pan motor position at end_pan_angle
            begin_tilt: tilt motor position at begin_tilt_angle
            end_tilt: tilt motor position at end_tilt_angle
            theta_prime_error: systematic error of tilt in radians (0 if not corrected)
            phi_prime_error: systematic error of pan in radians (0 if not corrected)
    */
    std::mutex mtx;
    double hfovx = get_hfov(focal_len, dist, num_x, px_size);
    double hfovy = get_hfov(focal_len, dist, num_y, px_size);
    std::thread driver(controller, stage, ks);

    if (!prev_cal) {
        stage->reset_axis(0);
        printf("Move Slide motor to start position and press 'Q'. Then move Slide motor to end position and press `E`.\n");
        calibrate(1, stage, mtx);
        printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `E`.\n");
        calibrate(2, stage, mtx);
        printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `E`.\n");
        calibrate(3, stage, mtx);
    }

    class MotorInfo info = stage->get_motor_info();
    std::cout << info.to_string();
    float begin_pan = info.motors[1].begin_position;
    float end_pan = info.motors[1].end_position;
    float begin_tilt = info.motors[2].begin_position;
    float end_tilt = info.motors[2].end_position;

    float theta_prime_error = 0;
    float phi_prime_error = 0;
    if (correction) {
        double r1;
        float theta1m, phi1m;
        double x1, y1;
        std::tie(theta1m, phi1m, r1, x1, y1) = get_calibration_point(stage, mtx);
        std::tie(theta_prime_error, phi_prime_error) = find_errors(hfovx, hfovy, num_x, num_y, sep, begin_pan, end_pan, begin_tilt, end_tilt, begin_pan_angle, end_pan_angle, begin_tilt_angle, end_tilt_angle, r1, x1, y1, theta1m, phi1m);
    }
    printf("Calibration complete. Press %s to exit manual control.\n", XKeysymToString(ks));
    driver.join();

    std::tuple<double, double, float, float, float, float, float, float> cal_params(hfovx, hfovy, begin_pan, end_pan, begin_tilt, end_tilt, theta_prime_error, phi_prime_error);
    return cal_params;
}