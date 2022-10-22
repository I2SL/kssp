#include <boost/asio.hpp>
#include <boost/endian/buffers.hpp>

class Utils {
public:
    static char *buffer_to_char_array(const boost::asio::streambuf &buffer) {
        using boost::asio::buffers_begin;

        auto bufs = buffer.data();
        std::string result_str(buffers_begin(bufs), buffers_begin(bufs) + buffer.size());

        char *result = strcpy(new char[result_str.length() + 1], result_str.c_str());
        return result;
    }

    static std::string buffer_to_string(const boost::asio::streambuf &buffer) {
        using boost::asio::buffers_begin;

        auto bufs = buffer.data();
        std::string result(buffers_begin(bufs), buffers_begin(bufs) + buffer.size());

        return result;
    }

    static boost::endian::big_uint8_buf_t buffer_to_big_uint8(const boost::asio::streambuf &buffer) {
        using boost::asio::buffers_begin;

        auto bufs = buffer.data();
        boost::endian::big_uint8_buf_t result(1);

        return result;
    }
};