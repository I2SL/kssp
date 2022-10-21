char* buffer_to_char_array(const boost::asio::streambuf &buffer)
{
    using boost::asio::buffers_begin;

    auto bufs = buffer.data();
    std::string result_str(buffers_begin(bufs), buffers_begin(bufs) + buffer.size());

    char* result = strcpy(new char[result_str.length() + 1], result_str.c_str());
    return result;
}