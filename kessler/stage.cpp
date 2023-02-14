#include "stage.hpp"

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
    std::vector<unsigned char> message = Utils::make_message_header(SSP_HEADER_SIZE, MessageID::DeviceInfo, MessageType::Get);
    ConnectSocket.send(boost::asio::buffer(message));

    while (!ready) {
        cv.wait(lck);
    }

    class DeviceInfo response = DeviceInfoQueue.front();
    DeviceInfoQueue.pop();
    ready = false;

    return response;
}

class MotorInfo Stage::get_motor_info() {
    std::vector<unsigned char> message = Utils::make_message_header(SSP_HEADER_SIZE, MessageID::MotorInfo, MessageType::Get);
    ConnectSocket.send(boost::asio::buffer(message));

    while (!ready) {
        cv.wait(lck);
    }

    class MotorInfo response = MotorInfoQueue.front();
    MotorInfoQueue.pop();
    ready = false;

    return response;
}

class DeviceGUID Stage::get_device_guid() {
    std::vector<unsigned char> message = Utils::make_message_header(SSP_HEADER_SIZE, MessageID::DeviceGUID, MessageType::Get);
    ConnectSocket.send(boost::asio::buffer(message));

    while (!ready) {
        cv.wait(lck);
    }

    class DeviceGUID response = DeviceGUIDQueue.front();
    DeviceGUIDQueue.pop();
    ready = false;

    return response;
}

class NetworkInfo Stage::get_network_info() {
    std::vector<unsigned char> params = {0x00}; //LimitedResponse true not implemented
    std::vector<unsigned char> message = Utils::make_message(SSP_HEADER_SIZE + 1, MessageID::NetworkInfo, MessageType::Get, params);
    ConnectSocket.send(boost::asio::buffer(message));

    while (!ready) {
        cv.wait(lck);
    }

    class NetworkInfo response = NetworkInfoQueue.front();
    NetworkInfoQueue.pop();
    ready = false;

    return response;
}

class LEDStatus Stage::get_led_status() {
    std::vector<unsigned char> message = Utils::make_message_header(SSP_HEADER_SIZE, MessageID::LEDStatus, MessageType::Get);
    ConnectSocket.send(boost::asio::buffer(message));

    while (!ready) {
        cv.wait(lck);
    }

    class LEDStatus response = LEDStatusQueue.front();
    LEDStatusQueue.pop();
    ready = false;

    return response;
}

void Stage::set_user_password(const std::string& password) {
    boost::uint16_t password_length = password.size();
    boost::uint16_t message_length = SSP_HEADER_SIZE + 2 + password_length;
    std::vector<unsigned char> params;
    params = Utils::push_unit16(params, password_length);
    params.insert(std::end(params), std::begin(password), std::end(password));
    std::vector<unsigned char> message = Utils::make_message(message_length, MessageID::UserPassword, MessageType::Set, params);
    ConnectSocket.send(boost::asio::buffer(message));
}

void Stage::set_device_password(const std::string& password) {
    boost::uint16_t password_length = password.size();
    boost::uint16_t message_length = SSP_HEADER_SIZE + 2 + password_length;
    std::vector<unsigned char> params;
    params = Utils::push_unit16(params, password_length);
    params.insert(std::end(params), std::begin(password), std::end(password));
    std::vector<unsigned char> message = Utils::make_message(message_length, MessageID::DevicePassword, MessageType::Set, params);
    ConnectSocket.send(boost::asio::buffer(message));
}

void Stage::set_position_speed_acceleration(const boost::uint8_t motor_address, const float position, const float speed, const float acceleration) {
    boost::uint16_t message_length = 20;
    std::vector<unsigned char> params;
    params.push_back(ActionID::SetPositionSpeedAcceleration);
    params.push_back(motor_address);
    params = Utils::push_float(params, position);
    params = Utils::push_float(params, speed);
    params = Utils::push_float(params, acceleration);
    std::vector<unsigned char> message = Utils::make_message(message_length, MessageID::Action, MessageType::Set, params);
    ConnectSocket.send(boost::asio::buffer(message));
}

void Stage::set_led_status(boost::uint8_t master_status, boost::uint8_t slave_status) {
    std::vector<unsigned char> params = {master_status, slave_status};
    std::vector<unsigned char> message = Utils::make_message(SSP_HEADER_SIZE + 2, MessageID::LEDStatus, MessageType::Set, params);
    ConnectSocket.send(boost::asio::buffer(message));
}

void Stage::handshake() {
    class DeviceInfo init = get_device_info();
    set_user_password(init.device_password);
    get_motor_info();
    get_device_guid();
}

void Stage::reset_device() {
    std::vector<unsigned char> params = {ActionID::ResetDevice};
    std::vector<unsigned char> message = Utils::make_message(SSP_HEADER_SIZE + 1, MessageID::Action, MessageType::Set, params);
    ConnectSocket.send(boost::asio::buffer(message));
}

void Stage::reset_axis(boost::uint8_t motor_address) {
    std::vector<unsigned char> params = {ActionID::ResetAxis, motor_address};
    std::vector<unsigned char> message = Utils::make_message(SSP_HEADER_SIZE + 2, MessageID::Action, MessageType::Set, params);
    ConnectSocket.send(boost::asio::buffer(message));
}

void Stage::mark_begin_position(boost::uint8_t motor_address) {
    std::vector<unsigned char> params = {ActionID::MarkBeginPosition, motor_address};
    std::vector<unsigned char> message = Utils::make_message(SSP_HEADER_SIZE + 2, MessageID::Action, MessageType::Set, params);
    ConnectSocket.send(boost::asio::buffer(message));
}

void Stage::mark_end_position(boost::uint8_t motor_address) {
    std::vector<unsigned char> params = {ActionID::MarkEndPosition, motor_address};
    std::vector<unsigned char> message = Utils::make_message(SSP_HEADER_SIZE + 2, MessageID::Action, MessageType::Set, params);
    ConnectSocket.send(boost::asio::buffer(message));
}

void Stage::event_queue_manager() {
    while (active) {
        event_mtx.lock();
        try {
            boost::uint8_t message_lead;
            message_lead = get_uint8();

            if (message_lead == SSP_PROTOCOL_VERSION) {
                boost::uint16_t message_length;
                boost::uint16_t message_id;
                boost::uint8_t message_type;

                message_length = get_uint16();
                message_id = get_uint16();
                message_type = get_uint8();

                if (message_type == MessageType::Response || message_type == MessageType::Ongoing) {
                    if (message_id == (unsigned short int)MessageID::DeviceInfo) {
                        on_receive_device_info_response();
                    }
                    else if (message_id == (unsigned short int)MessageID::MotorInfo) {
                        on_receive_motor_info_response(message_type);
                    }
                    else if (message_id == (unsigned short int)MessageID::DeviceGUID) {
                        on_receive_device_guid_response();
                    }
                    else if (message_id == (unsigned short int)MessageID::NetworkInfo) {
                        on_receive_network_info_response();
                    }
                    else if (message_id == (unsigned short int)MessageID::LEDStatus) {
                        on_receive_led_status_response();
                    }
                    else if (message_id == (unsigned short int)MessageID::Notification) {
                        boost::uint8_t notification_type = get_uint8();
                        if (notification_type == NoteID::AuxInputStatus) {
                            on_receive_aux_input_notification();
                        }
                        else if (notification_type == NoteID::ErrorStatus) {
                            on_receive_error_status_notification();
                        }
                        else if (notification_type == NoteID::MotorCalibrated) {
                            on_receive_motor_calibrated_notification();
                        }
                        else if (notification_type == NoteID::MotorPosition) {
                            on_receive_motor_position_notification();
                        }
                        else if (notification_type == NoteID::MotorStatus) {
                            on_receive_motor_status_notification();
                        }
                        else if (notification_type == NoteID::PlaybackStatus) {
                            on_receive_playback_status_notification();
                        }
                        else if (notification_type == NoteID::UnsupportedMessage) {
                            on_receive_unsupported_message_notification(message_length - SSP_HEADER_SIZE);
                        }
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
    std::string device_password;

    boost::uint8_t device_type = get_uint8();
    boost::uint8_t device_addr = get_uint8();
    boost::uint8_t playback_mode = get_uint8();
    boost::uint8_t playback_status = get_uint8();
    boost::uint8_t firmware_version_major = get_uint8();
    boost::uint8_t firmware_version_minor = get_uint8();
    boost::uint8_t firmware_version_release = get_uint8();
    boost::uint8_t firmware_version_build = get_uint8();
    boost::uint8_t network_id = get_uint8();
    boost::uint8_t hardware_id = get_uint8();

    boost::uint16_t string_length = get_uint16();
    if (string_length > 0) {
        device_password = get_string(string_length);
    }
    else {
        device_password = std::string("");
    }

    boost::uint8_t aux_input_status = get_uint8();
    float delay_time_remaining = get_float();
    float elapsed_time = get_float();
    boost::uint32_t photo_count = get_uint32();


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
            elapsed_time,
            photo_count
            );
    DeviceInfoQueue.push(response);
    ready = true;
    cv.notify_all();
}

void Stage::on_receive_motor_info_response(boost::uint8_t message_type) {
    boost::uint8_t motor_count = get_uint8();
    boost::uint8_t motor_address = get_uint8();
    float position = get_float();
    float begin_position = get_float();
    float end_position = get_float();
    float max_max_setup_speed = get_float();
    float max_max_move_speed = get_float();
    float max_max_acceleration = get_float();

    class Motor motor(
            motor_count,
            motor_address,
            position,
            begin_position,
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
    boost::uint16_t block_length = get_uint16();
    unsigned char* block_data = get_block(block_length);

    class DeviceGUID response(block_length, block_data);
    DeviceGUIDQueue.push(response);
    ready = true;
    cv.notify_all();
}

void Stage::on_receive_network_info_response() {
    boost::uint8_t DHCP = get_uint8();
    unsigned char* mac_address = get_block(6);
    boost::uint32_t ip_address = get_uint32();
    boost::uint32_t subnet_mask = get_uint32();
    boost::uint32_t gateway = get_uint32();
    boost::uint8_t device_type = get_uint8();
    boost::uint8_t device_addr = get_uint8();

    class NetworkInfo response(
            DHCP,
            mac_address,
            ip_address,
            subnet_mask,
            gateway,
            device_type,
            device_addr
            );
    NetworkInfoQueue.push(response);
    ready = true;
    cv.notify_all();
}

void Stage::on_receive_led_status_response() {
    boost::uint8_t master_status = get_uint8();
    boost::uint8_t slave_status = get_uint8();

    class LEDStatus response(master_status, slave_status);
    LEDStatusQueue.push(response);
    ready = true;
    cv.notify_all();
}

void Stage::on_receive_aux_input_notification() {
    boost::uint8_t status = get_uint8();
    class AuxInputStatus notification(status);
    AuxInputStatusQueue.push(notification);
}

void Stage::on_receive_error_status_notification() {
    boost::uint8_t error_code = get_uint8();
    boost::uint8_t motor_address = get_uint8();
    class ErrorStatus notification(error_code, motor_address);
    ErrorStatusQueue.push(notification);
}

void Stage::on_receive_motor_calibrated_notification() {
    boost::uint8_t motor_address = get_uint8();
    float begin_position = get_float();
    float end_position = get_float();
    class MotorCalibrated notification(motor_address, begin_position, end_position);
    MotorCalibratedQueue.push(notification);
//    unsigned char* bytes = get_block(9);
//    for (int i=0 ; i<9 ; i++ )
//    {
//        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int) bytes[i]<<"\t";
//    }
//    class MotorCalibrated notification(0, 0);
//    MotorCalibratedQueue.push(notification);
}

void Stage::on_receive_motor_position_notification() {
    boost::uint8_t motor_address = get_uint8();
    float position = get_float();
    class MotorPosition notification(motor_address, position);
    MotorPositionQueue.push(notification);
}

void Stage::on_receive_motor_status_notification() {
    boost::uint8_t motor_count = get_uint8();
    boost::uint8_t motor_addr = get_uint8();
    boost::uint8_t online = get_uint8();
    boost::uint8_t motor_type = get_uint8();
    float max_max_setup_speed = get_float();
    float max_max_move_speed = get_float();
    float max_max_acceleration = get_float();
    class MotorStatus notification(motor_count, motor_addr, online, motor_type, max_max_setup_speed, max_max_move_speed, max_max_acceleration);
    MotorStatusQueue.push(notification);
}

void Stage::on_receive_playback_status_notification() {
    boost::uint8_t playback_status = get_uint8();
    float elapsed_time = get_float();
    float photo_count = get_float();
    boost::uint8_t motor_count = get_uint8();
    boost::uint8_t motor_address = get_uint8();
    float position = get_float();
    class PlaybackStatus notification(playback_status, elapsed_time, photo_count, motor_count, motor_address, position);
    PlaybackStatusQueue.push(notification);
}

void Stage::on_receive_unsupported_message_notification(boost::uint16_t length) {
    unsigned char* message = get_block(length);
    class UnsupportedMessage notification(message);
    UnsupportedMessageQueue.push(notification);
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

boost::uint32_t Stage::get_uint32() {
    boost::uint32_t out;
    boost::asio::streambuf uint32sb;
    boost::asio::mutable_buffers_1 bufs = uint32sb.prepare(4);
    size_t n = ConnectSocket.receive(bufs);
    uint32sb.commit(n);
    out = boost::endian::load_big_u32(Utils::buffer_to_char_array(uint32sb));
    uint32sb.consume(n);

    return out;
}

float Stage::get_float() {
    float out;
    boost::asio::streambuf int32sb;
    boost::asio::mutable_buffers_1 bufs = int32sb.prepare(4);
    size_t n = ConnectSocket.receive(bufs);
    int32sb.commit(n);
    boost::int32_t out_as_int = boost::endian::load_big_s32(Utils::buffer_to_char_array(int32sb));
    out = Utils::int32_to_float(out_as_int);
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

void Stage::shutdown() {
    active = false;
    boost::system::error_code ec;
    ConnectSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    ConnectSocket.close();
}




