#pragma once

#ifndef LIBKESSLER_UNSUPPORTED_MESSAGE_H
#define LIBKESSLER_UNSUPPORTED_MESSAGE_H

#include <boost/endian/conversion.hpp>

class UnsupportedMessage {
public:
    explicit UnsupportedMessage(unsigned char* message);
    unsigned char* message;
};


#endif //LIBKESSLER_UNSUPPORTED_MESSAGE_H
