#include <thread>
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
    this->socket.connect(endpoint);
    for (const auto messageid: AllMessageIDs) {
        this->events->emplace(messageid, std::queue<std::string>());
    }
}

void Stage::get(MessageID messageid) {
    std::vector<char> msg = std::vector<char>(6);
    msg[0] = SSP_PROTOCOL_VERSION;
    msg[1] = 0x00;
    msg[2] = SSP_GET_MESSAGE_SIZE;
    msg[3] = 0x00;
    msg[4] = messageid;
    msg[5] = MessageType::Get;
    this->socket.send(boost::asio::buffer(msg));
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Stage::event_queue_manager() {
    while (true) {
        try {
            boost::asio::streambuf sb;
            boost::system::error_code ec;
            while (boost::asio::read(socket, sb, ec)) {
                std::cout << "received: '" << &sb << "'\n";

                if (ec) {
                    std::cout << "status: " << ec.message() << "\n";
                    break;
                }
            }
        }
        catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
}

