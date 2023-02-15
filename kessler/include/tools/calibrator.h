#include "pointer_utils.h"

std::tuple<int, int, double, double, double, float, float, float, float, float, float> calibrate_stage (Stage& kessler) {
    std::mutex mtx;
    std::string correction("n");
    double px;
    double f;
    double y0;
    double r;
    int ny;
    int nx;
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
    printf("Enter focal point distance in meters:\n");
    std::cin >> r;
    double hfovx = get_hfov(f, r, nx, px);
    double hfovy = get_hfov(f, r, ny, px);

    kessler.reset_axis(0);
    std::thread driver(controller, std::ref(kessler));

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
    float theta_prime_error, phi_prime_error = 0;
    if (correction == "y") {
        double r1;
        float theta1m, phi1m;
        int x1, y1;
        std::tie(theta1m, phi1m, r1, x1, y1) = get_calibration_point(kessler, mtx);
        std::tie(theta_prime_error, phi_prime_error) = find_errors(hfovx, hfovy, nx, ny, y0, begin_pan, end_pan, begin_tilt, end_tilt, r1, x1, y1, theta1m - begin_tilt, phi1m - begin_pan);
    }
    printf("Calibration complete. Press space to exit manual control.\n");
    driver.join();

    std::tuple<int, int, double, double, double, float, float, float, float, float, float> cal_params(nx, ny, hfovx, hfovy, y0, begin_pan, end_pan, begin_tilt, end_tilt, theta_prime_error, phi_prime_error);
    return cal_params;
}