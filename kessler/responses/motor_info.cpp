#include "motor_info.h"

#include <utility>

Motor::Motor(
            boost::uint8_t motor_count,
            boost::uint8_t motor_address,
            float position,
            float begin_position,
            float end_position,
            float max_max_setup_speed,
            float max_max_move_speed,
            float max_max_acceleration
        ):
        motor_count(motor_count),
        motor_address(motor_address),
        position(position),
        begin_position(begin_position),
        end_position(end_position),
        max_max_setup_speed(max_max_setup_speed),
        max_max_move_speed(max_max_move_speed),
        max_max_acceleration(max_max_acceleration)
{

}

std::string Motor::to_string() {
    std::string result = fmt::format("MOTOR {} INFO:\n", motor_address);
    result += std::string("-------------\n");
    result += fmt::format("Motor Count: {}\n", motor_count);
    result += fmt::format("Motor Address: {}\n", motor_address);
    result += fmt::format("Position: {}\n", position);
    result += fmt::format("Begin Position: {}\n", begin_position);
    result += fmt::format("End Position: {}\n", end_position);
    result += fmt::format("Max Setup Speed: {}\n", max_max_setup_speed);
    result += fmt::format("Max Move Speed: {}\n", max_max_move_speed);
    result += fmt::format("Max Acceleration: {}\n", max_max_acceleration);

    return result;
}

MotorInfo::MotorInfo(std::vector<Motor> motors):
    motors(std::move(motors))
{

}

std::string MotorInfo::to_string() {
    std::string result = std::string("");
    for (Motor motor : motors) {
        result += motor.to_string();
    }

    return result;
}