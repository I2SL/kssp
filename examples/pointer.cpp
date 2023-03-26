#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <kssp/tools/calibrator.h>

using json = nlohmann::json;

int main (int argc, char* argv[]) {
    if (argc != 2) {
        printf("Provide path to config JSON.\n");
        return 1;
    }
    std::string config_file = {std::string(argv[1])};
    std::ifstream f(config_file);
    json settings = json::parse(f);
    double focal_len = settings.value("FOCAL_LENGTH", 0.006);
    int nx = settings.value("NUM_X", 640);
    int ny = settings.value("NUM_Y", 480);
    double sep = settings.value("SEPARATION", 0.15);
    double dist = settings.value("DISTANCE", 10);
    double px = settings.value("PIXEL_SIZE", 0.000009);
    bool correction = settings.value("SYSTEMATIC_ERROR", false);
    bool prev_cal = settings.value("USE_PREVIOUS", false);
    float begin_pan_angle = (float)settings.value("START_PAN_ANGLE", -M_PI_2);
    float end_pan_angle = (float)settings.value("END_PAN_ANGLE", M_PI_2);
    float begin_tilt_angle = (float)settings.value("START_TILT_ANGLE", 0);
    float end_tilt_angle = (float)settings.value("END_TILT_ANGLE", 2*M_PI/3);
    std::mutex mtx;
    bool active = true;
    float begin_pan, end_pan, begin_tilt, end_tilt, theta_prime_error, phi_prime_error;
    double hfovx, hfovy, r, x, y;

    Stage stage("192.168.50.1", 5520);
    stage.handshake();
    std::cout << stage.get_device_info().to_string();
    std::thread pinger(ping, &stage, std::ref(mtx), std::ref(active));
    std::tie(hfovx, hfovy,begin_pan, end_pan, begin_tilt, end_tilt, theta_prime_error, phi_prime_error) = calibrate_stage(&stage, focal_len, sep, dist, px, nx, ny, correction, prev_cal, begin_pan_angle, end_pan_angle, begin_tilt_angle, end_tilt_angle, XK_C);

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
        double theta_prime = get_theta_prime(phi, theta, sep, r, theta_prime_error);
        double phi_prime = get_phi_prime(phi, theta, sep, r, phi_prime_error);

        printf("Calculated Coordinates in degrees (theta, phi):\n");
        printf("EBS: (%.2f, %.2f)\n", theta * 180 / M_PI, phi * 180 / M_PI);
        printf("FBC: (%.2f, %.2f)\n", theta_prime * 180 / M_PI, phi_prime * 180 / M_PI);
        printf("\n");

        float pan_position = get_motor_position(begin_pan, end_pan, begin_pan_angle, end_pan_angle, phi_prime);
        float tilt_position = get_motor_position(begin_tilt, end_tilt, begin_tilt_angle, end_tilt_angle, theta_prime);

        printf("Target motor positions:\n");
        printf("Pan: %.2f (Range: 0 - %.2f)\n", pan_position, end_pan - begin_pan);
        printf("Tilt: %.2f (Range: 0 - %.2f)\n", tilt_position, end_tilt - begin_tilt);

        std::string input;
        printf("Move to target? (y/n)\n");
        std::cin >> input;
        if (input == std::string("y")) {
            mtx.lock();
            stage.set_position_speed_acceleration(2, pan_position, PAN_MAX_SPEED, PAN_MAX_ACC);
            stage.set_position_speed_acceleration(3, tilt_position, TILT_MAX_SPEED, TILT_MAX_ACC);
            mtx.unlock();
        }
    }
    active = false;
    pinger.join();
}
