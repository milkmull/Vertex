#pragma once

// Copyright (c) Stefan Gustavson 2011-04-19. All rights reserved.
// This code is released under the conditions of the MIT license.
// See LICENSE file for details.
// https://github.com/stegu/webgl-noise

// Based on the work of Stefan Gustavson and Ashima Arts:
// https://github.com/ashima/webgl-noise
// https://github.com/ashima/webgl-noise/blob/master/src/cellular2D.glsl
// https://github.com/ashima/webgl-noise/blob/master/src/cellular3D.glsl
// https://github.com/ashima/webgl-noise/blob/master/src/cellular2x2.glsl
// https://github.com/ashima/webgl-noise/blob/master/src/cellular2x2x2.glsl

#include "_priv/fn_noise.h"
#include "../../math/fn/vec_fn_exponential.h"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
/// @brief Generates cellular noise for 2D coordinates.
///
/// @tparam T The type of the coordinates.
/// @param P The 2D coordinates for which cellular noise is calculated.
/// 
/// @return The cellular noise values F1 and F2 for the given coordinates,
/// ranging from -1 to 1.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> cellular_noise(const vec<2, T>& P)
{
    const T K = static_cast<T>(0.142857142857); // 1/7
    const T Ko = static_cast<T>(0.428571428571); // 3/7
    const T jitter = static_cast<T>(1); // Less gives more regular pattern

    vec<2, T> Pi = _priv::mod289(floor(P));
    vec<2, T> Pf = fract(P);
    vec<3, T> oi(static_cast<T>(-1.0), static_cast<T>(0.0), static_cast<T>(1.0));
    vec<3, T> of(static_cast<T>(-0.5), static_cast<T>(0.5), static_cast<T>(1.5));
    vec<3, T> px = _priv::permute(Pi.x + oi);
    vec<3, T> p  = _priv::permute(px.x + Pi.y + oi); // p11, p12, p13
    vec<3, T> ox = fract(p * K) - Ko;
    vec<3, T> oy = _priv::mod7(floor(p * K)) * K - Ko;
    vec<3, T> dx = Pf.x + static_cast<T>(0.5) + jitter * ox;
    vec<3, T> dy = Pf.y - of + jitter * oy;
    vec<3, T> d1 = dx * dx + dy * dy; // d11, d12 and d13, squared
    p  = _priv::permute(px.y + Pi.y + oi); // p21, p22, p23
    ox = fract(p * K) - Ko;
    oy = _priv::mod7(floor(p * K)) * K - Ko;
    dx = Pf.x - static_cast<T>(0.5) + jitter * ox;
    dy = Pf.y - of + jitter * oy;
    vec<3, T> d2 = dx * dx + dy * dy; // d21, d22 and d23, squared
    p  = _priv::permute(px.z + Pi.y + oi); // p31, p32, p33
    ox = fract(p * K) - Ko;
    oy = _priv::mod7(floor(p * K)) * K - Ko;
    dx = Pf.x - static_cast<T>(1.5) + jitter * ox;
    dy = Pf.y - of + jitter * oy;
    vec<3, T> d3 = dx * dx + dy * dy; // d31, d32 and d33, squared
    // Sort out the two smallest distances (F1, F2)
    vec<3, T> d1a = min(d1, d2);
    d2 = max(d1,  d2); // Swap to keep candidates for F2
    d2 = min(d2,  d3); // neither F1 nor F2 are now in d3
    d1 = min(d1a, d2); // F1 is now in d1
    d2 = max(d1a, d2); // Swap to keep candidates for F2

    // d1.xy = (d1.x < d1.y) ? d1.xy : d1.yx; // Swap if smaller
    // d1.xz = (d1.x < d1.z) ? d1.xz : d1.zx; // F1 is in d1.x
    // d1.yz = min(d1.yz, d2.yz); // F2 is now not in d2.yz
    d1 = vec<3, T>(min(d1.x, d1.y), max(d1.x, d1.y), d1.z);
    d1 = vec<3, T>(min(d1.x, d1.z), d1.y, max(d1.x, d1.z));
    d1 = vec<3, T>(d1.x, min(d1.y, d2.y), min(d1.z, d2.z));
    d1.y = min(d1.y, d1.z); // nor in  d1.z
    d1.y = min(d1.y, d2.x); // F2 is in d1.y, we're done.
    return sqrt(vec<2, T>(d1.x, d1.y));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Generates cellular noise for 3D coordinates.
///
/// @tparam T The type of the coordinates.
/// @param P The 2D coordinates for which cellular noise is calculated.
/// 
/// @return The cellular noise values F1 and F2 for the given coordinates,
/// ranging from -1 to 1.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> cellular_noise(const vec<3, T>& P)
{
    const T K      = static_cast<T>(0.142857142857); // 1/7
    const T Ko     = static_cast<T>(0.428571428571); // 1/2-K/2
    const T K2     = static_cast<T>(0.020408163265306); // 1/(7*7)
    const T Kz     = static_cast<T>(0.166666666667); // 1/6
    const T Kzo    = static_cast<T>(0.416666666667); // 1/2-1/6*2
    const T jitter = static_cast<T>(1); // smaller jitter gives more regular pattern

    vec<3, T> Pi = _priv::mod289(floor(P));
    vec<3, T> Pf = fract(P) - static_cast<T>(0.5);

    vec<3, T> Pfx = Pf.x + vec<3, T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(-1));
    vec<3, T> Pfy = Pf.y + vec<3, T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(-1));
    vec<3, T> Pfz = Pf.z + vec<3, T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(-1));

    vec<3, T> p  = _priv::permute(Pi.x + vec<3, T>(static_cast<T>(-1), static_cast<T>(0), static_cast<T>(1)));
    vec<3, T> p1 = _priv::permute(p + Pi.y - static_cast<T>(1));
    vec<3, T> p2 = _priv::permute(p + Pi.y);
    vec<3, T> p3 = _priv::permute(p + Pi.y + static_cast<T>(1));

    vec<3, T> p11 = _priv::permute(p1 + Pi.z - static_cast<T>(1));
    vec<3, T> p12 = _priv::permute(p1 + Pi.z);
    vec<3, T> p13 = _priv::permute(p1 + Pi.z + static_cast<T>(1));

    vec<3, T> p21 = _priv::permute(p2 + Pi.z - static_cast<T>(1));
    vec<3, T> p22 = _priv::permute(p2 + Pi.z);
    vec<3, T> p23 = _priv::permute(p2 + Pi.z + static_cast<T>(1));

    vec<3, T> p31 = _priv::permute(p3 + Pi.z - static_cast<T>(1));
    vec<3, T> p32 = _priv::permute(p3 + Pi.z);
    vec<3, T> p33 = _priv::permute(p3 + Pi.z + static_cast<T>(1));

    vec<3, T> ox11 = fract(p11 * K) - Ko;
    vec<3, T> oy11 = _priv::mod7(floor(p11 * K)) * K - Ko;
    vec<3, T> oz11 = floor(p11 * K2) * Kz - Kzo; // p11 < 289 guaranteed

    vec<3, T> ox12 = fract(p12 * K) - Ko;
    vec<3, T> oy12 = _priv::mod7(floor(p12 * K)) * K - Ko;
    vec<3, T> oz12 = floor(p12 * K2) * Kz - Kzo;

    vec<3, T> ox13 = fract(p13 * K) - Ko;
    vec<3, T> oy13 = _priv::mod7(floor(p13 * K)) * K - Ko;
    vec<3, T> oz13 = floor(p13 * K2) * Kz - Kzo;

    vec<3, T> ox21 = fract(p21 * K) - Ko;
    vec<3, T> oy21 = _priv::mod7(floor(p21 * K)) * K - Ko;
    vec<3, T> oz21 = floor(p21 * K2) * Kz - Kzo;

    vec<3, T> ox22 = fract(p22 * K) - Ko;
    vec<3, T> oy22 = _priv::mod7(floor(p22 * K)) * K - Ko;
    vec<3, T> oz22 = floor(p22 * K2) * Kz - Kzo;

    vec<3, T> ox23 = fract(p23 * K) - Ko;
    vec<3, T> oy23 = _priv::mod7(floor(p23 * K)) * K - Ko;
    vec<3, T> oz23 = floor(p23 * K2) * Kz - Kzo;

    vec<3, T> ox31 = fract(p31 * K) - Ko;
    vec<3, T> oy31 = _priv::mod7(floor(p31 * K)) * K - Ko;
    vec<3, T> oz31 = floor(p31 * K2) * Kz - Kzo;

    vec<3, T> ox32 = fract(p32 * K) - Ko;
    vec<3, T> oy32 = _priv::mod7(floor(p32 * K)) * K - Ko;
    vec<3, T> oz32 = floor(p32 * K2) * Kz - Kzo;

    vec<3, T> ox33 = fract(p33 * K) - Ko;
    vec<3, T> oy33 = _priv::mod7(floor(p33 * K)) * K - Ko;
    vec<3, T> oz33 = floor(p33 * K2) * Kz - Kzo;

    vec<3, T> dx11 = Pfx   + jitter * ox11;
    vec<3, T> dy11 = Pfy.x + jitter * oy11;
    vec<3, T> dz11 = Pfz.x + jitter * oz11;

    vec<3, T> dx12 = Pfx   + jitter * ox12;
    vec<3, T> dy12 = Pfy.x + jitter * oy12;
    vec<3, T> dz12 = Pfz.y + jitter * oz12;

    vec<3, T> dx13 = Pfx   + jitter * ox13;
    vec<3, T> dy13 = Pfy.x + jitter * oy13;
    vec<3, T> dz13 = Pfz.z + jitter * oz13;

    vec<3, T> dx21 = Pfx   + jitter * ox21;
    vec<3, T> dy21 = Pfy.y + jitter * oy21;
    vec<3, T> dz21 = Pfz.x + jitter * oz21;

    vec<3, T> dx22 = Pfx   + jitter * ox22;
    vec<3, T> dy22 = Pfy.y + jitter * oy22;
    vec<3, T> dz22 = Pfz.y + jitter * oz22;

    vec<3, T> dx23 = Pfx   + jitter * ox23;
    vec<3, T> dy23 = Pfy.y + jitter * oy23;
    vec<3, T> dz23 = Pfz.z + jitter * oz23;

    vec<3, T> dx31 = Pfx   + jitter * ox31;
    vec<3, T> dy31 = Pfy.z + jitter * oy31;
    vec<3, T> dz31 = Pfz.x + jitter * oz31;

    vec<3, T> dx32 = Pfx   + jitter * ox32;
    vec<3, T> dy32 = Pfy.z + jitter * oy32;
    vec<3, T> dz32 = Pfz.y + jitter * oz32;

    vec<3, T> dx33 = Pfx   + jitter * ox33;
    vec<3, T> dy33 = Pfy.z + jitter * oy33;
    vec<3, T> dz33 = Pfz.z + jitter * oz33;

    vec<3, T> d11 = dx11 * dx11 + dy11 * dy11 + dz11 * dz11;
    vec<3, T> d12 = dx12 * dx12 + dy12 * dy12 + dz12 * dz12;
    vec<3, T> d13 = dx13 * dx13 + dy13 * dy13 + dz13 * dz13;
    vec<3, T> d21 = dx21 * dx21 + dy21 * dy21 + dz21 * dz21;
    vec<3, T> d22 = dx22 * dx22 + dy22 * dy22 + dz22 * dz22;
    vec<3, T> d23 = dx23 * dx23 + dy23 * dy23 + dz23 * dz23;
    vec<3, T> d31 = dx31 * dx31 + dy31 * dy31 + dz31 * dz31;
    vec<3, T> d32 = dx32 * dx32 + dy32 * dy32 + dz32 * dz32;
    vec<3, T> d33 = dx33 * dx33 + dy33 * dy33 + dz33 * dz33;

    // Sort out the two smallest distances (F1, F2)
#if 0
    // Cheat and sort out only F1
    vec<3, T> d1 = min(min(d11, d12), d13);
    vec<3, T> d2 = min(min(d21, d22), d23);
    vec<3, T> d3 = min(min(d31, d32), d33);
    vec<3, T> d = min(min(d1, d2), d3);
    d.x = min(min(d.x, d.y), d.z);
    return vec<2, T>(sqrt(d.x)); // F1 duplicated, no F2 computed
#else
    // Do it right and sort out both F1 and F2
    vec<3, T> d1a = min(d11, d12);
    d12 = max(d11, d12);
    d11 = min(d1a, d13); // Smallest now not in d12 or d13
    d13 = max(d1a, d13);
    d12 = min(d12, d13); // 2nd smallest now not in d13
    vec<3, T> d2a = min(d21, d22);
    d22 = max(d21, d22);
    d21 = min(d2a, d23); // Smallest now not in d22 or d23
    d23 = max(d2a, d23);
    d22 = min(d22, d23); // 2nd smallest now not in d23
    vec<3, T> d3a = min(d31, d32);
    d32 = max(d31, d32);
    d31 = min(d3a, d33); // Smallest now not in d32 or d33
    d33 = max(d3a, d33);
    d32 = min(d32, d33); // 2nd smallest now not in d33
    vec<3, T> da = min(d11, d21);
    d21 = max(d11, d21);
    d11 = min(da, d31); // Smallest now in d11
    d31 = max(da, d31); // 2nd smallest now not in d31

    //d11.xy = (d11.x < d11.y) ? d11.xy : d11.yx;
    //d11.xz = (d11.x < d11.z) ? d11.xz : d11.zx; // d11.x now smallest
    d11 = vec<3, T>(min(d11.x, d11.y), max(d11.x, d11.y), d11.z);
    d11 = vec<3, T>(min(d11.x, d11.z), d11.y, max(d11.x, d11.z));

    d12 = min(d12, d21); // 2nd smallest now not in d21
    d12 = min(d12, d22); // nor in d22
    d12 = min(d12, d31); // nor in d31
    d12 = min(d12, d32); // nor in d32

    //d11.yz = min(d11.yz, d12.xy); // nor in d12.yz
    d11 = vec<3, T>(d11.x, min(d11.y, d12.x), min(d11.z, d12.y));

    d11.y = min(d11.y, d12.z); // Only two more to go
    d11.y = min(d11.y, d11.z); // Done! (Phew!)
    return sqrt(vec<2, T>(d11.x, d11.y)); // F1, F2
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Generates cellular noise for 2D coordinates.
///
/// This version is sped up by using a smaller search window at the expense
/// of some strong artifacts. F2 is often wrong and has sharp discontinuities.
/// If you need a smooth F2, use the slower 3x3 version.
/// F1 is sometimes wrong, too, but OK for most purposes.
///
/// @tparam T The type of the coordinates.
/// @param P The 2D coordinates for which cellular noise is calculated.
/// 
/// @return The cellular noise values F1 and F2 for the given coordinates,
/// ranging from -1 to 1.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
vec<2, T> cellular_noise_2x2(const vec<2, T>& P)
{
    const T K      = static_cast<T>(0.142857142857); // 1/7
    const T K2     = static_cast<T>(0.0714285714285); // K/2
    const T jitter = static_cast<T>(0.8); // jitter 1.0 makes F1 wrong more often

    vec<2, T> Pi = _priv::mod289(floor(P));
    vec<2, T> Pf = fract(P);
    vec<4, T> Pfx = Pf.x + vec<4, T>(static_cast<T>(-0.5), static_cast<T>(-1.5), static_cast<T>(-0.5), static_cast<T>(-1.5));
    vec<4, T> Pfy = Pf.y + vec<4, T>(static_cast<T>(-0.5), static_cast<T>(-0.5), static_cast<T>(-1.5), static_cast<T>(-1.5));
    vec<4, T> p = _priv::permute(Pi.x     + vec<4, T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(1)));
              p = _priv::permute(p + Pi.y + vec<4, T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(1)));
    vec<4, T> ox = _priv::mod7(p) * K + K2;
    vec<4, T> oy = _priv::mod7(floor(p * K)) * K + K2;
    vec<4, T> dx = Pfx + jitter * ox;
    vec<4, T> dy = Pfy + jitter * oy;
    vec<4, T> d = dx * dx + dy * dy; // d11, d12, d21 and d22, squared
    // Sort out the two smallest distances
#if 0
    // Cheat and pick only F1
    // d.xy = min(d.xy, d.zw);
    d = vec<4, T>(min(d.x, d.z), min(d.y, d.w), d.z, d.w);
    d.x = min(d.x, d.y);
    return vec<2, T>(sqrt(d.x)); // F1 duplicated, F2 not computed
#else
    // Do it right and find both F1 and F2
    // d.xy = (d.x < d.y) ? d.xy : d.yx; // Swap if smaller
    // d.xz = (d.x < d.z) ? d.xz : d.zx;
    // d.xw = (d.x < d.w) ? d.xw : d.wx;
    d = vec<4, T>(min(d.x, d.y), max(d.x, d.y), d.z, d.w);
    d = vec<4, T>(min(d.x, d.z), d.y, max(d.x, d.z), d.w);
    d = vec<4, T>(min(d.x, d.w), d.y, d.z, max(d.x, d.w));

    d.y = min(d.y, d.z);
    d.y = min(d.y, d.w);
    return sqrt(vec<2, T>(d.x, d.y));
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Generates cellular noise for 3D coordinates.
///
/// This version is sped up by using a smaller search window at the expense
/// of some strong artifacts. F2 is often wrong and has sharp discontinuities.
/// If you need a smooth F2, use the slower 3x3 version.
/// F1 is sometimes wrong, too, but OK for most purposes.
///
/// @tparam T The type of the coordinates.
/// @param P The 3D coordinates for which cellular noise is calculated.
/// 
/// @return The cellular noise values F1 and F2 for the given coordinates,
/// ranging from -1 to 1.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
vec<2, T> cellular_noise_2x2x2(const vec<3, T>& P)
{
    const T K  = static_cast<T>(0.142857142857); // 1/7
    const T Ko = static_cast<T>(0.428571428571); // 1/2-K/2
    const T K2 = static_cast<T>(0.020408163265306); // 1/(7*7)
    const T Kz = static_cast<T>(0.166666666667); // 1/6
    const T Kzo = static_cast<T>(0.416666666667); // 1/2-1/6*2
    const T jitter = static_cast<T>(0.8); // smaller jitter gives less errors in F2

    vec<3, T> Pi = _priv::mod289(floor(P));
    vec<3, T> Pf = fract(P);
    vec<4, T> Pfx = Pf.x + vec<4, T>(static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0),  static_cast<T>(-1));
    vec<4, T> Pfy = Pf.y + vec<4, T>(static_cast<T>(0), static_cast<T>(0),  static_cast<T>(-1), static_cast<T>(-1));
    vec<4, T> p = _priv::permute(Pi.x + vec<4, T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(1)));
    p = _priv::permute(p + Pi.y + vec<4, T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(1)));
    vec<4, T> p1 = _priv::permute(p + Pi.z); // z+0
    vec<4, T> p2 = _priv::permute(p + Pi.z + static_cast<T>(1)); // z+1
    vec<4, T> ox1 = fract(p1 * K) - Ko;
    vec<4, T> oy1 = _priv::mod7(floor(p1 * K)) * K - Ko;
    vec<4, T> oz1 = floor(p1 * K2) * Kz - Kzo; // p1 < 289 guaranteed
    vec<4, T> ox2 = fract(p2 * K) - Ko;
    vec<4, T> oy2 = _priv::mod7(floor(p2 * K)) * K - Ko;
    vec<4, T> oz2 = floor(p2 * K2) * Kz - Kzo;
    vec<4, T> dx1 = Pfx                      + jitter * ox1;
    vec<4, T> dy1 = Pfy                      + jitter * oy1;
    vec<4, T> dz1 = Pf.z                     + jitter * oz1;
    vec<4, T> dx2 = Pfx                      + jitter * ox2;
    vec<4, T> dy2 = Pfy                      + jitter * oy2;
    vec<4, T> dz2 = Pf.z - static_cast<T>(1) + jitter * oz2;
    vec<4, T> d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1; // z+0
    vec<4, T> d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2; // z+1

    // Sort out the two smallest distances (F1, F2)
#if 0
    // Cheat and sort out only F1
    d1 = min(d1, d2);
    // d1.xy = min(d1.xy, d1.wz);
    d1 = vec<4, T>(min(d1.x, d1.w), min(d1.y, d1.z), d1.z, d1.w);
    d1.x = min(d1.x, d1.y);
    return vec<2, T>(sqrt(d1.x));
#else
    // Do it right and sort out both F1 and F2
    vec<4, T> d = min(d1, d2); // F1 is now in d
    d2 = max(d1, d2); // Make sure we keep all candidates for F2

    // d.xy = (d.x < d.y) ? d.xy : d.yx; // Swap smallest to d.x
    // d.xz = (d.x < d.z) ? d.xz : d.zx;
    // d.xw = (d.x < d.w) ? d.xw : d.wx; // F1 is now in d.x
    // d.yzw = min(d.yzw, d2.yzw); // F2 now not in d2.yzw
    d = vec<4, T>(min(d.x, d.y), max(d.x, d.y), d.z, d.w);
    d = vec<4, T>(min(d.x, d.z), d.y, max(d.x, d.z), d.w);
    d = vec<4, T>(min(d.x, d.w), d.y, d.z, max(d.x, d.w));
    d = vec<4, T>(d.x, min(d.y, d2.y), min(d.z, d2.z), min(d.w, d2.w));

    d.y = min(d.y, d.z); // nor in d.z
    d.y = min(d.y, d.w); // nor in d.w
    d.y = min(d.y, d2.x); // F2 is now in d.y
    return sqrt(vec<2, T>(d.x, d.y)); // F1 and F2
#endif
}

}
}