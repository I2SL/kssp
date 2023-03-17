#pragma once

#ifndef KSSP_MOTOR_STATUS_H
#define KSSP_MOTOR_STATUS_H

#include <boost/endian/conversion.hpp>

class MotorStatus {
public:
    MotorStatus(boost::uint8_t motor_count,
                boost::uint8_t motor_addr,
                boost::uint8_t online,
                boost::uint8_t motor_type,
                float max_max_setup_speed,
                float max_max_move_speed,
                float max_max_acceleration
                );
    boost::uint8_t motor_count;
    boost::uint8_t motor_addr;
    boost::uint8_t online;
    boost::uint8_t motor_type;
    float max_max_setup_speed;
    float max_max_move_speed;
    float max_max_acceleration;
};


#endif //KSSP_MOTOR_STATUS_H
