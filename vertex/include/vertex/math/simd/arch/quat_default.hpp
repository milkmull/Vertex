#pragma once

namespace vx {
namespace math {
namespace simd {

template <typename T>
struct quat_t
{
    using scalar_type = T;
    using data_type = scalar_type[4];

    static constexpr size_t calulate_alignment() noexcept
    {
        return alignof(data_type);
    }

    static constexpr int HAVE_ADD = 0;
    static constexpr int HAVE_SUB = 0;
    static constexpr int HAVE_MUL = 0;
    static constexpr int HAVE_DIV = 0;
    static constexpr int HAVE_DIV_LOWP = 0;

    static constexpr int HAVE_MUL_VEC = 0;
};

///////////////////////////////////////////////////////////////////////////////
// matrix types
///////////////////////////////////////////////////////////////////////////////

// float
using quatf = quat_t<f32>;

// generic
using quat = quatf;

} // namespace simd
} // namespace math
} // namespace vx