#pragma once

#ifndef KSSP_MOTOR_POSITION_H
#define KSSP_MOTOR_POSITION_H

#include <boost/endian/conversion.hpp>

class MotorPosition {
public:
    MotorPosition(boost::uint8_t motor_address, float position);
    boost::uint8_t motor_address;
    float position;
};


#endif //KSSP_MOTOR_POSITION_H
