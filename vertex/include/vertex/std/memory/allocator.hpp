#pragma once

#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace mem {

//=========================================================================
// allocator traits (internal detection helpers)
//=========================================================================

namespace _mem_allocator_priv {

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_pointer
{
    using type = Default;
};

template <typename A, typename Default>
struct get_pointer<A, Default, traits::void_t<typename A::pointer>>
{
    using type = typename A::pointer;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_const_pointer
{
    using type = Default;
};

template <typename A, typename Default>
struct get_const_pointer<A, Default, traits::void_t<typename A::const_pointer>>
{
    using type = typename A::const_pointer;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_void_pointer
{
    using type = Default;
};

template <typename A, typename Default>
struct get_void_pointer<A, Default, traits::void_t<typename A::void_pointer>>
{
    using type = typename A::void_pointer;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_const_void_pointer
{
    using type = Default;
};

template <typename A, typename Default>
struct get_const_void_pointer<A, Default, traits::void_t<typename A::const_void_pointer>>
{
    using type = typename A::const_void_pointer;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_difference_type
{
    using type = Default;
};

template <typename A, typename Default>
struct get_difference_type<A, Default, traits::void_t<typename A::difference_type>>
{
    using type = typename A::difference_type;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_size_type
{
    using type = Default;
};

template <typename A, typename Default>
struct get_size_type<A, Default, traits::void_t<typename A::size_type>>
{
    using type = typename A::size_type;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_propagate_on_container_copy_assignment
{
    using type = Default;
};

template <typename A, typename Default>
struct get_propagate_on_container_copy_assignment<
    A,
    Default,
    traits::void_t<typename A::propagate_on_container_copy_assignment>>
{
    using type = typename A::propagate_on_container_copy_assignment;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_propagate_on_container_move_assignment
{
    using type = Default;
};

template <typename A, typename Default>
struct get_propagate_on_container_move_assignment<
    A,
    Default,
    traits::void_t<typename A::propagate_on_container_move_assignment>>
{
    using type = typename A::propagate_on_container_move_assignment;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_propagate_on_container_swap
{
    using type = Default;
};

template <typename A, typename Default>
struct get_propagate_on_container_swap<
    A,
    Default,
    traits::void_t<typename A::propagate_on_container_swap>>
{
    using type = typename A::propagate_on_container_swap;
};

//=====================================================================

template <typename A, typename Default, typename = void>
struct get_is_always_equal
{
    using type = Default;
};

template <typename A, typename Default>
struct get_is_always_equal<A, Default, traits::void_t<typename A::is_always_equal>>
{
    using type = typename A::is_always_equal;
};

//=====================================================================
// Rebind
//=====================================================================

// fallback: Alloc<T, Args...> -> Alloc<U, Args...> (type parameters only)
template <typename A, typename U>
struct rebind_template
{
};

template <template <typename, typename...> class A, typename T, typename... Args, typename U>
struct rebind_template<A<T, Args...>, U>
{
    using type = A<U, Args...>;
};

template <typename A, typename U, typename = void>
struct rebind_alloc_impl : rebind_template<A, U>
{
};

template <typename A, typename U>
struct rebind_alloc_impl<A, U, traits::void_t<typename A::template rebind<U>::other>>
{
    using type = typename A::template rebind<U>::other;
};

//=====================================================================
// member function detection
//=====================================================================

template <typename Void, typename A, typename N, typename H>
struct has_allocate_hint_impl : traits::false_type
{
};

template <typename A, typename N, typename H>
struct has_allocate_hint_impl<
    traits::void_t<decltype(traits::declval<A&>().allocate(traits::declval<N>(), traits::declval<H>()))>,
    A,
    N,
    H> : traits::true_type
{
};

template <typename A, typename N, typename H>
using has_allocate_hint = has_allocate_hint_impl<void, A, N, H>;

//=====================================================================

template <typename Void, typename A, typename P, typename N>
struct has_reallocate_impl : traits::false_type
{
};

template <typename A, typename P, typename N>
struct has_reallocate_impl<
    traits::void_t<decltype(traits::declval<A&>().reallocate(traits::declval<P>(), traits::declval<N>()))>,
    A,
    P,
    N> : traits::true_type
{
};

template <typename A, typename P, typename N>
using has_reallocate = has_reallocate_impl<void, A, P, N>;

//=====================================================================

template <typename Void, typename A, typename T, typename... Args>
struct has_construct_impl : traits::false_type
{
};

template <typename A, typename T, typename... Args>
struct has_construct_impl<
    traits::void_t<decltype(traits::declval<A&>().construct(traits::declval<T*>(), traits::declval<Args>()...))>,
    A,
    T,
    Args...> : traits::true_type
{
};

template <typename A, typename T, typename... Args>
using has_construct = has_construct_impl<void, A, T, Args...>;

//=====================================================================

template <typename Void, typename A, typename T>
struct has_destroy_impl : traits::false_type
{
};

template <typename A, typename T>
struct has_destroy_impl<traits::void_t<decltype(traits::declval<A&>().destroy(traits::declval<T*>()))>, A, T> : traits::true_type
{
};

template <typename A, typename T>
using has_destroy = has_destroy_impl<void, A, T>;

//=====================================================================

template <typename A, typename = void>
struct has_max_size : traits::false_type
{
};

template <typename A>
struct has_max_size<A, traits::void_t<decltype(traits::declval<const A&>().max_size())>> : traits::true_type
{
};

//=====================================================================

template <typename A, typename = void>
struct has_select_on_copy : traits::false_type
{
};

template <typename A>
struct has_select_on_copy<A, traits::void_t<decltype(traits::declval<const A&>().select_on_container_copy_construction())>> : traits::true_type
{
};

//=====================================================================
// Tag dispatch
//=====================================================================

template <typename A, typename P, typename N, typename H>
P allocate_hint_impl(traits::true_type, A& a, N n, H hint)
{
    return a.allocate(n, hint);
}

template <typename A, typename P, typename N, typename H>
P allocate_hint_impl(traits::false_type, A& a, N n, H)
{
    return a.allocate(n);
}

//=====================================================================

template <typename A, typename T, typename... Args>
void construct_impl(traits::true_type, A& a, T* p, Args&&... args)
{
    a.construct(p, vx::forward<Args>(args)...);
}

template <typename A, typename T, typename... Args>
void construct_impl(traits::false_type, A&, T* p, Args&&... args)
{
    mem::construct_in_place(p, vx::forward<Args>(args)...);
}

//=====================================================================

template <typename A, typename T>
void destroy_impl(traits::true_type, A& a, T* p)
{
    a.destroy(p);
}

template <typename A, typename T>
void destroy_impl(traits::false_type, A&, T* p)
{
    mem::destroy_in_place(p);
}

//=====================================================================

template <typename A, typename S>
S max_size_impl(traits::true_type, const A& a) noexcept
{
    return static_cast<S>(a.max_size());
}

template <typename A, typename S>
S max_size_impl(traits::false_type, const A&) noexcept
{
    return static_cast<S>(mem::max_array_size<typename A::value_type>());
}

//=====================================================================

template <typename A>
A select_on_copy_impl(traits::true_type, const A& a)
{
    return a.select_on_container_copy_construction();
}

template <typename A>
A select_on_copy_impl(traits::false_type, const A& a)
{
    return a;
}

//=====================================================================

template <typename A, typename B>
constexpr bool equal_impl(traits::true_type, const A&, const B&) noexcept
{
    return true;
}

template <typename A, typename B>
constexpr bool equal_impl(traits::false_type, const A& a, const B& b) noexcept
{
    return a == b;
}

} // namespace _mem_allocator_priv

//=========================================================================
// allocator traits
//=========================================================================

template <typename Allocator>
struct allocator_traits
{
    using allocator_type = Allocator;
    using value_type = typename Allocator::value_type;

    using pointer = typename _mem_allocator_priv::get_pointer<Allocator, value_type*>::type;

    using const_pointer = typename _mem_allocator_priv::get_const_pointer<
        Allocator,
        typename traits::pointer_traits<pointer>::template rebind<const value_type>>::type;

    using void_pointer = typename _mem_allocator_priv::get_void_pointer<
        Allocator,
        typename traits::pointer_traits<pointer>::template rebind<void>>::type;

    using const_void_pointer = typename _mem_allocator_priv::get_const_void_pointer<
        Allocator,
        typename traits::pointer_traits<pointer>::template rebind<const void>>::type;

    using difference_type = typename _mem_allocator_priv::get_difference_type<
        Allocator,
        typename traits::pointer_traits<pointer>::difference_type>::type;

    using size_type = typename _mem_allocator_priv::get_size_type<
        Allocator,
        typename traits::make_unsigned<difference_type>::type>::type;

    using propagate_on_container_copy_assignment =
        typename _mem_allocator_priv::get_propagate_on_container_copy_assignment<Allocator, traits::false_type>::type;

    using propagate_on_container_move_assignment =
        typename _mem_allocator_priv::get_propagate_on_container_move_assignment<Allocator, traits::false_type>::type;

    using propagate_on_container_swap =
        typename _mem_allocator_priv::get_propagate_on_container_swap<Allocator, traits::false_type>::type;

    using is_always_equal =
        typename _mem_allocator_priv::get_is_always_equal<Allocator, typename traits::is_empty<Allocator>::type>::type;

    template <typename U>
    using rebind_alloc = typename _mem_allocator_priv::rebind_alloc_impl<Allocator, U>::type;

    template <typename U>
    using rebind_traits = allocator_traits<rebind_alloc<U>>;

    // true if the allocator provides reallocate(pointer, size_type)
    using supports_reallocate = _mem_allocator_priv::has_reallocate<Allocator, pointer, size_type>;

    //=====================================================================
    // allocation
    //=====================================================================

    static VX_NO_DISCARD pointer allocate(Allocator& a, const size_type n)
    {
        return a.allocate(n);
    }

    static VX_NO_DISCARD pointer allocate(Allocator& a, const size_type n, const_void_pointer hint)
    {
        return _mem_allocator_priv::allocate_hint_impl<Allocator, pointer, size_type, const_void_pointer>(
            typename _mem_allocator_priv::has_allocate_hint<Allocator, size_type, const_void_pointer>::type{},
            a, n, hint);
    }

    // extension: only valid if the allocator itself provides reallocate().
    // Only legal for element types that can be bitwise relocated.
    static VX_NO_DISCARD pointer reallocate(Allocator& a, pointer p, const size_type n)
    {
        VX_STATIC_ASSERT_MSG(supports_reallocate::value, "allocator does not provide reallocate(pointer, size_type)");
        return a.reallocate(p, n);
    }

    static void deallocate(Allocator& a, pointer p, const size_type n)
    {
        a.deallocate(p, n);
    }

    //=====================================================================
    // construction / destruction
    //=====================================================================

    template <typename T, typename... Args>
    static void construct(Allocator& a, T* p, Args&&... args)
    {
        _mem_allocator_priv::construct_impl(
            typename _mem_allocator_priv::has_construct<Allocator, T, Args...>::type{},
            a, p, vx::forward<Args>(args)...);
    }

    template <typename T>
    static void destroy(Allocator& a, T* p)
    {
        _mem_allocator_priv::destroy_impl(
            typename _mem_allocator_priv::has_destroy<Allocator, T>::type{}, a, p);
    }

    //=====================================================================
    // queries
    //=====================================================================

    static size_type max_size(const Allocator& a) noexcept
    {
        return _mem_allocator_priv::max_size_impl<Allocator, size_type>(
            typename _mem_allocator_priv::has_max_size<Allocator>::type{}, a);
    }

    static Allocator select_on_container_copy_construction(const Allocator& a)
    {
        return _mem_allocator_priv::select_on_copy_impl(
            typename _mem_allocator_priv::has_select_on_copy<Allocator>::type{}, a);
    }

    // extension
    template <typename Other>
    static constexpr bool equal(const Allocator& a, const Other& b) noexcept
    {
        using AE = traits::integral_constant<bool, is_always_equal::value && allocator_traits<Other>::is_always_equal::value>;
        return _mem_allocator_priv::equal_impl(
            AE{},
            a, b);
    }
};

template <typename A, typename B>
constexpr bool allocators_equal(const A& a, const B& b) noexcept
{
    return allocator_traits<A>::equal(a, b);
}

#define VX_PRIV_ASSERT_ALLOCATORS_EQUAL(a, b) \
    VX_ASSERT(::vx::mem::allocators_equal(a, b))

//=========================================================================
// default allocator
//=========================================================================

template <typename T>
class default_allocator
{
public:

    using value_type = T;
    using pointer_type = value_type*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using is_always_equal = traits::true_type;

    static constexpr size_t alignment = alignof(T);

    VX_STATIC_ASSERT_MSG(alignof(T) <= max_align, "over-aligned type: use aligned_allocator<T, Alignment>");

    template <typename U>
    struct rebind
    {
        using other = default_allocator<U>;
    };

    default_allocator() noexcept = default;
    default_allocator(const default_allocator&) noexcept = default;
    default_allocator(default_allocator&&) noexcept = default;
    default_allocator& operator=(const default_allocator&) noexcept = default;
    default_allocator& operator=(default_allocator&&) noexcept = default;

    template <typename U>
    default_allocator(const default_allocator<U>&) noexcept
    {
    }

    static constexpr size_type max_size() noexcept
    {
        return max_array_size<T>();
    }

    VX_ALLOCATOR static VX_NO_DISCARD pointer_type allocate(const size_t count) noexcept
    {
        VX_RET_IF_UL(count == 0, nullptr);
        VX_RET_IF_UL(count > max_size(), nullptr);
        return static_cast<pointer_type>(mem::allocate(count * sizeof(T)));
    }

    static VX_NO_DISCARD pointer_type reallocate(pointer_type ptr, const size_t count) noexcept
    {
        VX_RET_IF_UL(count > max_size(), nullptr);
        return static_cast<pointer_type>(mem::reallocate(ptr, count * sizeof(T)));
    }

    static void deallocate(pointer_type ptr, const size_t count) noexcept
    {
        mem::deallocate(ptr, count * sizeof(T));
    }
};

template <typename T, typename U>
constexpr bool operator==(const default_allocator<T>&, const default_allocator<U>&) noexcept
{
    return true;
}

template <typename T, typename U>
constexpr bool operator!=(const default_allocator<T>&, const default_allocator<U>&) noexcept
{
    return false;
}

//=========================================================================
// aligned allocator
//=========================================================================

enum class alignment_policy
{
    at_least,
    exact,
};

template <typename T, size_t Alignment = 0>
class aligned_allocator
{
public:

    using value_type = T;
    using pointer_type = value_type*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using is_always_equal = traits::true_type;

    static constexpr size_t alignment = Alignment;

    VX_STATIC_ASSERT_MSG(alignment >= alignof(T), "Alignment must be at least alignof(T)");
    VX_STATIC_ASSERT_MSG(_mem_priv::is_pow_2(alignment), "Alignment must be power of 2");

    template <typename U>
    struct rebind
    {
        using other = aligned_allocator<U, Alignment>;
    };

    aligned_allocator() noexcept = default;
    aligned_allocator(const aligned_allocator&) noexcept = default;
    aligned_allocator(aligned_allocator&&) noexcept = default;
    aligned_allocator& operator=(const aligned_allocator&) noexcept = default;
    aligned_allocator& operator=(aligned_allocator&&) noexcept = default;

    template <typename U>
    aligned_allocator(const aligned_allocator<U, Alignment>&) noexcept
    {
    }

    static constexpr size_type max_size() noexcept
    {
        return max_array_size<T>();
    }

    VX_ALLOCATOR static VX_NO_DISCARD pointer_type allocate(const size_t count) noexcept
    {
        VX_RET_IF_UL(count == 0, nullptr);
        VX_RET_IF_UL(count > max_size(), nullptr);
        return static_cast<pointer_type>(allocate_aligned<alignment>(count * sizeof(T)));
    }

    static VX_NO_DISCARD pointer_type reallocate(pointer_type ptr, const size_t count) noexcept
    {
        VX_RET_IF_UL(count > max_size(), nullptr);
        return static_cast<pointer_type>(reallocate_aligned<alignment>(ptr, count * sizeof(T)));
    }

    static void deallocate(pointer_type ptr, const size_t count) noexcept
    {
        deallocate_aligned<alignment>(ptr, count * sizeof(T));
    }
};

template <typename T, typename U, size_t A>
constexpr bool operator==(const aligned_allocator<T, A>&, const aligned_allocator<U, A>&) noexcept
{
    return true;
}

template <typename T, typename U, size_t A>
constexpr bool operator!=(const aligned_allocator<T, A>&, const aligned_allocator<U, A>&) noexcept
{
    return false;
}

//=========================================================================
// alignment-based selection
//=========================================================================

namespace _mem_allocator_priv {

template <typename T, size_t Alignment>
struct select_allocator
{
    VX_STATIC_ASSERT_MSG(_mem_priv::is_pow_2(Alignment), "Alignment must be power of 2");
    VX_STATIC_ASSERT_MSG(Alignment >= alignof(T), "Alignment must be at least alignof(T)");

    using type = typename traits::conditional<
        (Alignment <= max_align),
        default_allocator<T>,
        aligned_allocator<T, Alignment>>::type;
};

} // namespace _mem_allocator_priv

// "at least Alignment": malloc when it already satisfies it, header scheme otherwise
template <typename T, size_t Alignment>
using allocator_with_alignment = typename _mem_allocator_priv::select_allocator<T, Alignment>::type;

template <size_t Alignment = alignof(unsigned char)>
using byte_allocator = allocator_with_alignment<unsigned char, Alignment>;

//=========================================================================
// allocator rebinding
//=========================================================================

template <typename Allocator, typename U>
struct rebind_allocator
{
    using type = typename allocator_traits<Allocator>::template rebind_alloc<U>;
};

} // namespace mem
} // namespace vx
