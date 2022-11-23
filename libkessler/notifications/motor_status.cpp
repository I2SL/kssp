#include "motor_status.h"

MotorStatus::MotorStatus(boost::uint8_t motor_count, boost::uint8_t motor_addr, boost::uint8_t online, boost::uint8_t motor_type,
                         float max_max_setup_speed, float max_max_move_speed, float max_max_acceleration):
                         motor_count(motor_count),
                         motor_addr(motor_addr),
                         online(online),
                         motor_type(motor_type),
                         max_max_setup_speed(max_max_setup_speed),
                         max_max_move_speed(max_max_move_speed),
                         max_max_acceleration(max_max_acceleration)
                         {}