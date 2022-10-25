#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#define BOOST_TEST_MODULE example

#include <boost/endian/conversion.hpp>
#include <boost/thread/future.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "stage.h"

BOOST_AUTO_TEST_SUITE(MockStageTests)

    struct MockServer {
        MockServer()  {
        }

        ~MockServer() {
            this->acceptor->close();
        }

        boost::asio::ip::address address = boost::asio::ip::address::from_string("127.0.0.1");
        boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(address, 5555);
        boost::asio::io_service service;

        std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(
                service,
                endpoint.protocol()
        );

        std::shared_ptr<boost::asio::ip::tcp::socket> client = std::make_shared<boost::asio::ip::tcp::socket>(
                service,
                endpoint.protocol()
        );

        boost::unique_future<void> accept() {
            return boost::async([this]() {
                this->acceptor->bind(endpoint);
                this->acceptor->listen();
                *this->client = this->acceptor->accept();
            });
        }

        void accept_and_say_hello() {
            this->accept().then([this](boost::unique_future<void> f) {
                this->client->send(boost::asio::buffer(std::string("Hello, world!")));
            });
        }

        void accept_and_send_message(char message[]) {
            this->accept().then([this, &message](boost::unique_future<void> f) {
                this->client->send(boost::asio::buffer(std::string(message)));
            });
        }
    };

    BOOST_AUTO_TEST_CASE(TestSizeOfEmptyVector) {
        std::vector<unsigned char> test{};
        BOOST_CHECK(test.size()==0);
    }

    BOOST_AUTO_TEST_CASE(TestVectorStringConversion) {
        std::string original = std::string("");
        std::vector<unsigned char> original_vect = Utils::string_to_char_vector(original);
        std::string original2 = Utils::char_vector_to_string(original_vect);
        printf("Vector Size: %d\n", original_vect.size());
        BOOST_CHECK(original==original2);
    }

    BOOST_AUTO_TEST_CASE(TestEmptyStringBehavior) {
        std::string original = std::string("");
        std::vector<unsigned char> original_vect = {'h', 'i'};
        original_vect.insert(std::end(original_vect), std::begin(original), std::end(original));
        BOOST_CHECK(original.size()==0);
        BOOST_CHECK(original_vect.size() == 2);
    }

    BOOST_AUTO_TEST_CASE(TestEndianLoad) {
        boost::uint16_t testendian(16);
        unsigned char* test;
        boost::endian::store_big_u16(test, testendian);
        boost::uint16_t loadresult = boost::endian::load_big_u16(test);
        BOOST_CHECK(testendian==loadresult);
    }

    BOOST_AUTO_TEST_CASE(TestFloattoInt32toFloat) {
        float input = -233.42;
        boost::int32_t input_as_int = reinterpret_cast <int&>(input);
        unsigned char* test;
        boost::endian::store_big_s32(test, input_as_int);
        boost::int32_t output_as_int = boost::endian::load_big_s32(test);
        float output = reinterpret_cast <float&>(output_as_int);
        BOOST_CHECK(input == output);
    }

    BOOST_AUTO_TEST_CASE(TestDeviceInfoObject) {
        class DeviceInfo instance(
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                10,
                "hello",
                11,
                12.1,
                13.1
                );
        std::cout << instance.to_string();
        BOOST_CHECK(instance.device_type == 1);
    }

    BOOST_FIXTURE_TEST_CASE(EstablishConnection, MockServer)
    {
        this->accept();
        Stage("127.0.0.1", 5555);
    }

    BOOST_FIXTURE_TEST_CASE(SayHello, MockServer)
    {
        this->accept_and_say_hello();
        Stage instance("127.0.0.1", 5555);
    }

    BOOST_FIXTURE_TEST_CASE(SendDeviceInfo, MockServer)
    {
        unsigned char sample_message[39];
        sample_message[0] = SSP_PROTOCOL_VERSION;
        sample_message[1] = 0x00;
        sample_message[2] = SSP_HEADER_SIZE;
        sample_message[3] = 0x00;
        sample_message[4] = MessageID::DeviceInfo;
        sample_message[5] = MessageType::Response;
        sample_message[6] = 1;
        sample_message[7] = 2;
        sample_message[8] = 3;
        sample_message[9] = 4;
        sample_message[10] = 5;
        sample_message[11] = 6;
        sample_message[12] = 7;
        sample_message[13] = 8;
        sample_message[14] = 9;
        sample_message[15] = 10;
        sample_message[16] = 0x00; //12-character password
        sample_message[17] = 0x0C;
        sample_message[18] = 0x68; //hello world!
        sample_message[19] = 0x65;
        sample_message[20] = 0x6C;
        sample_message[21] = 0x6C;
        sample_message[22] = 0x6F;
        sample_message[23] = 0x20;
        sample_message[24] = 0x77;
        sample_message[25] = 0x6F;
        sample_message[26] = 0x72;
        sample_message[27] = 0x6C;
        sample_message[28] = 0x64;
        sample_message[29] = 0x21;
        sample_message[30] = 11; // aux input
        sample_message[31] = 0x41;
        sample_message[32] = 0x41;
        sample_message[33] = 0x99;
        sample_message[34] = 0x9A;
        sample_message[35] = 0xC1;
        sample_message[36] = 0x51;
        sample_message[37] = 0x99;
        sample_message[38] = 0x9A;
        this->accept().then([this, &sample_message](boost::unique_future<void> f) {
            this->client->send(boost::asio::buffer(sample_message));
        });
        Stage instance("127.0.0.1", 5555);
        std::cout << instance.get_device_info().to_string();
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(StageTests)

    BOOST_AUTO_TEST_CASE(GetDeviceInfo)
    {
        Stage instance("192.168.50.1", 5520);
        class DeviceInfo test = instance.get_device_info();
        std::cout << test.to_string();
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(GetMotorInfo)
    {
        Stage instance("192.168.50.1", 5520);
        instance.get_device_info();
        class MotorInfo test = instance.get_motor_info();
        std::cout << test.to_string();
        BOOST_CHECK(test.motors[0].motor_count == 1);
    }

    BOOST_AUTO_TEST_CASE(GetDeviceGUID)
    {
        Stage instance("192.168.50.1", 5520);
        instance.get_device_info();
        instance.get_motor_info();
        class DeviceGUID test = instance.get_device_guid();
        std::cout << test.to_string();
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(MoveSlideMotor)
    {
        Stage instance("192.168.50.1", 5520);
        instance.get_device_info();
        //instance.set_user_password();
        instance.get_motor_info();
        instance.get_device_guid();
        instance.set_position_speed_acceleration(1, 25000, 100, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(1, -25000, 0, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(1, -25000, 100, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(1, -25000, 0, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(MovePanMotor)
    {
        Stage instance("192.168.50.1", 5520);
        instance.get_device_info();
        //instance.set_user_password();
        instance.get_motor_info();
        instance.get_device_guid();
        instance.set_position_speed_acceleration(2, 25000, 65, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(2, -25000, 0, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(2, -25000, 65, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(2, -25000, 0, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(MoveTiltMotor)
    {
        Stage instance("192.168.50.1", 5520);
        instance.get_device_info();
        //instance.set_user_password();
        instance.get_motor_info();
        instance.get_device_guid();
        instance.set_position_speed_acceleration(3, 25000, 65, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(3, -25000, 0, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(3, -25000, 65, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        instance.set_position_speed_acceleration(3, -25000, 0, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        BOOST_CHECK(true);
    }

BOOST_AUTO_TEST_SUITE_END()

