#include "../../include/notifications/motor_calibrated.h"

MotorCalibrated::MotorCalibrated(boost::uint8_t motor_address, float begin_position, float end_position):
    motor_address(motor_address),
    begin_position(begin_position),
    end_position(end_position)
{}