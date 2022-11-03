#include "error_status.h"

ErrorStatus::ErrorStatus(boost::uint8_t error_code, boost::uint8_t motor_address):
    error_code(error_code),
    motor_address(motor_address)
{}