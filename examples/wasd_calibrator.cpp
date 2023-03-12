#include <iostream>
#include <thread>

#include <kessler/tools/pointer_utils.h>

using namespace std;

void calibrate(boost::uint8_t motor_address, Stage* kessler) {
    bool start = false;
    bool end = false;
    printf("Press `Q` to mark Motor %hd start position.\n", motor_address);
    while (!start) {
        bool q_pressed = key_is_pressed(XK_Q);
        if (q_pressed) {
            kessler->mark_begin_position(motor_address);
            printf("Start position marked.\n");
            start = true;
        }
        this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    printf("Press `Q` to mark Motor %hd end position.\n", motor_address);
    while (!end) {
        bool q_pressed = key_is_pressed(XK_Q);
        if (q_pressed) {
            kessler->mark_end_position(motor_address);
            printf("End position marked.\n");
            end = true;
        }
        this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    printf("Motor %hd calibration complete.\n", motor_address);
}

int main () {
    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    cout << kessler.get_device_info().to_string();
    thread driver(controller, &kessler);
    printf("Move Slider motor to start position and press 'Q'. Then move Slider motor to end position and press `Q`.\n");
    calibrate(1, &kessler);
    printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `Q`.\n");
    calibrate(2, &kessler);
    printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `Q`.\n");
    calibrate(3, &kessler);
    printf("Calibration complete. Press space to exit.\n");
    cout << kessler.get_motor_info().to_string();
    driver.join();
}
