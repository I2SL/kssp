#pragma once

#ifndef LIBKESSLER_AUX_INPUT_STATUS_H
#define LIBKESSLER_AUX_INPUT_STATUS_H

#include <boost/endian/conversion.hpp>

class AuxInputStatus {
public:
    AuxInputStatus(boost::uint8_t status);
    boost::uint8_t status;
};


#endif //LIBKESSLER_AUX_INPUT_STATUS_H
