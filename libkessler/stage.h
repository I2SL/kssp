#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#pragma once

#ifndef LIBKESSLER_STAGE_H
#define LIBKESSLER_STAGE_H

#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
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
#include "utils.hpp"

class Stage{
public:
    Stage(const std::string &host, int port);
    class DeviceInfo get_device_info();
    class MotorInfo get_motor_info();
    class DeviceGUID get_device_guid();
    void set_user_password(const std::string& password);
    void set_device_password(const std::string& password);
    void handshake();
    void set_position_speed_acceleration(boost::uint8_t motor_address, float position, float speed, float acceleration);
    void shutdown();
private:
    boost::asio::io_service service;
    boost::asio::ip::tcp::socket ConnectSocket;
    std::queue<class DeviceInfo> DeviceInfoQueue;
    std::queue<class Motor> MotorQueue;
    std::queue<class MotorInfo> MotorInfoQueue;
    std::queue<class DeviceGUID> DeviceGUIDQueue;
    std::mutex event_mtx;
    std::mutex receive_mtx;
    std::unique_lock<std::mutex> lck;
    std::condition_variable cv;
    bool ready = false;
    void event_queue_manager();
    void on_receive_device_info_response();
    void on_receive_motor_info_response(boost::uint8_t message_type);
    void on_receive_device_guid_response();
    boost::uint8_t get_uint8();
    boost::uint16_t get_uint16();
    float get_float();
    std::string get_string(boost::uint16_t len);
    unsigned char* get_block(boost::uint16_t len);
};

#endif //LIBKESSLER_STAGE_H
