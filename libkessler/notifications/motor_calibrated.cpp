#include "motor_calibrated.h"

MotorCalibrated::MotorCalibrated(boost::uint8_t motor_address, float end_position):
    motor_address(motor_address),
    end_position(end_position)
{}