#include "vertex/test/test.hpp"

#include "vertex/stdlib/string/string.hpp"

using namespace vx::test;



///////////////////////////////////////////////////////////////////////////////

int main()
{
    test_suite::get_suite().run_tests();

    return 0;
}