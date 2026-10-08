#pragma once

#include <climits>
#include <cstddef>
#include <type_traits>

#include "vertex/config/assert.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/std/crypto/fnv1a.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/bit.hpp"

namespace vx {

template <typename T>
struct hash;

template <size_t N>
class bitset
{

    //=========================================================================
    // member types
    //=========================================================================

public:

    static constexpr size_t bits = N;
    using word_type = typename std::conditional<bits <= sizeof(unsigned long) * CHAR_BIT, unsigned long, unsigned long long>::type;

    // public values are signed
    static constexpr ptrdiff_t bits_per_word = static_cast<ptrdiff_t>(sizeof(word_type) * CHAR_BIT);
    static constexpr ptrdiff_t word_count = (bits == 0) ? 0 : static_cast<ptrdiff_t>((bits - 1) / (sizeof(word_type) * CHAR_BIT)); // index of the last word (number of words - 1)

private:

    friend hash<bitset<bits>>;

    // unsigned twins for internal bit arithmetic
    static constexpr size_t ubits_per_word = static_cast<size_t>(bits_per_word);
    static constexpr size_t uword_count = static_cast<size_t>(word_count);

    // mask for the last word (padding bits)
    static constexpr bool need_mask = (bits != 0) && (bits % ubits_per_word != 0);
    static constexpr unsigned long long mask = (1ull << (need_mask ? bits % ubits_per_word : 0)) - 1ull;
    static constexpr word_type native_mask = static_cast<word_type>(mask);

    // mask for the value constructor (only relevant when everything fits in one ull)
    static constexpr bool need_ctor_mask = bits < sizeof(unsigned long long) * CHAR_BIT;
    static constexpr unsigned long long ctor_mask = (1ull << (need_ctor_mask ? bits : 0)) - 1ull;

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

        constexpr const reference& operator=(const bool val) const noexcept
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

        bitset<bits>* m_bitset;
        size_t m_pos; // position of element in bitset
    };

private:

    word_type m_array[uword_count + 1];

public:

    //=========================================================================
    // constructors
    //=========================================================================

    constexpr bitset() noexcept
        : m_array{}
    {}

    constexpr bitset(unsigned long long val) noexcept
        : m_array{ static_cast<word_type>(need_ctor_mask ? val & ctor_mask : val) }
    {}

    //=========================================================================
    // element access
    //=========================================================================

private:

    constexpr bool subscript(size_t pos) const noexcept
    {
        return (m_array[pos / ubits_per_word] & (word_type{ 1 } << pos % ubits_per_word)) != 0;
    }

public:

    constexpr bool operator[](size_t pos) const noexcept
    {
        VX_ASSERT(pos < bits);
        return subscript(pos);
    }

    constexpr reference operator[](size_t pos) noexcept
    {
        VX_ASSERT(pos < bits);
        return reference{ *this, pos };
    }

    //=========================================================================

    constexpr word_type* data() noexcept
    {
        return m_array;
    }

    constexpr const word_type* data() const noexcept
    {
        return m_array;
    }

    //=========================================================================

    constexpr bool test_unchecked(size_t pos) const noexcept
    {
        VX_ASSERT(pos < bits);
        return subscript(pos);
    }

    constexpr expected<bool, error> test(size_t pos) const
    {
        VX_RET_UNEXPECTED_ERR_IF(pos >= bits, err::out_of_range);
        return test_unchecked(pos);
    }

    //=========================================================================

    constexpr bool any() const noexcept
    {
        for (ptrdiff_t i = 0; i <= word_count; ++i)
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
            constexpr bool no_padding = bits % ubits_per_word == 0;
            for (ptrdiff_t i = 0; i < word_count + no_padding; ++i)
            {
                if (m_array[i] != ~static_cast<word_type>(0))
                {
                    return false;
                }
            }
            return no_padding || m_array[word_count] == (static_cast<word_type>(1) << (bits % ubits_per_word)) - 1;
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
        size_t n = 0;
        for (ptrdiff_t i = 0; i <= word_count; ++i)
        {
            n += bit::popcount(m_array[i]);
        }
        return n;
    }

    //=========================================================================
    // conversions
    //=========================================================================

    // Converts to any unsigned integer type. Returns err::overflow if a set bit
    // does not fit in T. Relies on the padding bits being zero (see trim()).
    template <typename T, VX_REQUIRES(std::is_unsigned<T>::value && !std::is_same<T, bool>::value)>
    constexpr expected<T, error> to_integer() const
    {
        constexpr size_t t_bits = sizeof(T) * CHAR_BIT;

        T result = 0;
        for (ptrdiff_t i = 0; i <= word_count; ++i)
        {
            const word_type word = m_array[i];
            if (word == 0)
            {
                continue;
            }

            // first bit position this word contributes to
            const size_t shift = static_cast<size_t>(i) * ubits_per_word;

            // word starts entirely beyond T
            VX_RET_UNEXPECTED_ERR_IF(shift >= t_bits, err::out_of_range);

            // word straddles the top of T: the bits that don't fit must be zero
            if (shift + ubits_per_word > t_bits)
            {
                VX_RET_UNEXPECTED_ERR_IF((word >> (t_bits - shift)) != 0, err::out_of_range);
            }

            result |= static_cast<T>(static_cast<T>(word) << shift);
        }

        return result;
    }

    // Truncating conversion: returns the low min(bits, bits of T) bits, never fails
    template <typename T, VX_REQUIRES(std::is_unsigned<T>::value && !std::is_same<T, bool>::value)>
    constexpr T to_integer_unchecked() const noexcept
    {
        constexpr size_t t_bits = sizeof(T) * CHAR_BIT;

        // words needed to cover T, capped at the words the bitset actually has
        constexpr size_t t_words = (t_bits + ubits_per_word - 1) / ubits_per_word;
        constexpr ptrdiff_t words_used = static_cast<ptrdiff_t>(t_words < uword_count + 1 ? t_words : uword_count + 1);

        T result = 0;
        for (ptrdiff_t i = 0; i < words_used; ++i)
        {
            // the cast truncates a wide word (T narrower than the word)
            result |= static_cast<T>(static_cast<T>(m_array[i]) << (static_cast<size_t>(i) * ubits_per_word));
        }

        return result;
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

    constexpr void trim() noexcept
    {
        constexpr bool need_trim = bits == 0 || bits % ubits_per_word != 0;
        VX_IF_CONSTEXPR (need_trim)
        {
            m_array[word_count] &= native_mask;
        }
    }

public:

    constexpr void set() noexcept
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            for (ptrdiff_t i = 0; i <= word_count; ++i)
            {
                m_array[i] = ~static_cast<word_type>(0);
            }
        }
        else
        {
            mem::set(m_array, 0xFF, sizeof(m_array));
        }

        trim();
    }

    constexpr void set_unchecked(size_t pos, bool val = true) noexcept
    {
        VX_ASSERT(pos < bits);

        auto& selected_word = m_array[pos / ubits_per_word];
        const auto bit = word_type{ 1 } << pos % ubits_per_word;

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
            for (ptrdiff_t i = 0; i <= word_count; ++i)
            {
                m_array[i] = 0;
            }
        }
        else
        {
            mem::set(m_array, 0, sizeof(m_array));
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
        for (ptrdiff_t i = 0; i <= word_count; ++i)
        {
            m_array[i] = ~m_array[i];
        }
        trim();
    }

    constexpr void flip_unchecked(size_t pos) noexcept
    {
        VX_ASSERT(pos < bits);
        m_array[pos / ubits_per_word] ^= word_type{ 1 } << pos % ubits_per_word;
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
        for (ptrdiff_t i = 0; i <= word_count; ++i)
        {
            m_array[i] &= other.m_array[i];
        }
        return *this;
    }

    //=========================================================================

    constexpr bitset& operator|=(const bitset& other) noexcept
    {
        for (ptrdiff_t i = 0; i <= word_count; ++i)
        {
            m_array[i] |= other.m_array[i];
        }
        return *this;
    }

    //=========================================================================

    constexpr bitset& operator^=(const bitset& other) noexcept
    {
        for (ptrdiff_t i = 0; i <= word_count; ++i)
        {
            m_array[i] ^= other.m_array[i];
        }
        return *this;
    }

    //=========================================================================

    constexpr bitset operator~() const noexcept
    {
        bitset tmp = *this;
        tmp.flip();
        return tmp;
    }

    //=========================================================================

    constexpr bitset& operator<<=(size_t pos) noexcept
    {
        const auto word_shift = static_cast<ptrdiff_t>(pos / ubits_per_word);
        if (word_shift != 0)
        {
            for (ptrdiff_t i = word_count; 0 <= i; --i)
            {
                m_array[i] = word_shift <= i ? m_array[i - word_shift] : 0;
            }
        }

        if ((pos %= ubits_per_word) != 0)
        {
            for (ptrdiff_t i = word_count; 0 < i; --i)
            {
                m_array[i] = (m_array[i] << pos) | (m_array[i - 1] >> (ubits_per_word - pos));
            }
            m_array[0] <<= pos;
        }

        trim();
        return *this;
    }

    constexpr bitset& operator>>=(size_t pos) noexcept
    {
        const auto word_shift = static_cast<ptrdiff_t>(pos / ubits_per_word);
        if (word_shift != 0)
        {
            for (ptrdiff_t i = 0; i <= word_count; ++i)
            {
                m_array[i] = word_shift <= word_count - i ? m_array[i + word_shift] : 0;
            }
        }

        if ((pos %= ubits_per_word) != 0)
        {
            for (ptrdiff_t i = 0; i < word_count; ++i)
            {
                m_array[i] = (m_array[i] >> pos) | (m_array[i + 1] << (ubits_per_word - pos));
            }
            m_array[word_count] >>= pos;
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
            for (ptrdiff_t i = 0; i <= word_count; ++i)
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
        fnv.update(s.m_array, s.m_array + s.word_count + 1);
        return fnv.result();
    }
};

} // namespace vx
