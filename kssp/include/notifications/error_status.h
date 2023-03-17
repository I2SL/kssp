#pragma once

#ifndef KSSP_ERROR_STATUS_H
#define KSSP_ERROR_STATUS_H

#include <boost/endian/conversion.hpp>

class ErrorStatus {
public:
    ErrorStatus(boost::uint8_t error_code, boost::uint8_t motor_address);
    boost::uint8_t error_code;
    boost::uint8_t motor_address;
};

#endif //KSSP_ERROR_STATUS_H