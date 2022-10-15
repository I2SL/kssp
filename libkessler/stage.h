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
#include "responses/device_guid.h"
#include "responses/device_info.h"
#include "responses/motor_info.h"

class Stage{
public:
    Stage(const std::string &host, int port);
    void get(MessageID messageid) {
        getmessageidbytes = (char *) messageid;
        getmessage[0] = SSP_PROTOCOL_VERSION;
        getmessage[1] = getmessagelengthbytes[1];
        getmessage[2] = getmessagelengthbytes[0];
        getmessage[3] = getmessageidbytes[1];
        getmessage[4] = getmessageidbytes[0];
        getmessage[5] = MessageType::Get;
        socket->send(boost::asio::buffer(getmessage));
    };
private:
    char getmessage[SSP_GET_MESSAGE_SIZE];
    char *getmessageidbytes;
    char *getmessagelengthbytes = (char *) SSP_GET_MESSAGE_SIZE;
    boost::asio::ip::tcp::socket* socket;
    std::map<unsigned int, std::queue<std::string>>* events;
};

#endif //LIBKESSLER_STAGE_H
