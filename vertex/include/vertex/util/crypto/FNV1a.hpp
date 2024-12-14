#pragma once

#include <vector>

#include "vertex/util/type_traits.hpp"

namespace vx {
namespace crypto {

// https://github.com/microsoft/STL/blob/7643c270e5bfb1cfad62f8b5ff4045c662bdaf81/stl/inc/type_traits#L2282

class FNV1a
{
public:

#if defined(_WIN64)
    static constexpr size_t offset_basis = 14695981039346656037ULL;
    static constexpr size_t prime = 1099511628211ULL;
#else
    static constexpr size_t offset_basis = 2166136261U;
    static constexpr size_t prime = 16777619U;
#endif // _WIN64

public:

    FNV1a() = default;

    void update(const uint8_t* data, const size_t size) noexcept
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
        update(&reinterpret_cast<const uint8_t&>(value), sizeof(T));
    }

    template <typename T, VX_REQUIRES(std::is_trivial<T>::value)>
    void update(const T* first, const T* last) noexcept
    {
        const auto first_byte = reinterpret_cast<const unsigned char*>(first);
        const auto last_byte = reinterpret_cast<const unsigned char*>(last);
        return update(first_byte, static_cast<size_t>(last_byte - first_byte));
    }

    size_t result() const noexcept { return m_buffer; }

private:

    size_t m_buffer = 0;
};

} // namespace vx
} // namespace crypto