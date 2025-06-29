#pragma once

#include "./types.hpp"
#include "vertex/config/language_config.hpp"
#include "../core/types.hpp"

namespace vx {
namespace math {
namespace simd {

#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)

///////////////////////////////////////////////////////////////////////////////
// transpose
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// determinant
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// inverse
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// componant multiplication
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// outer product
///////////////////////////////////////////////////////////////////////////////

#endif // VX_SIMD_X86_SSE2_VERSION

} // namespace simd
} // namespace math
} // namespace vx