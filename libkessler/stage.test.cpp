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
};

BOOST_FIXTURE_TEST_CASE(EstablishConnection, MockServer)
{
    this->accept();
    Stage("127.0.0.1", 5555);
}

BOOST_FIXTURE_TEST_CASE(GetDeviceInfo, MockServer)
{
    this->accept_and_say_hello();
    Stage instance("127.0.0.1", 5555);
    instance.get(MessageID::DeviceInfo);
}

BOOST_AUTO_TEST_SUITE_END()

//BOOST_AUTO_TEST_SUITE(StageTests)

//BOOST_AUTO_TEST_CASE(SendGet)
//{
//    Stage instance("192.168.50.1", 5520);
//    instance.get(MessageID::DeviceInfo);
//    BOOST_CHECK(true);
//}

//BOOST_AUTO_TEST_SUITE_END()

