#define BOOST_TEST_MODULE example
#include <boost/test/unit_test.hpp>
#include "stage.h"

BOOST_AUTO_TEST_SUITE(StageTests)

BOOST_AUTO_TEST_CASE(EstablishConection)
{
    Stage instance("192.168.50.1", 5520);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(SendGet)
{
    Stage instance("192.168.50.1", 5520);
    instance.get(MessageID::DeviceInfo);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()