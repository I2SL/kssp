#pragma once

#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>

#include "consts.h"

class Utils {
public:
    static unsigned char* buffer_to_char_array(const boost::asio::streambuf &buffer) {
        auto* output = (unsigned char*)malloc(buffer.size());
        memcpy(output, boost::asio::buffer_cast<const void*>(buffer.data()), buffer.size());

        return output;
    }

    static float int32_to_float(const boost::int32_t input) {
        float output;
        std::memcpy(&output, &input, sizeof(input));

        return output;
    }

    static unsigned char* string_to_uchars(const std::string& input) {
        unsigned char* output;
        const char* chars = input.c_str();
        std::memcpy(&output, &chars, sizeof(chars));

        return output;
    }

    static std::vector<unsigned char> make_message_header(const boost::uint16_t message_length,
                                                          const boost::uint16_t message_id,
                                                          const boost::uint8_t message_type) {
        std::vector<unsigned char> header;
        unsigned char length_bytes[2];
        boost::endian::store_big_u16(length_bytes, message_length);
        unsigned char id_bytes[2];
        boost::endian::store_big_u16(id_bytes, message_id);
        header.push_back(SSP_PROTOCOL_VERSION);
        header.insert(std::end(header), std::begin(length_bytes), std::end(length_bytes));
        header.insert(std::end(header), std::begin(id_bytes), std::end(id_bytes));
        header.push_back(message_type);

        return header;
    }

    static std::vector<unsigned char> make_message(const boost::uint16_t message_length,
                                                   const boost::uint16_t message_id,
                                                   const boost::uint8_t message_type,
                                                   const std::vector<unsigned char> parameters) {
        std::vector<unsigned char> message = make_message_header(message_length, message_id, message_type);
        message.insert(std::end(message), std::begin(parameters), std::end(parameters));

        return message;
    }

    static std::string char_vector_to_string(const std::vector<unsigned char>& input) {
        std::string output(input.begin(), input.end());
        return output;
    }
};