#include "vertex/std/aligned_storage.hpp"
#include "vertex_test/test.hpp"
#include <type_traits>

using vx::aligned_storage;

//==============================================================================

struct simple_type
{
    int x;
    simple_type(int v)
        : x(v)
    {}
    ~simple_type()
    {
        x = 0;
    }
};

struct complex_type
{
    complex_type(int ax, int bx)
        : a(ax), b(bx)
    {}
    int a;
    int b;
};

//==============================================================================

VX_TEST_CASE(test_basic)
{
    aligned_storage<16, alignof(simple_type)> storage;

    storage.construct<simple_type>(123);
    VX_CHECK(storage.get<simple_type>().x == 123);
    storage.destroy<simple_type>();
}

//==============================================================================

VX_TEST_CASE(test_forward_ctor)
{
    aligned_storage<16, alignof(simple_type)> storage(simple_type(77));
    VX_CHECK(storage.get<simple_type>().x == 77);
    storage.destroy<simple_type>();
}

//==============================================================================

VX_TEST_CASE(test_multiargs)
{
    aligned_storage<32, alignof(complex_type)> storage;
    storage.construct<complex_type>(4, 9);

    const complex_type& ref = storage.get<complex_type>();
    VX_CHECK(ref.a == 4);
    VX_CHECK(ref.b == 9);

    storage.destroy<complex_type>();
}

//==============================================================================

VX_TEST_CASE(test_alignment)
{
    aligned_storage<64, 32> storage;
    void* p = storage.ptr<int>();

    VX_CHECK(reinterpret_cast<std::uintptr_t>(p) % 32 == 0);
}

//==============================================================================

VX_TEST_CASE(test_const_access)
{
    aligned_storage<16, alignof(simple_type)> storage;
    storage.construct<simple_type>(321);

    const auto& cref = static_cast<const aligned_storage<16, alignof(simple_type)>&>(storage);

    VX_CHECK(cref.get<simple_type>().x == 321);
    VX_CHECK(cref.ptr<simple_type>() != nullptr);

    storage.destroy<simple_type>();
}

//==============================================================================

VX_TEST_CASE(test_reuse)
{
    aligned_storage<32, alignof(complex_type)> storage;

    storage.construct<complex_type>(1, 2);
    VX_CHECK(storage.get<complex_type>().a == 1);
    storage.destroy<complex_type>();

    storage.construct<complex_type>(7, 8);
    VX_CHECK(storage.get<complex_type>().a == 7);
    VX_CHECK(storage.get<complex_type>().b == 8);
    storage.destroy<complex_type>();
}

//==============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
