#include "motor_position.h"

MotorPosition::MotorPosition(boost::uint8_t motor_address, float position):
    motor_address(motor_address),
    position(position)
{}
