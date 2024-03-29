#include <kssp/tools/pointer_utils.h>

int main () {
    std::mutex mtx;
    bool active = true;

    Stage stage("192.168.50.1", 5520);
    stage.handshake();
    std::cout << stage.get_device_info().to_string();
    stage.reset_axis(0);
    std::thread driver(controller, &stage, XK_space);

    printf("Move Slide motor to start position and press 'Q'. Then move Slide motor to end position and press `Q`.\n");
    calibrate(1, &stage, mtx);
    printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `Q`.\n");
    calibrate(2, &stage, mtx);
    printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `Q`.\n");
    calibrate(3, &stage, mtx);
    printf("Calibration complete. Press space to exit manual control.\n");
    driver.join();

    class MotorInfo info = stage.get_motor_info();
    float begin_slide = info.motors[0].begin_position;
    float end_slide = info.motors[0].end_position;
    float begin_pan = info.motors[1].begin_position;
    float end_pan = info.motors[1].end_position;
    float begin_tilt = info.motors[2].begin_position;
    float end_tilt = info.motors[2].end_position;

    printf("Given target motor ranges:\n");
    printf("Slide: (Range: %.2f - %.2f)\n", begin_slide, end_slide);
    printf("Pan: (Range: %.2f - %.2f)\n", begin_pan, end_pan);
    printf("Tilt: (Range: %.2f - %.2f)\n", begin_tilt, end_tilt);
    printf("\n");

    printf("Theoretical target motor ranges:\n");
    printf("Slide: (Range: %.2d - %.2f)\n", 0, end_slide - begin_slide);
    printf("Pan: (Range: %.2d - %.2f)\n", 0, end_pan - begin_pan);
    printf("Tilt: (Range: %.2d - %.2f)\n", 0, end_tilt - begin_tilt);

    while(!stage.MotorPositionQueue.empty()) stage.MotorPositionQueue.pop();
    std::thread pinger(ping, &stage, std::ref(mtx), std::ref(active));
    int addr = 0;
    float pos = 0;
    float scale = 0;
    while (true) {
        printf("Enter motor address (or 0 to quit):\n");
        std::cin >> addr;
        if (addr == 0) {
            break;
        }
        printf("Enter target position:\n");
        std::cin >> pos;
        printf("Enter speed/acc scale:\n");
        std::cin >> scale;

        std::string input;
        printf("Move to target? (y/n)\n");
        std::cin >> input;
        if (input == std::string("y")) {
            mtx.lock();
            if (addr == 2) {
                stage.set_position_speed_acceleration(2, pos, scale*PAN_MAX_SPEED, scale*PAN_MAX_ACC);
            }
            else if (addr == 3) {
                stage.set_position_speed_acceleration(3, pos, scale*TILT_MAX_SPEED, scale*TILT_MAX_ACC);
            }
            else {
                stage.set_position_speed_acceleration(1, pos, scale*SLIDE_MAX_SPEED, scale*SLIDE_MAX_ACC);
            }
            mtx.unlock();
            printf("Waiting for motor position notification...\n");
            while (stage.MotorPositionQueue.empty()) {
                // do nothing until notification received
            }
            class MotorPosition done = stage.MotorPositionQueue.front();
            stage.MotorPositionQueue.pop();
            printf("Received Position:\n");
            printf("------------------\n");
            printf("Motor Address: %hd\n", done.motor_address);
            printf("Position: %.2f\n", done.position);
        }
    }
    active = false;
    pinger.join();
}
