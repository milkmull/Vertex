// Tests for the allocator header:
//   vx::mem::allocator_traits (custom implementation), allocators_equal,
//   default_allocator, aligned_allocator<T, Alignment>, allocator_with_alignment,
//   byte_allocator, rebind_allocator, alignment_policy
//
// Build with -std=c++17 or later.
//
// Define VX_TEST_ALLOC_SUSPECT to also build the tests I expect to abort,
// fail to compile, or corrupt data with the current headers (see the comment
// on that block at the bottom of the file).
// Define VX_TEST_NO_MAIN if another translation unit provides main().

#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <list>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

#include "vertex/std/memory/allocator.hpp"
#include "vertex_test/test.hpp"

namespace mem = vx::mem;

using vx::test::allocator_stats;
using vx::test::movable_int;
using vx::test::tracking_allocator;
using vx::test::over_aligned;

namespace {

//=============================================================================
// helpers
//=============================================================================

struct trio
{
    int a, b, c; // 12 bytes
};

// counts constructions / destructions
struct tracked
{
    static inline int ctors = 0;
    static inline int dtors = 0;

    int value;

    explicit tracked(int v) noexcept
        : value(v)
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

// detection idiom: is `A == B` a valid (unambiguous) expression?
template <typename A, typename B, typename = void>
struct can_eq : std::false_type
{};

template <typename A, typename B>
struct can_eq<A, B, std::void_t<decltype(std::declval<A>() == std::declval<B>())>> : std::true_type
{};

// Only value_type / allocate / deallocate: everything else must come from
// the traits' defaults.
template <typename T>
struct minimal_allocator
{
    using value_type = T;

    T* allocate(std::size_t n)
    {
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t)
    {
        std::free(p);
    }
};

struct custom_log
{
    static inline int constructs = 0;
    static inline int destroys = 0;
    static inline int hinted_allocs = 0;
    static inline int select_calls = 0;
    static inline const void* last_hint = nullptr;

    static void reset() noexcept
    {
        constructs = 0;
        destroys = 0;
        hinted_allocs = 0;
        select_calls = 0;
        last_hint = nullptr;
    }
};

// Overrides every customization point the traits know about.
template <typename T>
struct custom_allocator
{
    using value_type = T;
    using size_type = std::uint32_t;
    using difference_type = std::int32_t;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
    using is_always_equal = std::false_type;

    int id = 0;

    custom_allocator() = default;

    explicit custom_allocator(int i)
        : id(i)
    {}

    template <typename U>
    custom_allocator(const custom_allocator<U>& o)
        : id(o.id)
    {}

    T* allocate(std::size_t n)
    {
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }

    T* allocate(std::size_t n, const void* hint)
    {
        custom_log::last_hint = hint;
        ++custom_log::hinted_allocs;
        return allocate(n);
    }

    T* reallocate(T* p, std::size_t n)
    {
        return static_cast<T*>(std::realloc(p, n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t)
    {
        std::free(p);
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        ++custom_log::constructs;
        ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p)
    {
        ++custom_log::destroys;
        p->~U();
    }

    std::uint32_t max_size() const noexcept
    {
        return 1234;
    }

    custom_allocator select_on_container_copy_construction() const
    {
        ++custom_log::select_calls;
        return custom_allocator(id + 100);
    }

    friend bool operator==(const custom_allocator& a, const custom_allocator& b)
    {
        return a.id == b.id;
    }

    friend bool operator!=(const custom_allocator& a, const custom_allocator& b)
    {
        return a.id != b.id;
    }
};

template <typename A>
void check_allocator_roundtrip(std::size_t count)
{
    using T = typename A::value_type;

    T* p = A::allocate(count);
    VX_CHECK(p != nullptr);
    VX_CHECK(is_aligned(p, A::alignment));

    const std::size_t bytes = count * sizeof(T);
    fill_pattern(p, bytes, 0x11);
    VX_CHECK(check_pattern(p, bytes, 0x11));

    A::deallocate(p, count);
}

template <typename A>
void check_allocate_overflow()
{
    using T = typename A::value_type;
    static_assert(sizeof(T) > 1, "count + 1 wraps to zero for single-byte types");

    // count * sizeof(T) wraps around size_t; the allocator should refuse
    constexpr std::size_t count = A::max_size() + 1;
    T* p = A::allocate(count);
    VX_SOFT_CHECK(p == nullptr);

    if (p)
    {
        A::deallocate(p, count); // don't leak if the check above failed
    }
}

} // namespace

//=============================================================================
// default_allocator / aligned_allocator: types and constants
//=============================================================================

VX_TEST_CASE(alloc_member_types)
{
    VX_SECTION("default_allocator");
    {
        using A = mem::default_allocator<int>;
        VX_STATIC_CHECK(std::is_same<A::value_type, int>::value);
        VX_STATIC_CHECK(std::is_same<A::pointer_type, int*>::value);
        VX_STATIC_CHECK(std::is_same<A::size_type, std::size_t>::value);
        VX_STATIC_CHECK(std::is_same<A::difference_type, std::ptrdiff_t>::value);
        VX_STATIC_CHECK(std::is_same<A::is_always_equal, std::true_type>::value);

        VX_STATIC_CHECK(std::is_empty<A>::value);
        VX_STATIC_CHECK(std::is_same<std::allocator_traits<A>::pointer, int*>::value);
        VX_STATIC_CHECK(std::allocator_traits<A>::is_always_equal::value);
    }

    VX_SECTION("aligned_allocator");
    {
        using A = mem::aligned_allocator<float, 64>;
        VX_STATIC_CHECK(std::is_same<A::value_type, float>::value);
        VX_STATIC_CHECK(std::is_same<A::pointer_type, float*>::value);
        VX_STATIC_CHECK(std::is_same<A::size_type, std::size_t>::value);
        VX_STATIC_CHECK(std::is_same<A::difference_type, std::ptrdiff_t>::value);
        VX_STATIC_CHECK(std::is_same<A::is_always_equal, std::true_type>::value);

        VX_STATIC_CHECK(std::is_empty<A>::value);
        VX_STATIC_CHECK(std::allocator_traits<A>::is_always_equal::value);
    }
}

VX_TEST_CASE(alloc_alignment_policy_enum)
{
    VX_STATIC_CHECK(std::is_enum<mem::alignment_policy>::value);
    VX_STATIC_CHECK(!std::is_convertible<mem::alignment_policy, int>::value); // enum class
    VX_STATIC_CHECK(mem::alignment_policy::at_least != mem::alignment_policy::exact);
}

VX_TEST_CASE(alloc_alignment_constants)
{
    VX_SECTION("default_allocator uses alignof(T)");
    {
        VX_STATIC_CHECK(mem::default_allocator<char>::alignment == 1);
        VX_STATIC_CHECK(mem::default_allocator<int>::alignment == alignof(int));
        VX_STATIC_CHECK(mem::default_allocator<double>::alignment == alignof(double));
        VX_STATIC_CHECK(mem::default_allocator<trio>::alignment == alignof(trio));
    }

    VX_SECTION("aligned_allocator uses its template argument");
    {
        VX_STATIC_CHECK((mem::aligned_allocator<int, 4>::alignment == 4));
        VX_STATIC_CHECK((mem::aligned_allocator<int, 16>::alignment == 16));
        VX_STATIC_CHECK((mem::aligned_allocator<int, 64>::alignment == 64));
        VX_STATIC_CHECK((mem::aligned_allocator<std::int64_t, 4096>::alignment == 4096));
        VX_STATIC_CHECK((mem::aligned_allocator<float, mem::ideal_align>::alignment == mem::ideal_align));
        VX_STATIC_CHECK((mem::aligned_allocator<over_aligned, 64>::alignment == 64));
    }
}

VX_TEST_CASE(alloc_max_size)
{
    constexpr std::size_t max = std::numeric_limits<std::size_t>::max();

    VX_STATIC_CHECK(mem::default_allocator<char>::max_size() == max);
    VX_STATIC_CHECK(mem::default_allocator<int>::max_size() == max / sizeof(int));
    VX_STATIC_CHECK(mem::default_allocator<trio>::max_size() == max / sizeof(trio));
    VX_STATIC_CHECK((mem::aligned_allocator<int, 64>::max_size() == max / sizeof(int)));
    VX_STATIC_CHECK(mem::default_allocator<int>::max_size() == mem::max_array_size<int>());
}

//=============================================================================
// allocator_with_alignment / byte_allocator
//=============================================================================

VX_TEST_CASE(alloc_allocator_with_alignment)
{
    VX_SECTION("alignment satisfied by malloc selects default_allocator");
    {
        VX_STATIC_CHECK(std::is_same<mem::allocator_with_alignment<int, 4>, mem::default_allocator<int>>::value);
        VX_STATIC_CHECK(std::is_same<mem::allocator_with_alignment<int, 8>, mem::default_allocator<int>>::value);
        VX_STATIC_CHECK(std::is_same<mem::allocator_with_alignment<int, mem::max_align>, mem::default_allocator<int>>::value);
        VX_STATIC_CHECK(std::is_same<mem::allocator_with_alignment<char, 1>, mem::default_allocator<char>>::value);
    }

    VX_SECTION("alignment above max_align selects aligned_allocator");
    {
        constexpr std::size_t big = mem::max_align * 2;
        VX_STATIC_CHECK((std::is_same<mem::allocator_with_alignment<int, big>, mem::aligned_allocator<int, big>>::value));
        VX_STATIC_CHECK((std::is_same<mem::allocator_with_alignment<int, 4096>, mem::aligned_allocator<int, 4096>>::value));
        VX_STATIC_CHECK((std::is_same<mem::allocator_with_alignment<double, 1024>, mem::aligned_allocator<double, 1024>>::value));
    }

    VX_SECTION("over-aligned element type");
    {
        VX_STATIC_CHECK(mem::max_align < 64);
        VX_STATIC_CHECK((std::is_same<mem::allocator_with_alignment<over_aligned, 64>, mem::aligned_allocator<over_aligned, 64>>::value));
    }

    VX_SECTION("allocations satisfy the requested alignment");
    {
        check_allocator_roundtrip<mem::allocator_with_alignment<int, 8>>(50);
        check_allocator_roundtrip<mem::allocator_with_alignment<int, 64>>(50);
        check_allocator_roundtrip<mem::allocator_with_alignment<int, 256>>(50);
        check_allocator_roundtrip<mem::allocator_with_alignment<over_aligned, 64>>(4);
    }
}

VX_TEST_CASE(alloc_byte_allocator)
{
    VX_STATIC_CHECK(std::is_same<mem::byte_allocator<>, mem::default_allocator<unsigned char>>::value);
    VX_STATIC_CHECK(std::is_same<mem::byte_allocator<1>, mem::default_allocator<unsigned char>>::value);
    VX_STATIC_CHECK(std::is_same<mem::byte_allocator<8>, mem::default_allocator<unsigned char>>::value);
    VX_STATIC_CHECK(std::is_same<mem::byte_allocator<>::value_type, unsigned char>::value);

    VX_STATIC_CHECK(mem::max_align < 64);
    VX_STATIC_CHECK(std::is_same<mem::byte_allocator<64>, mem::aligned_allocator<unsigned char, 64>>::value);
    VX_STATIC_CHECK(std::is_same<mem::byte_allocator<4096>, mem::aligned_allocator<unsigned char, 4096>>::value);

    VX_STATIC_CHECK(std::is_same<
        mem::byte_allocator<mem::ideal_align>,
        std::conditional<(mem::ideal_align <= mem::max_align),
            mem::default_allocator<unsigned char>,
            mem::aligned_allocator<unsigned char, mem::ideal_align>>::type>::value);

    check_allocator_roundtrip<mem::byte_allocator<>>(77);
    check_allocator_roundtrip<mem::byte_allocator<64>>(77);
    check_allocator_roundtrip<mem::byte_allocator<mem::ideal_align>>(77);
}

//=============================================================================
// rebind
//=============================================================================

VX_TEST_CASE(alloc_rebind)
{
    VX_SECTION("nested rebind");
    {
        VX_STATIC_CHECK(std::is_same<mem::default_allocator<int>::rebind<double>::other, mem::default_allocator<double>>::value);

        // the alignment carries over
        VX_STATIC_CHECK((std::is_same<
            mem::aligned_allocator<int, 64>::rebind<double>::other,
            mem::aligned_allocator<double, 64>>::value));
    }

    VX_SECTION("allocator_traits::rebind_alloc");
    {
        VX_STATIC_CHECK(std::is_same<
            mem::allocator_traits<mem::default_allocator<int>>::rebind_alloc<char>,
            mem::default_allocator<char>>::value);

        VX_STATIC_CHECK((std::is_same<
            mem::allocator_traits<mem::aligned_allocator<int, 64>>::rebind_alloc<double>,
            mem::aligned_allocator<double, 64>>::value));

        // no nested rebind: falls back to Alloc<T, Args...> -> Alloc<U, Args...>
        VX_STATIC_CHECK(std::is_same<
            mem::allocator_traits<minimal_allocator<int>>::rebind_alloc<double>,
            minimal_allocator<double>>::value);
        VX_STATIC_CHECK(std::is_same<
            mem::allocator_traits<tracking_allocator<int>>::rebind_alloc<double>,
            tracking_allocator<double>>::value);
        VX_STATIC_CHECK(std::is_same<
            mem::allocator_traits<std::allocator<int>>::rebind_alloc<double>,
            std::allocator<double>>::value);
    }

    VX_SECTION("rebind_traits");
    {
        VX_STATIC_CHECK(std::is_same<
            mem::allocator_traits<mem::default_allocator<int>>::rebind_traits<double>,
            mem::allocator_traits<mem::default_allocator<double>>>::value);
    }

    VX_SECTION("rebind_allocator");
    {
        VX_STATIC_CHECK(std::is_same<
            mem::rebind_allocator<mem::default_allocator<int>, double>::type,
            mem::default_allocator<double>>::value);

        VX_STATIC_CHECK((std::is_same<
            mem::rebind_allocator<mem::aligned_allocator<int, 64>, char>::type,
            mem::aligned_allocator<char, 64>>::value));

        VX_STATIC_CHECK(std::is_same<
            mem::rebind_allocator<custom_allocator<int>, double>::type,
            custom_allocator<double>>::value);
    }
}

//=============================================================================
// special members, conversions, equality
//=============================================================================

VX_TEST_CASE(alloc_special_members)
{
    using A = mem::default_allocator<int>;
    using AA = mem::aligned_allocator<int, 64>;

    VX_STATIC_CHECK(std::is_nothrow_default_constructible<A>::value);
    VX_STATIC_CHECK(std::is_nothrow_copy_constructible<A>::value);
    VX_STATIC_CHECK(std::is_nothrow_move_constructible<A>::value);
    VX_STATIC_CHECK(std::is_nothrow_copy_assignable<A>::value);
    VX_STATIC_CHECK(std::is_nothrow_move_assignable<A>::value);

    VX_STATIC_CHECK(std::is_nothrow_default_constructible<AA>::value);
    VX_STATIC_CHECK(std::is_nothrow_copy_constructible<AA>::value);
    VX_STATIC_CHECK(std::is_nothrow_move_constructible<AA>::value);
    VX_STATIC_CHECK(std::is_nothrow_copy_assignable<AA>::value);
    VX_STATIC_CHECK(std::is_nothrow_move_assignable<AA>::value);

    // the allocation functions must not throw
    VX_STATIC_CHECK(noexcept(A::allocate(std::size_t(1))));
    VX_STATIC_CHECK(noexcept(A::deallocate(nullptr, std::size_t(0))));
    VX_STATIC_CHECK(noexcept(A::reallocate(nullptr, std::size_t(1))));
    VX_STATIC_CHECK(noexcept(AA::allocate(std::size_t(1))));
    VX_STATIC_CHECK(noexcept(AA::deallocate(nullptr, std::size_t(0))));
    VX_STATIC_CHECK(noexcept(AA::reallocate(nullptr, std::size_t(1))));
}

VX_TEST_CASE(alloc_conversions)
{
    VX_SECTION("default_allocator between value types");
    {
        mem::default_allocator<int> a;
        mem::default_allocator<double> b(a);
        mem::default_allocator<double> c = a;
        VX_CHECK(b == c);

        VX_STATIC_CHECK((std::is_convertible<mem::default_allocator<int>, mem::default_allocator<double>>::value));
    }

    VX_SECTION("aligned_allocator between value types with the same alignment");
    {
        mem::aligned_allocator<int, 64> a;
        mem::aligned_allocator<double, 64> b(a);
        mem::aligned_allocator<double, 64> c = a;
        VX_CHECK(b == c);

        VX_STATIC_CHECK((std::is_convertible<mem::aligned_allocator<int, 64>, mem::aligned_allocator<double, 64>>::value));
    }

    VX_SECTION("no conversion across alignments or allocator kinds");
    {
        VX_STATIC_CHECK(!(std::is_convertible<mem::aligned_allocator<int, 16>, mem::aligned_allocator<int, 64>>::value));
        VX_STATIC_CHECK(!(std::is_convertible<mem::aligned_allocator<int, 64>, mem::default_allocator<int>>::value));
        VX_STATIC_CHECK(!(std::is_convertible<mem::default_allocator<int>, mem::aligned_allocator<int, 64>>::value));
    }
}

VX_TEST_CASE(alloc_equality)
{
    VX_SECTION("same type is always equal");
    {
        mem::default_allocator<int> a;
        mem::default_allocator<int> b;
        VX_CHECK(a == b);
        VX_CHECK(!(a != b));

        mem::aligned_allocator<float, 64> c;
        mem::aligned_allocator<float, 64> d;
        VX_CHECK(c == d);
        VX_CHECK(!(c != d));
    }

    VX_SECTION("rebound allocators compare equal (value types may differ)");
    {
        mem::default_allocator<int> a;
        mem::default_allocator<double> b;
        VX_CHECK(a == b);
        VX_CHECK(b == a);
        VX_CHECK(!(a != b));

        mem::aligned_allocator<int, 64> c;
        mem::aligned_allocator<double, 64> d;
        VX_CHECK(c == d);
        VX_CHECK(!(c != d));

        VX_STATIC_CHECK((can_eq<mem::default_allocator<int>, mem::default_allocator<double>>::value));
        VX_STATIC_CHECK((can_eq<mem::aligned_allocator<int, 64>, mem::aligned_allocator<double, 64>>::value));
    }

    VX_SECTION("different kinds or alignments are not comparable");
    {
        VX_STATIC_CHECK(!(can_eq<mem::default_allocator<int>, mem::aligned_allocator<int, 64>>::value));
        VX_STATIC_CHECK(!(can_eq<mem::aligned_allocator<int, 16>, mem::aligned_allocator<int, 64>>::value));
    }
}

//=============================================================================
// allocator_traits: member types
//=============================================================================

VX_TEST_CASE(alloc_traits_types_default_allocator)
{
    using A = mem::default_allocator<int>;
    using traits = mem::allocator_traits<A>;

    VX_STATIC_CHECK(std::is_same<traits::allocator_type, A>::value);
    VX_STATIC_CHECK(std::is_same<traits::value_type, int>::value);
    VX_STATIC_CHECK(std::is_same<traits::pointer, int*>::value);
    VX_STATIC_CHECK(std::is_same<traits::const_pointer, const int*>::value);
    VX_STATIC_CHECK(std::is_same<traits::void_pointer, void*>::value);
    VX_STATIC_CHECK(std::is_same<traits::const_void_pointer, const void*>::value);
    VX_STATIC_CHECK(std::is_same<traits::difference_type, std::ptrdiff_t>::value);
    VX_STATIC_CHECK(std::is_same<traits::size_type, std::size_t>::value);
    VX_STATIC_CHECK(std::is_same<traits::is_always_equal, std::true_type>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_copy_assignment, std::false_type>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_move_assignment, std::false_type>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_swap, std::false_type>::value);
}

VX_TEST_CASE(alloc_traits_types_minimal_allocator)
{
    // every member type must come from the traits' defaults
    using A = minimal_allocator<int>;
    using traits = mem::allocator_traits<A>;

    VX_STATIC_CHECK(std::is_same<traits::value_type, int>::value);
    VX_STATIC_CHECK(std::is_same<traits::pointer, int*>::value);
    VX_STATIC_CHECK(std::is_same<traits::const_pointer, const int*>::value);
    VX_STATIC_CHECK(std::is_same<traits::void_pointer, void*>::value);
    VX_STATIC_CHECK(std::is_same<traits::const_void_pointer, const void*>::value);
    VX_STATIC_CHECK(std::is_same<traits::difference_type, std::ptrdiff_t>::value);
    VX_STATIC_CHECK(std::is_same<traits::size_type, std::make_unsigned<std::ptrdiff_t>::type>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_copy_assignment, std::false_type>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_move_assignment, std::false_type>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_swap, std::false_type>::value);

    // is_always_equal defaults to is_empty
    VX_STATIC_CHECK(std::is_same<traits::is_always_equal, std::true_type>::value);
}

VX_TEST_CASE(alloc_traits_types_custom_allocator)
{
    // typedefs provided by the allocator must win over the defaults
    using A = custom_allocator<int>;
    using traits = mem::allocator_traits<A>;

    VX_STATIC_CHECK(std::is_same<traits::size_type, std::uint32_t>::value);
    VX_STATIC_CHECK(std::is_same<traits::difference_type, std::int32_t>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_copy_assignment, std::true_type>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_move_assignment, std::true_type>::value);
    VX_STATIC_CHECK(std::is_same<traits::propagate_on_container_swap, std::true_type>::value);
    VX_STATIC_CHECK(std::is_same<traits::is_always_equal, std::false_type>::value);
}

VX_TEST_CASE(alloc_traits_supports_reallocate)
{
    VX_STATIC_CHECK(mem::allocator_traits<mem::default_allocator<int>>::supports_reallocate::value);
    VX_STATIC_CHECK((mem::allocator_traits<mem::aligned_allocator<int, 64>>::supports_reallocate::value));
    VX_STATIC_CHECK(mem::allocator_traits<custom_allocator<int>>::supports_reallocate::value);
    VX_STATIC_CHECK(mem::allocator_traits<tracking_allocator<int>>::supports_reallocate::value);

    VX_STATIC_CHECK(!mem::allocator_traits<minimal_allocator<int>>::supports_reallocate::value);
    VX_STATIC_CHECK(!mem::allocator_traits<std::allocator<int>>::supports_reallocate::value);
}

//=============================================================================
// allocator_traits: behavior
//=============================================================================

VX_TEST_CASE(alloc_traits_with_default_allocator)
{
    using A = mem::default_allocator<int>;
    using traits = mem::allocator_traits<A>;
    A a;

    VX_SECTION("allocate / construct / destroy / deallocate");
    {
        int* p = traits::allocate(a, 4);
        VX_CHECK(p != nullptr);
        traits::construct(a, p, 42);
        VX_CHECK(*p == 42);
        traits::destroy(a, p);
        traits::deallocate(a, p, 4);
    }

    VX_SECTION("zero count");
    {
        VX_CHECK(traits::allocate(a, 0) == nullptr);
        traits::deallocate(a, nullptr, 0);
    }

    VX_SECTION("allocate with a hint falls back to allocate(n)");
    {
        int marker = 0;
        int* p = traits::allocate(a, 4, &marker);
        VX_CHECK(p != nullptr);
        traits::deallocate(a, p, 4);

        int* q = traits::allocate(a, 4, nullptr);
        VX_CHECK(q != nullptr);
        traits::deallocate(a, q, 4);
    }

    VX_SECTION("reallocate");
    {
        int* p = traits::allocate(a, 4);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 4 * sizeof(int), 0x21);

        int* q = traits::reallocate(a, p, 4096);
        VX_CHECK(q != nullptr);
        VX_CHECK(check_pattern(q, 4 * sizeof(int), 0x21));
        traits::deallocate(a, q, 4096);
    }

    VX_SECTION("max_size");
    {
        VX_CHECK(traits::max_size(a) == mem::default_allocator<int>::max_size());
        VX_CHECK(traits::max_size(a) == mem::max_array_size<int>());
    }

    VX_SECTION("select_on_container_copy_construction returns a copy");
    {
        A b = traits::select_on_container_copy_construction(a);
        VX_CHECK(b == a);
    }
}

VX_TEST_CASE(alloc_traits_with_minimal_allocator)
{
    using A = minimal_allocator<tracked>;
    using traits = mem::allocator_traits<A>;
    A a;

    VX_SECTION("allocate / deallocate");
    {
        tracked* p = traits::allocate(a, 3);
        VX_CHECK(p != nullptr);
        traits::deallocate(a, p, 3);
    }

    VX_SECTION("allocate with hint ignores the hint");
    {
        int marker = 0;
        tracked* p = traits::allocate(a, 3, &marker);
        VX_CHECK(p != nullptr);
        traits::deallocate(a, p, 3);
    }

    VX_SECTION("default construct / destroy");
    {
        tracked* p = traits::allocate(a, 1);
        VX_CHECK(p != nullptr);

        tracked::reset();
        traits::construct(a, p, 7);
        VX_CHECK(p->value == 7);
        VX_CHECK(tracked::ctors == 1);

        traits::destroy(a, p);
        VX_CHECK(tracked::dtors == 1);
        traits::deallocate(a, p, 1);
    }

    VX_SECTION("default max_size");
    {
        VX_CHECK(traits::max_size(a) == mem::max_array_size<tracked>());
    }

    VX_SECTION("default select_on_container_copy_construction returns a copy");
    {
        A b = traits::select_on_container_copy_construction(a);
        (void)b;
    }
}

VX_TEST_CASE(alloc_traits_with_custom_allocator)
{
    using A = custom_allocator<int>;
    using traits = mem::allocator_traits<A>;
    A a(5);

    VX_SECTION("allocate with hint forwards to the allocator's hint overload");
    {
        custom_log::reset();
        int marker = 0;
        int* p = traits::allocate(a, 4, &marker);
        VX_CHECK(p != nullptr);
        VX_CHECK(custom_log::hinted_allocs == 1);
        VX_CHECK(custom_log::last_hint == &marker);
        traits::deallocate(a, p, 4);
    }

    VX_SECTION("allocate without hint does not use the hint overload");
    {
        custom_log::reset();
        int* p = traits::allocate(a, 4);
        VX_CHECK(p != nullptr);
        VX_CHECK(custom_log::hinted_allocs == 0);
        traits::deallocate(a, p, 4);
    }

    VX_SECTION("construct / destroy go through the allocator");
    {
        custom_log::reset();
        int* p = traits::allocate(a, 1);
        VX_CHECK(p != nullptr);

        traits::construct(a, p, 123);
        VX_CHECK(*p == 123);
        VX_CHECK(custom_log::constructs == 1);

        traits::destroy(a, p);
        VX_CHECK(custom_log::destroys == 1);
        traits::deallocate(a, p, 1);
    }

    VX_SECTION("reallocate");
    {
        int* p = traits::allocate(a, 4);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 4 * sizeof(int), 0x31);

        int* q = traits::reallocate(a, p, 1024);
        VX_CHECK(q != nullptr);
        VX_CHECK(check_pattern(q, 4 * sizeof(int), 0x31));
        traits::deallocate(a, q, 1024);
    }

    VX_SECTION("max_size uses the allocator's max_size");
    {
        VX_CHECK(traits::max_size(a) == 1234);
    }

    VX_SECTION("select_on_container_copy_construction uses the allocator's version");
    {
        custom_log::reset();
        A b = traits::select_on_container_copy_construction(a);
        VX_CHECK(custom_log::select_calls == 1);
        VX_CHECK(b.id == 105);
    }
}

VX_TEST_CASE(alloc_traits_with_std_allocator)
{
    // std::allocator's optional members vary by standard version (construct,
    // destroy, max_size and the hint overload are gone in C++20), so only
    // check behavior that holds either way.
    using A = std::allocator<int>;
    using traits = mem::allocator_traits<A>;
    A a;

    int* p = traits::allocate(a, 4);
    VX_CHECK(p != nullptr);
    traits::construct(a, p, 5);
    VX_CHECK(*p == 5);
    traits::destroy(a, p);
    traits::deallocate(a, p, 4);

    VX_CHECK(traits::max_size(a) > 0);
    VX_STATIC_CHECK(traits::is_always_equal::value);
}

//=============================================================================
// allocators_equal / allocator_traits::equal
//=============================================================================

VX_TEST_CASE(alloc_allocators_equal)
{
    VX_SECTION("always-equal allocators (usable in constant expressions)");
    {
        VX_STATIC_CHECK(mem::allocators_equal(mem::default_allocator<int>(), mem::default_allocator<int>()));
        VX_STATIC_CHECK(mem::allocators_equal(mem::default_allocator<int>(), mem::default_allocator<double>()));
        VX_STATIC_CHECK((mem::allocators_equal(mem::aligned_allocator<int, 64>(), mem::aligned_allocator<double, 64>())));
        VX_STATIC_CHECK(mem::allocators_equal(std::allocator<int>(), std::allocator<int>()));

        // no operator== needed when the allocator is always equal
        VX_STATIC_CHECK(mem::allocators_equal(minimal_allocator<int>(), minimal_allocator<int>()));
    }

    VX_SECTION("stateful allocators compare with operator==");
    {
        allocator_stats s1, s2;
        tracking_allocator<int> a(1, &s1);
        tracking_allocator<int> b(1, &s2);
        tracking_allocator<int> c(2, &s1);

        VX_CHECK(mem::allocators_equal(a, a));
        VX_CHECK(mem::allocators_equal(a, b)); // same id
        VX_CHECK(!mem::allocators_equal(a, c));
        VX_CHECK(!mem::allocators_equal(c, b));
    }

    VX_SECTION("custom allocator");
    {
        custom_allocator<int> a(1);
        custom_allocator<int> b(1);
        custom_allocator<int> c(2);
        VX_CHECK(mem::allocators_equal(a, b));
        VX_CHECK(!mem::allocators_equal(a, c));
    }

    VX_SECTION("traits::equal is the same thing");
    {
        custom_allocator<int> a(1);
        custom_allocator<int> b(1);
        custom_allocator<int> c(2);
        VX_CHECK(mem::allocator_traits<custom_allocator<int>>::equal(a, b));
        VX_CHECK(!mem::allocator_traits<custom_allocator<int>>::equal(a, c));

        mem::default_allocator<int> d;
        VX_CHECK(mem::allocator_traits<mem::default_allocator<int>>::equal(d, d));
    }
}

//=============================================================================
// allocate / deallocate
//=============================================================================

VX_TEST_CASE(alloc_allocate_deallocate)
{
    VX_SECTION("default_allocator (malloc path)");
    {
        check_allocator_roundtrip<mem::default_allocator<char>>(100);
        check_allocator_roundtrip<mem::default_allocator<int>>(100);
        check_allocator_roundtrip<mem::default_allocator<double>>(100);
        check_allocator_roundtrip<mem::default_allocator<trio>>(33);
        check_allocator_roundtrip<mem::default_allocator<int>>(1);
    }

    VX_SECTION("aligned_allocator");
    {
        check_allocator_roundtrip<mem::aligned_allocator<int, 4>>(100);
        check_allocator_roundtrip<mem::aligned_allocator<int, 16>>(100);
        check_allocator_roundtrip<mem::aligned_allocator<int, 64>>(100);
        check_allocator_roundtrip<mem::aligned_allocator<int, 256>>(100);
        check_allocator_roundtrip<mem::aligned_allocator<std::int64_t, 4096>>(100);
        check_allocator_roundtrip<mem::aligned_allocator<float, mem::ideal_align>>(100);
        check_allocator_roundtrip<mem::aligned_allocator<char, 32>>(100);
        check_allocator_roundtrip<mem::aligned_allocator<over_aligned, 64>>(5);
        check_allocator_roundtrip<mem::aligned_allocator<int, 64>>(1);
    }

    VX_SECTION("pointers meet ideal_align");
    {
        mem::aligned_allocator<float, mem::ideal_align> a;
        float* p = a.allocate(10);
        VX_CHECK(p != nullptr);
        VX_CHECK(is_aligned(p, mem::ideal_align));
        a.deallocate(p, 10);
    }

    VX_SECTION("several live blocks stay independent");
    {
        using A = mem::aligned_allocator<int, 64>;
        int* a = A::allocate(16);
        int* b = A::allocate(16);
        int* c = A::allocate(16);
        VX_CHECK(a != nullptr);
        VX_CHECK(b != nullptr);
        VX_CHECK(c != nullptr);

        fill_pattern(a, 16 * sizeof(int), 1);
        fill_pattern(b, 16 * sizeof(int), 2);
        fill_pattern(c, 16 * sizeof(int), 3);
        VX_CHECK(check_pattern(a, 16 * sizeof(int), 1));
        VX_CHECK(check_pattern(b, 16 * sizeof(int), 2));
        VX_CHECK(check_pattern(c, 16 * sizeof(int), 3));

        A::deallocate(b, 16);
        VX_CHECK(check_pattern(a, 16 * sizeof(int), 1));
        VX_CHECK(check_pattern(c, 16 * sizeof(int), 3));
        A::deallocate(a, 16);
        A::deallocate(c, 16);
    }

    VX_SECTION("callable through an instance");
    {
        mem::default_allocator<int> a;
        int* p = a.allocate(8);
        VX_CHECK(p != nullptr);
        p[0] = 1;
        p[7] = 8;
        VX_CHECK(p[0] == 1);
        VX_CHECK(p[7] == 8);
        a.deallocate(p, 8);
    }

    VX_SECTION("through std::allocator_traits");
    {
        using A = mem::default_allocator<int>;
        using traits = std::allocator_traits<A>;
        A a;
        int* p = traits::allocate(a, 4);
        VX_CHECK(p != nullptr);
        traits::construct(a, p, 42);
        VX_CHECK(*p == 42);
        traits::destroy(a, p);
        traits::deallocate(a, p, 4);
    }
}

VX_TEST_CASE(alloc_zero_count)
{
    VX_CHECK(mem::default_allocator<int>::allocate(0) == nullptr);
    VX_CHECK((mem::aligned_allocator<int, 64>::allocate(0)) == nullptr);
    VX_CHECK(mem::byte_allocator<>::allocate(0) == nullptr);
    VX_CHECK(mem::byte_allocator<64>::allocate(0) == nullptr);

    // deallocating null is a no-op for every allocator
    mem::default_allocator<int>::deallocate(nullptr, 0);
    mem::aligned_allocator<int, 64>::deallocate(nullptr, 0);
    mem::byte_allocator<>::deallocate(nullptr, 0);
    mem::byte_allocator<64>::deallocate(nullptr, 0);
}

VX_TEST_CASE(alloc_overflow)
{
    // allocate() computes `count * sizeof(T)` with no overflow check, so a
    // count above max_size() wraps and hands back an undersized block instead
    // of nullptr. I expect these to FAIL on the current code.
    // Fix: return nullptr when count > max_size().
    VX_SECTION("default_allocator<int>");
    {
        check_allocate_overflow<mem::default_allocator<int>>();
    }

    VX_SECTION("default_allocator<int64>");
    {
        check_allocate_overflow<mem::default_allocator<std::int64_t>>();
    }

    VX_SECTION("12-byte type (wraps to a small non-zero size)");
    {
        check_allocate_overflow<mem::default_allocator<trio>>();
    }

    VX_SECTION("aligned_allocator");
    {
        check_allocate_overflow<mem::aligned_allocator<int, 64>>();
        check_allocate_overflow<mem::aligned_allocator<int, mem::ideal_align>>();
    }
}

//=============================================================================
// reallocate (default_allocator and null-pointer paths; non-null aligned
// reallocation is in the SUSPECT block)
//=============================================================================

VX_TEST_CASE(alloc_reallocate)
{
    using A = mem::default_allocator<int>;

    VX_SECTION("reallocate(nullptr, n) behaves like allocate");
    {
        int* p = A::reallocate(nullptr, 8);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 8 * sizeof(int), 0x21);
        VX_CHECK(check_pattern(p, 8 * sizeof(int), 0x21));
        A::deallocate(p, 8);
    }

    VX_SECTION("growing preserves contents");
    {
        int* p = A::allocate(4);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 4 * sizeof(int), 0x31);

        int* q = A::reallocate(p, 4096);
        VX_CHECK(q != nullptr);
        VX_CHECK(check_pattern(q, 4 * sizeof(int), 0x31));
        A::deallocate(q, 4096);
    }

    VX_SECTION("shrinking preserves the prefix");
    {
        int* p = A::allocate(256);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 256 * sizeof(int), 0x41);

        int* q = A::reallocate(p, 16);
        VX_CHECK(q != nullptr);
        VX_CHECK(check_pattern(q, 16 * sizeof(int), 0x41));
        A::deallocate(q, 16);
    }

    VX_SECTION("byte_allocator");
    {
        using B = mem::byte_allocator<>;
        unsigned char* p = B::allocate(10);
        VX_CHECK(p != nullptr);
        fill_pattern(p, 10, 0x51);
        unsigned char* q = B::reallocate(p, 5000);
        VX_CHECK(q != nullptr);
        VX_CHECK(check_pattern(q, 10, 0x51));
        B::deallocate(q, 5000);
    }

    VX_SECTION("aligned_allocator with a null pointer forwards to allocate");
    {
        using AA = mem::aligned_allocator<int, 64>;
        int* p = AA::reallocate(nullptr, 16);
        VX_CHECK(p != nullptr);
        VX_CHECK(is_aligned(p, 64));
        AA::deallocate(p, 16);
    }
}

//=============================================================================
// std container interop
//=============================================================================

VX_TEST_CASE(alloc_std_vector)
{
    VX_SECTION("default_allocator<int>");
    {
        std::vector<int, mem::default_allocator<int>> v;
        for (int i = 0; i < 100; ++i)
        {
            v.push_back(i);
        }

        VX_CHECK(v.size() == 100);
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(v[i] == i);
        }

        v.erase(v.begin(), v.begin() + 50);
        VX_CHECK(v.size() == 50);
        VX_CHECK(v.front() == 50);
        VX_CHECK(v.back() == 99);

        v.shrink_to_fit();
        VX_CHECK(v.front() == 50);
    }

    VX_SECTION("aligned_allocator keeps the buffer aligned across growth");
    {
        std::vector<int, mem::aligned_allocator<int, mem::ideal_align>> v;
        v.reserve(8);
        VX_CHECK(is_aligned(v.data(), mem::ideal_align));

        for (int i = 0; i < 1000; ++i)
        {
            v.push_back(i);
            VX_CHECK(is_aligned(v.data(), mem::ideal_align));
        }

        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(v[i] == i);
        }
    }

    VX_SECTION("over-aligned element type");
    {
        std::vector<over_aligned, mem::aligned_allocator<over_aligned, 64>> v(5);
        VX_CHECK(is_aligned(v.data(), 64));
        for (std::size_t i = 0; i < v.size(); ++i)
        {
            VX_CHECK(v[i].x == 3);
            VX_CHECK(is_aligned(&v[i], 64));
        }

        v.resize(50);
        VX_CHECK(is_aligned(v.data(), 64));
    }

    VX_SECTION("move-only elements");
    {
        std::vector<movable_int, mem::default_allocator<movable_int>> v;
        for (int i = 0; i < 50; ++i)
        {
            v.emplace_back(i);
        }

        VX_CHECK(v.size() == 50);
        for (int i = 0; i < 50; ++i)
        {
            VX_CHECK(v[i].val == i);
        }
    }

    VX_SECTION("copy and move of a container");
    {
        std::vector<int, mem::default_allocator<int>> a = { 1, 2, 3 };
        auto b = a;
        VX_CHECK(b.size() == 3);
        VX_CHECK(b[2] == 3);

        auto c = std::move(a);
        VX_CHECK(c.size() == 3);
        VX_CHECK(c[0] == 1);
    }
}

VX_TEST_CASE(alloc_std_list)
{
    // node-based containers go through rebind to the node type
    std::list<int, mem::default_allocator<int>> l;
    for (int i = 0; i < 20; ++i)
    {
        l.push_back(i);
    }

    VX_CHECK(l.size() == 20);
    int expected = 0;
    for (int x : l)
    {
        VX_CHECK(x == expected);
        ++expected;
    }

    l.pop_front();
    VX_CHECK(l.front() == 1);
}

//=============================================================================
// SUSPECT: these are built only when VX_TEST_ALLOC_SUSPECT is defined.
//
//   1. aligned_allocator<T> with the default Alignment = 0 trips its own
//      static_asserts (0 >= alignof(T) is false and 0 is not a power of two),
//      so the default argument can never be used. The old design treated 0 as
//      "use alignof(T)". This is a compile error, hence the gate.
//
//   2. reallocate on a non-null pointer goes through reallocate_aligned, which
//      has the abort and data-not-moved problems described in
//      memory_tests.cpp. Run these one at a time with --filter, since an
//      abort stops the whole run.
//=============================================================================

#ifdef VX_TEST_ALLOC_SUSPECT

VX_TEST_CASE(alloc_suspect_aligned_allocator_default_alignment)
{
    using A = mem::aligned_allocator<int>;
    VX_CHECK(A::alignment >= alignof(int));

    A a;
    int* p = a.allocate(4);
    VX_CHECK(p != nullptr);
    VX_CHECK(is_aligned(p, A::alignment));
    a.deallocate(p, 4);
}

VX_TEST_CASE(alloc_suspect_reallocate_aligned_path)
{
    using A = mem::aligned_allocator<int, 64>;

    int* p = A::allocate(16);
    VX_CHECK(p != nullptr);
    fill_pattern(p, 16 * sizeof(int), 0x61);

    int* q = A::reallocate(p, 4096);
    VX_CHECK(q != nullptr);
    VX_CHECK(is_aligned(q, 64));
    VX_CHECK(check_pattern(q, 16 * sizeof(int), 0x61));
    A::deallocate(q, 4096);
}

VX_TEST_CASE(alloc_suspect_reallocate_via_traits)
{
    using A = mem::aligned_allocator<int, mem::ideal_align>;
    using traits = mem::allocator_traits<A>;
    A a;

    int* p = traits::allocate(a, 16);
    VX_CHECK(p != nullptr);
    fill_pattern(p, 16 * sizeof(int), 0x71);

    int* q = traits::reallocate(a, p, 4096);
    VX_CHECK(q != nullptr);
    VX_CHECK(is_aligned(q, mem::ideal_align));
    VX_CHECK(check_pattern(q, 16 * sizeof(int), 0x71));
    traits::deallocate(a, q, 4096);
}

#endif // VX_TEST_ALLOC_SUSPECT

//=============================================================================

#ifndef VX_TEST_NO_MAIN
int main(int argc, char** argv)
{
    return VX_RUN_TESTS(argc, argv);
}
#endif
