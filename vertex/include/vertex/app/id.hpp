#pragma once

#include "vertex/os/atomic.hpp"

namespace vx {
namespace app {

using id_type = size_t;

enum : id_type
{
    INVALID_ID = 0,
    DEFAULT_ID = 1
};

inline constexpr bool is_valid_id(id_type id) noexcept
{
    return id != INVALID_ID;
}

class id_generator
{
public:

    id_generator() = default;
    ~id_generator() = default;

    id_type next() { return ++m_id; }
    void reset() { m_id = INVALID_ID; }

private:

    os::atomic<id_type> m_id = 0;
};

} // namespace app
} // namespace vx