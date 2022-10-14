#define BOOST_TEST_MODULE example
#include <boost/test/unit_test.hpp>
#include "stage.h"

BOOST_AUTO_TEST_CASE(MyTestCase)
{
    // To simplify this example test, let's suppose we'll test 'float'.
    // Some tests are stupid, but all should pass.
    float x = 9.5f;

    BOOST_CHECK(x != 0.0f);
    BOOST_CHECK_EQUAL((int)x, 9);
    BOOST_CHECK_CLOSE(x, 9.5f, 0.0001f); // Checks differ no more than 0.0001%
}

BOOST_AUTO_TEST_CASE(TestConstructor)
{
    Stage instance("127.0.0.1", 4200);
    BOOST_CHECK(true);
}