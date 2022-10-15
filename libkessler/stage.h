#ifndef LIBKESSLER_STAGE_H
#define LIBKESSLER_STAGE_H

#include <iostream>
#include <map>
#include <queue>
#include <string>

#include <boost/asio.hpp>
#include <fmt/format.h>

#include "consts.h"
#include "enums/actionid.h"
#include "enums/messageid.h"
#include "enums/messagetype.h"
#include "enums/noteid.h"

class Stage{
public:
    Stage(const std::string& host, int port);
private:
    boost::asio::ip::tcp::socket* socket;
    std::map<unsigned int, std::queue<std::string>>* events;
};

#endif //LIBKESSLER_STAGE_H
