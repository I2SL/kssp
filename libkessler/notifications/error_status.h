#pragma once

#ifndef LIBKESSLER_ERROR_STATUS_H
#define LIBKESSLER_ERROR_STATUS_H

#include <boost/endian/conversion.hpp>

class ErrorStatus {
public:
    ErrorStatus(boost::uint8_t error_code, boost::uint8_t motor_address);
    boost::uint8_t error_code;
    boost::uint8_t motor_address;
};


#endif //LIBKESSLER_ERROR_STATUS_H
