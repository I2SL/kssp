#ifndef LIBKESSLER_MOTOR_INFO_H
#define LIBKESSLER_MOTOR_INFO_H

#include <fmt/format.h>
#include <string>
#include <vector>

#include <boost/endian/conversion.hpp>

class Motor {
public:
    Motor(boost::uint8_t motor_count,
          boost::uint8_t motor_address,
          float position,
          float end_position,
          float max_max_setup_speed,
          float max_max_move_speed,
          float max_max_acceleration);
    boost::uint8_t motor_count;
    boost::uint8_t motor_address;
    float position;
    float end_position;
    float max_max_setup_speed;
    float max_max_move_speed;
    float max_max_acceleration;
    std::string to_string();
};

class MotorInfo {
public:
    explicit MotorInfo(std::vector<Motor> motors);
    std::vector<Motor> motors;
    std::string to_string();
};

#endif //LIBKESSLER_MOTOR_INFO_H
