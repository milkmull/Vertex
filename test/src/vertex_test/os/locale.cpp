#include "vertex_test/test.hpp"
#include "vertex/os/locale.hpp"

using namespace vx;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_locale)
{
    VX_SECTION("country code")
    {
        std::string cc;
        VX_CHECK(os::locale::get_country_code(cc));
        VX_CHECK(!cc.empty());

        VX_MESSAGE("  country code: ", cc);
    }

    VX_SECTION("language")
    {
        std::string lang;
        VX_CHECK(os::locale::get_language(lang));
        VX_CHECK(!lang.empty());

        VX_MESSAGE("  language: ", lang);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
