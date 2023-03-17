#pragma once

#ifndef KSSP_LED_STATUS_H
#define KSSP_LED_STATUS_H

#include <fmt/format.h>
#include <string>

#include <boost/endian/conversion.hpp>

class LEDStatus {
public:
    LEDStatus(boost::uint8_t master_status, boost::uint8_t slave_status);
    boost::uint8_t master_status;
    boost::uint8_t slave_status;
    [[nodiscard]] std::string to_string() const;
private:
    static std::string status_to_string(boost::uint8_t status);
};
#endif //KSSP_LED_STATUS_H
