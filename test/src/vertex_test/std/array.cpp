#include "vertex/std/dynamic_array.hpp"
#include "vertex_test/test.hpp"

#include "vertex/std/array.hpp"
#include "vertex_test/test.hpp"
#include <type_traits>

//==============================================================================

using vx::array;

//==============================================================================

VX_TEST_CASE(test_element_access)
{
    array<size_t, 4> arr;
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;

    VX_CHECK(arr.front() == 10);
    VX_CHECK(arr.back() == 40);

    size_t* p = arr.data();
    VX_CHECK(p[2] == 30);
}

//==============================================================================

VX_TEST_CASE(test_const_access)
{
    array<size_t, 3> arr;
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    const array<size_t, 3>& c = arr;

    VX_CHECK(c.front() == 1);
    VX_CHECK(c.back() == 3);
    VX_CHECK(c[1] == 2);
    VX_CHECK(c.data()[2] == 3);
}

//==============================================================================

VX_TEST_CASE(test_iterator_basic)
{
    array<size_t, 5> arr;
    for (size_t i = 0; i < 5; ++i)
    {
        arr[i] = i * 2;
    }

    size_t sum = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        sum += *it;
    }

    VX_CHECK(sum == (0 + 2 + 4 + 6 + 8));
}

//==============================================================================

VX_TEST_CASE(test_iterator_const)
{
    array<size_t, 4> arr;
    arr[0] = 3;
    arr[1] = 6;
    arr[2] = 9;
    arr[3] = 12;

    const array<size_t, 4>& c = arr;

    size_t sum = 0;
    for (auto it = c.begin(); it != c.end(); ++it)
    {
        sum += *it;
    }

    VX_CHECK(sum == (3 + 6 + 9 + 12));
}

//==============================================================================

VX_TEST_CASE(test_reverse_iterator)
{
    array<size_t, 4> arr;
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;

    size_t sum = 0;
    for (auto it = arr.rbegin(); it != arr.rend(); ++it)
    {
        sum += *it;
    }

    VX_CHECK(sum == (40 + 30 + 20 + 10));
}

//==============================================================================

VX_TEST_CASE(test_fill)
{
    VX_SECTION("fill")
    array<size_t, 6> arr;
    arr.fill(55);

    for (size_t i = 0; i < arr.size(); ++i)
    {
        VX_CHECK(arr[i] == 55);
    }
}

//==============================================================================

VX_TEST_CASE(test_swap)
{
    array<size_t, 3> a;
    array<size_t, 3> b;

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    b[0] = 9;
    b[1] = 8;
    b[2] = 7;

    a.swap(b);

    VX_CHECK(b[0] == 1 && b[1] == 2 && b[2] == 3);
    VX_CHECK(a[0] == 9 && a[1] == 8 && a[2] == 7);
}

//==============================================================================

VX_TEST_CASE(test_size)
{
    using A = array<size_t, 5>;

    VX_CHECK(A::size() == 5);
    VX_CHECK(!A::empty());
    VX_CHECK(A::data_size() == sizeof(size_t) * 5);
    VX_CHECK(A::max_size() >= 5); // depends on mem::max_array_size
}

//==============================================================================

VX_TEST_CASE(test_comparisons_equal)
{
    array<size_t, 3> a;
    array<size_t, 3> b;

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    b[0] = 1;
    b[1] = 2;
    b[2] = 3;

    VX_CHECK(a == b);
    VX_CHECK(!(a != b));

    b[2] = 99;

    VX_CHECK(a != b);
}

//==============================================================================

VX_TEST_CASE(test_comparisons_order)
{
    array<size_t, 3> a;
    array<size_t, 3> b;

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    b[0] = 2;
    b[1] = 0;
    b[2] = 0;

    VX_CHECK(a < b);
    VX_CHECK(a <= b);
    VX_CHECK(!(a > b));
    VX_CHECK(!(a >= b));
}

//==============================================================================

VX_TEST_CASE(test_reverse_const_iterator)
{
    array<size_t, 4> arr;
    arr[0] = 4;
    arr[1] = 8;
    arr[2] = 12;
    arr[3] = 16;

    const array<size_t, 4>& c = arr;

    size_t product = 1;
    for (auto it = c.rbegin(); it != c.rend(); ++it)
    {
        product *= *it;
    }

    VX_CHECK(product == (16 * 12 * 8 * 4));
}

//==============================================================================

VX_TEST_CASE(test_aggregate_initialization)
{
    // Runtime check: aggregate initialization must assign correctly
    array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };

    VX_CHECK(arr[0] == 'h');
    VX_CHECK(arr[1] == 'e');
    VX_CHECK(arr[2] == 'l');
    VX_CHECK(arr[3] == 'l');
    VX_CHECK(arr[4] == 'o');
}

//==============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
