#pragma once

#ifndef KESSLER_DEVICE_GUID_H
#define KESSLER_DEVICE_GUID_H

#include <cstring>
#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

class DeviceGUID {
public:
    DeviceGUID(boost::uint16_t block_length, unsigned char* block_data);
    boost::uuids::uuid device_guid{};
    [[nodiscard]] std::string to_string() const;
};

#endif //KESSLER_DEVICE_GUID_H
