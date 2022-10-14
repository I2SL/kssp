#ifndef LIBKESSLER_STAGE_H
#define LIBKESSLER_STAGE_H

#include <string>
#include <boost/asio.hpp>

class Stage{
public:
    Stage(const std::string& host, int port);
private:
    boost::asio::ip::tcp::socket* socket;
};
void hello();

#endif //LIBKESSLER_STAGE_H
