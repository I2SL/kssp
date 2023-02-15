#pragma once

#ifndef KESSLER_AUX_INPUT_STATUS_H
#define KESSLER_AUX_INPUT_STATUS_H

#include <boost/endian/conversion.hpp>

class AuxInputStatus {
public:
    explicit AuxInputStatus(boost::uint8_t status);
    boost::uint8_t status;
};


#endif //KESSLER_AUX_INPUT_STATUS_H
