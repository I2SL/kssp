#include <iostream>
#include <ctime>
#include <thread>

#include "../libkessler/stage.h"

std::mutex mtx;
bool active = true;

void controller (Stage& kessler)
{
    printf("\n");
    printf("CONTROLS:\n");
    printf("---------\n");
    printf("Flashlight: 'F'\n");
    printf("Tilt Up: 'W'\n");
    printf("Tilt Down: 'S'\n");
    printf("Pan Left: 'A'\n");
    printf("Pan Right: 'D'\n");
    printf("Slide Left: Left Arrow\n");
    printf("Slide Right: Right Arrow\n");
    printf("Speed Up: Up Arrow\n");
    printf("Speed Down: Down Arrow\n");
    printf("Stop: Space\n");

    bool running = true;
    bool pan_right = false;
    bool pan_left = false;
    bool tilt_up = false;
    bool tilt_down = false;
    bool slide_right = false;
    bool slide_left = false;
    bool flashlight = false;
    bool w_pressed = false;
    bool s_pressed = false;
    bool a_pressed = false;
    bool d_pressed = false;
    bool f_pressed = false;
    bool up_pressed = false;
    bool down_pressed = false;
    bool left_pressed = false;
    bool right_pressed = false;
    int speed = 0;
    float speed_p = 0;
    clock_t last_ping = clock();
    float since_last_ping;

    while (running)
    {
        since_last_ping = (float)(clock() - last_ping)/CLOCKS_PER_SEC;
        w_pressed = GetAsyncKeyState(0x57);
        s_pressed = GetAsyncKeyState(0x53);
        a_pressed = GetAsyncKeyState(0x41);
        d_pressed = GetAsyncKeyState(0x44);
        f_pressed = GetAsyncKeyState(0x46);
        up_pressed = GetAsyncKeyState(0x26);
        down_pressed = GetAsyncKeyState(0x28);
        left_pressed = GetAsyncKeyState(0x25);
        right_pressed = GetAsyncKeyState(0x27);
        speed_p = (float)speed / 100;


        if (w_pressed) {
            if(!tilt_up) {
                if (!s_pressed) {
                    kessler.set_position_speed_acceleration(3, 25000, TILT_MAX_SPEED*speed_p, TILT_MAX_ACC);
                    tilt_up = true;
                    last_ping = clock();
                }
            }
        }
        else {
            if(tilt_up) {
                tilt_up = false;
                if (!tilt_down) {
                    kessler.set_position_speed_acceleration(3, 25000, 0, TILT_MAX_ACC);
                    last_ping = clock();
                }
            }
        }

        if (s_pressed) {
            if(!tilt_down) {
                if (!w_pressed) {
                    kessler.set_position_speed_acceleration(3, -25000, TILT_MAX_SPEED*speed_p, TILT_MAX_ACC);
                    tilt_down = true;
                    last_ping = clock();
                }
            }
        }
        else {
            if(tilt_down) {
                tilt_down = false;
                if (!tilt_up) {
                    kessler.set_position_speed_acceleration(3, -25000, 0, TILT_MAX_ACC);
                    last_ping = clock();
                }
            }
        }

        if (a_pressed) {
            if(!pan_left) {
                if (!d_pressed) {
                    kessler.set_position_speed_acceleration(2, -25000, PAN_MAX_SPEED*speed_p, PAN_MAX_ACC);
                    pan_left = true;
                    last_ping = clock();
                }
            }
        }
        else {
            if(pan_left) {
                pan_left = false;
                if (!pan_right) {
                    kessler.set_position_speed_acceleration(2, -25000, 0, PAN_MAX_ACC);
                    last_ping = clock();
                }
            }
        }

        if (d_pressed) {
            if(!pan_right) {
                if (!a_pressed) {
                    kessler.set_position_speed_acceleration(2, 25000, PAN_MAX_SPEED*speed_p, PAN_MAX_ACC);
                    pan_right = true;
                    last_ping = clock();
                }
            }
        }
        else {
            if(pan_right) {
                pan_right = false;
                if (!pan_left) {
                    kessler.set_position_speed_acceleration(2, 25000, 0, PAN_MAX_ACC);
                    last_ping = clock();
                }
            }
        }

        if (up_pressed) {
            if (!down_pressed) {
                if (speed < 100) {
                    speed += 1;
                }
                printf("Speed: %d\n", speed);
            }
        }

        if (down_pressed) {
            if (!up_pressed) {
                if (speed > 0) {
                    speed -= 1;
                }
                printf("Speed: %d\n", speed);
            }
        }

        if (left_pressed) {
            if(!slide_left) {
                if (!right_pressed) {
                    kessler.set_position_speed_acceleration(1, -25000, SLIDE_MAX_SPEED*speed_p, SLIDE_MAX_ACC);
                    slide_left = true;
                    last_ping = clock();
                }
            }
        }
        else {
            if(slide_left) {
                slide_left = false;
                if (!slide_right) {
                    kessler.set_position_speed_acceleration(1, -25000, 0, SLIDE_MAX_ACC);
                    last_ping = clock();
                }
            }
        }

        if (right_pressed) {
            if(!slide_right) {
                if (!left_pressed) {
                    kessler.set_position_speed_acceleration(1, 25000, SLIDE_MAX_SPEED*speed_p, SLIDE_MAX_ACC);
                    slide_right = true;
                    last_ping = clock();
                }
            }
        }
        else {
            if(slide_right) {
                slide_right = false;
                if (!slide_left) {
                    kessler.set_position_speed_acceleration(1, 25000, 0, SLIDE_MAX_ACC);
                    last_ping = clock();
                }
            }
        }

        if (f_pressed) {
            if (flashlight) {
                flashlight = false;
                kessler.set_led_status(0, 0);
                last_ping = clock();
            }
            else {
                flashlight = true;
                kessler.set_led_status(1, 0);
                last_ping = clock();
            }

        }

        if (since_last_ping > 10) {
            mtx.lock();
            kessler.get_network_info();
            last_ping = clock();
            mtx.unlock();
        }

        if (GetAsyncKeyState(VK_SPACE)) {
            running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void calibrate(boost::uint8_t motor_address, Stage& kessler) {
    bool start = false;
    bool end = false;
    bool q_pressed = false;
    while(!kessler.MotorCalibratedQueue.empty()) kessler.MotorCalibratedQueue.pop();
    printf("Press `Q` to mark Motor %hd start position.\n", motor_address);
    while (!start) {
        q_pressed = GetAsyncKeyState(0x51);
        if (q_pressed) {
            mtx.lock();
            kessler.mark_begin_position(motor_address);
            printf("Start position marked.\n");
            start = true;
            mtx.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    printf("Press `Q` to mark Motor %hd end position.\n", motor_address);
    while (!end) {
        q_pressed = GetAsyncKeyState(0x51);
        if (q_pressed) {
            mtx.lock();
            kessler.mark_end_position(motor_address);
            printf("End position marked.\n");
            end = true;
            mtx.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while(kessler.MotorCalibratedQueue.empty()){
        continue;
    }

    class MotorCalibrated info = kessler.MotorCalibratedQueue.front();
    boost::uint8_t addr = info.motor_address;
    float start_pos = info.begin_position;
    float end_pos = info.end_position;
    printf("Calibration complete. Motor: %hd, Start: %.2f, End: %.2f\n", addr, start_pos, end_pos);
    kessler.MotorCalibratedQueue.pop();
}

void ping(Stage& kessler) {
    clock_t last_ping = clock();
    float since_last_ping;
    while (active) {
        since_last_ping = (float)(clock() - last_ping)/CLOCKS_PER_SEC;
        if (since_last_ping > 10) {
            mtx.lock();
            kessler.get_network_info();
            last_ping = clock();
            mtx.unlock();
        }
    }
}

int main () {
    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();
    kessler.reset_axis(0);
    std::thread driver(controller, std::ref(kessler));

    printf("Move Slide motor to start position and press 'Q'. Then move Slide motor to end position and press `Q`.\n");
    calibrate(1, kessler);
    printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `Q`.\n");
    calibrate(2, kessler);
    printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `Q`.\n");
    calibrate(3, kessler);
    printf("Calibration complete. Press space to exit manual control.\n");
    driver.join();

    class MotorInfo info = kessler.get_motor_info();
    float begin_slide = info.motors[0].begin_position;
    float end_slide = info.motors[0].end_position;
    float begin_pan = info.motors[1].begin_position;
    float end_pan = info.motors[1].end_position;
    float begin_tilt = info.motors[2].begin_position;
    float end_tilt = info.motors[2].end_position;

    printf("Target motor ranges:\n");
    printf("Slide: (Range: %.2f - %.2f)\n", begin_slide, end_slide);
    printf("Pan: (Range: %.2f - %.2f)\n", begin_pan, end_pan);
    printf("Tilt: (Range: %.2f - %.2f)\n", begin_tilt, end_tilt);

    while(!kessler.MotorPositionQueue.empty()) kessler.MotorPositionQueue.pop();
    std::thread pinger(ping, std::ref(kessler));
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
                kessler.set_position_speed_acceleration(2, pos, scale*PAN_MAX_SPEED, scale*PAN_MAX_ACC);
            }
            else if (addr == 3) {
                kessler.set_position_speed_acceleration(3, pos, scale*TILT_MAX_SPEED, scale*TILT_MAX_ACC);
            }
            else {
                kessler.set_position_speed_acceleration(1, pos, scale*SLIDE_MAX_SPEED, scale*SLIDE_MAX_ACC);
            }
            mtx.unlock();
            printf("Waiting for motor position notification...\n");
            while (kessler.MotorPositionQueue.empty()) {
                continue;
            }
            class MotorPosition done = kessler.MotorPositionQueue.front();
            kessler.MotorPositionQueue.pop();
            printf("Received Position:\n");
            printf("------------------\n");
            printf("Motor Address: %hd\n", done.motor_address);
            printf("Position: %.2f\n", done.position);
        }
    }
    active = false;
    pinger.join();
}
