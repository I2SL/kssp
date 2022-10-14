#define BOOST_TEST_MODULE example
#include <boost/test/unit_test.hpp>
#include "stage.h"

BOOST_AUTO_TEST_CASE(TestConstructor)
{
    Stage instance("127.0.0.1 ", 4200);
    BOOST_CHECK(true);
}