#include <iostream>
#include <ctime>
#include <thread>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "../libkessler/stage.h"

using namespace std;

bool key_is_pressed(KeySym ks) {
    Display *dpy = XOpenDisplay(":0");
    char keys_return[32];
    XQueryKeymap(dpy, keys_return);
    KeyCode kc2 = XKeysymToKeycode(dpy, ks);
    bool isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
    XCloseDisplay(dpy);
    return isPressed;
}

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
        w_pressed = key_is_pressed(XK_W);
        s_pressed = key_is_pressed(XK_S);
        a_pressed = key_is_pressed(XK_A);
        d_pressed = key_is_pressed(XK_D);
        f_pressed = key_is_pressed(XK_F);
        up_pressed = key_is_pressed(XK_Up);
        down_pressed = key_is_pressed(XK_Down);
        left_pressed = key_is_pressed(XK_Left);
        right_pressed = key_is_pressed(XK_Right);
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
            kessler.get_network_info();
            last_ping = clock();
        }

        if (key_is_pressed(XK_space)) {
            running = false;
        }

        this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void calibrate(boost::uint8_t motor_address, Stage& kessler) {
    bool start = false;
    bool end = false;
    bool q_pressed = false;
    printf("Press `Q` to mark Motor %hd start position.\n", motor_address);
    while (!start) {
        q_pressed = key_is_pressed(XK_Q);
        if (q_pressed) {
            kessler.mark_begin_position(motor_address);
            printf("Start position marked.\n");
            start = true;
        }
        this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    printf("Press `Q` to mark Motor %hd end position.\n", motor_address);
    while (!end) {
        q_pressed = key_is_pressed(XK_Q);
        if (q_pressed) {
            kessler.mark_end_position(motor_address);
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
    thread driver(controller, ref(kessler));
    printf("Move Slider motor to start position and press 'Q'. Then move Slider motor to end position and press `Q`.\n");
    calibrate(1, kessler);
    printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `Q`.\n");
    calibrate(2, kessler);
    printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `Q`.\n");
    calibrate(3, kessler);
    printf("Calibration complete. Press space to exit.\n");
    cout << kessler.get_motor_info().to_string();
    driver.join();
}
