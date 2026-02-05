#include "vertex/std/dynamic_array.hpp"
#include "vertex_test/test.hpp"

template <typename T>
using vec = vx::dynamic_array<T>;

using trivial_type = typename vec<int>::size_type;

struct non_trivial
{
    non_trivial()
        : x(0)
    {}
    non_trivial(trivial_type val)
        : x(val)
    {}
    non_trivial(const non_trivial& other)
    {
        x = other.x;
        ++copy_count;
    }
    non_trivial(non_trivial&& other) noexcept
    {
        x = other.x;
        other.x = 0;
        ++move_count;
    }
    ~non_trivial()
    {
        ++destruct_count;
    }

    non_trivial& operator=(const non_trivial& other)
    {
        x = other.x;
        ++copy_count;
        return *this;
    }
    non_trivial& operator=(non_trivial&& other) noexcept
    {
        x = other.x;
        other.x = 0;
        ++move_count;
        return *this;
    }

    trivial_type x;

    static inline size_t copy_count = 0;
    static inline size_t move_count = 0;
    static inline size_t destruct_count = 0;

    static void reset_counters()
    {
        copy_count = 0;
        move_count = 0;
        destruct_count = 0;
    }

    friend bool operator==(const non_trivial& lhs, const non_trivial& rhs)
    {
        return lhs.x == rhs.x;
    }

    friend bool operator<(const non_trivial& lhs, const non_trivial& rhs)
    {
        return lhs.x < rhs.x;
    }

    friend bool operator>(const non_trivial& lhs, const non_trivial& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const non_trivial& lhs, const non_trivial& rhs)
    {
        return !(rhs < lhs);
    }

    friend bool operator>=(const non_trivial& lhs, const non_trivial& rhs)
    {
        return !(lhs < rhs);
    }

    explicit operator trivial_type() const noexcept
    {
        return x;
    }
};

//=========================================================================
// constructors
//=========================================================================

template <typename T>
void test_constructors()
{
    constexpr bool is_non_trivial = std::is_same<T, non_trivial>::value;

    VX_SECTION("default")
    {
        vec<T> v;

        VX_CHECK(!v.data());
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 0);
    }

    VX_SECTION("count")
    {
        vec<T> v(5);

        VX_CHECK(v.data() != nullptr);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);
    }

    VX_SECTION("fill")
    {
        vec<T> v(5, 1);

        VX_CHECK(v.data() != nullptr);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == 1);
        }
    }

    VX_SECTION("list")
    {
        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v = { 0, 1, 2, 3, 4 };

        VX_CHECK(v.data() != nullptr);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 5);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }

    VX_SECTION("copy")
    {
        vec<T> v1 = { 0, 1, 2, 3, 4 };

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v2 = v1;

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 5);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }

    VX_SECTION("move")
    {
        vec<T> v1 = { 0, 1, 2, 3, 4 };

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v2 = std::move(v1);

        VX_CHECK(v1.data() == nullptr);
        VX_CHECK(v1.size() == 0);
        VX_CHECK(v1.capacity() == 0);

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 0);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }

    VX_SECTION("range")
    {
        vec<T> v1 = { 0, 1, 2, 3, 4 };

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v2(v1.begin(), v1.end());

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 5);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }
}

VX_TEST_CASE(constructors)
{
    test_constructors<trivial_type>();
}

VX_TEST_CASE(constructors_non_trivial)
{
    test_constructors<non_trivial>();
}

//=========================================================================
// assignment
//=========================================================================

template <typename T>
void test_assignment()
{
    constexpr bool is_non_trivial = std::is_same<T, non_trivial>::value;

    VX_SECTION("copy")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v2;
        v2 = v1;

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 5);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }

    VX_SECTION("list")
    {
        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v;
        v = { 0, 1, 2, 3, 4 };

        VX_CHECK(v.data() != nullptr);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 5);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }

    VX_SECTION("move")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v2;
        v2 = std::move(v1);

        VX_CHECK(v1.data() == nullptr);
        VX_CHECK(v1.size() == 0);
        VX_CHECK(v1.capacity() == 0);

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 0);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }

    VX_SECTION("assign copy")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v2;
        v2.assign(v1);

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 5);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }

    VX_SECTION("assign list")
    {
        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v;
        v.assign({ 0, 1, 2, 3, 4 });

        VX_CHECK(v.data() != nullptr);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 5);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }

    VX_SECTION("assign move")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v2;
        v2.assign(std::move(v1));

        VX_CHECK(v1.data() == nullptr);
        VX_CHECK(v1.size() == 0);
        VX_CHECK(v1.capacity() == 0);

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 0);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }


    VX_SECTION("assign range")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        vec<T> v2;
        v2.assign(v1.begin(), v1.end());

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::copy_count == 5);
            VX_CHECK(non_trivial::move_count == 0);
        }
    }
}

VX_TEST_CASE(assignment)
{
    test_assignment<trivial_type>();
}

VX_TEST_CASE(assignment_non_trivial)
{
    test_assignment<non_trivial>();
}

//=========================================================================
// assignment
//=========================================================================

template <typename T>
void test_element_access()
{
    VX_SECTION("front")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        VX_CHECK(v1.front() == 0);

        const vec<T> v2 = v1;
        VX_CHECK(v2.front() == 0);
    }

    VX_SECTION("back")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        VX_CHECK(v1.back() == 4);

        const vec<T> v2 = v1;
        VX_CHECK(v2.back() == 4);
    }

    VX_SECTION("data")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        VX_CHECK(v1.data());
        VX_CHECK(*v1.data() == 0);
        VX_CHECK(*(v1.data() + 4) == 4);

        const vec<T> v2 = v1;
        VX_CHECK(v2.data());
        VX_CHECK(*v2.data() == 0);
        VX_CHECK(*(v2.data() + 4) == 4);
    }

    VX_SECTION("index")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
            v1[i] = 4 - i;
        }

        const vec<T> v2 = v1;
        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == 4 - i);
        }
    }
}

VX_TEST_CASE(element_access)
{
    test_element_access<trivial_type>();
}

VX_TEST_CASE(element_access_non_trivial)
{
    test_element_access<non_trivial>();
}

//=========================================================================
// iterators
//=========================================================================

template <typename T>
void test_iterators()
{
    VX_SECTION("begin / end")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        auto it = v1.begin();
        for (trivial_type i = 0; i < 5; ++i, ++it)
        {
            VX_CHECK(*it == i);
        }
        VX_CHECK(it == v1.end());

        const vec<T> v2 = v1;
        auto cit = v2.begin();
        for (trivial_type i = 0; i < 5; ++i, ++cit)
        {
            VX_CHECK(*cit == i);
        }
        VX_CHECK(cit == v2.end());
    }

    VX_SECTION("cbegin / cend")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        auto it = v1.cbegin();
        for (trivial_type i = 0; i < 5; ++i, ++it)
        {
            VX_CHECK(*it == i);
        }
        VX_CHECK(it == v1.cend());
    }

    VX_SECTION("rbegin / rend")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        auto it = v1.rbegin();
        for (trivial_type i = 4; i > 0; --i, ++it)
        {
            VX_CHECK(*it == i);
        }
        VX_CHECK(++it == v1.rend());

        const vec<T> v2 = v1;
        auto cit = v2.rbegin();
        for (trivial_type i = 4; i > 0; --i, ++cit)
        {
            VX_CHECK(*cit == i);
        }
        VX_CHECK(++cit == v2.rend());
    }

    VX_SECTION("random access")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        auto it = v1.begin();
        VX_CHECK(it);
        VX_CHECK(it[0] == 0);
        VX_CHECK(it[2] == 2);
        VX_CHECK(*(it + 4) == 4);
        VX_CHECK((v1.end() - v1.begin()) == 5);

        const vec<T> v2 = v1;
        auto cit = v2.begin();
        VX_CHECK(cit);
        VX_CHECK(cit[1] == 1);
        VX_CHECK(*(cit + 3) == 3);
        VX_CHECK((v2.end() - v2.begin()) == 5);
    }
}

VX_TEST_CASE(iterators)
{
    test_iterators<trivial_type>();
}

VX_TEST_CASE(iterators_non_trivial)
{
    test_iterators<non_trivial>();
}

//=========================================================================
// memory
//=========================================================================

template <typename T>
void test_memory()
{
    constexpr bool is_non_trivial = std::is_same<T, non_trivial>::value;
    using allocator = typename vec<T>::allocator_type;

    VX_SECTION("clear")
    {
        vec<T> v{ 0, 1, 2, 3, 4 };
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        v.clear();
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 5);

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::destruct_count == 5);
        }
    }

    VX_SECTION("clear_and_deallocate")
    {
        vec<T> v{ 0, 1, 2, 3, 4 };
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();
        }

        v.clear_and_deallocate();
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 0);

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            VX_CHECK(non_trivial::destruct_count == 5);
        }
    }

    VX_SECTION("acquire / release")
    {
        vec<T> v;

        trivial_type n = 5;
        T* ptr = allocator::allocate(n);
        for (trivial_type i = 0; i < n; ++i)
        {
            vx::mem::construct_in_place(ptr + i, i);
        }

        VX_EXPECT_ERROR_CODE(v.acquire(ptr, v.max_size() + 1), vx::err::size_error);

        v.acquire(ptr, n);
        VX_CHECK(v.data() == ptr);
        VX_CHECK(v.size() == n);
        VX_CHECK(v.capacity() == n);

        T* released_ptr = v.release();
        VX_CHECK(released_ptr == ptr);
        VX_CHECK(v.data() == nullptr);
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 0);

        allocator::deallocate(released_ptr, n);
    }

    VX_SECTION("swap")
    {
        vec<T> v1;
        vec<T> v2;

        trivial_type n1 = 2;
        T* ptr1 = allocator::allocate(n1);
        for (trivial_type i = 0; i < n1; ++i)
        {
            vx::mem::construct_in_place(ptr1 + i, i);
        }

        trivial_type n2 = 3;
        T* ptr2 = allocator::allocate(n2);
        for (trivial_type i = 0; i < n2; ++i)
        {
            vx::mem::construct_in_place(ptr2 + i, i + 10);
        }

        v1.acquire(ptr1, n1);
        v2.acquire(ptr2, n2);

        v1.swap(v2);

        VX_CHECK(v1.data() == ptr2);
        VX_CHECK(v1.size() == n2);
        VX_CHECK(v1.capacity() == n2);

        VX_CHECK(v2.data() == ptr1);
        VX_CHECK(v2.size() == n1);
        VX_CHECK(v2.capacity() == n1);
    }
}

VX_TEST_CASE(memory)
{
    test_memory<trivial_type>();
}

VX_TEST_CASE(memory_non_trivial)
{
    test_memory<non_trivial>();
}

//=========================================================================
// memory
//=========================================================================

template <typename T>
void test_size_and_capacity()
{
    VX_SECTION("size")
    {
        vec<T> v{ 0, 1, 2, 3, 4 };
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.data_size() == 5 * sizeof(T));
        VX_CHECK(!v.empty());

        v.clear();
        VX_CHECK(v.data_size() == 0);
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.empty());

        VX_STATIC_CHECK(v.max_size() == SIZE_MAX / sizeof(T));
    }

    VX_SECTION("reserve / capacity")
    {
        vec<T> v;
        VX_CHECK(v.capacity() == 0);

        VX_CHECK_AND_EXPECT_NO_ERROR(v.reserve(0));
        VX_CHECK(v.capacity() == 0);
        VX_CHECK(v.size() == 0);

        VX_CHECK_AND_EXPECT_NO_ERROR(v.reserve(5));
        VX_CHECK(v.capacity() == 5);
        VX_CHECK(v.size() == 0);

        VX_CHECK_AND_EXPECT_NO_ERROR(v.reserve(2));
        VX_CHECK(v.capacity() == 5);
        VX_CHECK(v.size() == 0);

        VX_CHECK_AND_EXPECT_NO_ERROR(v.reserve(0));
        VX_CHECK(v.capacity() == 5);
        VX_CHECK(v.size() == 0);

        VX_EXPECT_ERROR_CODE(v.reserve(v.max_size() + 1), vx::err::size_error);
    }

    VX_SECTION("resize")
    {
        vec<T> v;

        VX_CHECK_AND_EXPECT_NO_ERROR(v.resize(0));
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 0);

        VX_CHECK_AND_EXPECT_NO_ERROR(v.resize(5, 1u));
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (trivial_type i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == 1);
            v[i] = i;
        }

        VX_CHECK_AND_EXPECT_NO_ERROR(v.resize(2));
        VX_CHECK(v.size() == 2);
        VX_CHECK(v.capacity() == 5);

        for (trivial_type i = 0; i < 2; ++i)
        {
            VX_CHECK(v[i] == i);
        }

        VX_CHECK_AND_EXPECT_NO_ERROR(v.resize(0));
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 5);

        VX_EXPECT_ERROR_CODE(v.resize(v.max_size() + 1), vx::err::size_error);
    }
}

VX_TEST_CASE(size_and_capacity)
{
    test_size_and_capacity<trivial_type>();
}

VX_TEST_CASE(size_and_capacity_non_trivial)
{
    test_size_and_capacity<non_trivial>();
}

//=========================================================================
// emplace / push back
//=========================================================================

struct emplace_type
{
    trivial_type a, b;
    emplace_type(trivial_type x, trivial_type y)
        : a(x), b(y)
    {}

    friend bool operator==(const emplace_type& lhs, const emplace_type& rhs)
    {
        return lhs.a == rhs.a && lhs.b == rhs.b;
    }
};

template <typename T>
void test_emplace_and_pushback()
{
    constexpr bool is_non_trivial = std::is_same<T, non_trivial>::value;

    VX_SECTION("emplace")
    {
        vec<emplace_type> v;

        // front
        auto it0 = v.emplace(v.begin(), 0u, 0u);
        VX_CHECK(it0);
        VX_CHECK(it0 == v.begin());
        VX_CHECK(it0->a == 0 && it0->b == 0);

        // back
        auto it1 = v.emplace(v.end(), 2u, 2u);
        VX_CHECK(it1);
        VX_CHECK(it1 == v.end() - 1);
        VX_CHECK(it1->a == 2 && it1->b == 2);

        // middle
        auto it_mid = v.emplace(v.begin() + 1, 1u, 1u);
        VX_CHECK(it_mid);
        VX_CHECK(it_mid == v.begin() + 1);
        VX_CHECK(it_mid->a == 1 && it_mid->b == 1);

        VX_CHECK(v.size() == 3);

        // order check: 0,1,2
        for (trivial_type i = 0; i < 3; ++i)
        {
            VX_CHECK(v[i].a == i);
            VX_CHECK(v[i].b == i);
        }
    }

    VX_SECTION("emplace_back")
    {
        vec<emplace_type> v;

        auto* p0 = v.emplace_back(1u, 2u);
        VX_CHECK(p0);
        VX_CHECK(*p0 == v.back());
        VX_CHECK(p0->a == 1);
        VX_CHECK(p0->b == 2);

        auto* p1 = v.emplace_back(3u, 4u);
        VX_CHECK(p1);
        VX_CHECK(*p1 == v.back());
        VX_CHECK(p1->a == 3);
        VX_CHECK(p1->b == 4);

        VX_CHECK(v.size() == 2);
    }

    VX_SECTION("push_back")
    {
        vec<T> v;

        VX_IF_CONSTEXPR (is_non_trivial)
        {
            non_trivial::reset_counters();

            T x(1);
            v.push_back(x); // copy
            VX_CHECK(non_trivial::copy_count == 1);

            v.push_back(T(2)); // move
            VX_CHECK(non_trivial::move_count >= 1);
        }
        else
        {
            v.push_back(T(1));
            v.push_back(T(2));
        }

        VX_CHECK(v.size() == 2);
        VX_CHECK(static_cast<trivial_type>(v[0]) == 1);
        VX_CHECK(static_cast<trivial_type>(v[1]) == 2);
    }
}

VX_TEST_CASE(emplace_and_push_back)
{
    test_emplace_and_pushback<trivial_type>();
}

VX_TEST_CASE(emplace_and_push_back_non_trivial)
{
    test_emplace_and_pushback<non_trivial>();
}

//=========================================================================
// growth rate
//=========================================================================

VX_TEST_CASE(growth_rate)
{
    VX_SECTION("1x")
    {
        using rate_1 = std::ratio<1, 1>;
        const trivial_type expected_caps_1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };

        vec<trivial_type> v;

        for (trivial_type i = 0; i < 20; ++i)
        {
            v.push_back<rate_1>(0);
            VX_CHECK(v.capacity() == expected_caps_1[i]);
        }
    }

    VX_SECTION("1.5x")
    {
        using rate_2 = std::ratio<3, 2>;
        const trivial_type expected_caps_2[] = { 1, 2, 3, 4, 6, 6, 9, 9, 9, 13, 13, 13, 13, 19, 19, 19, 19, 19, 19, 28 };

        vec<trivial_type> v;

        for (trivial_type i = 0; i < 20; ++i)
        {
            v.push_back<rate_2>(0);
            VX_CHECK(v.capacity() == expected_caps_2[i]);
        }
    }

    VX_SECTION("2x")
    {
        using rate_3 = std::ratio<2, 1>;
        const trivial_type expected_caps_3[] = { 2, 2, 4, 4, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16, 16, 16, 32, 32, 32, 32 };

        vec<trivial_type> v;

        for (trivial_type i = 0; i < 20; ++i)
        {
            v.push_back<rate_3>(0);
            VX_CHECK(v.capacity() == expected_caps_3[i]);
        }
    }
}

//=========================================================================
// insertion
//=========================================================================

template <typename T>
void test_insert_and_erase()
{
    constexpr bool is_non_trivial = std::is_same<T, non_trivial>::value;

    VX_SECTION("simple insert / erase")
    {
        vec<T> v{ T(0), T(1), T(2), T(3), T(4) };

        // Insert a fixed sentinel value before each element
        const T sentinel = T(999);

        for (auto it = v.begin(); it != v.end(); ++it)
        {
            it = v.insert(it, sentinel);

            VX_CHECK(it);
            VX_CHECK(*it == sentinel);
            ++it; // skip over inserted sentinel to avoid infinite loop
        }

        // Expected: sentinel, original, sentinel, original, ...
        vec<T> expected1{
            T(999), T(0), T(999), T(1), T(999), T(2), T(999), T(3), T(999), T(4)
        };
        VX_CHECK(v == expected1);

        // Erase all sentinels (every other element)
        for (auto it = v.begin(); it != v.end();)
        {
            if (*it == sentinel)
            {
                it = v.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Back to original vector
        vec<T> expected2{ T(0), T(1), T(2), T(3), T(4) };
        VX_CHECK(v == expected2);
    }

    VX_SECTION("insert move")
    {
        VX_IF_CONSTEXPR (is_non_trivial)
        {
            vec<T> v;

            non_trivial::reset_counters();
            T x(1);
            auto it = v.insert(v.end(), std::move(x));

            VX_CHECK(it);
            VX_CHECK(*it == 1);
            VX_CHECK(non_trivial::move_count == 1);
        }
    }

    VX_SECTION("insert move")
    {
        VX_IF_CONSTEXPR (is_non_trivial)
        {
            vec<T> v;

            non_trivial::reset_counters();
            T x(1);
            auto it = v.insert(v.end(), std::move(x));

            VX_CHECK(it);
            VX_CHECK(*it == 1);
            VX_CHECK(non_trivial::move_count == 1);
        }
    }

    VX_SECTION("insert count")
    {
        vec<T> v{ 0, 2 };

        auto it = v.insert(v.begin() + 1, 5, 1);
        VX_CHECK(it);
        VX_CHECK(*it == 1);

        for (; it != v.end(); ++it)
        {
            if (it == v.end() - 1)
            {
                VX_CHECK(*it == 2);
            }
            else
            {
                VX_CHECK(*it == 1);
            }
        }
    }

    VX_SECTION("insert list")
    {
        vec<T> v{ 0, 10 };

        auto it = v.insert(v.begin() + 1, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });
        VX_CHECK(it);
        VX_CHECK(*it == 1);

        for (trivial_type i = 0; i < v.size(); ++i)
        {
            VX_CHECK(v[i] == i);
        }
    }

    VX_SECTION("insert / erase range")
    {
        vec<T> v{ 0, 1, 2, 3, 4, 5 };

        // Take a range from the vector itself: [2,3,4]
        auto first = v.begin() + 2; // points to 2
        auto last = v.begin() + 5;  // one past 4

        // Insert that range at the front
        auto it = v.insert(v.begin(), first, last);

        // it should point to first inserted element (2)
        VX_CHECK(it);
        VX_CHECK(*it == 2);

        // v should now be: 2 3 4 0 1 2 3 4 5
        {
            trivial_type expected[] = { 2, 3, 4, 0, 1, 2, 3, 4, 5 };
            for (trivial_type i = 0; i < v.size(); ++i)
            {
                VX_CHECK(static_cast<trivial_type>(v[i]) == expected[i]);
            }
        }

        // Erase the inserted range again (the first three elements)
        auto erase_first = v.begin();
        auto erase_last = v.begin() + 3;

        auto ret = v.erase(erase_first, erase_last);

        // ret should now point to 0 (the original first element)
        VX_CHECK(*ret == 0);

        // Back to original content
        vec<T> expected{ 0, 1, 2, 3, 4, 5 };
        VX_CHECK(v == expected);
    }

    VX_SECTION("pop back")
    {
        vec<T> v{ 0, 1, 2, 3 };

        trivial_type i = static_cast<trivial_type>(v.back());

        do
        {
            const auto& back = v.back();
            VX_CHECK(back == i);
            const trivial_type last_size = v.size();
            v.pop_back();
            VX_CHECK(v.size() == last_size - 1);

        } while (i-- > 0);

        VX_CHECK(v.empty());
    }
}

VX_TEST_CASE(insert_and_erase)
{
    test_insert_and_erase<trivial_type>();
}

VX_TEST_CASE(insert_and_erase_non_trivial)
{
    test_insert_and_erase<non_trivial>();
}

//=========================================================================
// compare
//=========================================================================

template <typename T>
void test_compare()
{
    VX_SECTION("== / !=")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2{ 0, 1, 2, 3, 4 };
        vec<T> v3{ 0, 1, 2, 3 };
        vec<T> v4{ 0, 1, 2, 3, 5 };
        vec<T> v_empty1;
        vec<T> v_empty2;

        // Reflexivity
        VX_CHECK(v1 == v1);
        VX_CHECK(v_empty1 == v_empty1);

        // Equality of identical contents
        VX_CHECK(v1 == v2);
        VX_CHECK(v2 == v1);

        // Inequality with different sizes
        VX_CHECK(v1 != v3);
        VX_CHECK(v3 != v1);

        // Inequality with same size, different elements
        VX_CHECK(v1 != v4);
        VX_CHECK(v4 != v1);

        // Equality of empty vectors
        VX_CHECK(v_empty1 == v_empty2);

        // Inequality empty vs non-empty
        VX_CHECK(v_empty1 != v1);
        VX_CHECK(v1 != v_empty1);
    }

    VX_SECTION("<")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2{ 0, 1, 2, 3, 4 };
        vec<T> v3{ 0, 1, 2, 3 };
        vec<T> v4{ 0, 1, 2, 3, 5 };
        vec<T> v_empty1;
        vec<T> v_empty2;

        // Reflexivity
        VX_CHECK(!(v1 < v1));
        VX_CHECK(!(v_empty1 < v_empty1));

        // Equality of identical contents
        VX_CHECK(!(v1 < v2));
        VX_CHECK(!(v2 < v1));

        // Different sizes (prefix)
        VX_CHECK(v3 < v1);
        VX_CHECK(!(v1 < v3));

        // Same size, different elements
        VX_CHECK(v1 < v4);
        VX_CHECK(!(v4 < v1));

        // Empty vectors
        VX_CHECK(!(v_empty1 < v_empty2));

        // Empty vs non-empty
        VX_CHECK(v_empty1 < v1);
        VX_CHECK(!(v1 < v_empty1));
    }

    VX_SECTION(">")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2{ 0, 1, 2, 3, 4 };
        vec<T> v3{ 0, 1, 2, 3 };
        vec<T> v4{ 0, 1, 2, 3, 5 };
        vec<T> v_empty1;
        vec<T> v_empty2;

        // Reflexivity
        VX_CHECK(!(v1 > v1));
        VX_CHECK(!(v_empty1 > v_empty1));

        // Equality of identical contents
        VX_CHECK(!(v1 > v2));
        VX_CHECK(!(v2 > v1));

        // Different sizes (prefix)
        VX_CHECK(v1 > v3);
        VX_CHECK(!(v3 > v1));

        // Same size, different elements
        VX_CHECK(v4 > v1);
        VX_CHECK(!(v1 > v4));

        // Empty vectors
        VX_CHECK(!(v_empty1 > v_empty2));

        // Empty vs non-empty
        VX_CHECK(v1 > v_empty1);
        VX_CHECK(!(v_empty1 > v1));
    }

    VX_SECTION("<=")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2{ 0, 1, 2, 3, 4 };
        vec<T> v3{ 0, 1, 2, 3 };
        vec<T> v4{ 0, 1, 2, 3, 5 };
        vec<T> v_empty1;
        vec<T> v_empty2;

        // Reflexivity
        VX_CHECK(v1 <= v1);
        VX_CHECK(v_empty1 <= v_empty1);

        // Equality of identical contents
        VX_CHECK(v1 <= v2);
        VX_CHECK(v2 <= v1);

        // Different sizes
        VX_CHECK(v3 <= v1);
        VX_CHECK(!(v1 <= v3));

        // Same size, different elements
        VX_CHECK(v1 <= v4);
        VX_CHECK(!(v4 <= v1));

        // Empty vectors
        VX_CHECK(v_empty1 <= v_empty2);

        // Empty vs non-empty
        VX_CHECK(v_empty1 <= v1);
        VX_CHECK(!(v1 <= v_empty1));
    }

    VX_SECTION(">=")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2{ 0, 1, 2, 3, 4 };
        vec<T> v3{ 0, 1, 2, 3 };
        vec<T> v4{ 0, 1, 2, 3, 5 };
        vec<T> v_empty1;
        vec<T> v_empty2;

        // Reflexivity
        VX_CHECK(v1 >= v1);
        VX_CHECK(v_empty1 >= v_empty1);

        // Equality of identical contents
        VX_CHECK(v1 >= v2);
        VX_CHECK(v2 >= v1);

        // Different sizes
        VX_CHECK(v1 >= v3);
        VX_CHECK(!(v3 >= v1));

        // Same size, different elements
        VX_CHECK(v4 >= v1);
        VX_CHECK(!(v1 >= v4));

        // Empty vectors
        VX_CHECK(v_empty1 >= v_empty2);

        // Empty vs non-empty
        VX_CHECK(v1 >= v_empty1);
        VX_CHECK(!(v_empty1 >= v1));
    }
}

VX_TEST_CASE(compare)
{
    test_compare<trivial_type>();
}

VX_TEST_CASE(compare_non_trivial)
{
    test_compare<non_trivial>();
}

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
