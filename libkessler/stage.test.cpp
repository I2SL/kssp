#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#define BOOST_TEST_MODULE example
#include <memory>
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

BOOST_FIXTURE_TEST_CASE(EstablishConnection, MockServer)
{
    this->accept();
    Stage("127.0.0.1", 5555);
}

BOOST_FIXTURE_TEST_CASE(SayHello, MockServer)
{
    this->accept_and_say_hello();
    Stage instance("127.0.0.1", 5555);
    instance.get(MessageID::DeviceInfo);
}

BOOST_FIXTURE_TEST_CASE(SendProtocol, MockServer)
{
    char sample_message[1];
    sample_message[0] = SSP_PROTOCOL_VERSION;
    this->accept().then([this, &sample_message](boost::unique_future<void> f) {
        this->client->send(boost::asio::buffer(sample_message));
    });
    Stage instance("127.0.0.1", 5555);
    instance.get(MessageID::DeviceInfo);
}

BOOST_FIXTURE_TEST_CASE(GetMotorInfo, MockServer)
{
    unsigned short motor_count = 1;
    unsigned short motor_address = 2;
    float position = 32;
    float end_position = 31;
    float max_max_setup_speed = 30;
    float max_max_move_speed = 29;
    float max_max_acceleration = 28;
    char sample_message[28];
    std::sprintf(sample_message,
                 "%c%hd%hd%c%c%c%f%f%f%f%f",
                 SSP_PROTOCOL_VERSION,
                 28,
                 MessageID::MotorInfo,
                 MessageType::Response,
                 motor_count,
                 motor_address,
                 position,
                 end_position,
                 max_max_setup_speed,
                 max_max_move_speed,
                 max_max_acceleration
                 );
    //printf(sample_message);
    this->accept().then([this, &sample_message](boost::unique_future<void> f) {
       this->client->send(boost::asio::buffer(std::string(sample_message)));
    });
    Stage instance("127.0.0.1", 5555);
    instance.get(MessageID::MotorInfo);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(StageTests)

BOOST_AUTO_TEST_CASE(SendGet)
{
    Stage instance("192.168.50.1", 5520);
    instance.get(MessageID::DeviceInfo);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

