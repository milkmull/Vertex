#include "vertex/std/dynamic_array.hpp"
#include "vertex_test/test.hpp"

#include "vertex/std/array.hpp"
#include "vertex_test/test.hpp"
#include <type_traits>

//==============================================================================

using vx::array;

//==============================================================================

VX_TEST_CASE(test_array_element_access)
{
    array<int, 4> arr;
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;

    VX_CHECK(arr.front() == 10);
    VX_CHECK(arr.back() == 40);

    int* p = arr.data();
    VX_CHECK(p[2] == 30);
}

//==============================================================================

VX_TEST_CASE(test_array_const_access)
{
    array<int, 3> arr;
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    const array<int, 3>& c = arr;

    VX_CHECK(c.front() == 1);
    VX_CHECK(c.back() == 3);
    VX_CHECK(c[1] == 2);
    VX_CHECK(c.data()[2] == 3);
}

//==============================================================================

VX_TEST_CASE(test_array_iterator_basic)
{
    array<int, 5> arr;
    for (int i = 0; i < 5; ++i)
        arr[i] = i * 2;

    int sum = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        sum += *it;
    }

    VX_CHECK(sum == (0 + 2 + 4 + 6 + 8));
}

//==============================================================================

VX_TEST_CASE(test_array_iterator_const)
{
    array<int, 4> arr;
    arr[0] = 3;
    arr[1] = 6;
    arr[2] = 9;
    arr[3] = 12;

    const array<int, 4>& c = arr;

    int sum = 0;
    for (auto it = c.begin(); it != c.end(); ++it)
    {
        sum += *it;
    }

    VX_CHECK(sum == (3 + 6 + 9 + 12));
}

//==============================================================================

VX_TEST_CASE(test_array_reverse_iterator)
{
    array<int, 4> arr;
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;

    int sum = 0;
    for (auto it = arr.rbegin(); it != arr.rend(); ++it)
    {
        sum += *it;
    }

    VX_CHECK(sum == (40 + 30 + 20 + 10));
}

//==============================================================================

VX_TEST_CASE(test_array_fill)
{
    VX_SECTION("fill")
    array<int, 6> arr;
    arr.fill(55);

    for (size_t i = 0; i < arr.size(); ++i)
    {
        VX_CHECK(arr[i] == 55);
    }
}

//==============================================================================

VX_TEST_CASE(test_array_swap)
{
    array<int, 3> a;
    array<int, 3> b;

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

VX_TEST_CASE(test_array_size)
{
    using A = array<int, 5>;

    VX_CHECK(A::size() == 5);
    VX_CHECK(!A::empty());
    VX_CHECK(A::data_size() == sizeof(int) * 5);
    VX_CHECK(A::max_size() >= 5); // depends on mem::max_array_size
}

//==============================================================================

VX_TEST_CASE(test_array_comparisons_equal)
{
    array<int, 3> a;
    array<int, 3> b;

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

VX_TEST_CASE(test_array_comparisons_order)
{
    array<int, 3> a;
    array<int, 3> b;

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

VX_TEST_CASE(test_array_reverse_const_iterator)
{
    array<int, 4> arr;
    arr[0] = 4;
    arr[1] = 8;
    arr[2] = 12;
    arr[3] = 16;

    const array<int, 4>& c = arr;

    int product = 1;
    for (auto it = c.rbegin(); it != c.rend(); ++it)
    {
        product *= *it;
    }

    VX_CHECK(product == (16 * 12 * 8 * 4));
}

//==============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
