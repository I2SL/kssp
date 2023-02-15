#pragma once

#ifndef KESSLER_MOTOR_POSITION_H
#define KESSLER_MOTOR_POSITION_H

#include <boost/endian/conversion.hpp>

class MotorPosition {
public:
    MotorPosition(boost::uint8_t motor_address, float position);
    boost::uint8_t motor_address;
    float position;
};


#endif //KESSLER_MOTOR_POSITION_H
