#pragma once

#ifndef KSSP_DEVICE_GUID_H
#define KSSP_DEVICE_GUID_H

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

#endif //KSSP_DEVICE_GUID_H
