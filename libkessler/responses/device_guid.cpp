#include "device_guid.h"

DeviceGUID::DeviceGUID(boost::uint16_t block_length, unsigned char* block_data) {
    memcpy(&device_guid, block_data, block_length);
}

std::string DeviceGUID::to_string() const {
    std::string result = std::string("Device GUID: ");
    result +=  boost::uuids::to_string(device_guid);
    result += "\n";

    return result;
}