#include "led_status.h"

LEDStatus::LEDStatus(boost::uint8_t master_status, boost::uint8_t slave_status):
    master_status(master_status),
    slave_status(slave_status)
{

}

std::string LEDStatus::to_string() const {
    std::string result("LED INFO:\n");
    result += std::string("---------\n");
    result += fmt::format("Master Status: {}\n", status_to_string(master_status));
    result += fmt::format("Slave Status: {}\n", status_to_string(slave_status));

    return result;
}

std::string LEDStatus::status_to_string(boost::uint8_t status) {
    std::string result;
    if (status == 0) {
        result = std::string("Off");
    }
    else if (status == 1) {
        result = std::string("On");
    }
    else if (status == 2) {
        result = std::string("Slow Blink");
    }
    else if (status == 3) {
        result = std::string("Fast Blink");
    }

    return result;
}