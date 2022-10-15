#define BOOST_TEST_MODULE example
#include <boost/test/unit_test.hpp>
#include "stage.h"

BOOST_AUTO_TEST_CASE(TestConstructor)
{
    Stage instance("192.168.50.1", 5520);
    BOOST_CHECK(true);
}