#pragma once

#include "vertex/std/_memory/memory_base.hpp"

namespace vx {
namespace mem {

//=========================================================================
// allocator
//=========================================================================

enum class alignment_policy
{
    at_least,
    exact,
};

template <typename T, size_t Alignment = alignof(T), alignment_policy Policy = alignment_policy::at_least>
class default_allocator
{
public:

    VX_STATIC_ASSERT_MSG(Alignment >= alignof(T), "Alignment must be at alignof(T)");
    VX_STATIC_ASSERT_MSG(_mem_priv::is_pow_2(Alignment), "Alignment must be power of 2");

    using value_type = T;
    using pointer_type = value_type*;

    using size_type = size_t;
    using difference_type = ptrdiff_t;

    static constexpr size_t alignment = Alignment;
    static constexpr alignment_policy policy = Policy;

    template <typename U>
    struct rebind
    {
        // If Alignment matches T's natural alignment (the common/default case),
        // recompute it for U so Alignment >= alignof(U) still holds.
        // If the caller picked an explicit, non-default alignment, preserve it
        // as-is — if that's now invalid for U, the static_assert in the new
        // default_allocator<U, ...> instantiation will say so clearly.
        static constexpr size_t rebound_alignment =
            (Alignment == alignof(T)) ? alignof(U) : Alignment;

        using other = default_allocator<U, rebound_alignment, Policy>;
    };

    default_allocator() noexcept = default;

    // stateless, so these are trivial — declared explicitly (rather than left
    // implicit) so the Cpp17Allocator "shall not exit via exception"
    // requirement stays enforced even if a data member is ever added later.
    default_allocator(const default_allocator&) noexcept = default;
    default_allocator(default_allocator&&) noexcept = default;
    default_allocator& operator=(const default_allocator&) noexcept = default;
    default_allocator& operator=(default_allocator&&) noexcept = default;

    template <typename U, size_t UAlignment, alignment_policy UPolicy>
    default_allocator(const default_allocator<U, UAlignment, UPolicy>&) noexcept
    {
        VX_STATIC_ASSERT_MSG(UPolicy == Policy, "cannot convert between default_allocator instances with different alignment policies");
    }

    friend bool operator==(const default_allocator&, const default_allocator&) noexcept
    {
        return true;
    }

    friend bool operator!=(const default_allocator&, const default_allocator&) noexcept
    {
        return false;
    }

    static constexpr size_type max_size() noexcept
    {
        return max_array_size<T>();
    }

    VX_ALLOCATOR static pointer_type allocate(const size_t count) noexcept
    {
        VX_UNLIKELY_COLD_PATH(count == 0,
            {
                return nullptr;
            });

        const size_t bytes = count * sizeof(value_type);

        VX_IF_CONSTEXPR (policy == alignment_policy::at_least && alignment <= max_align)
        {
            return static_cast<pointer_type>(mem::allocate(bytes));
        }
        else
        {
            return static_cast<pointer_type>(allocate_aligned<alignment>(bytes));
        }
    }

    static pointer_type reallocate(pointer_type ptr, const size_t count) noexcept
    {
        const size_t bytes = count * sizeof(value_type);

        VX_IF_CONSTEXPR (policy == alignment_policy::at_least && alignment <= max_align)
        {
            return static_cast<T*>(mem::reallocate(ptr, bytes));
        }
        else
        {
            return static_cast<T*>(reallocate_aligned<alignment>(ptr, bytes));
        }
    }

    static void deallocate(pointer_type ptr, const size_t count) noexcept
    {
        const size_t bytes = count * sizeof(value_type);

        VX_IF_CONSTEXPR (policy == alignment_policy::at_least && alignment <= max_align)
        {
            mem::deallocate(ptr, bytes);
        }
        else
        {
            deallocate_aligned<alignment>(ptr, bytes);
        }
    }
};

template <typename T>
using aligned_allocator = default_allocator<T, ideal_align, alignment_policy::exact>;

template <size_t Alignment = alignof(unsigned char), alignment_policy Policy = alignment_policy::at_least>
using byte_allocator = default_allocator<unsigned char, Alignment, Policy>;

//=========================================================================
// allocator rebinding
//=========================================================================

template <typename Allocator, typename U>
struct rebind_allocator
{
    using type = typename Allocator::template rebind<U>::other;
};

} // namespace mem
} // namespace vx
