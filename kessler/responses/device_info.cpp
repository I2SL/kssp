#include "device_info.h"

DeviceInfo::DeviceInfo(
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
    elapsed_time(elapsed_time),
    photo_count(photo_count)
{

}

std::string DeviceInfo::to_string() {
    std::string result = std::string("DEVICE INFO:\n");
    result += std::string("------------\n");
    result += fmt::format("Device Type: {}\n", device_type);
    result += fmt::format("Device Address: {}\n", device_addr);
    result += fmt::format("Playback Mode: {}\n", playback_mode);
    result += fmt::format("Playback Status: {}\n", playback_status);
    result += fmt::format("Firmware Version Major: {}\n", firmware_version_major);
    result += fmt::format("Firmware Version Minor: {}\n", firmware_version_minor);
    result += fmt::format("Firmware Version Release: {}\n", firmware_version_release);
    result += fmt::format("Firmware Version Build: {}\n", firmware_version_build);
    result += fmt::format("Network ID: {}\n", network_id);
    result += fmt::format("Hardware ID: {}\n", hardware_id);
    result += fmt::format("Device Password: {}\n", device_password);
    result += fmt::format("Aux Input Status: {}\n", aux_input_status);
    result += fmt::format("Delay Time Remaining: {}\n", delay_time_remaining);
    result += fmt::format("Elapsed Time: {}\n", elapsed_time);
    result += fmt::format("Photo Count: {}\n", photo_count);
    return result;
}