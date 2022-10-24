#include <boost/asio.hpp>

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
};