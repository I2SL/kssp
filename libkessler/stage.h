#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#ifndef LIBKESSLER_STAGE_H
#define LIBKESSLER_STAGE_H

#include <iomanip>
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
    class DeviceInfo get_device_info();
private:
    void event_queue_manager();
    void on_receive_device_info_response();
    boost::asio::io_service service;
    boost::asio::ip::tcp::socket ConnectSocket;
    //::map<unsigned int, std::queue<std::string>> events = std::map<unsigned int, std::queue<std::string>>();
    std::queue<class DeviceInfo> DeviceInfoQueue;
};

#endif //LIBKESSLER_STAGE_H
