#include <cstring>

#include "vertex_test/test.hpp"
#include "vertex/os/system_info.hpp"
#include "vertex/system/assert.hpp"

using namespace vx;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_get_name)
{
    const char* name = os::get_name();
    VX_CHECK(name != nullptr);

    VX_ASSERT(name);
    VX_CHECK(std::strlen(name) > 0);

    VX_MESSAGE("  os name: ", name);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_get_model_name)
{
    const std::string model_name = os::get_model_name();
    VX_CHECK(!model_name.empty());

    VX_MESSAGE("  model name: ", model_name);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_get_distribution_name)
{
    const std::string distribution_name = os::get_distribution_name();
    VX_CHECK(!distribution_name.empty());

    VX_MESSAGE("  distribution name: ", distribution_name);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_get_system_name)
{
    const std::string system_name = os::get_system_name();
    VX_CHECK(!system_name.empty());

    VX_MESSAGE("  system name: ", system_name);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_processor_info)
{
    VX_SECTION("processor name")
    {
        const std::string processor_name = os::get_processor_name();
        VX_CHECK(!processor_name.empty());

        VX_MESSAGE("  processor name: ", processor_name);
    }

    VX_SECTION("processor count")
    {
        const auto processor_count = os::get_processor_count();
        VX_CHECK(processor_count > 0);

        VX_MESSAGE("  processor count: ", processor_count);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_version)
{
    uint32_t major = 0, minor = 0, patch = 0;
    VX_CHECK(os::get_version(&major, &minor, &patch));
    // At least one version component should be non-zero
    VX_CHECK(major > 0 || minor > 0 || patch > 0);

    VX_MESSAGE("  version: ", major, '.', minor, '.', patch);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}