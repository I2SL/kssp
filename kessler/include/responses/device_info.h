#pragma once

#ifndef KESSLER_DEVICE_INFO_H
#define KESSLER_DEVICE_INFO_H

#include <fmt/format.h>
#include <string>

#include <boost/endian/conversion.hpp>

class DeviceInfo {
public:
    DeviceInfo(
            boost::uint8_t device_type,
            boost::uint8_t device_addr,
            boost::uint8_t playback_mode,
            boost::uint8_t playback_status,
            boost::uint8_t firmware_version_major,
            boost::uint8_t firmware_version_minor,
            boost::uint8_t firmware_version_release,
            boost::uint8_t firmware_version_build,
            boost::uint8_t network_id,
            boost::uint8_t hardware_id,
            std::string device_password,
            boost::uint8_t aux_input_status,
            float delay_time_remaining,
            float elapsed_time,
            boost::uint32_t photo_count
            );
    boost::uint8_t device_type;
    boost::uint8_t device_addr;
    boost::uint8_t playback_mode;
    boost::uint8_t playback_status;
    boost::uint8_t firmware_version_major;
    boost::uint8_t firmware_version_minor;
    boost::uint8_t firmware_version_release;
    boost::uint8_t firmware_version_build;
    boost::uint8_t network_id;
    boost::uint8_t hardware_id;
    std::string device_password;
    boost::uint8_t aux_input_status;
    float delay_time_remaining;
    float elapsed_time;
    boost::uint32_t photo_count;
    std::string to_string();
};


#endif //KESSLER_DEVICE_INFO_H
