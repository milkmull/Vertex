#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <utility>

#include "vertex/std/memory/_priv/memory_base.hpp"
#include "vertex/std/memory/memory.hpp"
#include "vertex_test/test.hpp"

namespace {

//=============================================================================
// helpers
//=============================================================================

bool is_aligned(const void* p, std::size_t alignment)
{
    return reinterpret_cast<std::uintptr_t>(p) % alignment == 0;
}

void fill_pattern(void* p, std::size_t n, unsigned char seed)
{
    auto* b = static_cast<unsigned char*>(p);
    for (std::size_t i = 0; i < n; ++i)
    {
        b[i] = static_cast<unsigned char>(seed + i * 7);
    }
}

bool check_pattern(const void* p, std::size_t n, unsigned char seed)
{
    const auto* b = static_cast<const unsigned char*>(p);
    for (std::size_t i = 0; i < n; ++i)
    {
        if (b[i] != static_cast<unsigned char>(seed + i * 7))
        {
            return false;
        }
    }
    return true;
}

// counts constructions / destructions
struct tracked
{
    static inline int ctors = 0;
    static inline int dtors = 0;

    int value;

    tracked() noexcept
        : value(0)
    {
        ++ctors;
    }

    explicit tracked(int v) noexcept
        : value(v)
    {
        ++ctors;
    }

    tracked(const tracked& o) noexcept
        : value(o.value)
    {
        ++ctors;
    }

    ~tracked()
    {
        ++dtors;
    }

    static void reset() noexcept
    {
        ctors = 0;
        dtors = 0;
    }
};

struct with_default
{
    int x = 5;
    int y = 6;
};

template <std::size_t Align>
void check_aligned_roundtrip()
{
    void* p = vx::mem::allocate_aligned<Align>(100);
    VX_CHECK(p != nullptr);
    VX_CHECK(is_aligned(p, Align));

    // the whole requested range must be writable
    fill_pattern(p, 100, 0x11);
    VX_CHECK(check_pattern(p, 100, 0x11));

    vx::mem::deallocate_aligned<Align>(p, 100);
}

} // namespace

//=============================================================================
// memory_base: array helpers
//=============================================================================

using vx::test::movable_int;
using vx::test::over_aligned;

VX_TEST_CASE(mem_array_helpers)
{
    int arr[7] = {};
    char big[100] = {};

    VX_STATIC_CHECK(vx::mem::array_size(arr) == 7);
    VX_STATIC_CHECK(vx::mem::array_size(big) == 100);
    VX_STATIC_CHECK(std::is_same<decltype(vx::mem::array_size(arr)), std::size_t>::value);

    // literal_size excludes the null terminator
    VX_STATIC_CHECK(vx::mem::literal_size("hello") == 5);
    VX_STATIC_CHECK(vx::mem::literal_size("") == 0);
    VX_STATIC_CHECK(vx::mem::literal_size(L"abc") == 3);

    constexpr std::size_t max = std::numeric_limits<std::size_t>::max();
    VX_STATIC_CHECK(vx::mem::max_array_size<char>() == max);
    VX_STATIC_CHECK(vx::mem::max_array_size<int>() == max / sizeof(int));
    VX_STATIC_CHECK(vx::mem::max_array_size<std::int64_t>() == max / sizeof(std::int64_t));
}

//=============================================================================
// memory_base: raw allocation
//=============================================================================

VX_TEST_CASE(mem_raw_allocation)
{
    VX_SECTION("allocate / deallocate");
    {
        void* p = vx::mem::allocate(64);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 64, 0x21);
        VX_CHECK(check_pattern(p, 64, 0x21));
        vx::mem::deallocate(p, 64);
    }

    VX_SECTION("deallocate_raw");
    {
        void* p = vx::mem::allocate(32);
        VX_CHECK(p != nullptr);
        vx::mem::deallocate_raw(p);
    }

    VX_SECTION("freeing null is a no-op");
    {
        vx::mem::deallocate_raw(nullptr);
        vx::mem::deallocate(nullptr, 0);
    }

    VX_SECTION("reallocate(nullptr, n) behaves like allocate");
    {
        void* p = vx::mem::reallocate(nullptr, 24);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 24, 0x31);
        VX_CHECK(check_pattern(p, 24, 0x31));
        vx::mem::deallocate_raw(p);
    }

    VX_SECTION("reallocate preserves contents when growing");
    {
        void* p = vx::mem::allocate(32);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 32, 0x41);

        void* q = vx::mem::reallocate(p, 1 << 16);
        VX_CHECK(q != nullptr);
        VX_CHECK(check_pattern(q, 32, 0x41));
        vx::mem::deallocate(q, 1 << 16);
    }

    VX_SECTION("reallocate preserves the prefix when shrinking");
    {
        void* p = vx::mem::allocate(256);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 256, 0x51);

        void* q = vx::mem::reallocate(p, 16);
        VX_CHECK(q != nullptr);
        VX_CHECK(check_pattern(q, 16, 0x51));
        vx::mem::deallocate(q, 16);
    }
}

//=============================================================================
// memory_base: copy / move / set / compare / find
//=============================================================================

VX_TEST_CASE(mem_copy)
{
    unsigned char src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    VX_SECTION("full copy returns dst");
    {
        unsigned char dst[8] = {};
        void* r = vx::mem::copy(dst, src, 8);
        VX_CHECK(r == dst);
        VX_CHECK(std::memcmp(dst, src, 8) == 0);
    }

    VX_SECTION("partial copy leaves the rest untouched");
    {
        unsigned char dst[8] = { 9, 9, 9, 9, 9, 9, 9, 9 };
        vx::mem::copy(dst + 2, src, 3);
        VX_CHECK(dst[0] == 9);
        VX_CHECK(dst[1] == 9);
        VX_CHECK(dst[2] == 1);
        VX_CHECK(dst[3] == 2);
        VX_CHECK(dst[4] == 3);
        VX_CHECK(dst[5] == 9);
    }

    VX_SECTION("zero bytes copies nothing");
    {
        unsigned char dst[4] = { 7, 7, 7, 7 };
        void* r = vx::mem::copy(dst, src, 0);
        VX_CHECK(r == dst);
        VX_CHECK(dst[0] == 7);
        VX_CHECK(dst[3] == 7);
    }

    VX_SECTION("copies non-char objects bytewise");
    {
        int a[3] = { 10, 20, 30 };
        int b[3] = {};
        vx::mem::copy(b, a, sizeof(a));
        VX_CHECK(b[0] == 10);
        VX_CHECK(b[1] == 20);
        VX_CHECK(b[2] == 30);
    }
}

VX_TEST_CASE(mem_move)
{
    VX_SECTION("non-overlapping");
    {
        unsigned char src[4] = { 1, 2, 3, 4 };
        unsigned char dst[4] = {};
        void* r = vx::mem::move(dst, src, 4);
        VX_CHECK(r == dst);
        VX_CHECK(std::memcmp(dst, src, 4) == 0);
    }

    VX_SECTION("overlapping, destination after source");
    {
        unsigned char a[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        void* r = vx::mem::move(a + 2, a, 6);
        VX_CHECK(r == a + 2);
        const unsigned char expected[8] = { 1, 2, 1, 2, 3, 4, 5, 6 };
        VX_CHECK(std::memcmp(a, expected, 8) == 0);
    }

    VX_SECTION("overlapping, destination before source");
    {
        unsigned char a[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        void* r = vx::mem::move(a, a + 2, 6);
        VX_CHECK(r == a);
        const unsigned char expected[8] = { 3, 4, 5, 6, 7, 8, 7, 8 };
        VX_CHECK(std::memcmp(a, expected, 8) == 0);
    }

    VX_SECTION("same pointer is a no-op");
    {
        unsigned char a[4] = { 1, 2, 3, 4 };
        vx::mem::move(a, a, 4);
        VX_CHECK(a[0] == 1);
        VX_CHECK(a[3] == 4);
    }

    VX_SECTION("zero bytes");
    {
        unsigned char a[4] = { 1, 2, 3, 4 };
        void* r = vx::mem::move(a, a + 1, 0);
        VX_CHECK(r == a);
        VX_CHECK(a[0] == 1);
    }
}

VX_TEST_CASE(mem_set)
{
    VX_SECTION("fills the requested range and returns dst");
    {
        unsigned char buf[8] = {};
        void* r = vx::mem::set(buf, 0xAB, 8);
        VX_CHECK(r == buf);
        for (unsigned char b : buf)
        {
            VX_CHECK(b == 0xAB);
        }
    }

    VX_SECTION("only touches [dst, dst + bytes)");
    {
        unsigned char buf[8];
        vx::mem::set(buf, 0xFF, 8);
        vx::mem::set(buf + 2, 0, 3);
        VX_CHECK(buf[1] == 0xFF);
        VX_CHECK(buf[2] == 0);
        VX_CHECK(buf[3] == 0);
        VX_CHECK(buf[4] == 0);
        VX_CHECK(buf[5] == 0xFF);
    }

    VX_SECTION("value is truncated to one byte");
    {
        unsigned char buf[4] = {};
        vx::mem::set(buf, 0x1FF, 4);
        VX_CHECK(buf[0] == 0xFF);
        VX_CHECK(buf[3] == 0xFF);

        vx::mem::set(buf, -1, 4);
        VX_CHECK(buf[0] == 0xFF);
    }

    VX_SECTION("zero bytes");
    {
        unsigned char buf[4] = { 1, 2, 3, 4 };
        vx::mem::set(buf, 0, 0);
        VX_CHECK(buf[0] == 1);
        VX_CHECK(buf[3] == 4);
    }
}

VX_TEST_CASE(mem_compare)
{
    const unsigned char a[3] = { 1, 2, 3 };
    const unsigned char b[3] = { 1, 2, 4 };

    VX_CHECK(vx::mem::compare(a, a, 3) == 0);
    VX_CHECK(vx::mem::compare(a, b, 3) < 0);
    VX_CHECK(vx::mem::compare(b, a, 3) > 0);

    VX_SECTION("only the first `bytes` are compared");
    {
        VX_CHECK(vx::mem::compare(a, b, 2) == 0);
    }

    VX_SECTION("zero bytes compare equal");
    {
        VX_CHECK(vx::mem::compare(a, b, 0) == 0);
    }

    VX_SECTION("bytes compare as unsigned");
    {
        const unsigned char hi[1] = { 0x80 };
        const unsigned char lo[1] = { 0x01 };
        VX_CHECK(vx::mem::compare(hi, lo, 1) > 0);
        VX_CHECK(vx::mem::compare(lo, hi, 1) < 0);
    }
}

VX_TEST_CASE(mem_find)
{
    const char buf[] = "abcabc"; // 7 bytes including the terminator

    VX_CHECK(vx::mem::find(buf, 'a', 7) == buf);
    VX_CHECK(vx::mem::find(buf, 'c', 7) == buf + 2); // first occurrence
    VX_CHECK(vx::mem::find(buf, 'z', 7) == nullptr);

    VX_SECTION("search is limited to `bytes`");
    {
        VX_CHECK(vx::mem::find(buf, 'c', 2) == nullptr);
        VX_CHECK(vx::mem::find(buf, 'c', 3) == buf + 2);
        VX_CHECK(vx::mem::find(buf, 'a', 0) == nullptr);
    }

    VX_SECTION("can find the null byte");
    {
        VX_CHECK(vx::mem::find(buf, 0, 7) == buf + 6);
    }

    VX_SECTION("value is truncated to one byte");
    {
        VX_CHECK(vx::mem::find(buf, 0x100 + 'c', 7) == buf + 2);
    }
}

//=============================================================================
// aligned memory: constants and layout
//=============================================================================

VX_TEST_CASE(mem_alignment_constants)
{
    namespace priv = vx::mem::_mem_priv;

    VX_SECTION("is_pow_2");
    {
        VX_CHECK(!priv::is_pow_2(0));
        VX_CHECK(priv::is_pow_2(1));
        VX_CHECK(priv::is_pow_2(2));
        VX_CHECK(!priv::is_pow_2(3));
        VX_CHECK(priv::is_pow_2(4));
        VX_CHECK(!priv::is_pow_2(6));
        VX_CHECK(priv::is_pow_2(1024));
        VX_CHECK(!priv::is_pow_2(1023));
        VX_CHECK(priv::is_pow_2(std::size_t(1) << (sizeof(std::size_t) * 8 - 1)));
        VX_CHECK(!priv::is_pow_2(std::numeric_limits<std::size_t>::max()));
    }

    VX_SECTION("max_align / ideal_align");
    {
        VX_CHECK(priv::is_pow_2(vx::mem::max_align));
        VX_CHECK(priv::is_pow_2(vx::mem::ideal_align));
        VX_CHECK(vx::mem::ideal_align >= 2 * sizeof(std::uintptr_t));
        VX_CHECK(vx::mem::max_align >= alignof(void*));
#if !(defined(__MINGW32__) && !defined(__MINGW64__))
        VX_CHECK(vx::mem::max_align == alignof(std::max_align_t));
#endif
        VX_CHECK(vx::mem::max_align == priv::max_align);
        VX_CHECK(vx::mem::ideal_align == priv::ideal_align);
    }

    VX_SECTION("padding size");
    {
        VX_CHECK(priv::alignment_padding_size(1) == priv::aligned_header_size);
        VX_CHECK(priv::alignment_padding_size(16) == priv::aligned_header_size + 15);
        VX_CHECK(priv::alignment_padding_size(64) == priv::aligned_header_size + 63);
        VX_CHECK(priv::aligned_header_size >= sizeof(std::uintptr_t));
    }
}

VX_TEST_CASE(mem_aligned_allocate_template)
{
    VX_SECTION("pointer is aligned and usable for a range of alignments");
    {
        check_aligned_roundtrip<1>();
        check_aligned_roundtrip<2>();
        check_aligned_roundtrip<4>();
        check_aligned_roundtrip<8>();
        check_aligned_roundtrip<16>();
        check_aligned_roundtrip<32>();
        check_aligned_roundtrip<64>();
        check_aligned_roundtrip<128>();
        check_aligned_roundtrip<256>();
        check_aligned_roundtrip<4096>();
    }

    VX_SECTION("zero-byte allocation still yields a valid aligned pointer");
    {
        void* p = vx::mem::allocate_aligned<64>(0);
        VX_CHECK(p != nullptr);
        VX_CHECK(is_aligned(p, 64));
        vx::mem::deallocate_aligned<64>(p, 0);
    }

    VX_SECTION("header layout: back-pointer sits just below the user pointer");
    {
        namespace priv = vx::mem::_mem_priv;

        void* p = vx::mem::allocate_aligned<64>(32);
        VX_CHECK(p != nullptr);

        const std::uintptr_t block = *(static_cast<std::uintptr_t*>(p) - 1);
        const std::uintptr_t shift = reinterpret_cast<std::uintptr_t>(p) - block;
        VX_CHECK(shift >= priv::aligned_header_size);
        VX_CHECK(shift <= priv::alignment_padding_size(64));

#if VX_DEBUG
        const std::uintptr_t sentinel = *(static_cast<std::uintptr_t*>(p) - 2);
        VX_CHECK(sentinel == priv::aligned_allocation_sentinel);
#endif

        vx::mem::deallocate_aligned<64>(p, 32);
    }

    VX_SECTION("several live blocks do not clobber each other");
    {
        void* a = vx::mem::allocate_aligned<64>(48);
        void* b = vx::mem::allocate_aligned<64>(48);
        void* c = vx::mem::allocate_aligned<64>(48);
        VX_CHECK(a != nullptr);
        VX_CHECK(b != nullptr);
        VX_CHECK(c != nullptr);
        VX_CHECK(a != b);
        VX_CHECK(b != c);
        VX_CHECK(a != c);

        fill_pattern(a, 48, 1);
        fill_pattern(b, 48, 2);
        fill_pattern(c, 48, 3);
        VX_CHECK(check_pattern(a, 48, 1));
        VX_CHECK(check_pattern(b, 48, 2));
        VX_CHECK(check_pattern(c, 48, 3));

        vx::mem::deallocate_aligned<64>(b, 48);
        VX_CHECK(check_pattern(a, 48, 1));
        VX_CHECK(check_pattern(c, 48, 3));

        vx::mem::deallocate_aligned<64>(a, 48);
        vx::mem::deallocate_aligned<64>(c, 48);
    }

    VX_SECTION("size overflow returns nullptr");
    {
        void* p = vx::mem::allocate_aligned<64>(std::numeric_limits<std::size_t>::max());
        VX_CHECK(p == nullptr);
    }
}

VX_TEST_CASE(mem_aligned_deallocate)
{
    VX_SECTION("null is a no-op (template and runtime)");
    {
        vx::mem::deallocate_aligned<64>(nullptr, 0);
        vx::mem::deallocate_aligned(nullptr, 0, 64);
    }

    VX_SECTION("runtime-alignment deallocate accepts template-allocated memory");
    {
        void* p = vx::mem::allocate_aligned<128>(200);
        VX_CHECK(p != nullptr);
        VX_CHECK(is_aligned(p, 128));
        fill_pattern(p, 200, 0x61);
        VX_CHECK(check_pattern(p, 200, 0x61));
        vx::mem::deallocate_aligned(p, 200, 128);
    }
}

VX_TEST_CASE(mem_aligned_reallocate_null_ptr)
{
    // reallocate_aligned<A>(nullptr, n) forwards to allocate_aligned<A>(n)
    void* p = vx::mem::reallocate_aligned<64>(nullptr, 100);
    VX_CHECK(p != nullptr);
    VX_CHECK(is_aligned(p, 64));
    fill_pattern(p, 100, 0x71);
    VX_CHECK(check_pattern(p, 100, 0x71));
    vx::mem::deallocate_aligned<64>(p, 100);
}

//=============================================================================
// construct / destroy
//=============================================================================

VX_TEST_CASE(mem_construct_in_place)
{
    alignas(tracked) unsigned char buf[sizeof(tracked)];
    tracked* p = reinterpret_cast<tracked*>(buf);

    VX_SECTION("with arguments");
    {
        tracked::reset();
        vx::mem::construct_in_place(p, 5);
        VX_CHECK(p->value == 5);
        VX_CHECK(tracked::ctors == 1);
        vx::mem::destroy_in_place(p);
        VX_CHECK(tracked::dtors == 1);
    }

    VX_SECTION("no arguments value-initializes non-trivial types");
    {
        tracked::reset();
        vx::mem::construct_in_place(p);
        VX_CHECK(p->value == 0);
        VX_CHECK(tracked::ctors == 1);
        vx::mem::destroy_in_place(p);
        VX_CHECK(tracked::dtors == 1);
    }

    VX_SECTION("copy construction");
    {
        tracked src(8);
        tracked::reset();
        vx::mem::construct_in_place(p, src);
        VX_CHECK(p->value == 8);
        VX_CHECK(tracked::ctors == 1);
        vx::mem::destroy_in_place(p);
    }

    VX_SECTION("default member initializers are honored");
    {
        alignas(with_default) unsigned char raw[sizeof(with_default)];
        with_default* w = reinterpret_cast<with_default*>(raw);
        vx::mem::construct_in_place(w);
        VX_CHECK(w->x == 5);
        VX_CHECK(w->y == 6);
        vx::mem::destroy_in_place(w);
    }

    VX_SECTION("trivial types with arguments");
    {
        int i = 0;
        vx::mem::construct_in_place(&i, 42);
        VX_CHECK(i == 42);
    }

    VX_SECTION("construct_in_place_maybe_trivial");
    {
        tracked::reset();
        vx::mem::construct_in_place_maybe_trivial(p);
        VX_CHECK(p->value == 0);
        VX_CHECK(tracked::ctors == 1);
        vx::mem::destroy_in_place(p);

        vx::mem::construct_in_place_maybe_trivial(p, 11);
        VX_CHECK(p->value == 11);
        vx::mem::destroy_in_place(p);

        int i = 0;
        vx::mem::construct_in_place_maybe_trivial(&i, 7);
        VX_CHECK(i == 7);
    }

    VX_SECTION("move-only argument forwarding");
    {
        alignas(movable_int) unsigned char raw[sizeof(movable_int)];
        movable_int* m = reinterpret_cast<movable_int*>(raw);
        movable_int src(9);
        vx::mem::construct_in_place(m, std::move(src));
        VX_CHECK(m->val == 9);
        VX_CHECK(src.val == -1);
        vx::mem::destroy_in_place(m);
    }
}

VX_TEST_CASE(mem_destroy_in_place_trivial)
{
    // must compile and be a no-op for trivially destructible types
    int i = 5;
    vx::mem::destroy_in_place(&i);
    VX_CHECK(i == 5);
}

VX_TEST_CASE(mem_construct_destroy)
{
    VX_SECTION("construct with arguments, then destroy");
    {
        tracked::reset();
        tracked* p = vx::mem::construct<tracked>(5);
        VX_CHECK(p != nullptr);
        VX_CHECK(p->value == 5);
        VX_CHECK(tracked::ctors == 1);
        VX_CHECK(tracked::dtors == 0);

        vx::mem::destroy(p);
        VX_CHECK(tracked::dtors == 1);
    }

    VX_SECTION("construct with no arguments");
    {
        tracked::reset();
        tracked* p = vx::mem::construct<tracked>();
        VX_CHECK(p != nullptr);
        VX_CHECK(p->value == 0);
        vx::mem::destroy(p);
    }

    VX_SECTION("multi-argument forwarding");
    {
        movable_int* m = vx::mem::construct<movable_int>(1, 2, 3);
        VX_CHECK(m != nullptr);
        VX_CHECK(m->val == 0x123);
        vx::mem::destroy(m);
    }

    VX_SECTION("move-only argument forwarding");
    {
        movable_int src(9);
        movable_int* m = vx::mem::construct<movable_int>(std::move(src));
        VX_CHECK(m != nullptr);
        VX_CHECK(m->val == 9);
        VX_CHECK(src.val == -1);
        vx::mem::destroy(m);
    }

    VX_SECTION("trivial type");
    {
        int* p = vx::mem::construct<int>(123);
        VX_CHECK(p != nullptr);
        VX_CHECK(*p == 123);
        vx::mem::destroy(p);
    }

    VX_SECTION("destroy_safe nulls the pointer");
    {
        tracked::reset();
        tracked* p = vx::mem::construct<tracked>(1);
        vx::mem::destroy_safe(p);
        VX_CHECK(p == nullptr);
        VX_CHECK(tracked::dtors == 1);
    }

    VX_SECTION("destroy_safe on null does nothing");
    {
        tracked::reset();
        tracked* p = nullptr;
        vx::mem::destroy_safe(p);
        VX_CHECK(p == nullptr);
        VX_CHECK(tracked::dtors == 0);
    }
}

VX_TEST_CASE(mem_construct_over_aligned)
{
    VX_STATIC_CHECK(alignof(over_aligned) == 64);
    VX_STATIC_CHECK(alignof(over_aligned) > vx::mem::max_align); // exercises the aligned path

    over_aligned* p = vx::mem::construct<over_aligned>();
    VX_CHECK(p != nullptr);
    VX_CHECK(is_aligned(p, 64));
    VX_CHECK(p->x == 3);
    vx::mem::destroy(p);
}

VX_TEST_CASE(mem_construct_array)
{
    VX_SECTION("default-constructed elements");
    {
        tracked::reset();
        tracked* p = vx::mem::construct_array<tracked>(4);
        VX_CHECK(p != nullptr);
        VX_CHECK(tracked::ctors == 4);
        for (int i = 0; i < 4; ++i)
        {
            VX_CHECK(p[i].value == 0);
        }

        vx::mem::destroy_array(p, 4);
        VX_CHECK(tracked::dtors == 4);
    }

    VX_SECTION("elements copied from a value");
    {
        tracked proto(7);
        tracked::reset();
        tracked* p = vx::mem::construct_array<tracked>(3, proto);
        VX_CHECK(p != nullptr);
        VX_CHECK(tracked::ctors == 3);
        for (int i = 0; i < 3; ++i)
        {
            VX_CHECK(p[i].value == 7);
        }

        vx::mem::destroy_array(p, 3);
        VX_CHECK(tracked::dtors == 3);
    }

    VX_SECTION("trivial element type with fill value");
    {
        int* p = vx::mem::construct_array<int>(5, 9);
        VX_CHECK(p != nullptr);
        for (int i = 0; i < 5; ++i)
        {
            VX_CHECK(p[i] == 9);
        }
        vx::mem::destroy_array(p, 5);
    }

    VX_SECTION("count of zero returns nullptr");
    {
        VX_CHECK(vx::mem::construct_array<int>(0) == nullptr);
        VX_CHECK(vx::mem::construct_array<int>(0, 5) == nullptr);
        VX_CHECK(vx::mem::construct_array<tracked>(0) == nullptr);
    }

    VX_SECTION("over-aligned elements");
    {
        over_aligned* p = vx::mem::construct_array<over_aligned>(3);
        VX_CHECK(p != nullptr);
        VX_CHECK(is_aligned(p, 64));
        for (int i = 0; i < 3; ++i)
        {
            VX_CHECK(is_aligned(p + i, 64));
            VX_CHECK(p[i].x == 3);
        }
        vx::mem::destroy_array(p, 3);
    }

    VX_SECTION("elements are writable and independent");
    {
        int* p = vx::mem::construct_array<int>(4, 0);
        VX_CHECK(p != nullptr);
        p[0] = 1;
        p[3] = 4;
        VX_CHECK(p[1] == 0);
        VX_CHECK(p[2] == 0);
        VX_CHECK(p[0] == 1);
        VX_CHECK(p[3] == 4);
        vx::mem::destroy_array(p, 4);
    }
}

VX_TEST_CASE(mem_destroy_array_safe)
{
    VX_SECTION("destroys and nulls the pointer");
    {
        tracked::reset();
        tracked* p = vx::mem::construct_array<tracked>(3);
        VX_CHECK(p != nullptr);
        vx::mem::destroy_array_safe(p, 3);
        VX_CHECK(p == nullptr);
        VX_CHECK(tracked::dtors == 3);
    }

    VX_SECTION("null pointer is a no-op");
    {
        tracked::reset();
        tracked* p = nullptr;
        vx::mem::destroy_array_safe(p, 3);
        VX_CHECK(p == nullptr);
        VX_CHECK(tracked::dtors == 0);

        vx::mem::destroy_array_safe(p, 0);
        VX_CHECK(p == nullptr);
    }
}

VX_TEST_CASE(mem_range_will_overflow)
{
    namespace priv = vx::mem::_mem_priv;

    // single-byte types can never overflow
    VX_CHECK(!priv::range_will_overflow<char>(std::numeric_limits<std::size_t>::max()));
    VX_CHECK(!priv::range_will_overflow<unsigned char>(0));

    // larger types overflow once count exceeds max_array_size<T>()
    VX_CHECK(!priv::range_will_overflow<int>(0));
    VX_CHECK(!priv::range_will_overflow<int>(1000));
    VX_CHECK(!priv::range_will_overflow<int>(vx::mem::max_array_size<int>()));
    VX_CHECK(priv::range_will_overflow<int>(vx::mem::max_array_size<int>() + 1));
    VX_CHECK(priv::range_will_overflow<std::int64_t>(vx::mem::max_array_size<std::int64_t>() + 1));
    VX_CHECK(priv::range_will_overflow<int>(std::numeric_limits<std::size_t>::max()));
}

//=============================================================================

#ifndef VX_TEST_NO_MAIN
int main(int argc, char** argv)
{
    return VX_RUN_TESTS(argc, argv);
}
#endif
