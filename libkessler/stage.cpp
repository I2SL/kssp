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
    //for (const auto messageid: AllMessageIDs) {
    //    events.emplace(messageid, std::queue<std::string>());
    //}
    //std::queue<class DeviceInfo> line;
    //events.emplace(MessageID::DeviceInfo, line);
    //events.insert(std::pair<const unsigned int, std::queue<class DeviceInfo>>(MessageID::DeviceInfo, std::queue<class DeviceInfo>()));
    //std::pair<const unsigned int, std::queue<class DeviceInfo>>

    boost::async([this]() {
        this->event_queue_manager();
    });
}

class DeviceInfo Stage::get_device_info() {
    char msg[6];
    msg[0] = SSP_PROTOCOL_VERSION;
    msg[1] = 0x00;
    msg[2] = SSP_GET_MESSAGE_SIZE;
    msg[3] = 0x00;
    msg[4] = MessageID::DeviceInfo;
    msg[5] = MessageType::Get;
    ConnectSocket.send(boost::asio::buffer(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    class DeviceInfo response = DeviceInfoQueue.front();
    DeviceInfoQueue.pop();
    return response;
}

void Stage::event_queue_manager() {
    while (true) {
        try {
            char message_lead;
            boost::asio::streambuf sb1;
            boost::asio::streambuf sb2;
            boost::asio::streambuf sb3;
            boost::asio::streambuf sb4;
            boost::system::error_code ec;
            boost::asio::read(ConnectSocket, sb1, boost::asio::transfer_exactly(1), ec);
            //std::sscanf(Utils::buffer_to_char_array(sb1), "%c", &message_lead);
            message_lead = Utils::buffer_to_string(sb1)[0];

            if (message_lead == SSP_PROTOCOL_VERSION) {
                printf("Got a header\n");
                unsigned short int message_length;
                unsigned short int message_id;
                char message_type;
                boost::asio::read(ConnectSocket, sb2, boost::asio::transfer_exactly(2), ec);
                //std::string len_response = Utils::buffer_to_string(sb2);
                //printf("Length Response Elements: (%hd, %hd)\n", len_response[0], len_response[1]);
                std::sscanf(Utils::buffer_to_char_array(sb2), "%hd", &message_length);
                boost::asio::read(ConnectSocket, sb3, boost::asio::transfer_exactly(2), ec);
                //std::string id_response = Utils::buffer_to_string(sb3);
                //printf("ID Response Elements: (%hd, %hd)\n", id_response[0], id_response[1]);
                //std::sscanf(id_response, "%hd", &message_id);
                std::sscanf(Utils::buffer_to_char_array(sb3), "%hd", &message_id);
                boost::asio::read(ConnectSocket, sb4, boost::asio::transfer_exactly(1), ec);
                std::sscanf(Utils::buffer_to_char_array(sb4), "%c", &message_type);
                printf("Protocol: %hd\n", (short)message_lead);
                printf("Length: %hd\n", message_length);
                printf("ID: %hd\n", message_id);
                printf("Type: %hd\n", (short)message_type);

                if (message_type == (char)MessageType::Response) {
                    if (message_id == (unsigned short int)MessageID::DeviceInfo) {
                        on_receive_device_info_response();
                    }
                }

            } else {
                std::cout << "Received unhandled byte: '" << &sb1 << "'\n";
            }

            if (ec) {
                std::cout << "status: " << ec.message() << "\n";
                break;
            }
        }
        catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
}

void Stage::on_receive_device_info_response() {
    boost::endian::big_uint8_buf_t device_type;
    boost::endian::big_uint8_buf_t device_addr;
    boost::endian::big_uint8_buf_t playback_mode;
    boost::endian::big_uint8_buf_t playback_status;
    boost::endian::big_uint8_buf_t firmware_version_major;
    boost::endian::big_uint8_buf_t firmware_version_minor;
    boost::endian::big_uint8_buf_t firmware_version_release;
    boost::endian::big_uint8_buf_t firmware_version_build;
    boost::endian::big_uint8_buf_t network_id;
    boost::endian::big_uint8_buf_t hardware_id;
    std::string device_password;
    boost::endian::big_uint8_buf_t aux_input_status;
    boost::endian::big_float32_buf_t delay_time_remaining;
    boost::endian::big_float32_buf_t elapsed_time;
    boost::endian::big_uint16_buf_t string_length;

    boost::asio::streambuf sb1;
    boost::asio::streambuf sb2;
    boost::asio::streambuf sb3;
    boost::asio::streambuf sb4;
    boost::system::error_code ec;
    boost::asio::read(ConnectSocket, sb1, boost::asio::transfer_exactly(1), ec);
    //FINISH
    device_type = Utils::buffer_to_big_uint8(sb1);
    //device_type = boost::endian::endian_load<>();
    //device_type = reinterpret_cast<boost::endian::big_uint8_buf_t>(&sb1);
    boost::asio::read(ConnectSocket, sb2, boost::asio::transfer_exactly(2), ec);
    std::sscanf(Utils::buffer_to_char_array(sb2), "%hd", &string_length);
    if (string_length.value() > 0) {
        boost::asio::read(ConnectSocket, sb3, boost::asio::transfer_exactly(string_length.value()), ec);
        std::sscanf(Utils::buffer_to_char_array(sb3), "%s", &device_password);
    }
    else {
        device_password = std::string("");
    }
    boost::asio::read(ConnectSocket, sb4, boost::asio::transfer_exactly(9), ec);
    std::sscanf(Utils::buffer_to_char_array(sb4), "%c%f%f", &aux_input_status, &delay_time_remaining, &elapsed_time);

    class DeviceInfo response(
            device_type,
            device_addr,
            playback_mode,
            playback_status,
            firmware_version_major,
            firmware_version_minor,
            firmware_version_release,
            firmware_version_build,
            network_id,
            hardware_id,
            device_password,
            aux_input_status,
            delay_time_remaining,
            elapsed_time
            );
    DeviceInfoQueue.push(response);
}




