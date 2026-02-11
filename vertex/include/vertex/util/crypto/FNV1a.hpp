#pragma once

#include <cstdint>

#include "vertex/config/architecture.hpp"
#include "vertex/config/type_traits.hpp"

namespace vx {
namespace crypto {

// https://github.com/microsoft/STL/blob/7643c270e5bfb1cfad62f8b5ff4045c662bdaf81/stl/inc/type_traits#L2282

class fnv1a
{
public:

#if defined(VX_ARCH_WORD_BITS_64)
    static constexpr size_t offset_basis = 14695981039346656037ULL;
    static constexpr size_t prime = 1099511628211ULL;
#else
    static constexpr size_t offset_basis = 2166136261U;
    static constexpr size_t prime = 16777619U;
#endif // defined(VX_ARCH_WORD_BITS_64)

public:

    fnv1a() noexcept = default;

    void update(const unsigned char* const data, const size_t size) noexcept
    {
        for (size_t i = 0; i < size; ++i)
        {
            m_buffer ^= static_cast<size_t>(data[i]);
            m_buffer *= prime;
        }
    }

    template <typename T, VX_REQUIRES(std::is_trivial<T>::value)>
    void update(const T& value) noexcept
    {
        update(reinterpret_cast<const unsigned char* const>(&value), sizeof(T));
    }

    template <typename T, VX_REQUIRES(std::is_trivial<T>::value)>
    void update(const T* const first, const T* const last) noexcept
    {
        const auto first_byte = reinterpret_cast<const unsigned char* const>(first);
        const auto last_byte = reinterpret_cast<const unsigned char* const>(last);
        return update(first_byte, static_cast<size_t>(last_byte - first_byte));
    }

    size_t result() const noexcept { return m_buffer; }

private:

    size_t m_buffer = 0;
};

} // namespace vx
} // namespace crypto
