#pragma once

#include <cstddef>
#include <limits>

#include "vertex/os/atomic.hpp"

namespace vx {
namespace app {

using id_type = size_t;

enum : id_type
{
    invalid_id = 0,
    default_id = 1,
    max_id = std::numeric_limits<id_type>::max()
};

inline constexpr bool is_valid_id(id_type id) noexcept
{
    return id != invalid_id;
}

class id_generator
{
public:

    id_type next() { return ++m_id; }
    void reset() { m_id = invalid_id; }

    id_type operator()() { return next(); }

private:

    os::atomic<id_type> m_id = 0;
};

} // namespace app
} // namespace vx