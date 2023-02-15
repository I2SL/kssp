#include <iostream>
#include <ctime>
#include <thread>

#include "../kessler/pointer_utils.hpp"

using namespace std;

int main ()
{
    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();

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
    int speed = 0;
    clock_t last_ping = clock();
    float since_last_ping;

    while (running)
    {
        since_last_ping = (float)(clock() - last_ping)/CLOCKS_PER_SEC;
        bool w_pressed = key_is_pressed(XK_W);
        bool s_pressed = key_is_pressed(XK_S);
        bool a_pressed = key_is_pressed(XK_A);
        bool d_pressed = key_is_pressed(XK_D);
        bool f_pressed = key_is_pressed(XK_F);
        bool up_pressed = key_is_pressed(XK_Up);
        bool down_pressed = key_is_pressed(XK_Down);
        bool left_pressed = key_is_pressed(XK_Left);
        bool right_pressed = key_is_pressed(XK_Right);
        float speed_p = (float)speed / 100;


        if (w_pressed) {
            if(!tilt_up) {
                if (!s_pressed) {
                    kessler.set_position_speed_acceleration(3, 25000, TILT_MAX_SPEED*speed_p, TILT_MAX_ACC);
                    tilt_up = true;
                    cout << "Start Tilt Up" << endl;
                    last_ping = clock();
                }
            }
        }
        else {
            if(tilt_up) {
                tilt_up = false;
                if (!tilt_down) {
                    cout << "Stop Tilt" << endl;
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
                    cout << "Start Tilt Down" << endl;
                    last_ping = clock();
                }
            }
        }
        else {
            if(tilt_down) {
                tilt_down = false;
                if (!tilt_up) {
                    kessler.set_position_speed_acceleration(3, -25000, 0, TILT_MAX_ACC);
                    cout << "Stop Tilt" << endl;
                    last_ping = clock();
                }
            }
        }

        if (a_pressed) {
            if(!pan_left) {
                if (!d_pressed) {
                    kessler.set_position_speed_acceleration(2, -25000, PAN_MAX_SPEED*speed_p, PAN_MAX_ACC);
                    pan_left = true;
                    cout << "Start Pan Left" << endl;
                    last_ping = clock();
                }
            }
        }
        else {
            if(pan_left) {
                pan_left = false;
                if (!pan_right) {
                    kessler.set_position_speed_acceleration(2, -25000, 0, PAN_MAX_ACC);
                    cout << "Stop Pan Left" << endl;
                    last_ping = clock();
                }
            }
        }

        if (d_pressed) {
            if(!pan_right) {
                if (!a_pressed) {
                    kessler.set_position_speed_acceleration(2, 25000, PAN_MAX_SPEED*speed_p, PAN_MAX_ACC);
                    pan_right = true;
                    cout << "Start Pan Right" << endl;
                    last_ping = clock();
                }
            }
        }
        else {
            if(pan_right) {
                pan_right = false;
                if (!pan_left) {
                    kessler.set_position_speed_acceleration(2, 25000, 0, PAN_MAX_ACC);
                    cout << "Stop Pan" << endl;
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
                    cout << "Start Slide Left" << endl;
                    last_ping = clock();
                }
            }
        }
        else {
            if(slide_left) {
                slide_left = false;
                if (!slide_right) {
                    kessler.set_position_speed_acceleration(1, -25000, 0, SLIDE_MAX_ACC);
                    cout << "Stop Slide" << endl;
                    last_ping = clock();
                }
            }
        }

        if (right_pressed) {
            if(!slide_right) {
                if (!left_pressed) {
                    kessler.set_position_speed_acceleration(1, 25000, SLIDE_MAX_SPEED*speed_p, SLIDE_MAX_ACC);
                    slide_right = true;
                    cout << "Start Slide Right" << endl;
                    last_ping = clock();
                }
            }
        }
        else {
            if(slide_right) {
                slide_right = false;
                if (!slide_left) {
                    kessler.set_position_speed_acceleration(1, 25000, 0, SLIDE_MAX_ACC);
                    cout << "Stop Slide" << endl;
                    last_ping = clock();
                }
            }
        }

        if (f_pressed) {
            if (flashlight) {
                flashlight = false;
                kessler.set_led_status(0, 0);
                cout << "Flashlight Off" << endl;
                last_ping = clock();
            }
            else {
                flashlight = true;
                kessler.set_led_status(1, 0);
                cout << "Flashlight On" << endl;
                last_ping = clock();
            }

        }

        if (since_last_ping > 10) {
            cout << "Pinging Device..." << endl;
            kessler.get_network_info();
            last_ping = clock();
        }

        if (key_is_pressed(XK_space)) {
            kessler.set_position_speed_acceleration(1, 25000, 0, SLIDE_MAX_ACC);
            kessler.set_position_speed_acceleration(2, 25000, 0, PAN_MAX_ACC);
            kessler.set_position_speed_acceleration(3, 25000, 0, TILT_MAX_ACC);
            kessler.shutdown();
            running = false;
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
