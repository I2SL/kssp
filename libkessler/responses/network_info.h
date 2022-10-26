#pragma once

#ifndef LIBKESSLER_NETWORK_INFO_H
#define LIBKESSLER_NETWORK_INFO_H

#include <fmt/format.h>
#include <string>

#include <boost/endian/conversion.hpp>

class NetworkInfo {
public:
    NetworkInfo(
            boost::uint8_t DHCP,
            unsigned char* mac_address,
            boost::uint32_t ip_address,
            boost::uint32_t subnet_mask,
            boost::uint32_t gateway,
            boost::uint8_t device_type,
            boost::uint8_t device_addr
            );
    boost::uint8_t DHCP;
    unsigned char* mac_address;
    boost::uint32_t ip_address;
    boost::uint32_t subnet_mask;
    boost::uint32_t gateway;
    boost::uint8_t device_type;
    boost::uint8_t device_addr;
    std::string to_string();
private:
    char* mac_str;
};

#endif //LIBKESSLER_NETWORK_INFO_H
