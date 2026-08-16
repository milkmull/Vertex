#include "vertex/std/single_linked_list.hpp"
#include "vertex/std/iterator.hpp"
#include "vertex_test/test.hpp"

#include <algorithm>
#include <functional>
#include <iterator>

// https://github.com/microsoft/STL/blob/main/tests/tr1/tests/forward_list/test.cpp

//=========================================================================
// helpers
//=========================================================================

template <typename T, typename Allocator>
static typename vx::single_linked_list<T, Allocator>::difference_type list_size(
    const vx::single_linked_list<T, Allocator>& list)
{
    return std::distance(list.begin(), list.end());
}

template <typename T, typename Allocator>
static typename vx::single_linked_list<T, Allocator>::iterator list_before_end(
    vx::single_linked_list<T, Allocator>& list)
{
    auto it = list.before_begin();

    for (auto next = it; ++next != list.end(); it = next)
    {
    }

    return it;
}

template <typename T, typename Allocator>
static T list_back(const vx::single_linked_list<T, Allocator>& list)
{
    VX_ASSERT(!list.empty());

    auto it = list.begin();
    T value = *it;

    for (++it; it != list.end(); ++it)
    {
        value = *it;
    }

    return value;
}

//=========================================================================
// constructors
//=========================================================================

static void test_container()
{
    using list = vx::single_linked_list<char>;
    using alloc = vx::mem::default_allocator<char>;

    char carr[] = "abc";

    list v0;
    alloc al = v0.get_allocator();
    list v0a(al);

    VX_CHECK(v0.empty());
    VX_CHECK(list_size(v0) == 0);
    VX_CHECK(list_size(v0a) == 0);
    VX_CHECK(v0a.get_allocator() == al);

    list v1(5, '\0');
    list v1a(6, 'x');
    list v1b(7, 'y', al);

    VX_CHECK(list_size(v1) == 5);
    VX_CHECK(v1.front() == '\0');
    VX_CHECK(list_size(v1a) == 6);
    VX_CHECK(v1a.front() == 'x');
    VX_CHECK(list_size(v1b) == 7);
    VX_CHECK(v1b.front() == 'y');

    list v2(v1a);
    VX_CHECK(list_size(v2) == 6);
    VX_CHECK(v2.front() == 'x');

    list v2a(v1a, al);
    VX_CHECK(list_size(v2a) == 6);
    VX_CHECK(v2a.front() == 'x');

    list v3(v1a.begin(), v1a.end());
    VX_CHECK(list_size(v3) == 6);
    VX_CHECK(v3.front() == 'x');

    const list v4(v1a.begin(), v1a.end(), al);
    VX_CHECK(list_size(v4) == 6);
    VX_CHECK(v4.front() == 'x');

    v0 = v4;
    VX_CHECK(list_size(v0) == 6);
    VX_CHECK(v0.front() == 'x');
    VX_CHECK(*v0.begin() == 'x');

    v0.resize(8, '\0');
    VX_CHECK(list_size(v0) == 8);
    VX_CHECK(list_back(v0) == '\0');

    v0.resize(10, 'z');
    VX_CHECK(list_size(v0) == 10);
    VX_CHECK(list_back(v0) == 'z');

    VX_SECTION("iterators")
    {
        typename list::iterator p_it(v0.begin());
        typename list::const_iterator p_cit(v4.begin());
        VX_CHECK(*p_it == 'x');
        VX_CHECK(*p_cit == 'x');

        typename list::const_iterator p_it1 = typename list::const_iterator();
        typename list::const_iterator p_it2 = typename list::const_iterator();
        VX_CHECK(p_it1 == p_it2);

        typename list::iterator p_bit(v0.before_begin());
        typename list::const_iterator p_bcit(v0.before_begin());
        typename list::const_iterator p_bccit(v0.cbefore_begin());

        VX_CHECK(*++p_bit == 'x');
        VX_CHECK(*++p_bcit == 'x');
        VX_CHECK(*++p_bccit == 'x');
    }

    VX_SECTION("const iterators")
    {
        typename list::const_iterator p_it(v0.cbegin());
        typename list::const_iterator p_cit(v4.cbegin());

        VX_CHECK(*p_it == 'x');
        VX_CHECK(*p_cit == 'x');

        typename list::const_iterator p_it1 = typename list::const_iterator();
        typename list::const_iterator p_it2 = typename list::const_iterator();
        VX_CHECK(p_it1 == p_it2);
        VX_CHECK(v0.cend() == list::const_iterator());
    }

    VX_CHECK(v0.front() == 'x');
    VX_CHECK(v4.front() == 'x');

    v0.push_front('a');
    VX_CHECK(v0.front() == 'a');

    v0.pop_front();
    VX_CHECK(v0.front() == 'x');
    VX_CHECK(v4.front() == 'x');

    {
        list v5;
        v5.resize(10, '@');
        VX_CHECK(list_size(v5) == 10);
        VX_CHECK(*++v5.begin() == '@');

        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        list v6(20, 'x');
        list v7(std::move(v6));
        VX_CHECK(v6.empty());
        VX_CHECK(list_size(v7) == 20);

        list v8;
        v8 = std::move(v7);
        VX_CHECK(v7.empty());
        VX_CHECK(list_size(v8) == 20);

        list v8a(std::move(v8), alloc());
        VX_CHECK(v8.empty());
        VX_CHECK(list_size(v8a) == 20);

        VX_DISABLE_WARNING_POP();

        using movable_int = vx::test::movable_int;

        vx::single_linked_list<movable_int> v9;
        v9.resize(10);
        VX_CHECK((*++v9.begin()).val == 0);

        vx::single_linked_list<movable_int> v10;

        movable_int mi2(2);
        v10.push_front(std::move(mi2));
        VX_CHECK(mi2.val == -1);
        VX_CHECK(v10.front().val == 2);

        movable_int mi3(3);
        auto it = v10.insert_after(v10.before_begin(), std::move(mi3));
        VX_CHECK(it == v10.begin());
        VX_CHECK(mi3.val == -1);
        VX_CHECK(v10.front().val == 3);

        v10.emplace_front();
        VX_CHECK(v10.front().val == 0);
        v10.emplace_front(2);
        VX_CHECK(v10.front().val == 2);
        v10.emplace_front(3, 2);
        VX_CHECK(v10.front().val == 0x32);
        v10.emplace_front(4, 3, 2);
        VX_CHECK(v10.front().val == 0x432);
        v10.emplace_front(5, 4, 3, 2);
        VX_CHECK(v10.front().val == 0x5432);
        v10.emplace_front(6, 5, 4, 3, 2);
        VX_CHECK(v10.front().val == 0x65432);

        v10.emplace_after(v10.begin());
        VX_CHECK((*++v10.begin()).val == 0);
        v10.emplace_after(v10.begin(), 2);
        VX_CHECK((*++v10.begin()).val == 2);
        v10.emplace_after(v10.begin(), 3, 2);
        VX_CHECK((*++v10.begin()).val == 0x32);
        v10.emplace_after(v10.begin(), 4, 3, 2);
        VX_CHECK((*++v10.begin()).val == 0x432);
        v10.emplace_after(v10.begin(), 5, 4, 3, 2);
        VX_CHECK((*++v10.begin()).val == 0x5432);
        v10.emplace_after(v10.begin(), 6, 5, 4, 3, 2);
        VX_CHECK((*++v10.begin()).val == 0x65432);
    }

    VX_SECTION("lvalue stealing")
    {
        using copyable_int = vx::test::copyable_int;

        vx::single_linked_list<copyable_int> v11;

        copyable_int ci2(2);
        v11.push_front(ci2);
        VX_CHECK(ci2.val == 2);
        VX_CHECK(v11.front().val == 2);

        copyable_int ci3(3);
        auto it = v11.insert_after(v11.before_begin(), ci3);
        VX_CHECK(it == v11.begin());
        VX_CHECK(ci3.val == 3);
        VX_CHECK(v11.front().val == 3);

        vx::single_linked_list<copyable_int> v12(v11);
        VX_CHECK(v11 == v12);
        v11 = v12;
        VX_CHECK(v11 == v12);

        vx::single_linked_list<copyable_int> v13(vx::make_move_iterator(v11.begin()), vx::make_move_iterator(v11.end()));
        VX_CHECK(v13.front().val == 3);
        VX_CHECK(v11.front().val == -1);
    }

    VX_SECTION("assign")
    {
        v0.assign(v4.begin(), v4.end());
        VX_CHECK(list_size(v0) == list_size(v4));
        VX_CHECK(v0.front() == v4.front());
        v0.assign(4, 'w');
        VX_CHECK(list_size(v0) == 4);
        VX_CHECK(v0.front() == 'w');
        VX_CHECK(*v0.insert_after(v0.before_begin(), 'a') == 'a');
        VX_CHECK(v0.front() == 'a');
        VX_CHECK(*++v0.begin() == 'w');
        typename list::const_iterator it = v0.insert_after(v0.before_begin(), 2, 'b');
        VX_CHECK(it == ++v0.begin());
        VX_CHECK(v0.front() == 'b');
        VX_CHECK(*++v0.begin() == 'b');
        VX_CHECK(*++ ++v0.begin() == 'a');
        it = v0.insert_after(list_before_end(v0), v4.begin(), v4.end());
        VX_CHECK(++it == v0.end());
        VX_CHECK(list_back(v0) == list_back(v4));
        it = v0.insert_after(list_before_end(v0), carr, carr + 3);
        VX_CHECK(++it == v0.end());
        VX_CHECK(list_back(v0) == 'c');
        v0.erase_after(v0.before_begin());
        VX_CHECK(v0.front() == 'b');
        VX_CHECK(*++v0.begin() == 'a');
        v0.erase_after(v0.before_begin(), ++v0.begin());
        VX_CHECK(v0.front() == 'a');
    }

    v0.clear();
    VX_CHECK(v0.empty());
    v0.swap(v1);
    VX_CHECK(!v0.empty());
    VX_CHECK(v1.empty());
    std::swap(v0, v1);
    VX_CHECK(v0.empty());
    VX_CHECK(!v1.empty());

    VX_SECTION("comparison")
    {
        VX_CHECK(v1 == v1);
        VX_CHECK(v0 < v1);
        VX_CHECK(v0 != v1);
        VX_CHECK(v1 > v0);
        VX_CHECK(v0 <= v1);
        VX_CHECK(v1 >= v0);
    }

    VX_SECTION("splice")
    {
        v0.insert_after(v0.before_begin(), carr, carr + 3);
        v1.splice_after(v1.before_begin(), v0);
        VX_CHECK(v0.empty());
        VX_CHECK(v1.front() == 'a');

        v0.splice_after(list_before_end(v0), v1, v1.before_begin());
        VX_CHECK(list_size(v0) == 1);
        VX_CHECK(v0.front() == 'a');

        v0.splice_after(v0.before_begin(), v1, v1.before_begin(), v1.end());
        VX_CHECK(v0.front() == 'b');
        VX_CHECK(v1.empty());
    }

    VX_SECTION("remove")
    {
        v0.remove('b');
        VX_CHECK(v0.front() == 'c');

        v0.remove_if([](char c)
            { return c != 'c'; });
        VX_CHECK(v0.front() == 'c');
        VX_CHECK(list_size(v0) == 1);
    }

    VX_SECTION("merge")
    {
        v0.merge(v1, std::greater<char>());
        VX_CHECK(v0.front() == 'c');
        VX_CHECK(list_size(v0) == 1);

        v0.insert_after(v0.before_begin(), carr, carr + 3);
        v0.unique();
        VX_CHECK(list_back(v0) == 'c');
        VX_CHECK(list_size(v0) == 3);

        v0.unique(std::not_equal_to<char>());
        VX_CHECK(v0.front() == 'a');
        VX_CHECK(list_size(v0) == 1);

        v1.insert_after(v1.before_begin(), carr, carr + 3);
        v0.merge(v1);
        VX_CHECK(list_back(v0) == 'c');
        VX_CHECK(list_size(v0) == 4);
    }

    VX_SECTION("sort and reverse")
    {
        v0.sort(std::greater<char>());
        VX_CHECK(list_back(v0) == 'a');
        VX_CHECK(list_size(v0) == 4);

        v0.sort();
        VX_CHECK(list_back(v0) == 'c');
        VX_CHECK(list_size(v0) == 4);

        v0.reverse();
        VX_CHECK(list_back(v0) == 'a');
        VX_CHECK(list_size(v0) == 4);
    }

    VX_SECTION("move splice")
    {
        v0.clear();
        v1.clear();
        v0.insert_after(v0.before_begin(), carr, carr + 3);
        v1.splice_after(v1.before_begin(), std::move(v0));

        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        VX_CHECK(v0.empty());
        VX_CHECK(v1.front() == 'a');

        v0.splice_after(list_before_end(v0), std::move(v1), v1.before_begin());
        VX_CHECK(list_size(v0) == 1);
        VX_CHECK(v0.front() == 'a');

        v0.splice_after(
            v0.before_begin(), std::move(v1), v1.before_begin(), v1.end());
        VX_CHECK(v0.front() == 'b');
        VX_CHECK(v1.empty());

        VX_DISABLE_WARNING_POP();
    }

    VX_SECTION("move merge")
    {
        v0.assign(1, 'c');
        v1.assign(2, 'b');
        v0.merge(std::move(v1), std::greater<char>());
        VX_CHECK(v0.front() == 'c');

        v0.assign(1, 'c');
        v1.assign(2, 'b');
        v0.merge(std::move(v1));
        VX_CHECK(v0.front() == 'b');
    }

    VX_SECTION("iniializer lists")
    {
        std::initializer_list<char> init{ 'a', 'b', 'c' };
        list v11(init);
        VX_CHECK(list_size(v11) == 3);
        VX_CHECK(v11.front() == 'a');

        v11.clear();
        v11 = init;
        VX_CHECK(list_size(v11) == 3);
        VX_CHECK(v11.front() == 'a');

        list::iterator it0 = v11.insert_after(v11.begin(), init);
        VX_CHECK(*it0 == 'c');
        VX_CHECK(list_size(v11) == 6);
        VX_CHECK(*++v11.begin() == 'a');

        v11.assign(init);
        VX_CHECK(list_size(v11) == 3);
        VX_CHECK(v11.front() == 'a');
    }
}

VX_TEST_CASE(container)
{
    test_container();
}

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
