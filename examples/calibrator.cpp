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
    Stage stage("192.168.50.1", 5520);
    stage.handshake();
    std::cout << stage.get_device_info().to_string();
    CalibrationInit cal_init(focal_len, sep, dist, px, nx, ny, correction,
                             prev_cal, begin_pan_angle, end_pan_angle,
                             begin_tilt_angle, end_tilt_angle, XK_space);
    Calibration cal_params = calibrate_stage(&stage, cal_init);
    printf("Calibration Parameters:\nNx: %d\nNy: %d\nHFOVx: %.2f rad\nHFOVy: "
           "%.2f rad\nSeparation: %.2f m\nBegin Pan: %.2f\nEnd Pan: %.2f\n",
           nx, ny, cal_params.hfovx, cal_params.hfovy, sep,
           cal_params.begin_pan, cal_params.end_pan);
    printf(
        "Begin Tilt: %.2f\nEnd Tilt: %.2f\nTilt Error: %.2f\nPan Error: %.2f\n",
        cal_params.begin_tilt, cal_params.end_tilt,
        cal_params.theta_prime_error, cal_params.phi_prime_error);
}
