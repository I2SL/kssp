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
            boost::uint8_t message_lead;
            boost::asio::streambuf sb;
            boost::system::error_code ec;

            boost::asio::mutable_buffers_1 bufs = sb.prepare(1);
            size_t n = ConnectSocket.receive(bufs);
            sb.commit(n);
            message_lead = Utils::buffer_to_char_array(sb)[0];
            sb.consume(n);

            if (message_lead == SSP_PROTOCOL_VERSION) {
                printf("Got a header\n");
                boost::uint16_t message_length;
                boost::uint16_t message_id;
                boost::uint8_t message_type;

                bufs = sb.prepare(2);
                n = ConnectSocket.receive(bufs);
                sb.commit(n);
                message_length = boost::endian::load_big_u16(
                        Utils::buffer_to_char_array(sb)
                        );
                sb.consume(n);

                bufs = sb.prepare(2);
                n = ConnectSocket.receive(bufs);
                sb.commit(n);
                message_id = boost::endian::load_big_u16(
                        Utils::buffer_to_char_array(sb)
                );
                sb.consume(n);

                bufs = sb.prepare(1);
                n = ConnectSocket.receive(bufs);
                sb.commit(n);
                message_type = Utils::buffer_to_char_array(sb)[0];
                sb.consume(n);

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
                std::cout << "Received unhandled byte: '" << &sb << "'\n";
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
    boost::uint8_t device_type;
    boost::uint8_t device_addr;
    boost::uint8_t playback_mode;
    boost::uint8_t playback_status;
    boost::uint8_t firmware_version_major;
    boost::uint8_t firmware_version_minor;
    boost::uint8_t firmware_version_release;
    boost::uint8_t firmware_version_build;
    boost::uint8_t network_id;
    boost::uint8_t hardware_id;
    std::string device_password;
    boost::uint8_t aux_input_status;
    float delay_time_remaining;
    float elapsed_time;
    boost::uint16_t string_length;

    boost::asio::streambuf sb;
    boost::asio::mutable_buffers_1 bufs = sb.prepare(10);
    size_t n = ConnectSocket.receive(bufs);
    sb.commit(n);
    unsigned char* data = Utils::buffer_to_char_array(sb);
    sb.consume(n);
    device_type = data[0];
    device_addr = data[1];
    playback_mode = data[2];
    playback_status = data[3];
    firmware_version_major = data[4];
    firmware_version_minor = data[5];
    firmware_version_release = data[6];
    firmware_version_build = data[7];
    network_id = data[8];
    hardware_id = data[9];

    bufs = sb.prepare(2);
    n = ConnectSocket.receive(bufs);
    sb.commit(n);
    string_length = boost::endian::load_big_u16(
            Utils::buffer_to_char_array(sb)
    );
    sb.consume(n);

    if (string_length > 0) {
        bufs = sb.prepare(string_length);
        n = ConnectSocket.receive(bufs);
        sb.commit(n);
        device_password = Utils::buffer_to_string(sb);
        sb.consume(n);
    }
    else {
        device_password = std::string("");
    }

    bufs = sb.prepare(1);
    n = ConnectSocket.receive(bufs);
    sb.commit(n);
    aux_input_status = Utils::buffer_to_char_array(sb)[0];
    sb.consume(n);

    bufs = sb.prepare(4);
    n = ConnectSocket.receive(bufs);
    sb.commit(n);
    delay_time_remaining = Utils::int32_to_float(
            boost::endian::load_big_s32(Utils::buffer_to_char_array(sb))
    );
    sb.consume(n);

    bufs = sb.prepare(4);
    n = ConnectSocket.receive(bufs);
    sb.commit(n);
    elapsed_time = Utils::int32_to_float(
            boost::endian::load_big_s32(Utils::buffer_to_char_array(sb))
    );
    sb.consume(n);


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




