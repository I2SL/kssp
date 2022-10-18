#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#ifndef LIBKESSLER_STAGE_H
#define LIBKESSLER_STAGE_H

#include <iostream>
#include <map>
#include <queue>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread/future.hpp>
#include <fmt/format.h>

#include "consts.h"
#include "enums/actionid.h"
#include "enums/messageid.h"
#include "enums/messagetype.h"
#include "enums/noteid.h"
#include "responses/device_guid.h"
#include "responses/device_info.h"
#include "responses/motor_info.h"

class Stage{
public:
    Stage(const std::string &host, int port);
    void get(MessageID messageid);
private:
    /**
     * Receive bytes from the socket and parse them into event objects.
     */
    void event_queue_manager();

    // Put this in a constructor eventually
    boost::asio::ip::address address = boost::asio::ip::address::from_string("127.0.0.1");
    boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(address, 5555);
    boost::asio::io_service service;

    boost::asio::ip::tcp::socket socket = boost::asio::ip::tcp::socket(service, endpoint.protocol());
    std::map<unsigned int, std::queue<std::string>> events = std::map<unsigned int, std::queue<std::string>>();
};

#endif //LIBKESSLER_STAGE_H
