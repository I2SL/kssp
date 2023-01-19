#include "calibrator.hpp"

int main () {
    std::mutex mtx;
    bool active = true;
    float begin_pan, end_pan, begin_tilt, end_tilt, theta0, phi0, theta0p, phi0p;
    double hfovx, hfovy, y0, r;
    int nx, ny, x, y;

    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();
    std::tie(nx, ny, hfovx, hfovy, y0, begin_pan, end_pan, begin_tilt, end_tilt, theta0, phi0, theta0p, phi0p) = calibrate_stage(std::ref(kessler));
    std::thread pinger(ping, std::ref(kessler), std::ref(mtx), std::ref(active));

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
