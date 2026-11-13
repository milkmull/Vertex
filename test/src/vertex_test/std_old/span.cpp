#include "vertex/std/span.hpp"
#include "vertex/std/array.hpp"
#include "vertex_test/test.hpp"

//=============================================================================

template <typename T, size_t Extent = vx::dynamic_extent>
constexpr void func_taking_span(vx::span<T, Extent>)
{}

template <typename T, typename IT>
struct is_compatible_iterator : std::bool_constant<vx::type_traits::is_iterator<IT>::value && std::is_convertible<typename std::remove_reference<typename vx::type_traits::iter_reference<IT>::type>::type (*)[], T (*)[]>::value>
{};

template <typename T, typename IT, typename S>
struct is_compatible_sentinel
{
private:

    static constexpr bool iter_ok = is_compatible_iterator<T, IT>::value;

    template <typename X = IT, typename Y = S>
    static auto test(int) -> decltype(

        // 1. Comparable (sentinel_for requirement)
        static_cast<bool>(std::declval<X>() == std::declval<Y>()),

        // 2. If sized sentinel: both subtractions must be valid
        static_cast<typename std::iterator_traits<X>::difference_type>(
            std::declval<Y>() - std::declval<X>()),
        static_cast<typename std::iterator_traits<X>::difference_type>(
            std::declval<X>() - std::declval<Y>()),

        std::true_type{});

    template <typename, typename>
    static std::false_type test(...);

public:

    static constexpr bool value = iter_ok && decltype(test<IT, S>(0))::value;
};

VX_TEST_CASE(test_span)
{
    struct base
    {};

    struct derived : base
    {};

    VX_SECTION("constructors")
    {
        vx::span<int> sp_dyn;
        VX_CHECK(sp_dyn.data() == nullptr);
        VX_CHECK(sp_dyn.size() == 0);
        VX_CHECK(sp_dyn.empty());

        vx::span<int, 0> sp_zero;
        VX_CHECK(sp_zero.data() == nullptr);
        VX_CHECK(sp_zero.size() == 0);
        VX_CHECK(sp_zero.empty());

        func_taking_span<int>({});
        func_taking_span<int, 0>({});
    }

    int arr[3]{ 10, 20, 30 };
    const int const_arr[3]{ 10, 20, 30 };

    vx::array<int, 3> stl{
        { 100, 200, 300 }
    };

    const vx::array<int, 3> const_stl{
        { 100, 200, 300 }
    };

    vx::array<int*, 3> stl_nullptr{
        { nullptr, nullptr, nullptr }
    };

    const vx::array<int*, 3> const_stl_nullptr{
        { nullptr, nullptr, nullptr }
    };

    {
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, int*, size_t>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, int*, uint16_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const int*, size_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, double*, size_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, size_t, size_t>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, int*, size_t>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, int*, uint16_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const int*, size_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, double*, size_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, size_t, size_t>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, int*, size_t>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const int*, size_t>);

        vx::span<int> sp_dyn_a(arr, 3);
        vx::span<int> sp_dyn_b(std::begin(arr), 3);
        VX_CHECK(sp_dyn_a.data() == std::begin(arr));
        VX_CHECK(sp_dyn_b.data() == std::begin(arr));
        VX_CHECK(sp_dyn_a.size() == 3);
        VX_CHECK(sp_dyn_b.size() == 3);

        vx::span<int, 3> sp_three_a(arr, 3);
        vx::span<int, 3> sp_three_b(std::begin(arr), 3);
        VX_CHECK(sp_three_a.data() == std::begin(arr));
        VX_CHECK(sp_three_b.data() == std::begin(arr));
        VX_CHECK(sp_three_a.size() == 3);
        VX_CHECK(sp_three_b.size() == 3);

        vx::span<const int> sp_const_a(arr, 3);
        vx::span<const int> sp_const_b(std::begin(arr), 3);
        VX_CHECK(sp_const_a.data() == std::begin(arr));
        VX_CHECK(sp_const_b.data() == std::begin(arr));
        VX_CHECK(sp_const_a.size() == 3);
        VX_CHECK(sp_const_b.size() == 3);

        vx::span<const int> sp_const_c(static_cast<const int*>(arr), 3);
        vx::span<const int> sp_const_d(std::cbegin(arr), 3);
        VX_CHECK(sp_const_c.data() == std::begin(arr));
        VX_CHECK(sp_const_d.data() == std::begin(arr));
        VX_CHECK(sp_const_c.size() == 3);
        VX_CHECK(sp_const_d.size() == 3);

        func_taking_span<int>({ arr, 3 });
        func_taking_span<int>({ std::begin(arr), 3 });
        func_taking_span<const int>({ arr, 3 });
        func_taking_span<const int>({ std::begin(arr), 3 });
        func_taking_span<const int>({ static_cast<const int*>(arr), 3 });
        func_taking_span<const int>({ std::cbegin(arr), 3 });

        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ arr, 3 }), vx::span<int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ std::begin(arr), 3 }), vx::span<int>>);
        //
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ static_cast<const int*>(arr), 3 }), vx::span<const int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ std::cbegin(arr), 3 }), vx::span<const int>>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, vx::array<int, 3>::iterator, size_t>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, vx::array<int, 3>::const_iterator, size_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, vx::array<double, 3>::iterator, size_t>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, vx::array<int, 3>::iterator, size_t>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, vx::array<int, 3>::const_iterator, size_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, vx::array<double, 3>::iterator, size_t>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, vx::array<int, 3>::iterator, size_t>); // strengthened
        VX_STATIC_CHECK(
            std::is_nothrow_constructible_v<vx::span<const int>, vx::array<int, 3>::const_iterator, size_t>); // strengthened

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, base*, size_t>);    // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, base*, size_t>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, derived*, size_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, derived*, size_t>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, vx::array<base, 3>::iterator, size_t>);    // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, vx::array<base, 3>::iterator, size_t>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, vx::array<derived, 3>::iterator, size_t>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, vx::array<derived, 3>::iterator, size_t>);

        vx::span<int> sp_dyn_x(stl.begin(), 3);
        VX_CHECK(sp_dyn_x.data() == stl.data());
        VX_CHECK(sp_dyn_x.size() == 3);

        vx::span<int, 3> sp_three_x(stl.begin(), 3);
        VX_CHECK(sp_three_x.data() == stl.data());
        VX_CHECK(sp_three_x.size() == 3);

        vx::span<const int> sp_const_x(stl.begin(), 3);
        VX_CHECK(sp_const_x.data() == stl.data());
        VX_CHECK(sp_const_x.size() == 3);

        vx::span<const int> sp_const_y(stl.cbegin(), 3);
        VX_CHECK(sp_const_y.data() == stl.data());
        VX_CHECK(sp_const_y.size() == 3);

        func_taking_span<int>({ stl.begin(), 3 });
        func_taking_span<const int>({ stl.begin(), 3 });
        func_taking_span<const int>({ stl.cbegin(), 3 });

        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ stl.begin(), 3 }), vx::span<int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ stl.cbegin(), 3 }), vx::span<const int>>);
    }

    {
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, int*, int*>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, int*, double*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const int*, int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const int*, const int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const int*, double*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, double*, int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, double*, const int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, double*, double*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, size_t, int*>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, int*, int*>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, int*, double*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const int*, int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const int*, const int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const int*, double*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, double*, int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, double*, const int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, double*, double*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, size_t, int*>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, int*, int*>);             // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, int*, const int*>);       // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const int*, int*>);       // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const int*, const int*>); // strengthened

        vx::span<int> sp_dyn_a(std::begin(arr), std::end(arr));
        VX_CHECK(sp_dyn_a.data() == std::begin(arr));
        VX_CHECK(sp_dyn_a.size() == 3);

        vx::span<int, 3> sp_three_a(std::begin(arr), std::end(arr));
        VX_CHECK(sp_three_a.data() == std::begin(arr));
        VX_CHECK(sp_three_a.size() == 3);

        vx::span<const int> sp_const_a(std::begin(arr), std::end(arr));
        vx::span<const int> sp_const_b(std::begin(arr), std::cend(arr));
        vx::span<const int> sp_const_c(std::cbegin(arr), std::end(arr));
        vx::span<const int> sp_const_d(std::cbegin(arr), std::cend(arr));
        VX_CHECK(sp_const_a.data() == std::begin(arr));
        VX_CHECK(sp_const_b.data() == std::begin(arr));
        VX_CHECK(sp_const_c.data() == std::begin(arr));
        VX_CHECK(sp_const_d.data() == std::begin(arr));
        VX_CHECK(sp_const_a.size() == 3);
        VX_CHECK(sp_const_b.size() == 3);
        VX_CHECK(sp_const_c.size() == 3);
        VX_CHECK(sp_const_d.size() == 3);

        func_taking_span<int>({ std::begin(arr), std::end(arr) });
        func_taking_span<const int>({ std::begin(arr), std::end(arr) });
        func_taking_span<const int>({ std::begin(arr), std::cend(arr) });
        func_taking_span<const int>({ std::cbegin(arr), std::end(arr) });
        func_taking_span<const int>({ std::cbegin(arr), std::cend(arr) });

        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ std::begin(arr), std::end(arr) }), vx::span<int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ std::cbegin(arr), std::cend(arr) }), vx::span<const int>>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, int*, const int*>);    // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, int*, const int*>); // strengthened

        vx::span<int> sp_dyn_b(std::begin(arr), std::cend(arr));
        VX_CHECK(sp_dyn_b.data() == std::begin(arr));
        VX_CHECK(sp_dyn_b.size() == 3);

        vx::span<int, 3> sp_three_b(std::begin(arr), std::cend(arr));
        VX_CHECK(sp_three_b.data() == std::begin(arr));
        VX_CHECK(sp_three_b.size() == 3);

        func_taking_span<int>({ std::begin(arr), std::cend(arr) });

        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ std::begin(arr), std::cend(arr) }), vx::span<int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ std::cbegin(arr), std::end(arr) }), vx::span<const int>>);

        using I = vx::array<int, 3>::iterator;
        using C = vx::array<int, 3>::const_iterator;
        using Z = vx::array<double, 3>::iterator;

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, I, I>); // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, I, C>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, I, Z>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, C, I>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, C, C>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, C, Z>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, Z, I>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, Z, C>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, Z, Z>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, I, I>); // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, I, C>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, I, Z>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, C, I>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, C, C>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, C, Z>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, Z, I>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, Z, C>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, Z, Z>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, I, I>); // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, I, C>); // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, C, I>); // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, C, C>); // strengthened

        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, I, int*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, int*, I>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, base*, base*>);    // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, base*, base*>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, derived*, derived*>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, derived*, derived*>);

        using B = vx::array<base, 3>::iterator;
        using D = vx::array<derived, 3>::iterator;
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, B, B>);    // strengthened
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, B, B>); // strengthened
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, D, D>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, D, D>);

        vx::span<int> sp_dyn_w(stl.begin(), stl.end());
        vx::span<int> sp_dyn_x(stl.begin(), stl.cend());
        VX_CHECK(sp_dyn_w.data() == stl.data());
        VX_CHECK(sp_dyn_x.data() == stl.data());
        VX_CHECK(sp_dyn_w.size() == 3);
        VX_CHECK(sp_dyn_x.size() == 3);

        vx::span<int, 3> sp_three_w(stl.begin(), stl.end());
        vx::span<int, 3> sp_three_x(stl.begin(), stl.cend());
        VX_CHECK(sp_three_w.data() == stl.data());
        VX_CHECK(sp_three_x.data() == stl.data());
        VX_CHECK(sp_three_w.size() == 3);
        VX_CHECK(sp_three_x.size() == 3);

        vx::span<const int> sp_const_w(stl.begin(), stl.end());
        vx::span<const int> sp_const_x(stl.begin(), stl.cend());
        vx::span<const int> sp_const_y(stl.cbegin(), stl.end());
        vx::span<const int> sp_const_z(stl.cbegin(), stl.cend());
        VX_CHECK(sp_const_w.data() == stl.data());
        VX_CHECK(sp_const_x.data() == stl.data());
        VX_CHECK(sp_const_y.data() == stl.data());
        VX_CHECK(sp_const_z.data() == stl.data());
        VX_CHECK(sp_const_w.size() == 3);
        VX_CHECK(sp_const_x.size() == 3);
        VX_CHECK(sp_const_y.size() == 3);
        VX_CHECK(sp_const_z.size() == 3);

        func_taking_span<int>({ stl.begin(), stl.end() });
        func_taking_span<int>({ stl.begin(), stl.cend() });
        func_taking_span<const int>({ stl.begin(), stl.end() });
        func_taking_span<const int>({ stl.begin(), stl.cend() });
        func_taking_span<const int>({ stl.cbegin(), stl.end() });
        func_taking_span<const int>({ stl.cbegin(), stl.cend() });

        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ stl.begin(), stl.end() }), vx::span<int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ stl.begin(), stl.cend() }), vx::span<int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ stl.cbegin(), stl.end() }), vx::span<const int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ stl.cbegin(), stl.cend() }), vx::span<const int>>);
    }

    {
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, double (&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, vx::array<double, 3>&>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, double (&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, vx::array<double, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, int (&)[500]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, vx::array<int, 500>&>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, int (&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const int>, double (&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, vx::array<int, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const int>, vx::array<double, 3>&>);

        VX_STATIC_CHECK(std::is_convertible_v<int (&)[3], vx::span<int>>);
        VX_STATIC_CHECK(std::is_convertible_v<vx::array<int, 3>&, vx::span<int>>);
        VX_STATIC_CHECK(std::is_convertible_v<int (&)[3], vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_convertible_v<vx::array<int, 3>&, vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_convertible_v<int (&)[3], vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const int (&)[3], vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<vx::array<int, 3>&, vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::array<int, 3>&, vx::span<const int>>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, base(&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, vx::array<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, base(&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, vx::array<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, base(&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, const base(&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, vx::array<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, const vx::array<base, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, derived(&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, vx::array<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, derived(&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, vx::array<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, derived(&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, const derived(&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, vx::array<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, const vx::array<derived, 3>&>);

        vx::span<int> sp_dyn_a(arr);
        vx::span<int> sp_dyn_b(stl);
        VX_CHECK(sp_dyn_a.data() == std::begin(arr));
        VX_CHECK(sp_dyn_b.data() == stl.data());
        VX_CHECK(sp_dyn_a.size() == 3);
        VX_CHECK(sp_dyn_b.size() == 3);

        vx::span<int, 3> sp_three_a(arr);
        vx::span<int, 3> sp_three_b(stl);
        VX_CHECK(sp_three_a.data() == std::begin(arr));
        VX_CHECK(sp_three_b.data() == stl.data());
        VX_CHECK(sp_three_a.size() == 3);
        VX_CHECK(sp_three_b.size() == 3);

        vx::span<const int> sp_const_w(arr);
        vx::span<const int> sp_const_x(const_arr);
        vx::span<const int> sp_const_y(stl);
        vx::span<const int> sp_const_z(const_stl);
        vx::span<const int* const> sp_const_nullptr_1{ stl_nullptr };
        vx::span<const int* const> sp_const_nullptr_2{ const_stl_nullptr };
        VX_CHECK(sp_const_w.data() == std::begin(arr));
        VX_CHECK(sp_const_x.data() == std::begin(const_arr));
        VX_CHECK(sp_const_y.data() == stl.data());
        VX_CHECK(sp_const_z.data() == const_stl.data());
        VX_CHECK(sp_const_nullptr_1.data() == stl_nullptr.data());
        VX_CHECK(sp_const_nullptr_2.data() == const_stl_nullptr.data());
        VX_CHECK(sp_const_w.size() == 3);
        VX_CHECK(sp_const_x.size() == 3);
        VX_CHECK(sp_const_y.size() == 3);
        VX_CHECK(sp_const_z.size() == 3);
        VX_CHECK(sp_const_nullptr_1.size() == 3);
        VX_CHECK(sp_const_nullptr_2.size() == 3);

        //func_taking_span<int>(arr);
        //func_taking_span<int>(stl);
        //func_taking_span<int*>(stl_nullptr);
        func_taking_span<int, 3>(arr);
        func_taking_span<int, 3>(stl);
        func_taking_span<int*, 3>(stl_nullptr);
        //func_taking_span<const int>(arr);
        //func_taking_span<const int>(const_arr);
        //func_taking_span<const int>(stl);
        //func_taking_span<const int>(const_stl);
        //func_taking_span<const int* const>(stl_nullptr);
        //func_taking_span<const int* const>(const_stl_nullptr);

        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ arr }), vx::span<int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ const_arr }), vx::span<const int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ stl }), vx::span<int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ const_stl }), vx::span<const int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ stl_nullptr }), vx::span<int*, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(vx::span{ const_stl_nullptr }), vx::span<int* const, 3>>);
    }

    {
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, double (&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, vx::array<double, 3>&>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, double (&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, vx::array<double, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, int (&)[500]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, vx::array<int, 500>&>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, int (&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const int (&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const int>, double (&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, vx::array<int, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const vx::array<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const int>, vx::array<double, 3>&>);

        VX_STATIC_CHECK(std::is_convertible_v<int (&)[3], vx::span<int>>);
        VX_STATIC_CHECK(std::is_convertible_v<vx::array<int, 3>&, vx::span<int>>);
        VX_STATIC_CHECK(std::is_convertible_v<int (&)[3], vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_convertible_v<vx::array<int, 3>&, vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_convertible_v<int (&)[3], vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const int (&)[3], vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<vx::array<int, 3>&, vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::array<int, 3>&, vx::span<const int>>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, base(&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, vx::array<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, base(&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, vx::array<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, base(&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, const base(&)[3]>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, vx::array<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, const vx::array<base, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, derived(&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, vx::array<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, derived(&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, vx::array<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, derived(&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, const derived(&)[3]>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, vx::array<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, const vx::array<derived, 3>&>);

        vx::span<int> sp_dyn_a(arr);
        vx::span<int> sp_dyn_b(stl);
        VX_CHECK(sp_dyn_a.data() == std::begin(arr));
        VX_CHECK(sp_dyn_b.data() == stl.data());
        VX_CHECK(sp_dyn_a.size() == 3);
        VX_CHECK(sp_dyn_b.size() == 3);

        vx::span<int, 3> sp_three_a(arr);
        vx::span<int, 3> sp_three_b(stl);
        VX_CHECK(sp_three_a.data() == std::begin(arr));
        VX_CHECK(sp_three_b.data() == stl.data());
        VX_CHECK(sp_three_a.size() == 3);
        VX_CHECK(sp_three_b.size() == 3);

        vx::span<const int> sp_const_w(arr);
        vx::span<const int> sp_const_x(const_arr);
        vx::span<const int> sp_const_y(stl);
        vx::span<const int> sp_const_z(const_stl);
        vx::span<const int* const> sp_const_nullptr_1{ stl_nullptr };
        vx::span<const int* const> sp_const_nullptr_2{ const_stl_nullptr };
        VX_CHECK(sp_const_w.data() == std::begin(arr));
        VX_CHECK(sp_const_x.data() == std::begin(const_arr));
        VX_CHECK(sp_const_y.data() == stl.data());
        VX_CHECK(sp_const_z.data() == const_stl.data());
        VX_CHECK(sp_const_nullptr_1.data() == stl_nullptr.data());
        VX_CHECK(sp_const_nullptr_2.data() == const_stl_nullptr.data());
        VX_CHECK(sp_const_w.size() == 3);
        VX_CHECK(sp_const_x.size() == 3);
        VX_CHECK(sp_const_y.size() == 3);
        VX_CHECK(sp_const_z.size() == 3);
        VX_CHECK(sp_const_nullptr_1.size() == 3);
        VX_CHECK(sp_const_nullptr_2.size() == 3);

        //func_taking_span<int>(arr);
        //func_taking_span<int>(stl);
        //func_taking_span<int*>(stl_nullptr);
        func_taking_span<int, 3>(arr);
        func_taking_span<int, 3>(stl);
        func_taking_span<int*, 3>(stl_nullptr);
        //func_taking_span<const int>(arr);
        //func_taking_span<const int>(const_arr);
        //func_taking_span<const int>(stl);
        //func_taking_span<const int>(const_stl);
        //func_taking_span<const int* const>(stl_nullptr);
        //func_taking_span<const int* const>(const_stl_nullptr);

        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ arr }), vx::span<int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ const_arr }), vx::span<const int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ stl }), vx::span<int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ const_stl }), vx::span<const int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ stl_nullptr }), vx::span<int*, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ const_stl_nullptr }), vx::span<int* const, 3>>);
    }

    {
        VX_STATIC_CHECK(std::is_nothrow_copy_constructible_v<vx::span<int>>);
        VX_STATIC_CHECK(std::is_nothrow_copy_constructible_v<vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_nothrow_copy_constructible_v<vx::span<const int>>);
        VX_STATIC_CHECK(std::is_nothrow_copy_constructible_v<vx::span<const int, 3>>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, const vx::span<int>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, const vx::span<int, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int>, const vx::span<int, 500>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const vx::span<const int>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const vx::span<const int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const vx::span<const int, 500>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int>, const vx::span<double, 3>&>);

        VX_STATIC_CHECK(std::is_constructible_v<vx::span<int, 3>, const vx::span<int>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<int, 3>, const vx::span<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const vx::span<int, 500>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const vx::span<const int>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const vx::span<const int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const vx::span<const int, 500>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<int, 3>, const vx::span<double, 3>&>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const vx::span<int>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const vx::span<int, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const vx::span<int, 500>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const vx::span<const int>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const vx::span<const int, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int>, const vx::span<const int, 500>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const int>, const vx::span<double, 3>&>);

        VX_STATIC_CHECK(std::is_constructible_v<vx::span<const int, 3>, const vx::span<int>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int, 3>, const vx::span<int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const int, 3>, const vx::span<int, 500>&>);
        VX_STATIC_CHECK(std::is_constructible_v<vx::span<const int, 3>, const vx::span<const int>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const int, 3>, const vx::span<const int, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const int, 3>, const vx::span<const int, 500>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const int, 3>, const vx::span<double, 3>&>);

        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<int>&, vx::span<int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<int, 3>&, vx::span<int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<int, 500>&, vx::span<int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<int, 3>&, vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<int>&, vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<int, 3>&, vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<int, 500>&, vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<const int>&, vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<const int, 3>&, vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<const int, 500>&, vx::span<const int>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<int, 3>&, vx::span<const int, 3>>);
        VX_STATIC_CHECK(std::is_convertible_v<const vx::span<const int, 3>&, vx::span<const int, 3>>);

        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, const vx::span<base>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base>, const vx::span<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<base, 3>, const vx::span<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, const vx::span<base>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, const vx::span<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, const vx::span<const base>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base>, const vx::span<const base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base, 3>, const vx::span<base, 3>&>);
        VX_STATIC_CHECK(std::is_nothrow_constructible_v<vx::span<const base, 3>, const vx::span<const base, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, const vx::span<derived>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base>, const vx::span<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<base, 3>, const vx::span<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, const vx::span<derived>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, const vx::span<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, const vx::span<const derived>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base>, const vx::span<const derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base, 3>, const vx::span<derived, 3>&>);
        VX_STATIC_CHECK(!std::is_constructible_v<vx::span<const base, 3>, const vx::span<const derived, 3>&>);

        const vx::span<int> orig_dyn(arr);
        const vx::span<int, 3> orig_three(arr);
        const vx::span<const int> orig_const_dyn(arr);
        const vx::span<const int, 3> orig_const_three(arr);

        vx::span<int> sp_a(orig_dyn);
        vx::span<int> sp_b(orig_three);

        vx::span<int, 3> sp_c(orig_three);

        vx::span<const int> sp_d(orig_dyn);
        vx::span<const int> sp_e(orig_three);
        vx::span<const int> sp_f(orig_const_dyn);
        vx::span<const int> sp_g(orig_const_three);

        vx::span<const int, 3> sp_h(orig_three);
        vx::span<const int, 3> sp_i(orig_const_three);

        VX_CHECK(sp_a.data() == std::begin(arr));
        VX_CHECK(sp_b.data() == std::begin(arr));
        VX_CHECK(sp_c.data() == std::begin(arr));
        VX_CHECK(sp_d.data() == std::begin(arr));
        VX_CHECK(sp_e.data() == std::begin(arr));
        VX_CHECK(sp_f.data() == std::begin(arr));
        VX_CHECK(sp_g.data() == std::begin(arr));
        VX_CHECK(sp_h.data() == std::begin(arr));
        VX_CHECK(sp_i.data() == std::begin(arr));
        VX_CHECK(sp_a.size() == 3);
        VX_CHECK(sp_b.size() == 3);
        VX_CHECK(sp_c.size() == 3);
        //VX_CHECK(sp_d.size() == 3);
        //VX_CHECK(sp_e.size() == 3);
        //VX_CHECK(sp_f.size() == 3);
        //VX_CHECK(sp_g.size() == 3);
        VX_CHECK(sp_h.size() == 3);
        VX_CHECK(sp_i.size() == 3);

        func_taking_span<int>(orig_dyn);
        func_taking_span<int>(orig_three);
        func_taking_span<int, 3>(orig_three);
        //func_taking_span<const int>(orig_dyn);
        //func_taking_span<const int>(orig_three);
        func_taking_span<const int>(orig_const_dyn);
        func_taking_span<const int>(orig_const_three);
        func_taking_span<const int, 3>(orig_three);
        func_taking_span<const int, 3>(orig_const_three);

        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ orig_dyn }), vx::span<int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ orig_three }), vx::span<int, 3>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ orig_const_dyn }), vx::span<const int>>);
        //VX_STATIC_CHECK(std::is_same_v<decltype(span{ orig_const_three }), vx::span<const int, 3>>);
    }

    {
        VX_STATIC_CHECK(std::is_nothrow_copy_assignable_v<vx::span<int>>);
        VX_STATIC_CHECK(std::is_nothrow_copy_assignable_v<vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_nothrow_copy_assignable_v<vx::span<const int>>);
        VX_STATIC_CHECK(std::is_nothrow_copy_assignable_v<vx::span<const int, 3>>);

        vx::span<int> sp_dyn(arr);
        vx::span<int, 3> sp_three(arr);
        vx::span<const int> sp_const_dyn(arr);
        vx::span<const int, 3> sp_const_three(arr);

        VX_CHECK(sp_dyn.data() == std::begin(arr));
        VX_CHECK(sp_three.data() == std::begin(arr));
        VX_CHECK(sp_const_dyn.data() == std::begin(arr));
        VX_CHECK(sp_const_three.data() == std::begin(arr));
        VX_CHECK(sp_dyn.size() == 3);
        VX_CHECK(sp_three.size() == 3);
        VX_CHECK(sp_const_dyn.size() == 3);
        VX_CHECK(sp_const_three.size() == 3);

        int other[4]{ 12, 34, 56, 78 };

        sp_dyn = vx::span<int>{ other };
        sp_three = vx::span<int, 3>{ stl };
        sp_const_dyn = vx::span<const int>{ other };
        sp_const_three = vx::span<const int, 3>{ stl };

        VX_CHECK(sp_dyn.data() == std::begin(other));
        VX_CHECK(sp_three.data() == stl.data());
        VX_CHECK(sp_const_dyn.data() == std::begin(other));
        VX_CHECK(sp_const_three.data() == stl.data());
        VX_CHECK(sp_dyn.size() == 4);
        VX_CHECK(sp_three.size() == 3);
        VX_CHECK(sp_const_dyn.size() == 4);
        VX_CHECK(sp_const_three.size() == 3);
    }

    {
        VX_STATIC_CHECK(std::is_nothrow_destructible_v<vx::span<int>>);
        VX_STATIC_CHECK(std::is_nothrow_destructible_v<vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_nothrow_destructible_v<vx::span<const int>>);

        vx::span<int>::iterator it_dyn{};

        {
           vx::span<int> sp_dyn(arr);
            it_dyn = sp_dyn.begin();
        }

        VX_CHECK(*it_dyn == arr[0]);
        VX_CHECK(it_dyn[2] == arr[2]);

        vx::span<int, 3>::iterator it_three{};

        {
           vx::span<int, 3> sp_three(stl);
            it_three = sp_three.begin();
        }

        VX_CHECK(*it_three == stl[0]);
        VX_CHECK(it_three[2] == stl[2]);
    }

    {
        int sequence[9]{ 10, 20, 30, 40, 50, 60, 70, 80, 90 };

        const vx::span<int> sp_dyn(sequence);
        const vx::span<int, 9> sp_nine(sequence);

        auto first_3 = sp_dyn.first<3>();
        auto first_4 = sp_nine.first<4>();
        auto first_5 = sp_dyn.first(5);
        auto first_6 = sp_nine.first(6);
        VX_STATIC_CHECK(noexcept(sp_dyn.first<3>()));  // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.first<4>())); // strengthened
        VX_STATIC_CHECK(noexcept(sp_dyn.first(5)));    // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.first(6)));   // strengthened
        VX_STATIC_CHECK(std::is_same_v<decltype(first_3), vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(first_4), vx::span<int, 4>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(first_5), vx::span<int>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(first_6), vx::span<int>>);
        VX_CHECK(first_3.data() == std::begin(sequence));
        VX_CHECK(first_4.data() == std::begin(sequence));
        VX_CHECK(first_5.data() == std::begin(sequence));
        VX_CHECK(first_6.data() == std::begin(sequence));
        VX_CHECK(first_3.size() == 3);
        VX_CHECK(first_4.size() == 4);
        VX_CHECK(first_5.size() == 5);
        VX_CHECK(first_6.size() == 6);

        auto last_3 = sp_dyn.last<3>();
        auto last_4 = sp_nine.last<4>();
        auto last_5 = sp_dyn.last(5);
        auto last_6 = sp_nine.last(6);
        VX_STATIC_CHECK(noexcept(sp_dyn.last<3>()));  // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.last<4>())); // strengthened
        VX_STATIC_CHECK(noexcept(sp_dyn.last(5)));    // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.last(6)));   // strengthened
        VX_STATIC_CHECK(std::is_same_v<decltype(last_3), vx::span<int, 3>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(last_4), vx::span<int, 4>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(last_5), vx::span<int>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(last_6), vx::span<int>>);
        VX_CHECK(last_3.data() == std::begin(sequence) + 6);
        VX_CHECK(last_4.data() == std::begin(sequence) + 5);
        VX_CHECK(last_5.data() == std::begin(sequence) + 4);
        VX_CHECK(last_6.data() == std::begin(sequence) + 3);
        VX_CHECK(last_3.size() == 3);
        VX_CHECK(last_4.size() == 4);
        VX_CHECK(last_5.size() == 5);
        VX_CHECK(last_6.size() == 6);

        auto offset_3 = sp_dyn.subspan<3>();
        auto offset_4 = sp_nine.subspan<4>();
        auto offset_5 = sp_dyn.subspan(5);
        auto offset_6 = sp_nine.subspan(6);
        VX_STATIC_CHECK(noexcept(sp_dyn.subspan<3>()));  // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.subspan<4>())); // strengthened
        VX_STATIC_CHECK(noexcept(sp_dyn.subspan(5)));    // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.subspan(6)));   // strengthened
        VX_STATIC_CHECK(std::is_same_v<decltype(offset_3), vx::span<int>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(offset_4), vx::span<int, 5>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(offset_5), vx::span<int>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(offset_6), vx::span<int>>);
        VX_CHECK(offset_3.data() == std::begin(sequence) + 3);
        VX_CHECK(offset_4.data() == std::begin(sequence) + 4);
        VX_CHECK(offset_5.data() == std::begin(sequence) + 5);
        VX_CHECK(offset_6.data() == std::begin(sequence) + 6);
        VX_CHECK(offset_3.size() == 6);
        VX_CHECK(offset_4.size() == 5);
        VX_CHECK(offset_5.size() == 4);
        VX_CHECK(offset_6.size() == 3);

        auto subspan_3 = sp_dyn.subspan<3, 2>();
        auto subspan_4 = sp_nine.subspan<4, 2>();
        auto subspan_5 = sp_dyn.subspan(5, 2);
        auto subspan_6 = sp_nine.subspan(6, 2);
        VX_STATIC_CHECK(noexcept(sp_dyn.subspan<3, 2>()));  // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.subspan<4, 2>())); // strengthened
        VX_STATIC_CHECK(noexcept(sp_dyn.subspan(5, 2)));    // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.subspan(6, 2)));   // strengthened
        VX_STATIC_CHECK(std::is_same_v<decltype(subspan_3), vx::span<int, 2>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(subspan_4), vx::span<int, 2>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(subspan_5), vx::span<int>>);
        VX_STATIC_CHECK(std::is_same_v<decltype(subspan_6), vx::span<int>>);
        VX_CHECK(subspan_3.data() == std::begin(sequence) + 3);
        VX_CHECK(subspan_4.data() == std::begin(sequence) + 4);
        VX_CHECK(subspan_5.data() == std::begin(sequence) + 5);
        VX_CHECK(subspan_6.data() == std::begin(sequence) + 6);
        VX_CHECK(subspan_3.size() == 2);
        VX_CHECK(subspan_4.size() == 2);
        VX_CHECK(subspan_5.size() == 2);
        VX_CHECK(subspan_6.size() == 2);

        VX_STATIC_CHECK(noexcept(sp_dyn.size()));
        VX_STATIC_CHECK(noexcept(sp_dyn.size_bytes()));
        VX_STATIC_CHECK(noexcept(sp_dyn.empty()));
        VX_STATIC_CHECK(noexcept(sp_dyn[0]));      // strengthened
        VX_STATIC_CHECK(noexcept(sp_dyn.front())); // strengthened
        VX_STATIC_CHECK(noexcept(sp_dyn.back()));  // strengthened
        VX_STATIC_CHECK(noexcept(sp_dyn.data()));
        VX_STATIC_CHECK(noexcept(sp_dyn.begin()));
        VX_STATIC_CHECK(noexcept(sp_dyn.end()));
        VX_STATIC_CHECK(noexcept(sp_dyn.rbegin()));
        VX_STATIC_CHECK(noexcept(sp_dyn.rend()));

        VX_STATIC_CHECK(noexcept(sp_nine.size()));
        VX_STATIC_CHECK(noexcept(sp_nine.size_bytes()));
        VX_STATIC_CHECK(noexcept(sp_nine.empty()));
        VX_STATIC_CHECK(noexcept(sp_nine[0]));      // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.front())); // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.back()));  // strengthened
        VX_STATIC_CHECK(noexcept(sp_nine.data()));
        VX_STATIC_CHECK(noexcept(sp_nine.begin()));
        VX_STATIC_CHECK(noexcept(sp_nine.end()));
        VX_STATIC_CHECK(noexcept(sp_nine.rbegin()));
        VX_STATIC_CHECK(noexcept(sp_nine.rend()));

        VX_CHECK(sp_dyn.size() == 9);
        VX_CHECK(sp_nine.size() == 9);

        VX_CHECK(sp_dyn.size_bytes() == 9 * sizeof(int));
        VX_CHECK(sp_nine.size_bytes() == 9 * sizeof(int));

        VX_CHECK(!sp_dyn.empty());
        VX_CHECK(!sp_nine.empty());

        VX_CHECK(sp_dyn[0] == 10);
        VX_CHECK(sp_nine[0] == 10);
        VX_CHECK(sp_dyn[8] == 90);
        VX_CHECK(sp_nine[8] == 90);

        VX_CHECK(sp_dyn.front() == 10);
        VX_CHECK(sp_nine.front() == 10);

        VX_CHECK(sp_dyn.back() == 90);
        VX_CHECK(sp_nine.back() == 90);

        VX_CHECK(&sp_dyn.front() == std::begin(sequence));
        VX_CHECK(&sp_nine.front() == std::begin(sequence));
        VX_CHECK(&sp_dyn[4] == std::begin(sequence) + 4);
        VX_CHECK(&sp_nine[4] == std::begin(sequence) + 4);
        VX_CHECK(&sp_dyn.back() == std::begin(sequence) + 8);
        VX_CHECK(&sp_nine.back() == std::begin(sequence) + 8);

        VX_CHECK(sp_dyn.data() == std::begin(sequence));
        VX_CHECK(sp_nine.data() == std::begin(sequence));

        VX_CHECK(*sp_dyn.begin() == 10);
        VX_CHECK(*sp_nine.begin() == 10);

        VX_CHECK(sp_dyn.end()[-2] == 80);
        VX_CHECK(sp_nine.end()[-2] == 80);

        VX_CHECK(*sp_dyn.rbegin() == 90);
        VX_CHECK(*sp_nine.rbegin() == 90);

        VX_CHECK(sp_dyn.rend()[-2] == 20);
        VX_CHECK(sp_nine.rend()[-2] == 20);

        VX_STATIC_CHECK(std::is_same_v<decltype(sp_dyn.begin()), vx::span<int>::iterator>);
        VX_STATIC_CHECK(std::is_same_v<decltype(sp_nine.begin()), vx::span<int, 9>::iterator>);
        VX_STATIC_CHECK(std::is_same_v<decltype(sp_dyn.end()), vx::span<int>::iterator>);
        VX_STATIC_CHECK(std::is_same_v<decltype(sp_nine.end()), vx::span<int, 9>::iterator>);
        VX_STATIC_CHECK(std::is_same_v<decltype(sp_dyn.rbegin()), vx::span<int>::reverse_iterator>);
        VX_STATIC_CHECK(std::is_same_v<decltype(sp_nine.rbegin()), vx::span<int, 9>::reverse_iterator>);
        VX_STATIC_CHECK(std::is_same_v<decltype(sp_dyn.rend()), vx::span<int>::reverse_iterator>);
        VX_STATIC_CHECK(std::is_same_v<decltype(sp_nine.rend()), vx::span<int, 9>::reverse_iterator>);
    }
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
