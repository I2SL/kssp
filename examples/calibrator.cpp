#include <iostream>
#include <thread>

#include <kessler/tools/calibrator.h>

int main () {
    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();
    const auto [nx, ny, hfovx, hfovy, y0, begin_pan, end_pan, begin_tilt, end_tilt, theta_prime_error, phi_prime_error] = calibrate_stage(&kessler);
    printf("Calibration Parameters:\nNx: %d\nNy: %d\nHFOVx: %.2f rad\nHFOVy: %.2f rad\nSeparation: %.2f m\nBegin Pan: %.2f\nEnd Pan: %.2f\n", nx, ny, hfovx, hfovy, y0, begin_pan, end_pan);
    printf("Begin Tilt: %.2f\nEnd Tilt: %.2f\nTilt Error: %.2f\nPan Error: %.2f\n", begin_tilt, end_tilt, theta_prime_error, phi_prime_error);
}
