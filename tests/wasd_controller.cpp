#include <iostream>
#include <thread>

#include "../libkessler/stage.h"

using namespace std;

int main ()
{
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
    bool w_pressed = false;
    bool s_pressed = false;
    bool a_pressed = false;
    bool d_pressed = false;
    bool up_pressed = false;
    bool down_pressed = false;
    bool left_pressed = false;
    bool right_pressed = false;
    int speed = 0;

    //Stage kessler("192.168.50.1", 5520);
    while (running)
    {
        w_pressed = GetAsyncKeyState(0x57);
        s_pressed = GetAsyncKeyState(0x53);
        a_pressed = GetAsyncKeyState(0x41);
        d_pressed = GetAsyncKeyState(0x44);
        up_pressed = GetAsyncKeyState(0x26);
        down_pressed = GetAsyncKeyState(0x28);
        left_pressed = GetAsyncKeyState(0x25);
        right_pressed = GetAsyncKeyState(0x27);

        if (w_pressed) {
            if(!tilt_up) {
                if (!s_pressed) {
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
                }
            }
        }

        if (s_pressed) {
            if(!tilt_down) {
                if (!w_pressed) {
                    tilt_down = true;
                    cout << "Start Tilt Down" << endl;
                }
            }
        }
        else {
            if(tilt_down) {
                tilt_down = false;
                if (!tilt_up) {
                    cout << "Stop Tilt" << endl;
                }
            }
        }

        if (a_pressed) {
            if(!pan_left) {
                if (!d_pressed) {
                    pan_left = true;
                    cout << "Start Pan Left" << endl;
                }
            }
        }
        else {
            if(pan_left) {
                pan_left = false;
                if (!pan_right) {
                    cout << "Stop Pan Left" << endl;
                }
            }
        }

        if (d_pressed) {
            if(!pan_right) {
                if (!a_pressed) {
                    pan_right = true;
                    cout << "Start Pan Right" << endl;
                }
            }
        }
        else {
            if(pan_right) {
                pan_right = false;
                if (!pan_left) {
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
                    slide_left = true;
                    cout << "Start Slide Left" << endl;
                }
            }
        }
        else {
            if(slide_left) {
                slide_left = false;
                if (!slide_right) {
                    cout << "Stop Slide" << endl;
                }
            }
        }

        if (right_pressed) {
            if(!slide_right) {
                if (!left_pressed) {
                    slide_right = true;
                    cout << "Start Slide Right" << endl;
                }
            }
        }
        else {
            if(slide_right) {
                slide_right = false;
                if (!slide_left) {
                    cout << "Stop Slide" << endl;
                }
            }
        }

        if (GetAsyncKeyState(VK_SPACE)) {
            running = false;
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
