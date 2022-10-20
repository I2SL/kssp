#include <thread>
#include "stage.h"

Stage::Stage(const std::string &host, const int port)
    : ConnectSocket(
            service,
            boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string(host),
                    port
                    ).protocol()
            )
{
    boost::asio::ip::address address = boost::asio::ip::address::from_string(host);
    boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(address, port);
    ConnectSocket.connect(endpoint);
    for (const auto messageid: AllMessageIDs) {
        this->events.emplace(messageid, std::queue<std::string>());
    }

    boost::async([this]() {
        this->event_queue_manager();
    });
}

void Stage::get(MessageID messageid) {
    std::vector<char> msg = std::vector<char>(6);
    msg[0] = SSP_PROTOCOL_VERSION;
    msg[1] = 0x00;
    msg[2] = SSP_GET_MESSAGE_SIZE;
    msg[3] = 0x00;
    msg[4] = messageid;
    msg[5] = MessageType::Get;
    ConnectSocket.send(boost::asio::buffer(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Stage::event_queue_manager() {
    while (true) {
        try {
            boost::asio::streambuf sb;
            boost::system::error_code ec;
            while (boost::asio::read(ConnectSocket, sb,boost::asio::transfer_exactly(1), ec)) {
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



