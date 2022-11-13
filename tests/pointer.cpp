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
            kessler.get_network_info();
            last_ping = clock();
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
    printf("Press `Q` to mark Motor %hd start position.\n", motor_address);
    while (!start) {
        q_pressed = GetAsyncKeyState(0x51);
        if (q_pressed) {
            kessler.mark_begin_position(motor_address);
            printf("Start position marked.\n");
            start = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    printf("Press `Q` to mark Motor %hd end position.\n", motor_address);
    while (!end) {
        q_pressed = GetAsyncKeyState(0x51);
        if (q_pressed) {
            kessler.mark_end_position(motor_address);
            printf("End position marked.\n");
            end = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printf("Motor %hd calibration complete.\n", motor_address);
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

double get_phi(int x, int nx, double hfov) {
    return atan(2 * x * tan(hfov) / nx);
}

double get_theta(int y, int ny, double hfov) {
    return atan((float)ny / 2 / (float)y / tan(hfov));
}

double get_phi_prime(double phi, double theta, double y0, double r) {
    return phi - (y0 / r) * (cos(phi) / sin(theta));
}

double get_theta_prime(double phi, double theta, double y0, double r) {
    return theta - (y0 / r) * cos(theta) * sin(phi);
}

int main () {
    Stage kessler("192.168.50.1", 5520);
    kessler.handshake();
    std::cout << kessler.get_device_info().to_string();
    std::thread driver(controller, std::ref(kessler));

    printf("Move Pan motor to start position and press 'Q'. Then move Pan motor to end position and press `Q`.\n");
    calibrate(2, kessler);
    printf("Move Tilt motor to start position and press 'Q'. Then move Tilt motor to end position and press `Q`.\n");
    calibrate(3, kessler);
    printf("Calibration complete. Press space to exit manual control.\n");
    driver.join();

    class MotorInfo info = kessler.get_motor_info();
    float end_pan = info.motors[1].end_position;
    float end_tilt = info.motors[2].end_position;

    std::thread pinger(ping, std::ref(kessler));
    double hfovx;
    double hfovy;
    double px;
    double f;
    double y0;
    double r;
    int ny;
    int nx;
    int y;
    int x;
    printf("Enter focal length in millimeters:\n");
    std::cin >> f;
    f *= pow(10, -3);
    printf("Enter sensor separation in meters:\n");
    std::cin >> y0;
    printf("Enter number of vertical pixels:\n");
    std::cin >> ny;
    printf("Enter number of horizontal pixels:\n");
    std::cin >> nx;
    printf("Enter pixel size in microns:\n");
    std::cin >> px;
    px *= pow(10, -6);

    hfovx = atan(px * nx / f / 2);
    hfovy = atan(px * ny / f / 2);

    while (true) {
        printf("Enter target distance (or 0 to quit):\n");
        std::cin >> r;
        if (r == 0) {
            break;
        }
        printf("Enter target x coordinate:\n");
        std::cin >> x;
        printf("Enter target y coordinate:\n");
        std::cin >> y;

        double theta = get_theta(y, ny, hfovy);
        double phi = get_phi(x, nx, hfovx);
        double theta_prime = get_theta_prime(phi, theta, y0, r);
        double phi_prime = get_phi_prime(phi, theta, y0, r);

        printf("Calculated Coordinates in degrees (theta, phi):\n");
        printf("EBS: (%.2f, %.2f)\n", theta * 180 / M_PI, phi * 180 / M_PI);
        printf("FBC: (%.2f, %.2f)\n", theta_prime * 180 / M_PI, phi_prime * 180 / M_PI);
        printf("\n");

        float pan_position = end_pan * (float)(0.5 + theta_prime / M_PI);
        float tilt_position = 3 * end_tilt * (float)(theta_prime / M_PI) / 2;

        printf("Target motor positions:\n");
        printf("Pan: %.2f (Range: 0 - %.2f)\n", pan_position, end_pan);
        printf("Tilt: %.2f (Range: 0 - %.2f)\n", tilt_position, end_tilt);

        std::string input;
        printf("Move to target? (y/n)\n");
        std::cin >> input;
        if (input == std::string("y")) {
            mtx.lock();
            kessler.set_position_speed_acceleration(2, pan_position, PAN_MAX_SPEED, PAN_MAX_ACC);
            kessler.set_position_speed_acceleration(3, tilt_position, TILT_MAX_SPEED, TILT_MAX_ACC);
            mtx.unlock();
        }
    }
    active = false;
    pinger.join();
}
