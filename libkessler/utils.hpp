#include <boost/asio.hpp>
#include <boost/endian/buffers.hpp>

class Utils {
public:
    static unsigned char* buffer_to_char_array(const boost::asio::streambuf &buffer) {
        auto* output = (unsigned char*)malloc(buffer.size());
        memcpy(output, boost::asio::buffer_cast<const void*>(buffer.data()), buffer.size());;

        //auto* output = (unsigned char*)malloc(buffer.size());
        //buffer.sgetn(reinterpret_cast<char *>(output), buffer.size());
        //std::string output_str = buffer_to_string(buffer);
        //const char* output = output_str.c_str();

        //unsigned char p[output_str.length()];

        //int i;
        //for (i = 0; i < sizeof(p); i++) {
            //p[i] = output_str[i];
            //std::cout << p[i];
        //}
        //static unsigned char* output = p;

        return output;
    }

    static std::string buffer_to_string(const boost::asio::streambuf &buffer) {
        using boost::asio::buffers_begin;

        auto bufs = buffer.data();
        std::string result(buffers_begin(bufs), buffers_begin(bufs) + buffer.size());

        return result;
    }

    static float int32_to_float(boost::int32_t input) {
        float output = reinterpret_cast <float&>(input);
        return output;
    }
};