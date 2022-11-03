#include <iostream>
#include <thread>

#include "../libkessler/stage.h"

using namespace std;

int main ()
{
    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();

    printf("\n");
    printf("CONTROLS:\n");
    printf("---------\n");
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

    while (running)
    {
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
                    cout << "Start Tilt Up" << endl;
                }
            }
        }
        else {
            if(tilt_up) {
                tilt_up = false;
                if (!tilt_down) {
                    cout << "Stop Tilt" << endl;
                    kessler.set_position_speed_acceleration(3, 25000, 0, TILT_MAX_ACC);
                }
            }
        }

        if (s_pressed) {
            if(!tilt_down) {
                if (!w_pressed) {
                    kessler.set_position_speed_acceleration(3, -25000, TILT_MAX_SPEED*speed_p, TILT_MAX_ACC);
                    tilt_down = true;
                    cout << "Start Tilt Down" << endl;
                }
            }
        }
        else {
            if(tilt_down) {
                tilt_down = false;
                if (!tilt_up) {
                    kessler.set_position_speed_acceleration(3, -25000, 0, TILT_MAX_ACC);
                    cout << "Stop Tilt" << endl;
                }
            }
        }

        if (a_pressed) {
            if(!pan_left) {
                if (!d_pressed) {
                    kessler.set_position_speed_acceleration(2, -25000, PAN_MAX_SPEED*speed_p, PAN_MAX_ACC);
                    pan_left = true;
                    cout << "Start Pan Left" << endl;
                }
            }
        }
        else {
            if(pan_left) {
                pan_left = false;
                if (!pan_right) {
                    kessler.set_position_speed_acceleration(2, -25000, 0, PAN_MAX_ACC);
                    cout << "Stop Pan Left" << endl;
                }
            }
        }

        if (d_pressed) {
            if(!pan_right) {
                if (!a_pressed) {
                    kessler.set_position_speed_acceleration(2, 25000, PAN_MAX_SPEED*speed_p, PAN_MAX_ACC);
                    pan_right = true;
                    cout << "Start Pan Right" << endl;
                }
            }
        }
        else {
            if(pan_right) {
                pan_right = false;
                if (!pan_left) {
                    kessler.set_position_speed_acceleration(2, 25000, 0, PAN_MAX_ACC);
                    cout << "Stop Pan" << endl;
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
                    cout << "Start Slide Left" << endl;
                }
            }
        }
        else {
            if(slide_left) {
                slide_left = false;
                if (!slide_right) {
                    kessler.set_position_speed_acceleration(1, -25000, 0, SLIDE_MAX_ACC);
                    cout << "Stop Slide" << endl;
                }
            }
        }

        if (right_pressed) {
            if(!slide_right) {
                if (!left_pressed) {
                    kessler.set_position_speed_acceleration(1, 25000, SLIDE_MAX_SPEED*speed_p, SLIDE_MAX_ACC);
                    slide_right = true;
                    cout << "Start Slide Right" << endl;
                }
            }
        }
        else {
            if(slide_right) {
                slide_right = false;
                if (!slide_left) {
                    kessler.set_position_speed_acceleration(1, 25000, 0, SLIDE_MAX_ACC);
                    cout << "Stop Slide" << endl;
                }
            }
        }

        if (f_pressed) {
            if (flashlight) {
                flashlight = false;
                kessler.set_led_status(0, 0);
                cout << "Flashlight Off" << endl;
            }
            else {
                flashlight = true;
                kessler.set_led_status(1, 0);
                cout << "Flashlight On" << endl;
            }

        }

        if (GetAsyncKeyState(VK_SPACE)) {
            kessler.set_position_speed_acceleration(1, 25000, 0, SLIDE_MAX_ACC);
            kessler.set_position_speed_acceleration(2, 25000, 0, PAN_MAX_ACC);
            kessler.set_position_speed_acceleration(3, 25000, 0, TILT_MAX_ACC);
            kessler.shutdown();
            running = false;
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
