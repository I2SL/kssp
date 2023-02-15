#pragma once

#ifndef KESSLER_UNSUPPORTED_MESSAGE_H
#define KESSLER_UNSUPPORTED_MESSAGE_H

#include <boost/endian/conversion.hpp>

class UnsupportedMessage {
public:
    explicit UnsupportedMessage(unsigned char* message);
    unsigned char* message;
};


#endif //KESSLER_UNSUPPORTED_MESSAGE_H
