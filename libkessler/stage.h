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
#include "notifications/aux_input_status.h"
#include "notifications/error_status.h"
#include "notifications/motor_calibrated.h"
#include "notifications/motor_position.h"
#include "notifications/motor_status.h"
#include "notifications/playback_status.h"
#include "notifications/unsupported_message.h"
#include "responses/device_guid.h"
#include "responses/device_info.h"
#include "responses/led_status.h"
#include "responses/motor_info.h"
#include "responses/network_info.h"
#include "utils.hpp"

class Stage{
public:
    Stage(const std::string &host, int port);
    class DeviceInfo get_device_info();
    class MotorInfo get_motor_info();
    class DeviceGUID get_device_guid();
    class NetworkInfo get_network_info();
    class LEDStatus get_led_status();
    void set_user_password(const std::string& password);
    void set_device_password(const std::string& password);
    void set_led_status(boost::uint8_t master_status, boost::uint8_t slave_status);
    void handshake();
    void set_position_speed_acceleration(boost::uint8_t motor_address, float position, float speed, float acceleration);
    void reset_device();
    void reset_axis(boost::uint8_t motor_address);
    void mark_begin_position(boost::uint8_t motor_address);
    void mark_end_position(boost::uint8_t motor_address);
    void shutdown();
private:
    bool active = true;
    boost::asio::io_service service;
    boost::asio::ip::tcp::socket ConnectSocket;

    std::queue<class DeviceInfo> DeviceInfoQueue;
    std::queue<class Motor> MotorQueue;
    std::queue<class MotorInfo> MotorInfoQueue;
    std::queue<class DeviceGUID> DeviceGUIDQueue;
    std::queue<class NetworkInfo> NetworkInfoQueue;
    std::queue<class LEDStatus> LEDStatusQueue;
    std::queue<class AuxInputStatus> AuxInputStatusQueue;
    std::queue<class ErrorStatus> ErrorStatusQueue;
    std::queue<class MotorCalibrated> MotorCalibratedQueue;
    std::queue<class MotorPosition> MotorPositionQueue;
    std::queue<class MotorStatus> MotorStatusQueue;
    std::queue<class PlaybackStatus> PlaybackStatusQueue;
    std::queue<class UnsupportedMessage> UnsupportedMessageQueue;

    std::mutex event_mtx;
    std::mutex receive_mtx;
    std::unique_lock<std::mutex> lck;
    std::condition_variable cv;
    bool ready = false;
    void event_queue_manager();
    void on_receive_device_info_response();
    void on_receive_motor_info_response(boost::uint8_t message_type);
    void on_receive_device_guid_response();
    void on_receive_network_info_response();
    void on_receive_led_status_response();
    boost::uint8_t get_uint8();
    boost::uint16_t get_uint16();
    boost::uint32_t get_uint32();
    float get_float();
    std::string get_string(boost::uint16_t len);
    unsigned char* get_block(boost::uint16_t len);
};

#endif //LIBKESSLER_STAGE_H
