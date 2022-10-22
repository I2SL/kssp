#include "device_info.h"

DeviceInfo::DeviceInfo(
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
        ):
    device_type(device_type),
    device_addr(device_addr),
    playback_mode(playback_mode),
    playback_status(playback_status),
    firmware_version_major(firmware_version_major),
    firmware_version_minor(firmware_version_minor),
    firmware_version_release(firmware_version_release),
    firmware_version_build(firmware_version_build),
    network_id(network_id),
    hardware_id(hardware_id),
    device_password(device_password),
    aux_input_status(aux_input_status),
    delay_time_remaining(delay_time_remaining),
    elapsed_time(elapsed_time)
{

}

std::string DeviceInfo::to_string() {
    std::string result = std::string("DEVICE INFO:\n");
    result += std::string("------------\n");
    result += fmt::format("Device Type: {}\n", device_type.value());
    result += fmt::format("Device Address: {}\n", device_addr.value());
    result += fmt::format("Playback Mode: {}\n", playback_mode.value());
    result += fmt::format("Playback Status: {}\n", playback_status.value());
    result += fmt::format("Firmware Version Major: {}\n", firmware_version_major.value());
    result += fmt::format("Firmware Version Minor: {}\n", firmware_version_minor.value());
    result += fmt::format("Firmware Version Release: {}\n", firmware_version_release.value());
    result += fmt::format("Firmware Version Build: {}\n", firmware_version_build.value());
    result += fmt::format("Network ID: {}\n", network_id.value());
    result += fmt::format("Hardware ID: {}\n", hardware_id.value());
    result += fmt::format("Device Password: {}\n", device_password);
    result += fmt::format("Aux Input Status: {}\n", aux_input_status.value());
    result += fmt::format("Delay Time Remaining: {}\n", delay_time_remaining.value());
    result += fmt::format("Elapsed Time: {}\n", elapsed_time.value());
    return result;
}