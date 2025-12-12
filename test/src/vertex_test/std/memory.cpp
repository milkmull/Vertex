#include "vertex_test/test.hpp"
#include "vertex/std/memory.hpp"

///////////////////////////////////////////////////////////////////////////////
// byteswap
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(unaligned)
{
    VX_SECTION("allocate and free (no header)")
    {
        void* ptr = vx::mem::allocate(128);
        VX_CHECK(ptr != nullptr);
        vx::mem::deallocate(ptr);
    }

    VX_SECTION("allocate_zero")
    {
        constexpr size_t size = 64;
        void* ptr = vx::mem::allocate_zero(size);
        VX_CHECK(ptr != nullptr);

        uint8_t* bytes = static_cast<uint8_t*>(ptr);
        for (size_t i = 0; i < size; ++i)
        {
            VX_CHECK(bytes[i] == 0);
        }

        vx::mem::deallocate(ptr);
    }

    VX_SECTION("reallocate (grow)")
    {
        void* ptr = vx::mem::allocate(32);
        VX_CHECK(ptr != nullptr);
        ptr = vx::mem::reallocate(ptr, 64);
        VX_CHECK(ptr != nullptr);
        vx::mem::deallocate(ptr);
    }

    VX_SECTION("reallocate (shrink)")
    {
        void* ptr = vx::mem::allocate(64);
        VX_CHECK(ptr != nullptr);
        ptr = vx::mem::reallocate(ptr, 32);
        VX_CHECK(ptr != nullptr);
        vx::mem::deallocate(ptr);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(aligned)
{
    VX_SECTION("allocate_aligned")
    {
        constexpr size_t alignment = 64;
        void* aligned_ptr = vx::mem::allocate_aligned(128, alignment);
        VX_CHECK(aligned_ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(aligned_ptr) % alignment == 0);
        vx::mem::deallocate_aligned(aligned_ptr, alignment);
    }

    VX_SECTION("reallocate_aligned")
    {
        constexpr size_t alignment = 64;
        void* ptr = vx::mem::allocate_aligned(128, alignment);
        VX_CHECK(ptr != nullptr);

        void* new_ptr = vx::mem::reallocate_aligned(ptr, 256, alignment);
        VX_CHECK(new_ptr != nullptr);
        VX_CHECK(reinterpret_cast<uintptr_t>(new_ptr) % alignment == 0);

        vx::mem::deallocate_aligned(new_ptr, alignment);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(construct_destroy)
{
    VX_SECTION("construct and destroy single object")
    {
        struct test_t
        {
            int x = 0;
            bool constructed = false;
            test_t() noexcept
                : x(42), constructed(true)
            {}
            ~test_t() noexcept
            {
                constructed = false;
            }
        };

        void* mem_ptr = vx::mem::allocate(sizeof(test_t));
        VX_CHECK(mem_ptr != nullptr);

        test_t* obj = vx::mem::construct<test_t>(static_cast<test_t*>(mem_ptr));
        VX_CHECK(obj->x == 42);

        vx::mem::destroy(obj);
    }
}

///////////////////////////////////////////////////////////////////////////////

struct instance_counter
{
    int value = 0;
    static int instances;
    instance_counter() noexcept
    {
        ++instances;
    }
    ~instance_counter() noexcept
    {
        --instances;
    }
};

int instance_counter::instances = 0;

VX_TEST_CASE(array_management)
{
    struct non_trivial_t
    {
        int value;
        non_trivial_t(int v) noexcept
            : value(v)
        {}
        non_trivial_t(const non_trivial_t& other) noexcept
            : value(other.value)
        {}
    };

    struct test_t
    {
        int value;
        test_t(int v) noexcept
            : value(v)
        {}
    };

    VX_SECTION("construct_array, array_size, destroy_array")
    {
        size_t count = 10;
        instance_counter* arr = vx::mem::construct_array<instance_counter>(count);
        VX_CHECK(arr != nullptr);
        VX_CHECK(instance_counter::instances == static_cast<int>(count));

        vx::mem::destroy_array(arr, count);
        VX_CHECK(instance_counter::instances == 0);
    }

    VX_SECTION("construct_array with args and construct_array_in_place")
    {
        size_t count = 5;
        test_t* arr = vx::mem::construct_array<test_t>(count, 42);
        VX_CHECK(arr != nullptr);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(arr[i].value == 42);
        }
        vx::mem::destroy_array(arr, count);

        void* raw = vx::mem::allocate(sizeof(test_t) * count);
        vx::mem::construct_array_in_place(static_cast<test_t*>(raw), count, 24);
        test_t* arr_in_place = static_cast<test_t*>(raw);
        for (size_t i = 0; i < count; ++i)
        {
            VX_CHECK(arr_in_place[i].value == 24);
        }

        // Manually destroy objects and free raw memory
        for (size_t i = 0; i < count; ++i)
        {
            arr_in_place[i].~test_t();
        }
        vx::mem::deallocate(raw);
    }

    VX_SECTION("copy_array_elements (trivial)")
    {
        int src[3] = { 1, 2, 3 };
        int* dst = static_cast<int*>(vx::mem::allocate(sizeof(int) * 3));
        vx::mem::copy_array_elements(dst, src, 3);
        VX_CHECK(dst[0] == 1);
        VX_CHECK(dst[1] == 2);
        VX_CHECK(dst[2] == 3);
        vx::mem::deallocate(dst);
    }

    VX_SECTION("copy_array_elements (non-trivial")
    {
        non_trivial_t source[2] = { non_trivial_t(10), non_trivial_t(20) };
        void* raw_mem = vx::mem::allocate(sizeof(non_trivial_t) * 2);
        non_trivial_t* dest_arr = static_cast<non_trivial_t*>(raw_mem);

        vx::mem::copy_array_elements(dest_arr, source, 2);
        VX_CHECK(dest_arr[0].value == 10);
        VX_CHECK(dest_arr[1].value == 20);

        for (int i = 0; i < 2; ++i)
        {
            dest_arr[i].~non_trivial_t();
        }
        vx::mem::deallocate(raw_mem);
    }

    VX_SECTION("destroy_array_elements (trivial)")
    {
        int* arr = static_cast<int*>(vx::mem::allocate(sizeof(int) * 3));
        arr[0] = 1;
        arr[1] = 2;
        arr[2] = 3;
        vx::mem::destroy_array_elements(arr, 3);
        VX_CHECK(arr[0] == 1);
        VX_CHECK(arr[1] == 2);
        VX_CHECK(arr[2] == 3);
        vx::mem::deallocate(arr);
    }

    VX_SECTION("destroy_array_elements (non-trivial)")
    {
        void* raw_mem = vx::mem::allocate(sizeof(instance_counter) * 2);
        instance_counter* arr = static_cast<instance_counter*>(raw_mem);
        new (&arr[0]) instance_counter();
        new (&arr[1]) instance_counter();
        VX_CHECK(instance_counter::instances == 2);
        vx::mem::destroy_array_elements(arr, 2);
        VX_CHECK(instance_counter::instances == 0);
    }

    VX_SECTION("destroy_array_safe")
    {
        size_t count = 3;
        instance_counter* arr = vx::mem::construct_array<instance_counter>(count);
        VX_CHECK(instance_counter::instances == static_cast<int>(count));

        vx::mem::destroy_array_safe(arr, count);
        VX_CHECK(instance_counter::instances == 0);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(memory_operations)
{
    VX_SECTION("copy")
    {
        const char src[] = "Hello, world!";
        char dst[20] = {};
        void* result = vx::mem::copy(dst, src, sizeof(src));
        VX_CHECK(result == dst);
        VX_CHECK(std::memcmp(dst, src, sizeof(src)) == 0);
    }

    VX_SECTION("move")
    {
        char buffer[20] = "Hello, world!";
        // Overlapping move within same buffer
        void* result = vx::mem::move(buffer + 2, buffer, 13); // move "Hello, world!" 2 bytes forward
        VX_CHECK(result == buffer + 2);
        VX_CHECK(std::memcmp(buffer + 2, "Hello, world!", 13) == 0);
    }

    VX_SECTION("set")
    {
        char buffer[10];
        void* result = vx::mem::set(buffer, 0xAA, sizeof(buffer));
        VX_CHECK(result == buffer);
        for (size_t i = 0; i < sizeof(buffer); ++i)
        {
            VX_CHECK(static_cast<uint8_t>(buffer[i]) == 0xAA);
        }
    }

    VX_SECTION("compare")
    {
        char a[] = "Test memory";
        char b[] = "Test memory";
        char c[] = "Test memoRy";

        VX_CHECK(vx::mem::compare(a, b, sizeof(a)) == 0);
        VX_CHECK(vx::mem::compare(a, c, sizeof(a)) != 0);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}
