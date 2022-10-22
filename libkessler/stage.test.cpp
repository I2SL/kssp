#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#define BOOST_TEST_MODULE example
#include <memory>

#include <boost/endian/buffers.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/test/unit_test.hpp>

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

    BOOST_AUTO_TEST_CASE(TestBytestoInt) {
        unsigned char bytes[2];
        bytes[0] = (unsigned char)0;
        bytes[1] = (unsigned char)15;
        unsigned short int comp(15);
        unsigned short int conv = Utils::bytes_to_int(reinterpret_cast<char *>(bytes));
        printf("Result: %hd\n", conv);
        BOOST_CHECK(conv == comp);
    }

    BOOST_AUTO_TEST_CASE(TestDeviceInfoObject) {
        class DeviceInfo instance(
                boost::endian::big_uint8_buf_t(1),
                boost::endian::big_uint8_buf_t(2),
                boost::endian::big_uint8_buf_t(3),
                boost::endian::big_uint8_buf_t(4),
                boost::endian::big_uint8_buf_t(5),
                boost::endian::big_uint8_buf_t(6),
                boost::endian::big_uint8_buf_t(7),
                boost::endian::big_uint8_buf_t(8),
                boost::endian::big_uint8_buf_t(9),
                boost::endian::big_uint8_buf_t(10),
                "hello",
                boost::endian::big_uint8_buf_t(11),
                boost::endian::big_float32_buf_t(12.1),
                boost::endian::big_float32_buf_t(13.1)
                );
        std::cout << instance.to_string();
        BOOST_CHECK(instance.device_type.value() == 1);
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

    BOOST_FIXTURE_TEST_CASE(SendProtocol, MockServer)
    {
        char sample_message[1];
        sample_message[0] = SSP_PROTOCOL_VERSION;
        std::cout << "Sent: '" << std::setw(2) << std::setfill('0') << std::hex << (int)( sample_message[0] ) << "'\n";
        this->accept().then([this, &sample_message](boost::unique_future<void> f) {
            this->client->send(boost::asio::buffer(sample_message));
        });
        Stage instance("127.0.0.1", 5555);
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

BOOST_AUTO_TEST_SUITE_END()

