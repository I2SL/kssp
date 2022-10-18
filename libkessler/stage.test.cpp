#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#define BOOST_TEST_MODULE example
#include <memory>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/test/unit_test.hpp>
#include "stage.h"

BOOST_AUTO_TEST_SUITE(StageTests)

BOOST_AUTO_TEST_CASE(EstablishConection)
{
    Stage instance("192.168.50.1", 5520);
    BOOST_CHECK(true);
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

BOOST_AUTO_TEST_SUITE_END()