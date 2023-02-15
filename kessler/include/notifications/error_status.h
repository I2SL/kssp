#pragma once

#ifndef KESSLER_ERROR_STATUS_H
#define KESSLER_ERROR_STATUS_H

#include <boost/endian/conversion.hpp>

class ErrorStatus {
public:
    ErrorStatus(boost::uint8_t error_code, boost::uint8_t motor_address);
    boost::uint8_t error_code;
    boost::uint8_t motor_address;
};


#endif //KESSLER_ERROR_STATUS_H
