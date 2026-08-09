#pragma once

#include "vertex/std/math/checked_arithmetic.hpp"
#include "vertex/std/memory.hpp"
#include "vertex/std/string_utils.hpp"

namespace vx {
namespace str {

template <typename C>
class packed_string_array
{
public:

    using char_type = C;
    using string_type = const C*;
    using array_type = const C**;

private:

    array_type m_data = nullptr;
    size_t m_count = 0;

public:

    packed_string_array() noexcept = default;

    ~packed_string_array()
    {
        reset();
    }

    packed_string_array(const packed_string_array&) = delete;
    packed_string_array& operator=(const packed_string_array&) = delete;

    packed_string_array(packed_string_array&& other) noexcept
        : m_data(other.m_data)
        , m_count(other.m_count)
    {
        other.m_data = nullptr;
        other.m_count = 0;
    }

    packed_string_array& operator=(packed_string_array&& other) noexcept
    {
        if (this != &other)
        {
            reset();

            m_data = other.m_data;
            m_count = other.m_count;

            other.m_data = nullptr;
            other.m_count = 0;
        }

        return *this;
    }

    VX_NO_DISCARD static packed_string_array create(
        const C* const* src,
        const size_t count)
    {
        VX_ASSERT(src);
        VX_ASSERT(count);

        packed_string_array result;

        // Space for the pointer table:
        // (count + 1) entries of 'const C*'
        // The +1 is for the final nullptr terminator.
        size_t pointer_count;
        size_t pointer_bytes;
        size_t string_bytes = 0;
        size_t total_bytes;

#if defined(VX_PACKED_STRING_ARRAY_DISABLE_MAX_SIZE_CHECKS)

        pointer_count = count + 1;
        pointer_bytes = pointer_count * sizeof(array_type);

        // Add the space required for all strings stored consecutively,
        // each including its null terminator.
        for (size_t i = 0; i < count; ++i)
        {
            VX_ASSERT(src[i]);
            const size_t length = str::length(src[i]) + 1;
            string_bytes += (length * sizeof(C));
        }

        total_bytes = pointer_bytes + string_bytes;

#else

        // pointer_count must not have overflowed, and must not overflow
        // when multiplied by the pointer size.
        if (!math::checked_add(count, size_t(1), pointer_count) ||
            !math::checked_mul(pointer_count, sizeof(array_type), pointer_bytes))
        {
            err::set(err::size_error);
            return result;
        }

        // Add the space required for all strings stored consecutively,
        // each including its null terminator.
        for (size_t i = 0; i < count; ++i)
        {
            VX_ASSERT(src[i]);

            size_t length;
            size_t bytes;
            size_t new_string_bytes;

            if (!math::checked_add(str::length(src[i]), size_t(1), length) ||
                !math::checked_mul(length, sizeof(C), bytes) ||
                !math::checked_add(string_bytes, bytes, new_string_bytes))
            {
                err::set(err::size_error);
                return result;
            }

            string_bytes = new_string_bytes;
        }

        if (!math::checked_add(pointer_bytes, string_bytes, total_bytes))
        {
            err::set(err::size_error);
            return result;
        }

#endif // VX_PACKED_STRING_ARRAY_DISABLE_MAX_SIZE_CHECKS

        // Allocate a single contiguous block:
        // [pointer table][string data...]
        void* memory = mem::allocate(total_bytes);
        if (!memory)
        {
            return result;
        }

        // Pointer to the pointer table
        auto** table = static_cast<array_type>(memory);

        result.m_data = table;
        result.m_count = count;

        // Pointer where string storage begins.
        // This jumps past the pointer table:
        auto* string_data = result.storage();

        // Copy each string into the packed region and
        // populate the pointer table to point at each copied string.
        for (size_t i = 0; i < count; ++i)
        {
            table[i] = string_data;
            const size_t length = str::length(src[i]) + 1;
            mem::copy(string_data, src[i], length * sizeof(C));
            string_data += length;
        }

        // Null-terminate the pointer array
        table[count] = nullptr;
        return result;
    }

    void reset() noexcept
    {
        if (m_data)
        {
            mem::deallocate_raw(m_data);
            m_data = nullptr;
            m_count = 0;
        }
    }

    array_type data() const noexcept
    {
        return m_data;
    }

    size_t size() const noexcept
    {
        return m_count;
    }

    bool empty() const noexcept
    {
        return m_count == 0;
    }

    string_type operator[](size_t index) const noexcept
    {
        return m_data[index];
    }

    explicit operator bool() const noexcept
    {
        return m_data != nullptr;
    }

private:

    C* storage() noexcept
    {
        return reinterpret_cast<C*>(
            reinterpret_cast<unsigned char*>(m_data) +
            pointer_table_bytes());
    }

    const C* storage() const noexcept
    {
        return reinterpret_cast<const C*>(
            reinterpret_cast<const unsigned char*>(m_data) +
            pointer_table_bytes());
    }

    size_t pointer_table_bytes() const noexcept
    {
        return (m_count + 1) * sizeof(const C*);
    }
};

} // namespace str
} // namespace vx
