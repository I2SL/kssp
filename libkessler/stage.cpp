#include "stage.h"

Stage::Stage(const std::string &host, const int port)
    : ConnectSocket(
            service,
            boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address::from_string(host),
                    port
                    ).protocol()
            ),
    lck(receive_mtx)
{
    boost::asio::ip::address address = boost::asio::ip::address::from_string(host);
    boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(address, port);
    ConnectSocket.connect(endpoint);

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

    while (!ready) {
        cv.wait(lck);
    }

    class DeviceInfo response = DeviceInfoQueue.front();
    DeviceInfoQueue.pop();
    ready = false;

    return response;
}

class MotorInfo Stage::get_motor_info() {
    char msg[6];
    msg[0] = SSP_PROTOCOL_VERSION;
    msg[1] = 0x00;
    msg[2] = SSP_GET_MESSAGE_SIZE;
    msg[3] = 0x00;
    msg[4] = MessageID::MotorInfo;
    msg[5] = MessageType::Get;
    ConnectSocket.send(boost::asio::buffer(msg));

    while (!ready) {
        cv.wait(lck);
    }

    class MotorInfo response = MotorInfoQueue.front();
    MotorInfoQueue.pop();
    ready = false;

    return response;
}

class DeviceGUID Stage::get_device_guid() {
    char msg[6];
    msg[0] = SSP_PROTOCOL_VERSION;
    msg[1] = 0x00;
    msg[2] = SSP_GET_MESSAGE_SIZE;
    msg[3] = 0x00;
    msg[4] = MessageID::DeviceGUID;
    msg[5] = MessageType::Get;
    ConnectSocket.send(boost::asio::buffer(msg));

    while (!ready) {
        cv.wait(lck);
    }

    class DeviceGUID response = DeviceGUIDQueue.front();
    DeviceInfoQueue.pop();
    ready = false;

    return response;
}

void Stage::event_queue_manager() {
    while (true) {
        event_mtx.lock();
        try {
            boost::uint8_t message_lead;
            message_lead = get_uint8();

            if (message_lead == SSP_PROTOCOL_VERSION) {
                printf("Got a header\n");
                boost::uint16_t message_length;
                boost::uint16_t message_id;
                boost::uint8_t message_type;

                message_length = get_uint16();
                message_id = get_uint16();
                message_type = get_uint8();

                printf("Protocol: %hd\n", (short)message_lead);
                printf("Length: %hd\n", message_length);
                printf("ID: %hd\n", message_id);
                printf("Type: %hd\n", (short)message_type);

                if (message_type == MessageType::Response || message_type == MessageType::Ongoing) {
                    if (message_id == (unsigned short int)MessageID::DeviceInfo) {
                        printf("Got Device Info Response\n");
                        on_receive_device_info_response();
                    }
                    else if (message_id == (unsigned short int)MessageID::MotorInfo) {
                        printf("Got Motor Info Response: {}\n", message_type);
                        on_receive_motor_info_response(message_type);
                    }
                    else if (message_id == (unsigned short int)MessageID::DeviceGUID) {
                        printf("Got Device GUID Response\n");
                        on_receive_device_guid_response();
                    }
                }

            }
        }
        catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
        event_mtx.unlock();
    }
}

void Stage::on_receive_device_info_response() {
    printf("Processing Device Info Response\n");
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

    device_type = get_uint8();
    device_addr = get_uint8();
    playback_mode = get_uint8();
    playback_status = get_uint8();
    firmware_version_major = get_uint8();
    firmware_version_minor = get_uint8();
    firmware_version_release = get_uint8();
    firmware_version_build = get_uint8();
    network_id = get_uint8();
    hardware_id = get_uint8();

    string_length = get_uint16();
    if (string_length > 0) {
        device_password = get_string(string_length);
    }
    else {
        device_password = std::string("");
    }

    aux_input_status = get_uint8();
    delay_time_remaining = get_float();
    elapsed_time = get_float();


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
    ready = true;
    cv.notify_all();
}

void Stage::on_receive_motor_info_response(boost::uint8_t message_type) {
    printf("Processing Motor Info Response\n");
    boost::uint8_t motor_count = get_uint8();
    boost::uint8_t motor_address = get_uint8();
    float position = get_float();
    float end_position = get_float();
    float max_max_setup_speed = get_float();
    float max_max_move_speed = get_float();
    float max_max_acceleration = get_float();

    class Motor motor(
            motor_count,
            motor_address,
            position,
            end_position,
            max_max_setup_speed,
            max_max_move_speed,
            max_max_acceleration
            );
    MotorQueue.push(motor);

    // Response message type indicates final response.
    // Construct MotorInfo object by emptying Motor queue
    if(message_type == MessageType::Response) {
        std::vector<Motor> motors;
        while (!MotorQueue.empty()) {
            motors.push_back(MotorQueue.front());
            MotorQueue.pop();
        }
        class MotorInfo response(motors);
        MotorInfoQueue.push(response);
        ready = true;
        cv.notify_all();
    }
}

void Stage::on_receive_device_guid_response() {
    printf("Processing Device GUID Response\n");
    boost::uint16_t block_length = get_uint16();
    unsigned char* block_data = get_block(block_length);

    class DeviceGUID response(block_length, block_data);
    DeviceGUIDQueue.push(response);
    ready = true;
    cv.notify_all();
}

boost::uint8_t Stage::get_uint8() {
    boost::uint8_t out;
    boost::asio::streambuf uint8sb;
    boost::asio::mutable_buffers_1 bufs = uint8sb.prepare(1);
    size_t n = ConnectSocket.receive(bufs);
    uint8sb.commit(n);
    out = Utils::buffer_to_char_array(uint8sb)[0];
    uint8sb.consume(n);

    return out;
}

boost::uint16_t Stage::get_uint16() {
    boost::uint16_t out;
    boost::asio::streambuf uint16sb;
    boost::asio::mutable_buffers_1 bufs = uint16sb.prepare(2);
    size_t n = ConnectSocket.receive(bufs);
    uint16sb.commit(n);
    out = boost::endian::load_big_u16(Utils::buffer_to_char_array(uint16sb));
    uint16sb.consume(n);

    return out;
}

float Stage::get_float() {
    float out;
    boost::asio::streambuf int32sb;
    boost::asio::mutable_buffers_1 bufs = int32sb.prepare(4);
    size_t n = ConnectSocket.receive(bufs);
    int32sb.commit(n);
    out = Utils::int32_to_float(
            boost::endian::load_big_s32(Utils::buffer_to_char_array(int32sb))
    );
    int32sb.consume(n);

    return out;
}

std::string Stage::get_string(boost::uint16_t len) {
    std::string out;
    boost::asio::streambuf strsb;
    boost::asio::mutable_buffers_1 bufs = strsb.prepare(len);
    size_t n = ConnectSocket.receive(bufs);
    strsb.commit(n);
    unsigned char* uchars = Utils::buffer_to_char_array(strsb);
    char* chars;
    std::memcpy(&chars, &uchars, sizeof(uchars));
    out = chars;
    strsb.consume(n);

    return out;
}

unsigned char* Stage::get_block(boost::uint16_t len) {
    std::string out;
    boost::asio::streambuf blocksb;
    boost::asio::mutable_buffers_1 bufs = blocksb.prepare(len);
    size_t n = ConnectSocket.receive(bufs);
    blocksb.commit(n);
    unsigned char* uchars = Utils::buffer_to_char_array(blocksb);
    blocksb.consume(n);

    return uchars;
}




