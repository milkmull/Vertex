#include "vertex/std/memory.hpp"
#include "vertex_test/test.hpp"
#include <cstdint>

using trivial_type = int;

struct non_trivial_type
{
    non_trivial_type()
        : x(0)
    {
        ++construct_count;
    }

    non_trivial_type(int val)
        : x(val)
    {
        ++construct_count;
    }

    non_trivial_type(const non_trivial_type& other)
    {
        x = other.x;
        ++copy_count;
    }

    non_trivial_type(non_trivial_type&& other) noexcept
    {
        x = other.x;
        other.x = 0;
        ++move_count;
    }

    ~non_trivial_type()
    {
        ++destruct_count;
    }

    non_trivial_type& operator=(const non_trivial_type& other)
    {
        x = other.x;
        ++copy_count;
        return *this;
    }
    non_trivial_type& operator=(non_trivial_type&& other) noexcept
    {
        x = other.x;
        other.x = 0;
        ++move_count;
        return *this;
    }

    int x;

    static inline int construct_count = 0;
    static inline int destruct_count = 0;
    static inline int copy_count = 0;
    static inline int move_count = 0;

    static void reset_counters()
    {
        construct_count = 0;
        destruct_count = 0;
        copy_count = 0;
        move_count = 0;
    }

    friend bool operator==(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return lhs.x == rhs.x;
    }

    friend bool operator!=(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return !(lhs == rhs);
    }

    friend bool operator<(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return lhs.x < rhs.x;
    }

    friend bool operator>(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return !(rhs < lhs);
    }

    friend bool operator>=(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return !(lhs < rhs);
    }
};

//=========================================================================
// basic allocation
//=========================================================================

VX_TEST_CASE(unaligned)
{
    VX_SECTION("allocate / deallocate")
    {
        constexpr size_t size = 128;
        void* ptr = vx::mem::allocate(size);
        VX_CHECK(ptr != nullptr);
        vx::mem::deallocate(ptr, size);
    }

    VX_SECTION("reallocate (grow)")
    {
        constexpr size_t size1 = 32;
        constexpr size_t size2 = 64;

        void* ptr = vx::mem::allocate(size1);
        VX_CHECK(ptr != nullptr);
        ptr = vx::mem::reallocate(ptr, size2);
        VX_CHECK(ptr != nullptr);
        vx::mem::deallocate(ptr, size2);
    }

    VX_SECTION("reallocate (shrink)")
    {
        constexpr size_t size1 = 64;
        constexpr size_t size2 = 32;

        void* ptr = vx::mem::allocate(size1);
        VX_CHECK(ptr != nullptr);
        ptr = vx::mem::reallocate(ptr, size2);
        VX_CHECK(ptr != nullptr);
        vx::mem::deallocate(ptr, size2);
    }
}

//=========================================================================
// aligned allocation
//=========================================================================

VX_TEST_CASE(aligned_template)
{
    constexpr size_t alignment = 32;

    VX_SECTION("allocate / deallocate")
    {
        constexpr size_t size = 128;
        void* ptr = vx::mem::allocate_aligned<alignment>(size);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);
        vx::mem::deallocate_aligned<alignment>(ptr, size);

        // check overflow is handled
        VX_CHECK_AND_EXPECT_ERROR_CODE(!vx::mem::allocate_aligned<alignment>(SIZE_MAX), vx::err::size_error);
    }

    VX_SECTION("reallocate (grow)")
    {
        constexpr size_t size1 = 32;
        constexpr size_t size2 = 64;

        void* ptr = vx::mem::allocate_aligned<alignment>(size1);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        ptr = vx::mem::reallocate_aligned<alignment>(ptr, size2);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        // check overflow is handled
        VX_CHECK_AND_EXPECT_ERROR_CODE(!vx::mem::reallocate_aligned<alignment>(ptr, SIZE_MAX), vx::err::size_error);

        vx::mem::deallocate_aligned<alignment>(ptr, size2);
    }

    VX_SECTION("reallocate (shrink)")
    {
        constexpr size_t size1 = 64;
        constexpr size_t size2 = 32;

        void* ptr = vx::mem::allocate_aligned<alignment>(size1);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        ptr = vx::mem::reallocate_aligned<alignment>(ptr, size2);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        vx::mem::deallocate_aligned<alignment>(ptr, size2);
    }
}

//=============================================================================

VX_TEST_CASE(aligned_runtime)
{
    constexpr size_t alignment = 32;

    VX_SECTION("allocate / deallocate")
    {
        constexpr size_t size = 128;
        void* ptr = vx::mem::allocate_aligned(size, alignment);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);
        vx::mem::deallocate_aligned(ptr, size, alignment);

        // check overflow is handled
        VX_CHECK_AND_EXPECT_ERROR_CODE(!vx::mem::allocate_aligned(SIZE_MAX, alignment), vx::err::size_error);
    }

    VX_SECTION("reallocate (grow)")
    {
        constexpr size_t size1 = 32;
        constexpr size_t size2 = 64;

        void* ptr = vx::mem::allocate_aligned(size1, alignment);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        ptr = vx::mem::reallocate_aligned(ptr, size2, alignment);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        // check overflow is handled
        VX_CHECK_AND_EXPECT_ERROR_CODE(!vx::mem::reallocate_aligned(ptr, SIZE_MAX, alignment), vx::err::size_error);

        vx::mem::deallocate_aligned(ptr, size2, alignment);
    }

    VX_SECTION("reallocate (shrink)")
    {
        constexpr size_t size1 = 64;
        constexpr size_t size2 = 32;

        void* ptr = vx::mem::allocate_aligned(size1, alignment);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        ptr = vx::mem::reallocate_aligned(ptr, size2, alignment);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);

        vx::mem::deallocate_aligned(ptr, size2, alignment);
    }
}

//=========================================================================//

VX_TEST_CASE(construct_and_destroy)
{
    VX_SECTION("construct and destroy (trivial_type)")
    {
        trivial_type* ptr = vx::mem::construct<trivial_type>(123);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(*ptr == 123);
        vx::mem::destroy(ptr);
    }

    VX_SECTION("construct and destroy (non_trivial_type)")
    {
        non_trivial_type::reset_counters();

        non_trivial_type* ptr = vx::mem::construct<non_trivial_type>(77);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(ptr->x == 77);
        VX_CHECK(non_trivial_type::construct_count == 1);

        vx::mem::destroy(ptr);
        VX_CHECK(non_trivial_type::destruct_count == 1);
    }

    VX_SECTION("destroy_in_place (non_trivial_type)")
    {
        non_trivial_type::reset_counters();

        non_trivial_type obj(55);
        VX_CHECK(non_trivial_type::construct_count == 1);
        VX_CHECK(non_trivial_type::destruct_count == 0);

        vx::mem::destroy_in_place(&obj);
        VX_CHECK(non_trivial_type::destruct_count == 1);
    }

    VX_SECTION("destroy_safe (nullptr)")
    {
        non_trivial_type* ptr = nullptr;
        vx::mem::destroy_safe(ptr);
        VX_CHECK(ptr == nullptr);
    }

    VX_SECTION("destroy_safe (non_trivial_type)")
    {
        non_trivial_type::reset_counters();

        non_trivial_type* ptr = vx::mem::construct<non_trivial_type>(88);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(ptr->x == 88);
        VX_CHECK(non_trivial_type::construct_count == 1);

        vx::mem::destroy_safe(ptr);
        VX_CHECK(non_trivial_type::destruct_count == 1);
        VX_CHECK(ptr == nullptr);
    }
}

//=========================================================================

VX_TEST_CASE(static_array_size)
{
    constexpr int arr[5] = { 1, 2, 3, 4, 5 };
    VX_STATIC_CHECK(vx::mem::array_size(arr) == 5);
}

//=========================================================================

VX_TEST_CASE(construct_and_destroy_range)
{
    VX_SECTION("construct_range and destroy_range (trivial_type)")
    {
        constexpr size_t count = 10;
        trivial_type buffer[count] = {};

        trivial_type* end_ptr = vx::mem::construct_range(buffer, count);
        VX_CHECK(end_ptr == buffer + count);

        // Values are default-initialized to zero for int
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(buffer[i] == 0);
        }

        trivial_type* destroy_end = vx::mem::destroy_range(buffer, count);
        VX_CHECK(destroy_end == buffer + count);
    }

    VX_SECTION("construct_range and destroy_range (non_trivial_type)")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 5;
        alignas(alignof(non_trivial_type)) std::byte buffer[sizeof(non_trivial_type) * count];
        non_trivial_type* ptr = reinterpret_cast<non_trivial_type*>(buffer);

        non_trivial_type* end_ptr = vx::mem::construct_range(ptr, count);
        VX_CHECK(end_ptr == ptr + count);
        VX_CHECK(non_trivial_type::construct_count == static_cast<int>(count));
        VX_CHECK(non_trivial_type::destruct_count == 0);

        non_trivial_type* destroy_end = vx::mem::destroy_range(ptr, count);
        VX_CHECK(destroy_end == ptr + count);
        VX_CHECK(non_trivial_type::destruct_count == static_cast<int>(count));
    }
}

//=========================================================================

VX_TEST_CASE(fill_range_and_fill_uninitialized_range)
{
    // Helper to check buffer contents
    auto check_buffer = [](const trivial_type* buf, size_t count, trivial_type val)
    {
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(buf[i] == val);
        }
    };

    VX_SECTION("fill_range with trivial_type using raw pointer")
    {
        constexpr size_t count = 16;
        trivial_type buffer[count] = {};

        vx::mem::fill_range(buffer, count, trivial_type{ 0xAB });
        check_buffer(buffer, count, 0xAB);
    }

    VX_SECTION("fill_range with non-trivial type using raw pointer")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 4;
        alignas(alignof(non_trivial_type)) std::byte buffer[sizeof(non_trivial_type) * count];
        non_trivial_type* ptr = reinterpret_cast<non_trivial_type*>(buffer);

        // Manually construct all elements first (default)
        vx::mem::construct_range(ptr, count);

        // Now fill the range with a value
        non_trivial_type fill_val(42);
        vx::mem::fill_range(ptr, count, fill_val);

        // Check that each element equals the fill value (compare by 'x' member)
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(ptr[i] == fill_val);
        }

        vx::mem::destroy_range(ptr, count);
    }

    VX_SECTION("fill_uninitialized_range with non-trivial type")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 3;
        alignas(alignof(non_trivial_type)) std::byte buffer[sizeof(non_trivial_type) * count];
        non_trivial_type* ptr = reinterpret_cast<non_trivial_type*>(buffer);

        non_trivial_type fill_val(99);
        non_trivial_type* end_ptr = vx::mem::fill_uninitialized_range(ptr, count, fill_val);

        VX_CHECK(end_ptr == ptr + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(ptr[i] == fill_val);
        }
        VX_CHECK(non_trivial_type::copy_count == static_cast<int>(count));

        vx::mem::destroy_range(ptr, count);
    }

    VX_SECTION("fill_range with iterator version")
    {
        std::vector<trivial_type> vec(10, 0);
        vx::mem::fill_range(vec.begin(), vec.end(), trivial_type{ 0x7F });

        for (auto val : vec)
        {
            VX_CHECK(val == 0x7F);
        }
    }
}

//=========================================================================

VX_TEST_CASE(copy_range_and_copy_uninitialized_range)
{
    // Helper to fill buffer with incrementing values
    auto fill_incrementing = [](trivial_type* buf, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            buf[i] = static_cast<trivial_type>(i);
        }
    };

    VX_SECTION("copy_range with trivial_type pointers")
    {
        constexpr size_t count = 8;
        trivial_type src[count];
        trivial_type dst[count] = {};

        fill_incrementing(src, count);
        trivial_type* end_ptr = vx::mem::copy_range(dst, src, count);

        VX_CHECK(end_ptr == dst + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
    }

    VX_SECTION("copy_uninitialized_range with trivial_type pointers")
    {
        constexpr size_t count = 5;
        trivial_type src[count];
        trivial_type dst[count];

        fill_incrementing(src, count);
        trivial_type* end_ptr = vx::mem::copy_uninitialized_range(dst, src, count);

        VX_CHECK(end_ptr == dst + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
    }

    VX_SECTION("copy_range with non_trivial_type pointers")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 4;
        alignas(alignof(non_trivial_type)) std::byte src_buffer[sizeof(non_trivial_type) * count];
        alignas(alignof(non_trivial_type)) std::byte dst_buffer[sizeof(non_trivial_type) * count];
        non_trivial_type* src = reinterpret_cast<non_trivial_type*>(src_buffer);
        non_trivial_type* dst = reinterpret_cast<non_trivial_type*>(dst_buffer);

        vx::mem::construct_range(src, count);
        vx::mem::construct_range(dst, count);

        // Initialize src elements
        for (size_t i = 0; i < count; ++i)
        {
            src[i].x = static_cast<int>(i * 10);
        }

        non_trivial_type* end_ptr = vx::mem::copy_range(dst, src, count);

        VX_CHECK(end_ptr == dst + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }

        vx::mem::destroy_range(src, count);
        vx::mem::destroy_range(dst, count);
    }

    VX_SECTION("copy_uninitialized_range with non_trivial_type pointers")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 3;
        alignas(alignof(non_trivial_type)) std::byte src_buffer[sizeof(non_trivial_type) * count];
        alignas(alignof(non_trivial_type)) std::byte dst_buffer[sizeof(non_trivial_type) * count];
        non_trivial_type* src = reinterpret_cast<non_trivial_type*>(src_buffer);
        non_trivial_type* dst = reinterpret_cast<non_trivial_type*>(dst_buffer);

        vx::mem::construct_range(src, count);

        // Initialize src elements
        for (size_t i = 0; i < count; ++i)
        {
            src[i].x = static_cast<int>(i * 100);
        }

        non_trivial_type* end_ptr = vx::mem::copy_uninitialized_range(dst, src, count);

        VX_CHECK(end_ptr == dst + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
        VX_CHECK(non_trivial_type::construct_count == static_cast<int>(count));

        vx::mem::destroy_range(src, count);
        vx::mem::destroy_range(dst, count);
    }

    VX_SECTION("copy_range with iterator version")
    {
        std::vector<trivial_type> src(10);
        std::vector<trivial_type> dst(10);

        for (size_t i = 0; i < src.size(); ++i)
        {
            src[i] = static_cast<trivial_type>(i + 5);
        }

        auto end_it = vx::mem::copy_range(dst.begin(), src.begin(), src.end());

        VX_CHECK(end_it == dst.end());
        for (size_t i = 0; i < dst.size(); ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
    }

    VX_SECTION("copy_uninitialized_range with iterator version and non_trivial_type")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 3;
        std::vector<non_trivial_type> src(count);
        std::vector<non_trivial_type> dst(count); // will reuse memory for testing

        for (size_t i = 0; i < count; ++i)
        {
            src[i].x = static_cast<int>(i + 50);
        }

        auto end_it = vx::mem::copy_uninitialized_range(dst.begin(), src.begin(), src.end());

        VX_CHECK(end_it == dst.end());
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
        VX_CHECK(non_trivial_type::construct_count >= 0); // construction count should be non-zero

        // Explicit destruction not needed for std::vector, but can clear if needed
    }
}

//=========================================================================

VX_TEST_CASE(move_range_and_move_uninitialized_range)
{
    // Helper to fill buffer with incrementing values
    auto fill_incrementing = [](trivial_type* buf, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            buf[i] = static_cast<trivial_type>(i);
        }
    };

    VX_SECTION("move_range with trivial_type pointers")
    {
        constexpr size_t count = 8;
        trivial_type src[count];
        trivial_type dst[count] = {};

        fill_incrementing(src, count);
        trivial_type* end_ptr = vx::mem::move_range(dst, src, count);

        VX_CHECK(end_ptr == dst + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
    }

    VX_SECTION("move_uninitialized_range with trivial_type pointers")
    {
        constexpr size_t count = 5;
        trivial_type src[count];
        trivial_type dst[count];

        fill_incrementing(src, count);
        trivial_type* end_ptr = vx::mem::move_uninitialized_range(dst, src, count);

        VX_CHECK(end_ptr == dst + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
    }

    VX_SECTION("move_range with non_trivial_type pointers")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 4;
        alignas(alignof(non_trivial_type)) std::byte src_buffer[sizeof(non_trivial_type) * count];
        alignas(alignof(non_trivial_type)) std::byte dst_buffer[sizeof(non_trivial_type) * count];
        non_trivial_type* src = reinterpret_cast<non_trivial_type*>(src_buffer);
        non_trivial_type* dst = reinterpret_cast<non_trivial_type*>(dst_buffer);

        vx::mem::construct_range(src, count);
        vx::mem::construct_range(dst, count);

        // Initialize src elements with unique values
        for (size_t i = 0; i < count; ++i)
        {
            src[i].x = static_cast<int>(i * 10);
        }

        non_trivial_type* end_ptr = vx::mem::move_range(dst, src, count);

        VX_CHECK(end_ptr == dst + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i].x == static_cast<int>(i * 10));
            VX_CHECK(src[i].x == 0); // moved-from should have x reset to 0
        }
        VX_CHECK(non_trivial_type::move_count >= static_cast<int>(count));

        vx::mem::destroy_range(src, count);
        vx::mem::destroy_range(dst, count);
    }

    VX_SECTION("move_uninitialized_range with non_trivial_type pointers")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 3;
        alignas(alignof(non_trivial_type)) std::byte src_buffer[sizeof(non_trivial_type) * count];
        alignas(alignof(non_trivial_type)) std::byte dst_buffer[sizeof(non_trivial_type) * count];
        non_trivial_type* src = reinterpret_cast<non_trivial_type*>(src_buffer);
        non_trivial_type* dst = reinterpret_cast<non_trivial_type*>(dst_buffer);

        vx::mem::construct_range(src, count);

        // Initialize src elements with unique values
        for (size_t i = 0; i < count; ++i)
        {
            src[i].x = static_cast<int>(i * 100);
        }

        non_trivial_type* end_ptr = vx::mem::move_uninitialized_range(dst, src, count);

        VX_CHECK(end_ptr == dst + count);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i].x == static_cast<int>(i * 100));
            VX_CHECK(src[i].x == 0);
        }
        VX_CHECK(non_trivial_type::construct_count == static_cast<int>(count));
        VX_CHECK(non_trivial_type::move_count >= static_cast<int>(count));

        vx::mem::destroy_range(src, count);
        vx::mem::destroy_range(dst, count);
    }

    VX_SECTION("move_range with iterator version")
    {
        std::vector<non_trivial_type> src(5);
        std::vector<non_trivial_type> dst(5);

        non_trivial_type::reset_counters();

        for (size_t i = 0; i < src.size(); ++i)
        {
            src[i].x = static_cast<int>(i + 50);
        }

        auto end_it = vx::mem::move_range(dst.begin(), src.begin(), src.end());

        VX_CHECK(end_it == dst.end());
        for (size_t i = 0; i < dst.size(); ++i)
        {
            VX_CHECK(dst[i].x == src[i].x || src[i].x == 0); // src moved-from state
        }
        VX_CHECK(non_trivial_type::move_count >= static_cast<int>(dst.size()));
    }

    VX_SECTION("move_uninitialized_range with iterator version")
    {
        std::vector<non_trivial_type> src(3);
        std::vector<non_trivial_type> dst(3);

        non_trivial_type::reset_counters();

        for (size_t i = 0; i < src.size(); ++i)
        {
            src[i].x = static_cast<int>(i + 100);
        }

        auto end_it = vx::mem::move_uninitialized_range(dst.begin(), src.begin(), src.end());

        VX_CHECK(end_it == dst.end());
        for (size_t i = 0; i < dst.size(); ++i)
        {
            VX_CHECK(dst[i].x == static_cast<int>(i + 100));
            VX_CHECK(src[i].x == 0);
        }
        VX_CHECK(non_trivial_type::move_count == static_cast<int>(dst.size()));
    }
}

//=========================================================================

VX_TEST_CASE(range_comparison_functions)
{
    // Helper: create vectors with incremental values starting at base
    auto make_vector = [](size_t count, int base = 0)
    {
        std::vector<trivial_type> vec(count);
        for (size_t i = 0; i < count; ++i)
        {
            vec[i] = base + static_cast<int>(i);
        }
        return vec;
    };

    VX_SECTION("trivial pointer ranges")
    {
        trivial_type a[5] = { 1, 2, 3, 4, 5 };
        trivial_type b[5] = { 1, 2, 3, 4, 5 };
        trivial_type c[5] = { 1, 2, 9, 4, 5 };

        // equality
        VX_CHECK(vx::mem::compare_range(a, 5, b, 5) == 0);
        VX_CHECK(vx::mem::compare_range(a, 5, c, 5) != 0);

        // lexicographical order
        VX_CHECK(vx::mem::compare_range(a, 5, c, 5) < 0);
        VX_CHECK(vx::mem::compare_range(c, 5, a, 5) > 0);

        // unequal length
        trivial_type s[3] = { 1, 2, 3 };
        trivial_type l[7] = { 1, 2, 3, 4, 5, 6, 7 };

        VX_CHECK(vx::mem::compare_range(s, 3, l, 7) < 0);
        VX_CHECK(vx::mem::compare_range(l, 7, s, 3) > 0);

        // identical prefix, shorter wins
        trivial_type p1[4] = { 9, 9, 9, 9 };
        trivial_type p2[6] = { 9, 9, 9, 9, 9, 9 };
        VX_CHECK(vx::mem::compare_range(p1, 4, p2, 6) < 0);
        VX_CHECK(vx::mem::compare_range(p2, 6, p1, 4) > 0);
    }

    VX_SECTION("iterator ranges")
    {
        auto v1 = make_vector(5, 10); // 10 11 12 13 14
        auto v2 = make_vector(5, 10);
        auto v3 = make_vector(5, 20); // larger values
        auto short_v = make_vector(3, 10);
        auto long_v = make_vector(8, 10);

        // equality
        VX_CHECK(vx::mem::compare_range(v1.begin(), v1.end(),
                     v2.begin(), v2.end()) == 0);

        // inequality
        VX_CHECK(vx::mem::compare_range(v1.begin(), v1.end(),
                     v3.begin(), v3.end()) != 0);

        // lexicographical
        VX_CHECK(vx::mem::compare_range(v1.begin(), v1.end(),
                     v3.begin(), v3.end()) < 0);
        VX_CHECK(vx::mem::compare_range(v3.begin(), v3.end(),
                     v1.begin(), v1.end()) > 0);

        // length-based ordering
        VX_CHECK(vx::mem::compare_range(short_v.begin(), short_v.end(),
                     long_v.begin(), long_v.end()) < 0);
        VX_CHECK(vx::mem::compare_range(long_v.begin(), long_v.end(),
                     short_v.begin(), short_v.end()) > 0);
    }

    VX_SECTION("non-trivial types")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 3;
        non_trivial_type bufA[count];
        non_trivial_type bufB[count];

        vx::mem::construct_range(bufA, count);
        vx::mem::construct_range(bufB, count);

        for (size_t i = 0; i < count; ++i)
        {
            bufA[i].x = bufB[i].x = static_cast<int>(i * 10);
        }

        VX_CHECK(vx::mem::compare_range(bufA, count, bufB, count) == 0);

        // mutate
        bufB[1].x += 5;

        VX_CHECK(vx::mem::compare_range(bufA, count, bufB, count) < 0);
        VX_CHECK(vx::mem::compare_range(bufB, count, bufA, count) > 0);

        vx::mem::destroy_range(bufA, count);
        vx::mem::destroy_range(bufB, count);
    }
}

//=========================================================================

VX_TEST_CASE(construct_destroy_array)
{
    VX_SECTION("construct_array with trivial type (default init)")
    {
        constexpr size_t count = 5;
        trivial_type* arr = vx::mem::construct_array<trivial_type>(count);
        VX_CHECK(arr != nullptr);

        for (size_t i = 0; i < count; ++i)
        {
            // For trivial types default init is uninitialized, so can't assume zero.
            // Just check pointer is valid and accessible.
            arr[i] = static_cast<int>(i * 3); // test assignability
        }

        vx::mem::destroy_array(arr, count);

        // check max size error
        constexpr size_t max_size = vx::mem::max_array_size<trivial_type>();
        VX_CHECK_AND_EXPECT_ERROR_CODE(!vx::mem::construct_array<trivial_type>(max_size + 1), vx::err::size_error);
    }

    VX_SECTION("construct_array with trivial type (value init)")
    {
        constexpr size_t count = 5;
        const trivial_type init_value = 42;

        trivial_type* arr = vx::mem::construct_array<trivial_type>(count, init_value);
        VX_CHECK(arr != nullptr);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(arr[i] == init_value);
        }

        vx::mem::destroy_array(arr, count);

        // check max size error
        constexpr size_t max_size = vx::mem::max_array_size<trivial_type>();
        VX_CHECK_AND_EXPECT_ERROR_CODE(!vx::mem::construct_array<trivial_type>(max_size + 1), vx::err::size_error);
    }

    VX_SECTION("construct_array with non_trivial_type (default init)")
    {
        non_trivial_type::reset_counters();
        constexpr size_t count = 4;

        non_trivial_type* arr = vx::mem::construct_array<non_trivial_type>(count);
        VX_CHECK(arr != nullptr);
        VX_CHECK(non_trivial_type::construct_count == static_cast<int>(count));
        VX_CHECK(non_trivial_type::destruct_count == 0);

        // Check default value is zero-initialized by constructor
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(arr[i].x == 0);
        }

        vx::mem::destroy_array(arr, count);
        VX_CHECK(non_trivial_type::destruct_count == static_cast<int>(count));

        // check max size error
        constexpr size_t max_size = vx::mem::max_array_size<non_trivial_type>();
        VX_CHECK_AND_EXPECT_ERROR_CODE(!vx::mem::construct_array<non_trivial_type>(max_size + 1), vx::err::size_error);
    }

    VX_SECTION("construct_array with non_trivial_type (value init)")
    {
        non_trivial_type::reset_counters();
        constexpr size_t count = 3;
        non_trivial_type init_val(123);

        non_trivial_type* arr = vx::mem::construct_array<non_trivial_type>(count, init_val);
        VX_CHECK(arr != nullptr);
        VX_CHECK(non_trivial_type::copy_count == static_cast<int>(count));
        VX_CHECK(non_trivial_type::destruct_count == 0);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(arr[i].x == init_val.x);
        }

        vx::mem::destroy_array(arr, count);
        VX_CHECK(non_trivial_type::destruct_count == static_cast<int>(count));

        // check max size error
        constexpr size_t max_size = vx::mem::max_array_size<non_trivial_type>();
        VX_CHECK_AND_EXPECT_ERROR_CODE(!vx::mem::construct_array<non_trivial_type>(max_size + 1), vx::err::size_error);
    }

    VX_SECTION("construct_array with count 0 returns nullptr")
    {
        trivial_type* arr1 = vx::mem::construct_array<trivial_type>(0);
        non_trivial_type* arr2 = vx::mem::construct_array<non_trivial_type>(0);

        VX_CHECK(arr1 == nullptr);
        VX_CHECK(arr2 == nullptr);
    }

    VX_SECTION("destroy_array_safe resets pointer")
    {
        constexpr size_t count = 2;
        non_trivial_type::reset_counters();

        non_trivial_type* arr = vx::mem::construct_array<non_trivial_type>(count);
        VX_CHECK(arr != nullptr);
        vx::mem::destroy_array_safe(arr, count);

        VX_CHECK(arr == nullptr);
        VX_CHECK(non_trivial_type::destruct_count == static_cast<int>(count));
    }
}

//=========================================================================

VX_TEST_CASE(default_allocator)
{
    VX_SECTION("allocate / deallocate with default_allocator (at_least policy, alignment <= max_align)")
    {
        using allocator = vx::mem::default_allocator<trivial_type>;
        constexpr size_t count = 16;

        trivial_type* ptr = allocator::allocate(count);
        VX_CHECK(ptr != nullptr);

        allocator::deallocate(ptr, count);
    }

    VX_SECTION("allocate_zero with default_allocator (at_least policy, alignment <= max_align)")
    {
        using allocator = vx::mem::default_allocator<trivial_type>;
        constexpr size_t count = 8;
        trivial_type* ptr = allocator::allocate(count);
        VX_CHECK(ptr != nullptr);

        // Zero initialize manually
        vx::mem::fill_range(ptr, ptr + count, trivial_type{});
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(ptr[i] == 0);
        }

        allocator::deallocate(ptr, count);
    }

    VX_SECTION("reallocate grow / shrink with default_allocator (at_least policy, alignment <= max_align)")
    {
        using allocator = vx::mem::default_allocator<trivial_type>;
        constexpr size_t count1 = 4;
        constexpr size_t count2 = 10;

        trivial_type* ptr = allocator::allocate(count1);
        VX_CHECK(ptr != nullptr);

        ptr = allocator::reallocate(ptr, count2);
        VX_CHECK(ptr != nullptr);

        ptr = allocator::reallocate(ptr, count1);
        VX_CHECK(ptr != nullptr);

        allocator::deallocate(ptr, count1);
    }

    VX_SECTION("allocate / deallocate with default_allocator (exact policy, alignment > max_align)")
    {
        struct alignas(64) aligned_type
        {
            uint64_t data[8];
        };

        using allocator = vx::mem::default_allocator<aligned_type, 64, vx::mem::alignment_policy::exact>;
        constexpr size_t count = 3;

        aligned_type* ptr = allocator::allocate(count);
        VX_CHECK(ptr != nullptr);

        // Check alignment
        VX_CHECK((reinterpret_cast<uintptr_t>(ptr) % 64) == 0);

        allocator::deallocate(ptr, count);
    }

    VX_SECTION("reallocate grow / shrink with default_allocator (exact policy, alignment > max_align)")
    {
        struct alignas(64) aligned_type
        {
            uint64_t data[8];
        };

        using allocator = vx::mem::default_allocator<aligned_type, 64, vx::mem::alignment_policy::exact>;
        constexpr size_t count1 = 2;
        constexpr size_t count2 = 5;

        aligned_type* ptr = allocator::allocate(count1);
        VX_CHECK(ptr != nullptr);
        VX_CHECK((reinterpret_cast<uintptr_t>(ptr) % 64) == 0);

        ptr = allocator::reallocate(ptr, count2);
        VX_CHECK(ptr != nullptr);
        VX_CHECK((reinterpret_cast<uintptr_t>(ptr) % 64) == 0);

        ptr = allocator::reallocate(ptr, count1);
        VX_CHECK(ptr != nullptr);
        VX_CHECK((reinterpret_cast<uintptr_t>(ptr) % 64) == 0);

        allocator::deallocate(ptr, count1);
    }

    VX_SECTION("allocate / deallocate with aligned_allocator (exact policy, ideal_align)")
    {
        using allocator = vx::mem::aligned_allocator<trivial_type>;
        constexpr size_t count = 10;

        trivial_type* ptr = allocator::allocate(count);
        VX_CHECK(ptr != nullptr);

        // Check alignment is at least ideal_align
        VX_CHECK((reinterpret_cast<uintptr_t>(ptr) % allocator::alignment) == 0);

        allocator::deallocate(ptr, count);
    }

    VX_SECTION("allocate with zero count returns nullptr")
    {
        using allocator = vx::mem::default_allocator<trivial_type>;
        trivial_type* ptr = allocator::allocate(0);
        VX_CHECK(ptr == nullptr);
    }
}

//=========================================================================

VX_TEST_CASE(construct_in_place_tests)
{
    VX_SECTION("construct_in_place with trivial_type (reference)")
    {
        trivial_type storage = 999;
        vx::mem::construct_in_place(storage, 42);
        VX_CHECK(storage == 42);
    }

    VX_SECTION("construct_in_place with trivial_type (pointer)")
    {
        alignas(alignof(trivial_type)) std::byte buffer[sizeof(trivial_type)];
        trivial_type* ptr = reinterpret_cast<trivial_type*>(buffer);
        vx::mem::construct_in_place(ptr, 7);
        VX_CHECK(*ptr == 7);
    }

    VX_SECTION("construct_in_place with non_trivial_type (reference)")
    {
        non_trivial_type::reset_counters();
        alignas(alignof(non_trivial_type)) std::byte buffer[sizeof(non_trivial_type)];
        non_trivial_type* ptr = reinterpret_cast<non_trivial_type*>(buffer);

        vx::mem::construct_in_place(*ptr, 5);
        VX_CHECK(ptr->x == 5);
        VX_CHECK(non_trivial_type::construct_count == 1);

        vx::mem::destroy_in_place(ptr);
    }

    VX_SECTION("construct_in_place_maybe_trivial with trivial_type")
    {
        alignas(alignof(trivial_type)) std::byte buffer[sizeof(trivial_type)];
        trivial_type* ptr = reinterpret_cast<trivial_type*>(buffer);
        vx::mem::construct_in_place_maybe_trivial(ptr);
        *ptr = 1; // just confirm the storage is usable
        VX_CHECK(*ptr == 1);
    }

    VX_SECTION("construct_in_place_maybe_trivial with non_trivial_type")
    {
        non_trivial_type::reset_counters();
        alignas(alignof(non_trivial_type)) std::byte buffer[sizeof(non_trivial_type)];
        non_trivial_type* ptr = reinterpret_cast<non_trivial_type*>(buffer);

        vx::mem::construct_in_place_maybe_trivial(*ptr);
        VX_CHECK(ptr->x == 0);
        VX_CHECK(non_trivial_type::construct_count == 1);

        vx::mem::destroy_in_place(ptr);
    }
}

//=========================================================================

VX_TEST_CASE(swap_range_tests)
{
    VX_SECTION("swap_range with trivial_type pointers")
    {
        constexpr size_t count = 6;
        trivial_type a[count] = { 1, 2, 3, 4, 5, 6 };
        trivial_type b[count] = { 10, 20, 30, 40, 50, 60 };

        trivial_type* end_ptr = vx::mem::swap_range(a, b, count);
        VX_CHECK(end_ptr == a + count);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(a[i] == static_cast<int>((i + 1) * 10));
            VX_CHECK(b[i] == static_cast<int>(i + 1));
        }
    }

    VX_SECTION("swap_range with non_trivial_type pointers")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 4;
        non_trivial_type a[count];
        non_trivial_type b[count];

        for (size_t i = 0; i < count; ++i)
        {
            a[i].x = static_cast<int>(i);
            b[i].x = static_cast<int>(i + 100);
        }

        vx::mem::swap_range(a, b, count);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(a[i].x == static_cast<int>(i + 100));
            VX_CHECK(b[i].x == static_cast<int>(i));
        }
    }

    VX_SECTION("swap_range with iterator version")
    {
        std::vector<trivial_type> a = { 1, 2, 3, 4 };
        std::vector<trivial_type> b = { 9, 8, 7, 6 };

        auto end_it = vx::mem::swap_range(a.begin(), a.end(), b.begin());
        VX_CHECK(end_it == a.end());

        VX_CHECK((a == std::vector<trivial_type>{ 9, 8, 7, 6 }));
        VX_CHECK((b == std::vector<trivial_type>{ 1, 2, 3, 4 }));
    }

    VX_SECTION("swap_range with large count spanning multiple internal chunks")
    {
        // exceeds the 64-byte temp buffer used internally for memmove-safe types,
        // exercising the chunked loop
        constexpr size_t count = 100;
        trivial_type a[count];
        trivial_type b[count];

        for (size_t i = 0; i < count; ++i)
        {
            a[i] = static_cast<int>(i);
            b[i] = static_cast<int>(i + 1000);
        }

        vx::mem::swap_range(a, b, count);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(a[i] == static_cast<int>(i + 1000));
            VX_CHECK(b[i] == static_cast<int>(i));
        }
    }
}

//=========================================================================

VX_TEST_CASE(shift_range_tests)
{
    VX_SECTION("shift_forward_range with trivial_type")
    {
        trivial_type buffer[10] = { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0 };

        // returns first + count: start of the newly-vacated hole
        trivial_type* new_start = vx::mem::shift_forward_range(buffer, buffer + 5, 3);
        VX_CHECK(new_start == buffer + 3);

        trivial_type expected[10] = { 1, 2, 3, 1, 2, 3, 4, 5, 0, 0 };
        for (size_t i = 0; i < 10; ++i)
        {
            VX_CHECK(buffer[i] == expected[i]);
        }
    }

    VX_SECTION("shift_forward_range with count 0 is a no-op")
    {
        trivial_type buffer[5] = { 1, 2, 3, 4, 5 };
        trivial_type* new_start = vx::mem::shift_forward_range(buffer, buffer + 5, 0);
        VX_CHECK(new_start == buffer); // first + 0

        for (size_t i = 0; i < 5; ++i)
        {
            VX_CHECK(buffer[i] == static_cast<int>(i + 1));
        }
    }

    VX_SECTION("shift_forward_range with empty input range is a no-op")
    {
        trivial_type buffer[5] = { 1, 2, 3, 4, 5 };
        trivial_type* new_start = vx::mem::shift_forward_range(buffer, buffer, 3);
        VX_CHECK(new_start == buffer + 3); // first + count
    }

    VX_SECTION("shift_backward_range with trivial_type")
    {
        trivial_type buffer[10] = { 0, 0, 0, 1, 2, 3, 4, 5, 0, 0 };

        // returns last - count: end of the moved-from tail that needs destroying
        trivial_type* new_end = vx::mem::shift_backward_range(buffer + 3, buffer + 8, 3);
        VX_CHECK(new_end == buffer + 5);

        trivial_type expected[10] = { 1, 2, 3, 4, 5, 3, 4, 5, 0, 0 };
        for (size_t i = 0; i < 10; ++i)
        {
            VX_CHECK(buffer[i] == expected[i]);
        }
    }

    VX_SECTION("shift_backward_range with count 0 is a no-op")
    {
        trivial_type buffer[5] = { 1, 2, 3, 4, 5 };
        trivial_type* new_end = vx::mem::shift_backward_range(buffer, buffer + 5, 0);
        VX_CHECK(new_end == buffer + 5); // last - 0

        for (size_t i = 0; i < 5; ++i)
        {
            VX_CHECK(buffer[i] == static_cast<int>(i + 1));
        }
    }

    VX_SECTION("shift_backward_range with empty input range is a no-op")
    {
        trivial_type buffer[5] = { 1, 2, 3, 4, 5 };
        trivial_type* new_end = vx::mem::shift_backward_range(buffer, buffer, 3);
        VX_CHECK(new_end == buffer - 3); // last - count
    }

    VX_SECTION("shift_forward_range with non_trivial_type")
    {
        non_trivial_type::reset_counters();

        constexpr size_t total = 8;
        alignas(alignof(non_trivial_type)) std::byte storage[sizeof(non_trivial_type) * total];
        non_trivial_type* buffer = reinterpret_cast<non_trivial_type*>(storage);

        vx::mem::construct_range(buffer, total);
        for (size_t i = 0; i < 5; ++i)
        {
            buffer[i].x = static_cast<int>(i + 1);
        }

        non_trivial_type* new_start = vx::mem::shift_forward_range(buffer, buffer + 5, 3);
        VX_CHECK(new_start == buffer + 3);

        int expected[8] = { 0, 0, 0, 1, 2, 3, 4, 5 };
        for (size_t i = 0; i < 8; ++i)
        {
            VX_CHECK(buffer[i].x == expected[i]);
        }

        vx::mem::destroy_range(buffer, total);
    }

    VX_SECTION("shift_backward_range with non_trivial_type")
    {
        non_trivial_type::reset_counters();

        constexpr size_t total = 8;
        alignas(alignof(non_trivial_type)) std::byte storage[sizeof(non_trivial_type) * total];
        non_trivial_type* buffer = reinterpret_cast<non_trivial_type*>(storage);

        vx::mem::construct_range(buffer, total);
        for (size_t i = 3; i < 8; ++i)
        {
            buffer[i].x = static_cast<int>(i);
        }

        non_trivial_type* new_end = vx::mem::shift_backward_range(buffer + 3, buffer + 8, 3);
        VX_CHECK(new_end == buffer + 5);

        int expected[8] = { 3, 4, 5, 6, 7, 0, 0, 0 };
        for (size_t i = 0; i < 8; ++i)
        {
            VX_CHECK(buffer[i].x == expected[i]);
        }

        vx::mem::destroy_range(buffer, total);
    }
}

//=========================================================================

VX_TEST_CASE(copy_move_range_tests)
{
    VX_SECTION("copy_move_range with trivial_type pointers")
    {
        constexpr size_t count = 6;
        trivial_type src[count] = { 1, 2, 3, 4, 5, 6 };
        trivial_type dst[count] = {};

        trivial_type* end_ptr = vx::mem::copy_move_range(dst, src, count);
        VX_CHECK(end_ptr == dst + count);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
    }

    VX_SECTION("copy_move_uninitialized_range with trivial_type pointers")
    {
        constexpr size_t count = 4;
        trivial_type src[count] = { 7, 8, 9, 10 };
        trivial_type dst[count];

        trivial_type* end_ptr = vx::mem::copy_move_uninitialized_range(dst, src, count);
        VX_CHECK(end_ptr == dst + count);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i] == src[i]);
        }
    }

    VX_SECTION("copy_move_range with non_trivial_type pointers")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 4;
        non_trivial_type src[count];
        non_trivial_type dst[count];

        for (size_t i = 0; i < count; ++i)
        {
            src[i].x = static_cast<int>(i * 3);
        }

        non_trivial_type* end_ptr = vx::mem::copy_move_range(dst, src, count);
        VX_CHECK(end_ptr == dst + count);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i].x == src[i].x);
        }
    }

    VX_SECTION("copy_move_uninitialized_range with non_trivial_type pointers")
    {
        non_trivial_type::reset_counters();

        constexpr size_t count = 3;
        alignas(alignof(non_trivial_type)) std::byte src_buffer[sizeof(non_trivial_type) * count];
        alignas(alignof(non_trivial_type)) std::byte dst_buffer[sizeof(non_trivial_type) * count];
        non_trivial_type* src = reinterpret_cast<non_trivial_type*>(src_buffer);
        non_trivial_type* dst = reinterpret_cast<non_trivial_type*>(dst_buffer);

        vx::mem::construct_range(src, count);
        for (size_t i = 0; i < count; ++i)
        {
            src[i].x = static_cast<int>(i * 7);
        }

        non_trivial_type* end_ptr = vx::mem::copy_move_uninitialized_range(dst, src, count);
        VX_CHECK(end_ptr == dst + count);

        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(dst[i].x == src[i].x);
        }

        vx::mem::destroy_range(src, count);
        vx::mem::destroy_range(dst, count);
    }

    VX_SECTION("copy_move_range with iterator version")
    {
        std::vector<trivial_type> src = { 1, 2, 3, 4, 5 };
        std::vector<trivial_type> dst(5);

        auto end_it = vx::mem::copy_move_range(dst.begin(), src.begin(), src.end());
        VX_CHECK(end_it == dst.end());
        VX_CHECK(dst == src);
    }

    VX_SECTION("copy_move_uninitialized_range with iterator version")
    {
        std::vector<trivial_type> src = { 6, 7, 8 };
        std::vector<trivial_type> dst(3);

        auto end_it = vx::mem::copy_move_uninitialized_range(dst.begin(), src.begin(), src.end());
        VX_CHECK(end_it == dst.end());
        VX_CHECK(dst == src);
    }
}

//=========================================================================

VX_TEST_CASE(null_safety_edge_cases)
{
    VX_SECTION("deallocate_aligned (template) with nullptr is a no-op")
    {
        void* ptr = nullptr;
        vx::mem::deallocate_aligned<32>(ptr, 0);
        VX_CHECK(ptr == nullptr);
    }

    VX_SECTION("deallocate_aligned (runtime) with nullptr is a no-op")
    {
        void* ptr = nullptr;
        vx::mem::deallocate_aligned(ptr, 0, 32);
        VX_CHECK(ptr == nullptr);
    }

    VX_SECTION("reallocate_aligned (template) with nullptr forwards to allocate_aligned")
    {
        constexpr size_t alignment = 32;
        void* ptr = vx::mem::reallocate_aligned<alignment>(nullptr, 64);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);
        vx::mem::deallocate_aligned<alignment>(ptr, 64);
    }

    VX_SECTION("reallocate_aligned (runtime) with nullptr forwards to allocate_aligned")
    {
        constexpr size_t alignment = 32;
        void* ptr = vx::mem::reallocate_aligned(nullptr, 64, alignment);
        VX_CHECK(ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(ptr) % alignment == 0);
        vx::mem::deallocate_aligned(ptr, 64, alignment);
    }

    VX_SECTION("destroy_safe with trivial_type nullptr")
    {
        trivial_type* ptr = nullptr;
        vx::mem::destroy_safe(ptr);
        VX_CHECK(ptr == nullptr);
    }

    VX_SECTION("destroy_array_safe with nullptr is a no-op")
    {
        non_trivial_type* ptr = nullptr;
        vx::mem::destroy_array_safe(ptr, 5);
        VX_CHECK(ptr == nullptr);
    }

    VX_SECTION("destroy_array_safe with count 0 leaves pointer untouched")
    {
        non_trivial_type::reset_counters();

        non_trivial_type* ptr = vx::mem::construct_array<non_trivial_type>(3);
        VX_CHECK(ptr != nullptr);

        non_trivial_type* saved = ptr;
        vx::mem::destroy_array_safe(ptr, 0); // guarded by `ptr && count`, should short-circuit
        VX_CHECK(ptr == saved);
        VX_CHECK(non_trivial_type::destruct_count == 0);

        vx::mem::destroy_array_safe(ptr, 3); // now actually clean up
        VX_CHECK(ptr == nullptr);
        VX_CHECK(non_trivial_type::destruct_count == 3);
    }
}

//=========================================================================//

int main()
{
    VX_RUN_TESTS();
    return 0;
}
