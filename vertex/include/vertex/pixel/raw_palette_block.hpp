#pragma once

#include "vertex/pixel/pixel_format.hpp"
#include "vertex/pixel/palette.hpp"

namespace vx {
namespace pixel {

///////////////////////////////////////////////////////////////////////////////
// base
///////////////////////////////////////////////////////////////////////////////

template <palette_format f> struct raw_palette_block;

#define _STATIC_CHECK_SIZE(F, FS)         VX_STATIC_ASSERT(sizeof(raw_palette_block<F>) == sizeof(typename raw_palette_block<F>::block_type), "invalid block size for format " FS)
#define _STATIC_CHECK_ALIGNMENT(F, FS)    VX_STATIC_ASSERT(alignof(raw_palette_block<F>) == alignof(typename raw_palette_block<F>::block_type), "invalid block alignment for format " FS)
#define _STATIC_FORMAT_CHECK(F)           _STATIC_CHECK_SIZE(palette_format::F, #F); _STATIC_CHECK_ALIGNMENT(palette_format::F, #F)

#define _DECODE_BLOCK_LSB(si) (((data) >> ((si) * bpsi)) & ((1u << bpsi) - 1))
#define _ENCODE_BLOCK_LSB(si, i) (((data) & ~(((1u << (bpsi)) - 1) << ((si) * bpsi))) | (((i) & ((1u << (bpsi)) - 1)) << ((si) * bpsi)))

#define _DECODE_BLOCK_MSB(si) (((data) >> (sizeof(block_type) * 8 - (si) - (bpsi)))
#define _ENCODE_BLOCK_MSB(si, i) (((data) & ~(((1u << (bpsi)) - 1) << (sizeof(block_type) * 8 - (si) - (bpsi)))) | (((i) & ((1u << (bpsi)) - 1)) << (sizeof(block_type) * 8 - (si) - (bpsi)))))

///////////////////////////////////////////////////////////////////////////////
// 1 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_palette_block<palette_format::INDEX_1LSB>
{
    using block_type = uint8_t;
    static constexpr palette_format format = palette_format::INDEX_1LSB;

    static constexpr size_t bpb = get_bits_per_block(format);
    static constexpr size_t bpsi = get_bits_per_sub_index(format);

    block_type data;

    VX_FORCE_INLINE constexpr raw_palette_block() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_palette_block(block_type i) noexcept : data{ i } {}

    VX_FORCE_INLINE constexpr operator block_type() const noexcept
    {
        return data;
    }

    VX_FORCE_INLINE constexpr block_type get_index(size_t sub_index) const noexcept
    {
        return _DECODE_BLOCK_LSB(sub_index);
    }

    VX_FORCE_INLINE constexpr void set_index(size_t sub_index, block_type value) noexcept
    {
        data = _ENCODE_BLOCK_LSB(sub_index, value);
    }
};

_STATIC_FORMAT_CHECK(INDEX_1LSB);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_palette_block<palette_format::INDEX_1MSB>
{
    using block_type = uint8_t;
    static constexpr palette_format format = palette_format::INDEX_1MSB;

    static constexpr size_t bpb = get_bits_per_block(format);
    static constexpr size_t bpsi = get_bits_per_sub_index(format);

    block_type data;

    VX_FORCE_INLINE constexpr raw_palette_block() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_palette_block(block_type i) noexcept : data{ i } {}

    VX_FORCE_INLINE constexpr operator block_type() const noexcept
    {
        return data;
    }

    VX_FORCE_INLINE constexpr block_type get_index(uint8_t sub_index) const noexcept
    {
        return _DECODE_BLOCK_LSB(sub_index);
    }

    VX_FORCE_INLINE constexpr void set_index(uint8_t sub_index, block_type value) noexcept
    {
        data = _ENCODE_BLOCK_LSB(sub_index, value);
    }
};

_STATIC_FORMAT_CHECK(INDEX_1MSB);

///////////////////////////////////////////////////////////////////////////////
// 2 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_palette_block<palette_format::INDEX_2LSB>
{
    using block_type = uint8_t;
    static constexpr palette_format format = palette_format::INDEX_2LSB;

    static constexpr size_t bpb = get_bits_per_block(format);
    static constexpr size_t bpsi = get_bits_per_sub_index(format);

    block_type data;

    VX_FORCE_INLINE constexpr raw_palette_block() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_palette_block(block_type i) noexcept : data{ i } {}

    VX_FORCE_INLINE constexpr operator block_type() const noexcept
    {
        return data;
    }

    VX_FORCE_INLINE constexpr block_type get_index(size_t sub_index) const noexcept
    {
        return _DECODE_BLOCK_LSB(sub_index);
    }

    VX_FORCE_INLINE constexpr void set_index(size_t sub_index, block_type value) noexcept
    {
        data = _ENCODE_BLOCK_LSB(sub_index, value);
    }
};

_STATIC_FORMAT_CHECK(INDEX_2LSB);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_palette_block<palette_format::INDEX_2MSB>
{
    using block_type = uint8_t;
    static constexpr palette_format format = palette_format::INDEX_2MSB;

    static constexpr size_t bpb = get_bits_per_block(format);
    static constexpr size_t bpsi = get_bits_per_sub_index(format);

    block_type data;

    VX_FORCE_INLINE constexpr raw_palette_block() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_palette_block(block_type i) noexcept : data{ i } {}

    VX_FORCE_INLINE constexpr operator block_type() const noexcept
    {
        return data;
    }

    VX_FORCE_INLINE constexpr block_type get_index(size_t sub_index) const noexcept
    {
        return _DECODE_BLOCK_LSB(sub_index);
    }

    VX_FORCE_INLINE constexpr void set_index(size_t sub_index, block_type value) noexcept
    {
        data = _ENCODE_BLOCK_LSB(sub_index, value);
    }
};

_STATIC_FORMAT_CHECK(INDEX_2MSB);

///////////////////////////////////////////////////////////////////////////////
// 4 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_palette_block<palette_format::INDEX_4LSB>
{
    using block_type = uint8_t;
    static constexpr palette_format format = palette_format::INDEX_4LSB;

    static constexpr size_t bpb = get_bits_per_block(format);
    static constexpr size_t bpsi = get_bits_per_sub_index(format);

    block_type data;

    VX_FORCE_INLINE constexpr raw_palette_block() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_palette_block(block_type i) noexcept : data{ i } {}

    VX_FORCE_INLINE constexpr operator block_type() const noexcept
    {
        return data;
    }

    VX_FORCE_INLINE constexpr block_type get_index(size_t sub_index) const noexcept
    {
        return _DECODE_BLOCK_LSB(sub_index);
    }

    VX_FORCE_INLINE constexpr void set_index(size_t sub_index, block_type value) noexcept
    {
        data = _ENCODE_BLOCK_LSB(sub_index, value);
    }
};

_STATIC_FORMAT_CHECK(INDEX_4LSB);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_palette_block<palette_format::INDEX_4MSB>
{
    using block_type = uint8_t;
    static constexpr palette_format format = palette_format::INDEX_4MSB;

    static constexpr size_t bpb = get_bits_per_block(format);
    static constexpr size_t bpsi = get_bits_per_sub_index(format);

    block_type data;

    VX_FORCE_INLINE constexpr raw_palette_block() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_palette_block(block_type i) noexcept : data{ i } {}

    VX_FORCE_INLINE constexpr operator block_type() const noexcept
    {
        return data;
    }

    VX_FORCE_INLINE constexpr block_type get_index(size_t sub_index) const noexcept
    {
        return _DECODE_BLOCK_LSB(sub_index);
    }

    VX_FORCE_INLINE constexpr void set_index(size_t sub_index, block_type value) noexcept
    {
        data = _ENCODE_BLOCK_LSB(sub_index, value);
    }
};

_STATIC_FORMAT_CHECK(INDEX_4MSB);

///////////////////////////////////////////////////////////////////////////////
// 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_palette_block<palette_format::INDEX_8>
{
    using block_type = uint8_t;
    static constexpr palette_format format = palette_format::INDEX_8;

    static constexpr size_t bpb = get_bits_per_block(format);
    static constexpr size_t bpsi = get_bits_per_sub_index(format);

    block_type data;

    VX_FORCE_INLINE constexpr raw_palette_block() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_palette_block(block_type i) noexcept : data{ i } {}

    VX_FORCE_INLINE constexpr operator block_type() const noexcept
    {
        return data;
    }

    VX_FORCE_INLINE constexpr uint8_t get_index(size_t) const noexcept
    {
        return data;
    }

    VX_FORCE_INLINE constexpr void set_index(size_t, block_type value) noexcept
    {
        data = static_cast<block_type>(value);
    }
};

_STATIC_FORMAT_CHECK(INDEX_8);

///////////////////////////////////////////////////////////////////////////////

template <palette_format F>
class palette_pixel_proxy
{
public:

    using block_type = raw_palette_block<F>;
    using color_type = typename palette::color_type;

    palette_pixel_proxy(block_type& block, size_t sub_index, const palette* pal) noexcept
        : m_block(&block)
        , m_sub_index(sub_index)
        , m_palette(pal) {}

    // ------------------------------------------------------------
    // assignment & conversion
    // ------------------------------------------------------------

    VX_FORCE_INLINE palette_pixel_proxy& operator=(size_t i) noexcept
    {
        m_block->set_index(m_sub_index, static_cast<typename block_type::block_type>(i));
        return *this;
    }

    VX_FORCE_INLINE palette_pixel_proxy& operator=(const color_type& c) noexcept
    {
        size_t index = 0;
        m_palette->find_closest(c, &index);
        m_block->set_index(m_sub_index, static_cast<typename block_type::block_type>(index));
        return *this;
    }

    VX_FORCE_INLINE operator size_t() const noexcept
    {
        return m_block->get_index(m_sub_index);
    }

    VX_FORCE_INLINE operator color_type() const noexcept
    {
        const size_t i = m_block->get_index(m_sub_index);
        return (*m_palette)[i];
    }

    // ------------------------------------------------------------
    // accessors
    // ------------------------------------------------------------

    VX_FORCE_INLINE size_t index() const noexcept
    {
        return m_block->get_index(m_sub_index);
    }

    VX_FORCE_INLINE color_type color() const noexcept
    {
        return static_cast<color_type>(*this);
    }

private:

    block_type* m_block;
    size_t m_sub_index;
    const palette* m_palette;
};

///////////////////////////////////////////////////////////////////////////////

#undef _STATIC_CHECK_SIZE
#undef _STATIC_CHECK_ALIGNMENT
#undef _STATIC_FORMAT_CHECK

#undef _DECODE_BLOCK_LSB
#undef _DECODE_BLOCK_MSB

#undef _ENCODE_BLOCK_LSB
#undef _ENCODE_BLOCK_MSB

} // namespace pixel
} // namespace vx