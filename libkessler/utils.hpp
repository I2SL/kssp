#include <boost/asio.hpp>
#include <boost/endian/buffers.hpp>

class Utils {
public:
    static unsigned char* buffer_to_char_array(const boost::asio::streambuf &buffer) {
        auto* output = (unsigned char*)malloc(buffer.size());
        memcpy(output, boost::asio::buffer_cast<const void*>(buffer.data()), buffer.size());

        return output;
    }

    static std::string buffer_to_string(const boost::asio::streambuf &buffer) {
        using boost::asio::buffers_begin;

        auto bufs = buffer.data();
        std::string result(buffers_begin(bufs), buffers_begin(bufs) + buffer.size());

        return result;
    }

    static float int32_to_float(const boost::int32_t input) {
        float output;
        std::memcpy(&output, &input, sizeof(input));

        return output;
    }
};