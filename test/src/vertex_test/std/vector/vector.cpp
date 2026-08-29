#include "vertex/std/vector.hpp"
#include "vertex_test/test.hpp"

// https://github.com/microsoft/STL/blob/e2ef398685f7e470dbaeaf65ff919de72bda7489/tests/tr1/tests/vector/test.cpp

//=========================================================================
// constructors
//=========================================================================

static void test_container()
{
    using vec = vx::vector<char>;
    using alloc = vx::mem::default_allocator<char>;

    vec v0;
    alloc al = v0.get_allocator();
    vec v0a(al);
    VX_CHECK(v0.empty());
    VX_CHECK(v0.size() == 0);
    VX_CHECK(v0a.size() == 0);
    VX_CHECK(v0a.get_allocator() == al);

    vec v1(5);
    vec v1a(6, 'x');
    vec v1b(7, 'y', al);
    VX_CHECK(v1.size() == 5);
    VX_CHECK(v1a.size() == 6);
    VX_CHECK(v1a.back().value() == 'x');
    VX_CHECK(v1b.size() == 7);
    VX_CHECK(v1b.back().value() == 'y');

    vec v2(v1a);
    VX_CHECK(v2.size() == 6);
    VX_CHECK(v2.front().value() == 'x');

    vec v2a(v2, al);
    VX_CHECK(v2a.size() == 6);
    VX_CHECK(v2a.front().value() == 'x');

    vec v3(v1a.begin(), v1a.end());
    VX_CHECK(v3.size() == 6);
    VX_CHECK(v3.front().value() == 'x');

    const vec v4(v1a.begin(), v1a.end(), al);
    v0 = v4;
    VX_CHECK(v0.size() == 6);
    VX_CHECK(v0.front().value() == 'x');
    VX_CHECK(v0[0] == 'x');
    VX_CHECK(v0.at(5).value() == 'x');

    v0.reserve(12);
    VX_CHECK(12 <= v0.capacity());
    v0.resize(8);
    VX_CHECK(v0.size() == 8);
    v0.resize(10, 'z');
    VX_CHECK(v0.size() == 10);
    VX_CHECK(v0.back().value() == 'z');
    VX_CHECK(v0.size() <= v0.max_size());

    VX_SECTION("iterators")
    {
        typename vec::iterator p_it(v0.begin());
        typename vec::const_iterator p_cit(v4.begin());
        typename vec::reverse_iterator p_rit(v0.rbegin());
        typename vec::const_reverse_iterator p_crit(v4.rbegin());
        VX_CHECK(*p_it == 'x');
        VX_CHECK(*--(p_it = v0.end()) == 'z');
        VX_CHECK(*p_cit == 'x');
        VX_CHECK(*--(p_cit = v4.end()) == 'x');
        VX_CHECK(*p_rit == 'z');
        VX_CHECK(*--(p_rit = v0.rend()) == 'x');
        VX_CHECK(*p_crit == 'x');
        VX_CHECK(*--(p_crit = v4.rend()) == 'x');

        typename vec::const_iterator p_it1 = typename vec::const_iterator();
        typename vec::const_iterator p_it2 = typename vec::const_iterator();
        VX_CHECK(p_it1 == p_it2); // VX_CHECK null forward iterator comparisons
    }

    VX_SECTION("const iterators")
    {
        typename vec::const_iterator p_it(v0.cbegin());
        typename vec::const_iterator p_cit(v4.cbegin());
        typename vec::const_reverse_iterator p_rit(v0.crbegin());
        typename vec::const_reverse_iterator p_crit(v4.crbegin());
        VX_CHECK(*p_it == 'x');
        VX_CHECK(*--(p_it = v0.cend()) == 'z');
        VX_CHECK(*p_cit == 'x');
        VX_CHECK(*--(p_cit = v4.cend()) == 'x');
        VX_CHECK(*p_rit == 'z');
        VX_CHECK(*--(p_rit = v0.crend()) == 'x');
        VX_CHECK(*p_crit == 'x');
        VX_CHECK(*--(p_crit = v4.crend()) == 'x');

        typename vec::const_iterator p_it1 = typename vec::const_iterator();
        typename vec::const_iterator p_it2 = typename vec::const_iterator();
        VX_CHECK(p_it1 == p_it2); // VX_CHECK null forward iterator comparisons
    }

    VX_CHECK(v0.front().value() == 'x');
    VX_CHECK(v4.front().value() == 'x');

    v0.push_back('a');
    VX_CHECK(v0.back().value() == 'a');
    v0.pop_back();
    VX_CHECK(v0.back().value() == 'z');
    VX_CHECK(v4.back().value() == 'x');

    {
        vec v5;
        v5.resize(10, '\0');
        VX_CHECK(v5.size() == 10);
        VX_CHECK(v5[9] == 0);

        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        vec v6(20, 'x');
        vec v7(std::move(v6));
        VX_CHECK(v6.size() == 0);
        VX_CHECK(v7.size() == 20);

        vec v8;
        v8 = std::move(v7);
        VX_CHECK(v7.size() == 0);
        VX_CHECK(v8.size() == 20);

        // matching allocator -> takes the acquire() fast path rather than
        // the real element-wise move (construct_method::move_range)
        vec v8a(std::move(v8), alloc());
        VX_CHECK(v8.size() == 0);
        VX_CHECK(v8a.size() == 20);

        VX_DISABLE_WARNING_POP();

        vec v9;
        v9.resize(10, '\0');
        VX_CHECK(v9.size() == 10);
        VX_CHECK(v9[9] == 0);

        using movable_int = vx::test::movable_int;
        vx::vector<movable_int> v10;
        movable_int mi1(1);
        v10.push_back(std::move(mi1));
        VX_CHECK(mi1.val == -1);
        VX_CHECK(v10[0].val == 1);

        movable_int mi3(3);
        v10.insert(v10.begin(), std::move(mi3));
        VX_CHECK(mi3.val == -1);
        VX_CHECK(v10[0].val == 3);
        VX_CHECK(v10[1].val == 1);

        v10.emplace_back();
        VX_CHECK(v10.back().value().val == 0);
        v10.emplace_back(2);
        VX_CHECK(v10.back().value().val == 2);
        v10.emplace_back(3, 2);
        VX_CHECK(v10.back().value().val == 0x32);
        v10.emplace_back(4, 3, 2);
        VX_CHECK(v10.back().value().val == 0x432);
        v10.emplace_back(5, 4, 3, 2);
        VX_CHECK(v10.back().value().val == 0x5432);
        v10.emplace_back(6, 5, 4, 3, 2);
        VX_CHECK(v10.back().value().val == 0x65432);

        v10.emplace(v10.begin() + 1);
        VX_CHECK(v10[1].val == 0);
        v10.emplace(v10.begin() + 1, 2);
        VX_CHECK(v10[1].val == 2);
        v10.emplace(v10.begin() + 1, 3, 2);
        VX_CHECK(v10[1].val == 0x32);
        v10.emplace(v10.begin() + 1, 4, 3, 2);
        VX_CHECK(v10[1].val == 0x432);
        v10.emplace(v10.begin() + 1, 5, 4, 3, 2);
        VX_CHECK(v10[1].val == 0x5432);
        v10.emplace(v10.begin() + 1, 6, 5, 4, 3, 2);
        VX_CHECK(v10[1].val == 0x65432);
    }

    VX_SECTION("lvalue stealing")
    {
        using copyable_int = vx::test::copyable_int;
        vx::vector<copyable_int> v11;
        copyable_int ci1(1);
        v11.push_back(ci1);
        VX_CHECK(ci1.val == 1);
        VX_CHECK(v11[0].val == 1);

        copyable_int ci3(3);
        v11.insert(v11.begin(), ci3);
        VX_CHECK(ci3.val == 3);
        VX_CHECK(v11[0].val == 3);
        VX_CHECK(v11[1].val == 1);

        vx::vector<copyable_int> v12(v11);
        VX_CHECK(v11 == v12);
        v11 = v12;
        VX_CHECK(v11 == v12);
    }

    VX_SECTION("front/back")
    {
        char carr[] = "abc";

        typename vec::iterator p_it;
        v0.assign(v4.begin(), v4.end());
        VX_CHECK(v0.size() == v4.size());
        VX_CHECK(v0.front().value() == v4.front().value());
        v0.assign(4, 'w');
        VX_CHECK(v0.size() == 4);
        VX_CHECK(v0.front().value() == 'w');
        VX_CHECK(*v0.insert(v0.begin(), 'a') == 'a');
        VX_CHECK(v0.front().value() == 'a');
        VX_CHECK(*++(p_it = v0.begin()) == 'w');
        VX_CHECK(*v0.insert(v0.begin(), 2, 'b') == 'b');
        VX_CHECK(v0.front().value() == 'b');
        VX_CHECK(*++(p_it = v0.begin()) == 'b');
        VX_CHECK(*++ ++(p_it = v0.begin()) == 'a');
        VX_CHECK(*v0.insert(v0.end(), v4.begin(), v4.end()) == *v4.begin());
        VX_CHECK(v0.back().value() == v4.back().value());
        VX_CHECK(*v0.insert(v0.end(), carr, carr + 3) == *carr);
        VX_CHECK(v0.back().value() == 'c');
        v0.erase(v0.begin());
        VX_CHECK(v0.front().value() == 'b');
        VX_CHECK(*++(p_it = v0.begin()) == 'a');
        v0.erase(v0.begin(), ++(p_it = v0.begin()));
        VX_CHECK(v0.front().value() == 'a');
    }

    {
        vec v0x;

        char* pd = v0x.data();
        VX_CHECK(pd == nullptr);
        v0x.push_back('a');
        VX_CHECK(*v0x.data() == 'a');

        v0x.shrink_to_fit();
        VX_CHECK(*v0x.data() == 'a');
    }

    {
        std::initializer_list<char> init{ 'a', 'b', 'c' };
        vec v11(init);
        VX_CHECK(v11.size() == 3);
        VX_CHECK(v11[2] == 'c');

        v11.clear();
        v11 = init;
        VX_CHECK(v11.size() == 3);
        VX_CHECK(v11[2] == 'c');

        v11.insert(v11.begin() + 1, init);
        VX_CHECK(v11.size() == 6);
        VX_CHECK(v11[2] == 'b');

        v11.assign(init);
        VX_CHECK(v11.size() == 3);
        VX_CHECK(v11[2] == 'c');
    }

    v0.clear();
    VX_CHECK(v0.empty());
    v0.swap(v1);
    VX_CHECK(!v0.empty());
    VX_CHECK(v1.empty());
    std::swap(v0, v1);
    VX_CHECK(v0.empty());
    VX_CHECK(!v1.empty());
    VX_CHECK(v1 == v1);
    VX_CHECK(v0 < v1);
    VX_CHECK(v0 != v1);
    VX_CHECK(v1 > v0);
    VX_CHECK(v0 <= v1);
    VX_CHECK(v1 >= v0);
}

VX_TEST_CASE(container)
{
    test_container();
}

//=========================================================================
// element access errors: front / back / at
//=========================================================================

static void test_element_access_errors()
{
    using vec = vx::vector<int>;

    vec empty_v;
    VX_CHECK_EXPECTED_ERROR(empty_v.front(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(empty_v.back(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(empty_v.at(0), vx::err::out_of_range);

    const vec const_empty_v;
    VX_CHECK_EXPECTED_ERROR(const_empty_v.front(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(const_empty_v.back(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(const_empty_v.at(0), vx::err::out_of_range);

    vec v{ 1, 2, 3 };
    VX_CHECK(v.front().value() == 1);
    VX_CHECK(v.back().value() == 3);
    VX_CHECK(v.at(1).value() == 2);
    VX_CHECK_EXPECTED_ERROR(v.at(3), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(v.at(1000), vx::err::out_of_range);

    const vec cv{ 4, 5, 6 };
    VX_CHECK(cv.front().value() == 4);
    VX_CHECK(cv.back().value() == 6);
    VX_CHECK(cv.at(2).value() == 6);
    VX_CHECK_EXPECTED_ERROR(cv.at(3), vx::err::out_of_range);

    // clearing empties it back out again
    v.clear();
    VX_CHECK_EXPECTED_ERROR(v.front(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(v.back(), vx::err::out_of_range);
}

VX_TEST_CASE(element_access_errors)
{
    test_element_access_errors();
}

//=========================================================================
// erase() offset-based error handling
//=========================================================================

static void test_erase_errors()
{
    using vec = vx::vector<int>;

    vec v{ 1, 2, 3, 4, 5 };

    // single-element erase, off == size and off > size both invalid
    VX_CHECK_EXPECTED_ERROR(v.erase(v.size()), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(v.erase(v.size() + 1), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(v.erase(1000), vx::err::out_of_range);

    // range erase: off beyond size
    VX_CHECK_EXPECTED_ERROR(v.erase(v.size() + 1, 1), vx::err::out_of_range);
    // range erase: off valid but count runs past the end
    VX_CHECK_EXPECTED_ERROR(v.erase(3, 5), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(v.erase(0, v.size() + 1), vx::err::out_of_range);

    // vector must be untouched by all the failed calls above
    VX_CHECK(v.size() == 5);
    VX_CHECK(v[0] == 1);
    VX_CHECK(v[4] == 5);

    // sanity: valid single erase still succeeds and returns a value
    auto e1 = v.erase(1);
    VX_CHECK(e1);
    VX_CHECK(v.size() == 4);
    VX_CHECK(v[1] == 3); // 2 was removed

    // sanity: valid range erase still succeeds
    auto e2 = v.erase(0, 2);
    VX_CHECK(e2);
    VX_CHECK(v.size() == 2);

    // erase(off, 0) at a valid offset is a legal no-op, not an error
    auto e3 = v.erase(0, 0);
    VX_CHECK(e3);
    VX_CHECK(v.size() == 2);

    // erasing everything via a full range is valid too
    auto e4 = v.erase(0, v.size());
    VX_CHECK(e4);
    VX_CHECK(v.empty());
}

VX_TEST_CASE(erase_errors)
{
    test_erase_errors();
}

//=========================================================================
// offset-based insert() / emplace() error handling
//=========================================================================

static void test_offset_insert_errors()
{
    using vec = vx::vector<int>;

    vec v{ 1, 2, 3 };
    const vec::size_type bad_off = v.size() + 1;

    VX_CHECK_EXPECTED_ERROR(v.insert(bad_off, 99), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(v.emplace(bad_off, 99), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(v.insert(bad_off, 3, 99), vx::err::out_of_range);

    std::initializer_list<int> init{ 7, 8, 9 };
    VX_CHECK_EXPECTED_ERROR(v.insert(bad_off, init), vx::err::out_of_range);

    int carr[] = { 10, 11, 12 };
    VX_CHECK_EXPECTED_ERROR(v.insert(bad_off, carr, carr + 3), vx::err::out_of_range);

    // vector must be untouched by all the failed calls above
    VX_CHECK(v.size() == 3);
    VX_CHECK(v[0] == 1);
    VX_CHECK(v[2] == 3);

    // off == size() is one-past-the-end and IS valid (append)
    auto ok_append = v.insert(v.size(), 42);
    VX_CHECK(ok_append);
    VX_CHECK(v.back().value() == 42);

    // sanity: valid offset insert still succeeds for every overload
    vec v2{ 1, 2, 3 };
    auto i1 = v2.insert(0, 100);
    VX_CHECK(i1);
    VX_CHECK(v2.front().value() == 100);

    auto i2 = v2.emplace(1, 200);
    VX_CHECK(i2);
    VX_CHECK(v2[1] == 200);

    auto i3 = v2.insert(0, 2, 300);
    VX_CHECK(i3);
    VX_CHECK(v2[0] == 300);
    VX_CHECK(v2[1] == 300);

    auto i4 = v2.insert(0, init);
    VX_CHECK(i4);
    VX_CHECK(v2[0] == 7);

    auto i5 = v2.insert(0, carr, carr + 3);
    VX_CHECK(i5);
    VX_CHECK(v2[0] == 10);
}

VX_TEST_CASE(offset_insert_errors)
{
    test_offset_insert_errors();
}

//=========================================================================
// capacity / size_error handling: reserve, resize, offset insert overflow
//=========================================================================

static void test_size_error_handling()
{
    using vec = vx::vector<int>;

    vec v{ 1, 2, 3 };
    const vec::size_type too_big = static_cast<vec::size_type>(-1);

    VX_CHECK_ERROR(v.reserve(too_big), vx::err::size_error);
    VX_CHECK_ERROR(v.resize(too_big), vx::err::size_error);
    VX_CHECK_ERROR(v.resize(too_big, 42), vx::err::size_error);

    // vector must be untouched by all the failed calls above
    VX_CHECK(v.size() == 3);
    VX_CHECK(v[0] == 1);

    // insert_reallocate's own size_error check, distinct from reserve's:
    // requesting more elements than fit even with a fresh reallocation
    VX_CHECK_EXPECTED_ERROR(v.insert(0, too_big, 99), vx::err::size_error);

    // sanity: reasonable reserve/resize calls still succeed
    VX_CHECK(v.reserve(100));
    VX_CHECK(v.capacity() >= 100);

    VX_CHECK(v.resize(10));
    VX_CHECK(v.size() == 10);

    VX_CHECK(v.resize(2));
    VX_CHECK(v.size() == 2);

    VX_CHECK(v.resize(5, 7));
    VX_CHECK(v.size() == 5);
    VX_CHECK(v.back().value() == 7);
}

VX_TEST_CASE(size_error_handling)
{
    test_size_error_handling();
}

//=========================================================================
// assign() error handling
//=========================================================================

static void test_assign_errors()
{
    using vec = vx::vector<int>;

    vec v{ 1, 2, 3 };
    const vec::size_type too_big = static_cast<vec::size_type>(-1);

    // count > max_size() is checked before the source is ever touched,
    // so it's safe to pass small/short-lived sources here
    VX_CHECK_ERROR(v.assign(too_big, 42), vx::err::size_error);

    int carr[] = { 1, 2, 3 };
    VX_CHECK_ERROR(v.assign(carr, too_big), vx::err::size_error);

    // vector must be untouched by all the failed calls above
    VX_CHECK(v.size() == 3);
    VX_CHECK(v[0] == 1);

    // sanity: valid assigns still succeed, growing, shrinking, and
    // exactly-matching-size cases (each takes a different internal branch)
    VX_CHECK(v.assign(5, 9)); // grow: count > size
    VX_CHECK(v.size() == 5);
    VX_CHECK(v[0] == 9);

    VX_CHECK(v.assign(5, 9)); // exact match: count == size
    VX_CHECK(v.size() == 5);

    VX_CHECK(v.assign(2, 9)); // shrink: count < size
    VX_CHECK(v.size() == 2);

    VX_CHECK(v.assign(carr, 3)); // pointer overload, grows via realloc
    VX_CHECK(v.size() == 3);
    VX_CHECK(v[0] == 1);

    std::initializer_list<int> init{ 4, 5, 6, 7 };
    VX_CHECK(v.assign(init));
    VX_CHECK(v.size() == 4);

    vec other{ 8, 9 };
    VX_CHECK(v.assign(other));
    VX_CHECK(v.size() == 2);
    VX_CHECK(v[0] == 8);

    // self-assign is a documented no-op / early-return success, not an error
    VX_CHECK(v.assign(v));
    VX_CHECK(v.size() == 2);
}

VX_TEST_CASE(assign_errors)
{
    test_assign_errors();
}

//=========================================================================
// fallible construction via vector::create()
//=========================================================================

static void test_fallible_construction()
{
    using vec = vx::vector<int>;
    using alloc = vx::mem::default_allocator<int>;

    auto c0 = vec::create();
    VX_CHECK(c0);
    VX_CHECK(c0.value().empty());

    alloc al;
    auto c0a = vec::create(al);
    VX_CHECK(c0a);
    VX_CHECK(c0a.value().get_allocator() == al);

    auto c1 = vec::create(5);
    VX_CHECK(c1);
    VX_CHECK(c1.value().size() == 5);

    auto c1a = vec::create(6, 7);
    VX_CHECK(c1a);
    VX_CHECK(c1a.value().size() == 6);
    VX_CHECK(c1a.value().back().value() == 7);

    std::initializer_list<int> init{ 1, 2, 3 };
    auto c2 = vec::create(init);
    VX_CHECK(c2);
    VX_CHECK(c2.value().size() == 3);
    VX_CHECK(c2.value()[2] == 3);

    vec src_copy{ 10, 20, 30 };
    auto c3 = vec::create(src_copy);
    VX_CHECK(c3);
    VX_CHECK(c3.value().size() == 3);
    VX_CHECK(c3.value()[0] == 10);
    VX_CHECK(src_copy.size() == 3); // source untouched by a copy-create

    vec src_move{ 100, 200 };
    auto c4 = vec::create(std::move(src_move));
    VX_CHECK(c4);
    VX_CHECK(c4.value().size() == 2);
    VX_CHECK(c4.value()[0] == 100);

    VX_DISABLE_USE_AFTER_MOVE_WARNING();
    VX_CHECK(src_move.empty()); // acquire() fast path: same default allocator
    VX_DISABLE_WARNING_POP();

    // pointer-forward iterator path (vx::vector::iterator)
    vec src_iter{ 1, 2, 3, 4 };
    auto c5 = vec::create(src_iter.begin(), src_iter.end());
    VX_CHECK(c5);
    VX_CHECK(c5.value().size() == 4);
    VX_CHECK(c5.value()[3] == 4);

    // generic (non-vx-pointer) iterator path, e.g. raw pointers / std types
    int carr[] = { 5, 6, 7 };
    auto c6 = vec::create(carr, carr + 3);
    VX_CHECK(c6);
    VX_CHECK(c6.value().size() == 3);
    VX_CHECK(c6.value()[0] == 5);

    std::vector<int> std_src{ 9, 8, 7 };
    auto c7 = vec::create(std_src.begin(), std_src.end());
    VX_CHECK(c7);
    VX_CHECK(c7.value().size() == 3);
    VX_CHECK(c7.value()[1] == 8);

    // error paths: requesting more than max_size()
    const vec::size_type too_big = static_cast<vec::size_type>(-1);

    auto ce1 = vec::create(too_big);
    VX_CHECK_EXPECTED_ERROR(ce1, vx::err::size_error);

    auto ce2 = vec::create(too_big, 5);
    VX_CHECK_EXPECTED_ERROR(ce2, vx::err::size_error);
}

VX_TEST_CASE(fallible_construction)
{
    test_fallible_construction();
}

//=========================================================================
// empty-input edge cases (0-sized ranges shouldn't error or misbehave)
//=========================================================================

static void test_zero_sized_operations()
{
    using vec = vx::vector<int>;

    // 0-count constructors/creates should succeed as empty vectors
    vec v0(0, 5);
    VX_CHECK(v0.empty());

    auto c0 = vec::create(0, 5);
    VX_CHECK(c0);
    VX_CHECK(c0.value().empty());

    std::initializer_list<int> empty_init{};
    vec v1(empty_init);
    VX_CHECK(v1.empty());

    // 0-count insert/assign/erase at valid offsets are no-ops, not errors
    vec v{ 1, 2, 3 };
    auto ins0 = v.insert(1, 0, 42);
    VX_CHECK(ins0);
    VX_CHECK(v.size() == 3);

    VX_CHECK(v.assign(0, 42));
    VX_CHECK(v.empty());
}

VX_TEST_CASE(zero_sized_operations)
{
    test_zero_sized_operations();
}

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
