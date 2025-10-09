#pragma once

#include "vertex/config/assert.hpp"
#include "vertex/util/bit/bit.hpp"
#include "vertex/config/type_traits.hpp"

namespace vx {

template <size_t N, typename T = size_t>
class bit_field
{
    VX_STATIC_ASSERT(N > 0, "N must be greater than 0");
    VX_STATIC_ASSERT(std::is_unsigned<T>::value, "T must be unsigned integer type");

public:

    ///////////////////////////////////////////////////////////////////////////////
    // member types
    ///////////////////////////////////////////////////////////////////////////////

    using value_type = bool;
    using block_type = T;

    static constexpr size_t size = N;
    static constexpr size_t bits = N;
    static constexpr size_t block_bits = sizeof(block_type) * 8;
    static constexpr size_t blocks = (bits + block_bits - 1) / block_bits;

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    constexpr bit_field() noexcept { reset(); }

    // construct with all bits set or cleared
    explicit constexpr bit_field(bool value) noexcept
    {
        if (value)
        {
            set();
        }
        else
        {
            reset();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // element access
    ///////////////////////////////////////////////////////////////////////////////

    constexpr bool test(size_t pos) const noexcept
    {
        VX_ASSERT(pos < bits);
        return (m_data[pos / block_bits] >> (pos % block_bits)) & static_cast<block_type>(1);
    }

    constexpr bool operator[](size_t pos) const noexcept { return test(pos); }

    ///////////////////////////////////////////////////////////////////////////////
    // modifiers
    ///////////////////////////////////////////////////////////////////////////////

    constexpr void set(size_t pos) noexcept
    {
        VX_ASSERT(pos < bits);
        m_data[pos / block_bits] |= (static_cast<block_type>(1) << (pos % block_bits));
    }

    constexpr void reset(size_t pos) noexcept
    {
        VX_ASSERT(pos < bits);
        m_data[pos / block_bits] &= ~(static_cast<block_type>(1) << (pos % block_bits));
    }

    constexpr void flip(size_t pos) noexcept
    {
        VX_ASSERT(pos < bits);
        m_data[pos / block_bits] ^= (static_cast<block_type>(1) << (pos % block_bits));
    }

    constexpr void set() noexcept
    {
        for (size_t i = 0; i < blocks; ++i)
        {
            m_data[i] = ~static_cast<block_type>(0);
        }

        trim_unused();
    }

    constexpr void reset() noexcept
    {
        for (size_t i = 0; i < blocks; ++i)
        {
            m_data[i] = static_cast<block_type>(0);
        }
    }

    constexpr void flip() noexcept
    {
        for (size_t i = 0; i < blocks; ++i)
        {
            m_data[i] = ~m_data[i];
        }

        trim_unused();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // capacity helpers
    ///////////////////////////////////////////////////////////////////////////////

    constexpr bool any() const noexcept
    {
        for (size_t i = 0; i < blocks; ++i)
        {
            if (m_data[i])
            {
                return true;
            }
        }

        return false;
    }

    constexpr bool none() const noexcept { return !any(); }

    constexpr size_t count() const noexcept
    {
        size_t total = 0;

        for (size_t i = 0; i < blocks; ++i)
        {
            total += bit::popcount(m_data[i]);
        }

        return total;
    }

private:

    constexpr void trim_unused() noexcept
    {
        // mask off unused bits in the last block
        constexpr size_t used = bits % block_bits;

        VX_IF_CONSTEXPR(used != 0)
        {
            const block_type mask = (static_cast<block_type>(1) << used) - 1;
            m_data[blocks - 1] &= mask;
        }
    }

private:

    block_type m_data[blocks];
};

} // namespace vx
