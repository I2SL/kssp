#pragma once

#ifndef LIBKESSLER_MOTOR_POSITION_H
#define LIBKESSLER_MOTOR_POSITION_H

#include <boost/endian/conversion.hpp>

class MotorPosition {
public:
    MotorPosition(boost::uint8_t motor_address, float position);
    boost::uint8_t motor_address;
    float position;
};


#endif //LIBKESSLER_MOTOR_POSITION_H
