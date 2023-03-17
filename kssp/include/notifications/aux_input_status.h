#pragma once

#ifndef KSSP_AUX_INPUT_STATUS_H
#define KSSP_AUX_INPUT_STATUS_H

#include <boost/endian/conversion.hpp>

class AuxInputStatus {
public:
    explicit AuxInputStatus(boost::uint8_t status);
    boost::uint8_t status;
};

#endif //KSSP_AUX_INPUT_STATUS_H