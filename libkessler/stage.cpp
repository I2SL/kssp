#include "stage.h"

//def event_queue_manager(self) -> None:
//message_lead = struct.pack(">B", 0x04)
//while True:
//try:
//protocol_version = self.socket.recv(1)
//if protocol_version == message_lead:
//message_length = bytes_to_int(self.socket.recv(2))
//message_id = bytes_to_int(self.socket.recv(2))
//message_type = bytes_to_int(self.socket.recv(1))
//if message_type == MessageType.Response.value:
//if message_id == MessageID.MotorInfo.value:
//self.on_receive_motor_info_response()
//elif message_id == MessageID.DeviceInfo.value:
//self.on_receive_device_info_response()
//elif message_id == MessageID.DeviceGUID.value:
//self.on_receive_device_guid()
//else:
//warn(f"Unhandled MsgID {hex(message_id)}")
//except ConnectionResetError:
//# Problematic
//break
Stage::Stage(const std::string &host, const int port) {
    auto address = boost::asio::ip::address::from_string(host);
    boost::asio::ip::tcp::endpoint endpoint(address, port);
    boost::asio::io_service service;
    this->socket = new boost::asio::ip::tcp::socket(service, endpoint.protocol());
    this->socket->connect(endpoint);

    this->events = new std::map<unsigned int, std::queue<std::string>>();
    for (const auto messageid: AllMessageIDs) {
        this->events->emplace(messageid, std::queue<std::string>());
    }
}

