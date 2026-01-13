#include "vertex/std/_priv/dynamic_array_base.hpp"
#include "vertex_test/test.hpp"

struct non_trivial
{
    non_trivial()
        : x(0)
    {}
    non_trivial(int val)
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

    int x;

    static inline int copy_count = 0;
    static inline int move_count = 0;
    static inline int destruct_count = 0;

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

    explicit operator int() const noexcept
    {
        return x;
    }
};

template <typename T>
using vec = vx::_priv::dynamic_array_base<T>;

//=========================================================================
// constructors
//=========================================================================

template <typename T>
void test_constructors()
{
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

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == 1);
        }
    }

    VX_SECTION("list")
    {
        vec<T> v = { 0, 1, 2, 3, 4 };

        VX_CHECK(v.data() != nullptr);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == i);
        }
    }

    VX_SECTION("copy")
    {
        vec<T> v1 = { 0, 1, 2, 3, 4 };
        vec<T> v2 = v1;

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }
    }

    VX_SECTION("move")
    {
        vec<T> v1 = { 0, 1, 2, 3, 4 };
        vec<T> v2 = std::move(v1);

        VX_CHECK(v1.data() == nullptr);
        VX_CHECK(v1.size() == 0);
        VX_CHECK(v1.capacity() == 0);

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }
    }

    VX_SECTION("range")
    {
        vec<T> v1 = { 0, 1, 2, 3, 4 };
        vec<T> v2(v1.begin(), v1.end());

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }
    }
}

VX_TEST_CASE(constructors)
{
    test_constructors<int>();
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
    VX_SECTION("copy")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2;

        v2 = v1;

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }
    }

    VX_SECTION("list")
    {
        vec<T> v;
        v = { 0, 1, 2, 3, 4 };

        VX_CHECK(v.data() != nullptr);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == i);
        }
    }

    VX_SECTION("move")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2;

        v2 = std::move(v1);

        VX_CHECK(v1.data() == nullptr);
        VX_CHECK(v1.size() == 0);
        VX_CHECK(v1.capacity() == 0);

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }
    }

    VX_SECTION("assign copy")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2;

        v2.assign(v1);

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }
    }

    VX_SECTION("assign list")
    {
        vec<T> v;
        v.assign({ 0, 1, 2, 3, 4 });

        VX_CHECK(v.data() != nullptr);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == i);
        }
    }

    VX_SECTION("assign move")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2;

        v2.assign(std::move(v1));

        VX_CHECK(v1.data() == nullptr);
        VX_CHECK(v1.size() == 0);
        VX_CHECK(v1.capacity() == 0);

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }
    }


    VX_SECTION("assign range")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        vec<T> v2;

        v2.assign(v1.begin(), v1.end());

        VX_CHECK(v1.data() != nullptr);
        VX_CHECK(v1.size() == 5);
        VX_CHECK(v1.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
        }

        VX_CHECK(v2.data() != nullptr);
        VX_CHECK(v2.size() == 5);
        VX_CHECK(v2.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == i);
        }
    }
}

VX_TEST_CASE(assignment)
{
    test_assignment<int>();
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
        VX_CHECK(*v1.front() == 0);

        const vec<T> v2 = v1;
        VX_CHECK(*v2.front() == 0);
    }

    VX_SECTION("back")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };
        VX_CHECK(*v1.back() == 4);

        const vec<T> v2 = v1;
        VX_CHECK(*v2.back() == 4);
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
        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v1[i] == i);
            v1[i] = 4 - i;
        }

        const vec<T> v2 = v1;
        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v2[i] == 4 - i);
        }
    }
}

VX_TEST_CASE(element_access)
{
    test_element_access<int>();
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
        for (int i = 0; i < 5; ++i, ++it)
        {
            VX_CHECK(*it == i);
        }
        VX_CHECK(it == v1.end());

        const vec<T> v2 = v1;
        auto cit = v2.begin();
        for (int i = 0; i < 5; ++i, ++cit)
        {
            VX_CHECK(*cit == i);
        }
        VX_CHECK(cit == v2.end());
    }

    VX_SECTION("cbegin / cend")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        auto it = v1.cbegin();
        for (int i = 0; i < 5; ++i, ++it)
        {
            VX_CHECK(*it == i);
        }
        VX_CHECK(it == v1.cend());
    }

    VX_SECTION("rbegin / rend")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        auto it = v1.rbegin();
        for (int i = 4; i >= 0; --i, ++it)
        {
            VX_CHECK(*it == i);
        }
        VX_CHECK(it == v1.rend());

        const vec<T> v2 = v1;
        auto cit = v2.rbegin();
        for (int i = 4; i >= 0; --i, ++cit)
        {
            VX_CHECK(*cit == i);
        }
        VX_CHECK(cit == v2.rend());
    }

    VX_SECTION("random access")
    {
        vec<T> v1{ 0, 1, 2, 3, 4 };

        auto it = v1.begin();
        VX_CHECK(it[0] == 0);
        VX_CHECK(it[2] == 2);
        VX_CHECK(*(it + 4) == 4);
        VX_CHECK((v1.end() - v1.begin()) == 5);

        const vec<T> v2 = v1;
        auto cit = v2.begin();
        VX_CHECK(cit[1] == 1);
        VX_CHECK(*(cit + 3) == 3);
        VX_CHECK((v2.end() - v2.begin()) == 5);
    }
}

VX_TEST_CASE(iterators)
{
    test_iterators<int>();
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
    using allocator = typename vec<T>::allocator_type;

    VX_SECTION("clear")
    {
        vec<T> v{ 0, 1, 2, 3, 4 };
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        v.clear();
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 5);
    }

    VX_SECTION("clear_and_deallocate")
    {
        vec<T> v{ 0, 1, 2, 3, 4 };
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        v.clear_and_deallocate();
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 0);
    }

    VX_SECTION("acquire / release")
    {
        vec<T> v;

        int n = 5;
        T* ptr = allocator::allocate(n);
        for (int i = 0; i < n; ++i)
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

        size_t n1 = 2;
        T* ptr1 = allocator::allocate(n1);
        for (int i = 0; i < n1; ++i)
        {
            vx::mem::construct_in_place(ptr1 + i, i);
        }

        size_t n2 = 3;
        T* ptr2 = allocator::allocate(n2);
        for (int i = 0; i < n2; ++i)
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
    test_memory<int>();
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

        VX_CHECK_AND_EXPECT_NO_ERROR(v.resize(5, 1));
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i] == 1);
            v[i] = i;
        }

        VX_CHECK_AND_EXPECT_NO_ERROR(v.resize(2));
        VX_CHECK(v.size() == 2);
        VX_CHECK(v.capacity() == 5);

        for (int i = 0; i < 2; ++i)
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
    test_size_and_capacity<int>();
}

VX_TEST_CASE(size_and_capacity_non_trivial)
{
    test_size_and_capacity<non_trivial>();
}

//=========================================================================
// insertion
//=========================================================================

template <typename T>
void test_insert_and_erase()
{
    VX_SECTION("insert / erase")
    {
        vec<T> v{ 0, 1, 2, 3, 4 };

        // front
        {
            VX_CHECK_AND_EXPECT_NO_ERROR(v.insert(v.begin(), -1));
            VX_CHECK(v.size() == 6);

            VX_CHECK(v[0] == -1);
            VX_CHECK(v[1] == 0);
            VX_CHECK(v[2] == 1);
            VX_CHECK(v[3] == 2);
            VX_CHECK(v[4] == 3);
            VX_CHECK(v[5] == 4);

            VX_CHECK_AND_EXPECT_NO_ERROR(v.erase(v.begin()));
            VX_CHECK(v.size() == 5);

            VX_CHECK(v[0] == 0);
            VX_CHECK(v[1] == 1);
            VX_CHECK(v[2] == 2);
            VX_CHECK(v[3] == 3);
            VX_CHECK(v[4] == 4);
        }

        // back
        {
            VX_CHECK_AND_EXPECT_NO_ERROR(v.insert(v.end(), 5));
            VX_CHECK(v.size() == 6);

            VX_CHECK(v[0] == 0);
            VX_CHECK(v[1] == 1);
            VX_CHECK(v[2] == 2);
            VX_CHECK(v[3] == 3);
            VX_CHECK(v[4] == 4);
            VX_CHECK(v[5] == 5);

            VX_CHECK_AND_EXPECT_NO_ERROR(v.erase(v.end() - 1));
            VX_CHECK(v.size() == 5);

            VX_CHECK(v[0] == 0);
            VX_CHECK(v[1] == 1);
            VX_CHECK(v[2] == 2);
            VX_CHECK(v[3] == 3);
            VX_CHECK(v[4] == 4);
        }

        // middle
        {
            VX_CHECK_AND_EXPECT_NO_ERROR(v.insert(v.begin() + 2, -1));
            VX_CHECK(v.size() == 6);

            for (const auto& i : v)
            {
                std::cout << (int)i << std::endl;
            }

            VX_CHECK(v[0] == 0);
            VX_CHECK(v[1] == 1);
            VX_CHECK(v[2] == -1);
            VX_CHECK(v[3] == 2);
            VX_CHECK(v[4] == 3);
            VX_CHECK(v[5] == 4);

            VX_CHECK_AND_EXPECT_NO_ERROR(v.erase(v.begin() + 2));
            VX_CHECK(v.size() == 5);

            VX_CHECK(v[0] == 0);
            VX_CHECK(v[1] == 1);
            VX_CHECK(v[2] == 2);
            VX_CHECK(v[3] == 3);
            VX_CHECK(v[4] == 4);
        }
    }
}

VX_TEST_CASE(insert_and_erase)
{
    test_insert_and_erase<int>();
}

VX_TEST_CASE(insert_and_erase_non_trivial)
{
    test_insert_and_erase<non_trivial>();
}

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
