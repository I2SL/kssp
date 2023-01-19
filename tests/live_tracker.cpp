#include "ebs_tracking_utils.hpp"
#include "calibrator.hpp"

void select_sensor () {
    // Select a frame-based or EBS monitoring sensor and connect
}

void shutdown () {
    // Close connections for shutdown
}

void tracker (bool& active) {
    while (active) {
        // Process tracking data and generate motor commands
        // Pass reference to most recent result and overwrite?
    }
}

std::tuple<float, float> get_commands (float prev_tilt, float prev_pan) {
    // get most recent command from tracker
    // compare against previous commands and check if they are reasonable
    std::tuple<float, float> commands(prev_tilt, prev_pan);
    return commands;
}

int main () {

    // Monitoring sensor setup
    select_sensor();

    // Calibrate the classification sensor
    std::mutex mtx;
    bool active = true;
    float begin_pan, end_pan, begin_tilt, end_tilt, theta0, phi0, theta0p, phi0p;
    double hfovx, hfovy, y0, r;
    int nx, ny, update;
    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();
    std::tie(nx, ny, hfovx, hfovy, y0, begin_pan, end_pan, begin_tilt, end_tilt, theta0, phi0, theta0p, phi0p) = calibrate_stage(std::ref(kessler));
    std::thread pinging_thread(ping, std::ref(kessler), std::ref(mtx), std::ref(active));
    printf("Enter approximate target distance in meters:\n");
    std::cin >> r;
    printf("Enter update time in ms:\n");
    std::cin >> update;
    printf("Tracking active. Press SPACE to stop.\n");

    // Run tracking until user presses space
    float tilt_command, pan_command = 0;
    std::thread tracking_thread(tracker, std::ref(active));
    while (active) {
        clock_t loop_start = clock();
        if (GetAsyncKeyState(VK_SPACE)) {
            active = false;
        }

        std::tie(tilt_command, pan_command) = get_commands(tilt_command, pan_command);
        kessler.set_position_speed_acceleration(3, tilt_command, TILT_MAX_SPEED, TILT_MAX_ACC);
        kessler.set_position_speed_acceleration(2, pan_command, PAN_MAX_SPEED, PAN_MAX_ACC);

        // Warn if loop took longer than specified time. Otherwise, wait until specified time has passed.
        int loop_time = (int)((clock() - loop_start)/CLOCKS_PER_SEC/1000);
        if (loop_time > update){
            printf("WARNING: Processing exceeded update time.\n");
            printf("Processing Time (ms): %i\nUpdate Time (ms): %i", loop_time, update);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(update - loop_time));
        }
    }
    pinging_thread.join();
    tracking_thread.join();
    shutdown();

    return 0;
}