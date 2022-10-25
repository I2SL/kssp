#pragma once

#ifndef LIBKESSLER_DEVICE_GUID_H
#define LIBKESSLER_DEVICE_GUID_H

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

class DeviceGUID {
public:
    DeviceGUID(boost::uint16_t block_length, unsigned char* block_data);
    boost::uuids::uuid device_guid{};
    std::string to_string() const;
};

#endif //LIBKESSLER_DEVICE_GUID_H
