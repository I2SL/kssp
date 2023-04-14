#include <iostream>
#include <thread>

#include <kssp/tools/pointer_utils.h>

using namespace std;

int main () {
    Stage stage("192.168.50.1", 5520);
    stage.handshake();
    std::cout << stage.get_device_info().to_string();

    std::thread driver(controller, &stage, XK_space);
    driver.join();
}
