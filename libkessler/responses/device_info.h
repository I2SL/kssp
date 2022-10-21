#ifndef LIBKESSLER_DEVICE_INFO_H
#define LIBKESSLER_DEVICE_INFO_H

#include <fmt/format.h>
#include <string>

class DeviceInfo {
public:
    DeviceInfo(
            unsigned short device_type,
            unsigned short device_addr,
            unsigned short playback_mode,
            unsigned short playback_status,
            unsigned short firmware_version_major,
            unsigned short firmware_version_minor,
            unsigned short firmware_version_release,
            unsigned short firmware_version_build,
            unsigned short network_id,
            unsigned short hardware_id,
            std::string device_password,
            unsigned short aux_input_status,
            float delay_time_remaining,
            float elapsed_time
            );
    unsigned short device_type;
    unsigned short device_addr;
    unsigned short playback_mode;
    unsigned short playback_status;
    unsigned short firmware_version_major;
    unsigned short firmware_version_minor;
    unsigned short firmware_version_release;
    unsigned short firmware_version_build;
    unsigned short network_id;
    unsigned short hardware_id;
    std::string device_password;
    unsigned short aux_input_status;
    float delay_time_remaining;
    float elapsed_time;
    std::string to_string();
};


#endif //LIBKESSLER_DEVICE_INFO_H
