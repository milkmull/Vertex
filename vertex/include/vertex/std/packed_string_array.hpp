#pragma once

#include "vertex/std/math/checked_arithmetic.hpp"
#include "vertex/std/memory.hpp"
#include "vertex/std/string_utils.hpp"

namespace vx {
namespace str {

template <typename C, typename Allocator = mem::byte_allocator<alignof(C)>>
class packed_string_array
{
public:

    using char_type = C;
    using string_type = const C*;
    using array_type = const C**;
    using allocator_type = Allocator;

    VX_STATIC_ASSERT_MSG(type_traits::is_char<C>::value, "C must be character type.");

    VX_STATIC_ASSERT_MSG(
        sizeof(typename Allocator::value_type) == 1,
        "packed_string_array's Allocator must allocate in byte-sized units (use a byte_allocator)");

private:

    struct buffer_type
    {
        array_type data = nullptr;
        size_t count = 0;
    };

    // allocator_storage gives EBO when allocator_type is stateless (the common case),
    // and falls back to a real member when it isn't.
    mem::_mem_priv::allocator_storage<allocator_type, buffer_type> m_storage;

    allocator_type& allocator() noexcept
    {
        return m_storage.allocator();
    }
    const allocator_type& allocator() const noexcept
    {
        return m_storage.allocator();
    }

public:

    packed_string_array() noexcept = default;

    ~packed_string_array()
    {
        reset();
    }

    packed_string_array(const packed_string_array&) = delete;
    packed_string_array& operator=(const packed_string_array&) = delete;

    packed_string_array(packed_string_array&& other) noexcept
        : m_storage(std::move(other.m_storage))
    {
        other.m_storage.value.data = nullptr;
        other.m_storage.value.count = 0;
    }

    packed_string_array& operator=(packed_string_array&& other) noexcept
    {
        if (this != &other)
        {
            reset();

            m_storage = std::move(other.m_storage);

            other.m_storage.value.data = nullptr;
            other.m_storage.value.count = 0;
        }

        return *this;
    }

    VX_NO_DISCARD static packed_string_array create(
        const C* const* src,
        const size_t count,
        const allocator_type& alloc = allocator_type())
    {
        VX_ASSERT(src);
        VX_ASSERT(count);

        packed_string_array result;

        size_t pointer_count;
        size_t pointer_bytes;
        size_t string_bytes = 0;
        size_t total_bytes;

#if defined(VX_PACKED_STRING_ARRAY_DISABLE_MAX_SIZE_CHECKS)

        pointer_count = count + 1;
        pointer_bytes = pointer_count * sizeof(array_type);

        for (size_t i = 0; i < count; ++i)
        {
            VX_ASSERT(src[i]);
            const size_t length = str::length(src[i]) + 1;
            string_bytes += (length * sizeof(C));
        }

        total_bytes = pointer_bytes + string_bytes;

#else

        if (!math::checked_add(count, size_t(1), pointer_count) ||
            !math::checked_mul(pointer_count, sizeof(array_type), pointer_bytes))
        {
            err::set(err::size_error);
            return result;
        }

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

        // Allocate a single contiguous block via the allocator.
        // allocator_type::value_type is expected to be a 1-byte type, so `total_bytes`
        // counts elements too.
        typename allocator_type::value_type* memory = result.allocator().allocate(total_bytes);
        if (!memory)
        {
            return result;
        }

        auto** table = reinterpret_cast<array_type>(memory);

        result.m_storage.value.data = table;
        result.m_storage.value.count = count;

        auto* string_data = result.storage();

        for (size_t i = 0; i < count; ++i)
        {
            table[i] = string_data;
            const size_t length = str::length(src[i]) + 1;
            mem::copy(string_data, src[i], length * sizeof(C));
            string_data += length;
        }

        table[count] = nullptr;
        return result;
    }

    void reset() noexcept
    {
        if (m_storage.value.data)
        {
            // Recompute the block size at destruction time rather than caching
            // it separately. Relies on the string data still being intact and
            // null-terminated (nothing in this class mutates it after create()).
            const size_t bytes = calculate_total_bytes();

            allocator().deallocate(
                reinterpret_cast<typename allocator_type::value_type*>(
                    const_cast<C**>(m_storage.value.data)),
                bytes);

            m_storage.value.data = nullptr;
            m_storage.value.count = 0;
        }
    }

    array_type data() const noexcept
    {
        return m_storage.value.data;
    }

    size_t size() const noexcept
    {
        return m_storage.value.count;
    }

    bool empty() const noexcept
    {
        return m_storage.value.count == 0;
    }

    string_type operator[](size_t index) const noexcept
    {
        return m_storage.value.data[index];
    }

    explicit operator bool() const noexcept
    {
        return m_storage.value.data != nullptr;
    }

    const allocator_type& get_allocator() const noexcept
    {
        return allocator();
    }

private:

    C* storage() noexcept
    {
        return reinterpret_cast<C*>(
            reinterpret_cast<unsigned char*>(const_cast<C**>(m_storage.value.data)) +
            pointer_table_bytes());
    }

    const C* storage() const noexcept
    {
        return reinterpret_cast<const C*>(
            reinterpret_cast<const unsigned char*>(m_storage.value.data) +
            pointer_table_bytes());
    }

    size_t pointer_table_bytes() const noexcept
    {
        return (m_storage.value.count + 1) * sizeof(const C*);
    }

    size_t calculate_total_bytes() const noexcept
    {
        size_t bytes = pointer_table_bytes();

        for (size_t i = 0; i < m_storage.value.count; ++i)
        {
            const size_t length = str::length(m_storage.value.data[i]) + 1;
            bytes += length * sizeof(C);
        }

        return bytes;
    }
};

} // namespace str
} // namespace vx
