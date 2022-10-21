#ifndef LIBKESSLER_DEVICE_INFO_H
#define LIBKESSLER_DEVICE_INFO_H

#include <fmt/format.h>
#include <string>

class DeviceInfo {
public:
    DeviceInfo(
            unsigned char device_type,
            unsigned char device_addr,
            unsigned char playback_mode,
            unsigned char playback_status,
            unsigned char firmware_version_major,
            unsigned char firmware_version_minor,
            unsigned char firmware_version_release,
            unsigned char firmware_version_build,
            unsigned char network_id,
            unsigned char hardware_id,
            std::string device_password,
            unsigned char aux_input_status,
            float delay_time_remaining,
            float elapsed_time
            );
    unsigned char device_type;
    unsigned char device_addr;
    unsigned char playback_mode;
    unsigned char playback_status;
    unsigned char firmware_version_major;
    unsigned char firmware_version_minor;
    unsigned char firmware_version_release;
    unsigned char firmware_version_build;
    unsigned char network_id;
    unsigned char hardware_id;
    std::string device_password;
    unsigned char aux_input_status;
    float delay_time_remaining;
    float elapsed_time;
    std::string to_string();
};


#endif //LIBKESSLER_DEVICE_INFO_H
