#pragma once

#ifndef LIBKESSLER_MOTOR_CALIBRATED_H
#define LIBKESSLER_MOTOR_CALIBRATED_H

#include <boost/endian/conversion.hpp>

class MotorCalibrated {
public:
    MotorCalibrated(boost::uint8_t motor_address, float end_position);
    boost::uint8_t motor_address;
    float end_position;
};


#endif //LIBKESSLER_MOTOR_CALIBRATED_H
