#pragma once

#include <bitset>

#include "vertex/config/assert.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/std/crypto/fnv1a.hpp"
#include "vertex/std/expected.hpp"

namespace vx {

template <typename T>
struct hash;

template <size_t N>
class bitset
{
private:

    //=========================================================================
    // member types
    //=========================================================================

    friend hash<bitset<N>>;

    using T = typename std::conditional<N <= sizeof(unsigned long) * CHAR_BIT, unsigned long, unsigned long long>::type;

    static constexpr size_t bits = N;
    static constexpr size_t bits_per_word = sizeof(T) * CHAR_BIT;
    static constexpr size_t words = (bits == 0) ? 0 : (bits - 1) / bits_per_word;

    static constexpr bool need_mask = (bits != 0) && (bits % bits_per_word != 0);
    static constexpr unsigned long long mask = 1ull << (need_mask ? bits % bits_per_word : 0) - 1ull;
    static constexpr T native_mask = static_cast<T>(mask);

public:

    class reference
    {
        friend bitset;

        constexpr reference(bitset& owner, const size_t pos) noexcept
            : m_bitset(&owner), m_pos(pos)
        {}

    public:

        constexpr reference(const reference&) noexcept = default;
        ~reference() noexcept = default;

        constexpr operator bool() const noexcept
        {
            return m_bitset->subscript(m_pos);
        }

        constexpr reference& operator=(const bool val) noexcept
        {
            m_bitset->set_unchecked(m_pos, val);
            return *this;
        }

        constexpr reference& operator=(const bool val) const noexcept
        {
            m_bitset->set_unchecked(m_pos, val);
            return *this;
        }

        constexpr reference& operator=(const reference& ref) noexcept
        {
            m_bitset->set_unchecked(m_pos, static_cast<bool>(ref));
            return *this;
        }

        constexpr bool operator~() const noexcept
        {
            return !m_bitset->subscript(m_pos);
        }

        constexpr reference& flip() noexcept
        {
            m_bitset->flip_unchecked(m_pos);
            return *this;
        }

        friend constexpr void swap(reference lhs, reference rhs) noexcept
        {
            bool val = lhs;
            lhs = rhs;
            rhs = val;
        }

        friend constexpr void swap(reference lhs, bool& rhs) noexcept
        {
            bool val = lhs;
            lhs = rhs;
            rhs = val;
        }

        friend constexpr void swap(bool& lhs, reference rhs) noexcept
        {
            swap(rhs, lhs);
        }

    private:

        bitset<N>* m_bitset;
        size_t m_pos; // position of element in bitset
    };

private:

    T m_array[words + 1];

public:

    //=========================================================================
    // constructors
    //=========================================================================

    constexpr bitset() noexcept
        : m_array{}
    {}

    constexpr bitset(unsigned long long val) noexcept
        : m_array{ static_cast<T>(need_mask ? val & mask : val) }
    {}

    //=========================================================================
    // element access
    //=========================================================================

private:

    constexpr bool subscript(size_t pos) const noexcept
    {
        return (m_array[pos / bits_per_word] & (T{ 1 } << pos % bits_per_word)) != 0;
    }

public:

    constexpr bool operator[](size_t pos) const noexcept
    {
        return subscript(pos);
    }

    constexpr reference operator[](size_t pos) noexcept
    {
        return reference{ *this, pos };
    }

    //=========================================================================

    constexpr bool text_unchecked(size_t pos) const noexcept
    {
        VX_ASSERT(pos < bits);
        return subscript(pos);
    }

    constexpr bool text_unchecked(size_t pos) const
    {
        VX_ASSERT(pos < bits);
        return subscript(pos);
    }

    constexpr expected<bool, error> test(size_t pos) const
    {
        VX_RET_UNEXPECTED_ERR_IF(pos >= N, err::out_of_range);
        return text_unchecked(pos);
    }

    //=========================================================================

    constexpr bool any() const noexcept
    {
        for (size_t i = 0; i <= words; ++i)
        {
            if (m_array[i] != 0)
            {
                return true;
            }
        }
        return false;
    }

    //=========================================================================

    constexpr bool all() const noexcept
    {
        VX_IF_CONSTEXPR (bits == 0)
        {
            return true;
        }
        else
        {
            constexpr bool no_padding = bits % bits_per_word == 0;
            for (size_t i = 0; i < words + no_padding; ++i)
            {
                if (m_array[i] != ~static_cast<T>(0))
                {
                    return false;
                }
            }
            return no_padding || m_array[words] == (static_cast<T>(1) << (bits % bits_per_word)) - 1;
        }
    }

    //=========================================================================

    constexpr bool none() const noexcept
    {
        return !any();
    }

    //=========================================================================

    constexpr size_t count() const noexcept
    {
        // todo
    }

    //=========================================================================
    // size
    //=========================================================================

    constexpr size_t size() const noexcept
    {
        return bits;
    }

    //=========================================================================
    // modifiers
    //=========================================================================

private:

    void trim() noexcept
    {
        constexpr bool need_trim = bits == 0 || bits % bits_per_word != 0;
        VX_IF_CONSTEXPR (need_trim)
        {
            m_array[words] &= native_mask;
        }
    }

public:

    constexpr void set() noexcept
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            for (size_t i = 0; i <= words; ++i)
            {
                m_array[i] = ~static_cast<T>(0);
            }
        }
        else
        {
            mem::set(&m_array, 0xFF, sizeof(m_array));
        }

        trim();
    }

    constexpr void set_unchecked(size_t pos, bool val = true)
    {
        VX_ASSERT(pos < bits);

        auto& selected_word = m_array[pos / bits_per_word];
        const auto bit = T{ 1 } << pos % bits_per_word;

        if (val)
        {
            selected_word |= bit;
        }
        else
        {
            selected_word &= ~bit;
        }
    }

    constexpr success set(size_t pos, bool val = true)
    {
        VX_RET_ERR_IF(pos >= bits, err::out_of_range);
        set_unchecked(pos, val);
        return success{};
    }

    //=========================================================================

    constexpr void reset() noexcept
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            for (size_t i = 0; i <= words; ++i)
            {
                m_array[i] = 0;
            }
        }
        else
        {
            mem::set(&m_array, 0, sizeof(m_array));
        }
    }

    constexpr success reset(size_t pos)
    {
        VX_RET_ERR_IF(pos >= bits, err::out_of_range);
        set_unchecked(pos, false);
        return success{};
    }

    //=========================================================================

    constexpr void flip() noexcept
    {
        for (size_t i = 0; i <= words; ++i)
        {
            m_array[i] = ~m_array[i];
        }
        trim();
    }

    constexpr void flip_unchecked(size_t pos)
    {
        VX_ASSERT(pos < bits);
        m_array[pos / bits_per_word] ^= T{ 1 } << pos % bits_per_word;
    }

    constexpr success flip(size_t pos)
    {
        VX_RET_ERR_IF(pos >= bits, err::out_of_range);
        flip_unchecked(pos);
        return success{};
    }

    //=========================================================================

    constexpr bitset& operator&=(const bitset& other) noexcept
    {
        for (size_t i = 0; i <= words; ++i)
        {
            m_array[i] &= other.m_array[i];
        }
        return *this;
    }

    //=========================================================================

    constexpr bitset& operator|=(const bitset& other) noexcept
    {
        for (size_t i = 0; i <= words; ++i)
        {
            m_array[i] |= other.m_array[i];
        }
        return *this;
    }

    //=========================================================================

    constexpr bitset& operator^=(const bitset& other) noexcept
    {
        for (size_t i = 0; i <= words; ++i)
        {
            m_array[i] ^= other.m_array[i];
        }
        return *this;
    }

    //=========================================================================

    constexpr bitset operator~() noexcept
    {
        bitset tmp = *this;
        tmp.flip();
        return tmp;
    }

    //=========================================================================

    constexpr bitset& operator<<=(size_t pos) noexcept
    {
        const auto word_shift = static_cast<ptrdiff_t>(pos / bits_per_word);
        if (word_shift != 0)
        {
            for (ptrdiff_t wpos = words; 0 <= wpos; --wpos)
            {
                m_array[wpos] = word_shift <= wpos ? m_array[wpos - word_shift] : 0;
            }
        }

        if ((pos %= bits_per_word) != 0)
        {
            for (ptrdiff_t wpos = words; 0 < wpos; --wpos)
            {
                m_array[wpos] = (m_array[wpos] << pos) | (m_array[wpos - 1] >> (bits_per_word - pos));
            }
            m_array[0] <<= pos;
        }

        trim();
        return *this;
    }

    constexpr bitset& operator>>=(size_t pos) noexcept
    {
        const auto word_shift = static_cast<ptrdiff_t>(pos / bits_per_word);
        if (word_shift != 0)
        {
            for (ptrdiff_t wpos = 0; wpos <= words; ++wpos)
            {
                m_array[wpos] = word_shift <= words - wpos ? m_array[wpos + word_shift] : 0;
            }
        }

        if ((pos %= bits_per_word) != 0)
        {
            for (ptrdiff_t wpos = 0; wpos < words; ++wpos)
            {
                m_array[wpos] = (m_array[wpos] >> pos) | (m_array[wpos + 1] << (bits_per_word - pos));
            }
            m_array[words] >>= pos;
        }

        return *this;
    }

    //=========================================================================

    constexpr bitset operator<<(size_t pos) const noexcept
    {
        bitset tmp = *this;
        tmp <<= pos;
        return tmp;
    }

    constexpr bitset operator>>(size_t pos) const noexcept
    {
        bitset tmp = *this;
        tmp >>= pos;
        return tmp;
    }

    //=========================================================================
    // comparison
    //=========================================================================

    constexpr bool operator==(const bitset& rhs) const noexcept
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            for (size_t i = 0; i <= words; ++i)
            {
                if (m_array[i] != rhs.m_array[i])
                {
                    return false;
                }
            }

            return true;
        }
        else
        {
            return mem::compare(&m_array[0], &rhs.m_array[0], sizeof(m_array)) == 0;
        }
    }

    constexpr bool operator!=(const bitset& rhs) const noexcept
    {
        return !operator==(rhs);
    }
};

//=========================================================================
// operators
//=========================================================================

template <size_t N>
constexpr bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
    bitset<N> res = lhs;
    res &= rhs;
    return res;
}

template <size_t N>
constexpr bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
    bitset<N> res = lhs;
    res |= rhs;
    return res;
}

template <size_t N>
constexpr bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
    bitset<N> res = lhs;
    res ^= rhs;
    return res;
}

} // namespace vx

//=========================================================================
// hashing
//=========================================================================

namespace vx {

template <size_t N>
struct hash<vx::bitset<N>>
{
    size_t operator()(const vx::bitset<N>& s) const noexcept
    {
        fnv1a fnv;
        fnv.update(reinterpret_cast<const unsigned char*>(s.m_array), count * sizeof(char_type));
        return fnv.result();
    }
};

} // namespace vx
