#pragma once

#ifndef KSSP_UNSUPPORTED_MESSAGE_H
#define KSSP_UNSUPPORTED_MESSAGE_H

#include <boost/endian/conversion.hpp>

class UnsupportedMessage {
public:
    explicit UnsupportedMessage(unsigned char* message);
    unsigned char* message;
};


#endif //KSSP_UNSUPPORTED_MESSAGE_H
