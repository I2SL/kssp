#pragma once

#ifndef KSSP_MOTOR_CALIBRATED_H
#define KSSP_MOTOR_CALIBRATED_H

#include <boost/endian/conversion.hpp>

class MotorCalibrated {
public:
    MotorCalibrated(boost::uint8_t motor_address, float begin_position, float end_position);
    boost::uint8_t motor_address;
    float begin_position;
    float end_position;
};


#endif //KSSP_MOTOR_CALIBRATED_H
