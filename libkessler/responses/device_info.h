#ifndef LIBKESSLER_DEVICE_INFO_H
#define LIBKESSLER_DEVICE_INFO_H

#include <fmt/format.h>
#include <string>

#include <boost/endian/buffers.hpp>

class DeviceInfo {
public:
    DeviceInfo(
            boost::endian::big_uint8_buf_t device_type,
            boost::endian::big_uint8_buf_t device_addr,
            boost::endian::big_uint8_buf_t playback_mode,
            boost::endian::big_uint8_buf_t playback_status,
            boost::endian::big_uint8_buf_t firmware_version_major,
            boost::endian::big_uint8_buf_t firmware_version_minor,
            boost::endian::big_uint8_buf_t firmware_version_release,
            boost::endian::big_uint8_buf_t firmware_version_build,
            boost::endian::big_uint8_buf_t network_id,
            boost::endian::big_uint8_buf_t hardware_id,
            std::string device_password,
            boost::endian::big_uint8_buf_t aux_input_status,
            boost::endian::big_float32_buf_t delay_time_remaining,
            boost::endian::big_float32_buf_t elapsed_time
            );
    boost::endian::big_uint8_buf_t device_type;
    boost::endian::big_uint8_buf_t device_addr;
    boost::endian::big_uint8_buf_t playback_mode;
    boost::endian::big_uint8_buf_t playback_status;
    boost::endian::big_uint8_buf_t firmware_version_major;
    boost::endian::big_uint8_buf_t firmware_version_minor;
    boost::endian::big_uint8_buf_t firmware_version_release;
    boost::endian::big_uint8_buf_t firmware_version_build;
    boost::endian::big_uint8_buf_t network_id;
    boost::endian::big_uint8_buf_t hardware_id;
    std::string device_password;
    boost::endian::big_uint8_buf_t aux_input_status;
    boost::endian::big_float32_buf_t delay_time_remaining;
    boost::endian::big_float32_buf_t elapsed_time;
    std::string to_string();
};


#endif //LIBKESSLER_DEVICE_INFO_H
