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

#include "./noise_helpers.hpp"

namespace vx {
namespace math {

////////////////////////////////////////////////////////////////////////////////
// 2D
////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> cellular_noise(const vec<2, T>& P) noexcept
{
    constexpr T K = static_cast<T>(0.142857142857); // 1/7
    constexpr T Ko = static_cast<T>(0.428571428571); // 3/7
    constexpr T jitter = static_cast<T>(1); // Less gives more regular pattern

    const vec<2, T> Pi = __detail::mod289(floor(P));
    const vec<2, T> Pf = fract(P);
    const vec<3, T> oi(static_cast<T>(-1.0), static_cast<T>(0.0), static_cast<T>(1.0));
    const vec<3, T> of(static_cast<T>(-0.5), static_cast<T>(0.5), static_cast<T>(1.5));
    const vec<3, T> px = __detail::permute(Pi.x + oi);

    vec<3, T> p = __detail::permute(px.x + Pi.y + oi);
    vec<3, T> ox = fract(p * K) - Ko;
    vec<3, T> oy = __detail::mod7(floor(p * K)) * K - Ko;
    vec<3, T> dx = Pf.x + static_cast<T>(0.5) + jitter * ox;
    vec<3, T> dy = Pf.y - of + jitter * oy;
    const vec<3, T> d1 = dx * dx + dy * dy;

    p = __detail::permute(px.y + Pi.y + oi);
    ox = fract(p * K) - Ko;
    oy = __detail::mod7(floor(p * K)) * K - Ko;
    dx = Pf.x - static_cast<T>(0.5) + jitter * ox;
    dy = Pf.y - of + jitter * oy;
    const vec<3, T> d2 = dx * dx + dy * dy;

    p = __detail::permute(px.z + Pi.y + oi);
    ox = fract(p * K) - Ko;
    oy = __detail::mod7(floor(p * K)) * K - Ko;
    dx = Pf.x - static_cast<T>(1.5) + jitter * ox;
    dy = Pf.y - of + jitter * oy;
    const vec<3, T> d3 = dx * dx + dy * dy;

    // Sort two smallest distances (F1, F2)
    const vec<3, T> d1a = min(d1, d2);
    vec<3, T> d2b = max(d1, d2); // temporary for sorting
    d2b = min(d2b, d3);
    vec<3, T> d1b = min(d1a, d2b);
    d2b = max(d1a, d2b);

    d1b = vec<3, T>(min(d1b.x, d1b.y), max(d1b.x, d1b.y), d1b.z);
    d1b = vec<3, T>(min(d1b.x, d1b.z), d1b.y, max(d1b.x, d1b.z));
    d1b = vec<3, T>(d1b.x, min(d1b.y, d2b.y), min(d1b.z, d2b.z));
    d1b.y = min(d1b.y, d1b.z);
    d1b.y = min(d1b.y, d2b.x);

    return sqrt(vec<2, T>(d1.x, d1.y));
}

////////////////////////////////////////////////////////////////////////////////
// 3D
////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> cellular_noise(const vec<3, T>& P) noexcept
{
    constexpr T K = static_cast<T>(0.142857142857); // 1/7
    constexpr T Ko = static_cast<T>(0.428571428571); // 1/2-K/2
    constexpr T K2 = static_cast<T>(0.020408163265306); // 1/(7*7)
    constexpr T Kz = static_cast<T>(0.166666666667); // 1/6
    constexpr T Kzo = static_cast<T>(0.416666666667); // 1/2-1/6*2
    constexpr T jitter = static_cast<T>(1); // smaller jitter gives more regular pattern

    const vec<3, T> Pi = __detail::mod289(floor(P));
    const vec<3, T> Pf = fract(P) - static_cast<T>(0.5);

    const vec<3, T> Pfx = Pf.x + vec<3, T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(-1));
    const vec<3, T> Pfy = Pf.y + vec<3, T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(-1));
    const vec<3, T> Pfz = Pf.z + vec<3, T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(-1));

    const vec<3, T> p = __detail::permute(Pi.x + vec<3, T>(static_cast<T>(-1), static_cast<T>(0), static_cast<T>(1)));
    const vec<3, T> p1 = __detail::permute(p + Pi.y - static_cast<T>(1));
    const vec<3, T> p2 = __detail::permute(p + Pi.y);
    const vec<3, T> p3 = __detail::permute(p + Pi.y + static_cast<T>(1));

    const vec<3, T> p11 = __detail::permute(p1 + Pi.z - static_cast<T>(1));
    const vec<3, T> p12 = __detail::permute(p1 + Pi.z);
    const vec<3, T> p13 = __detail::permute(p1 + Pi.z + static_cast<T>(1));

    const vec<3, T> p21 = __detail::permute(p2 + Pi.z - static_cast<T>(1));
    const vec<3, T> p22 = __detail::permute(p2 + Pi.z);
    const vec<3, T> p23 = __detail::permute(p2 + Pi.z + static_cast<T>(1));

    const vec<3, T> p31 = __detail::permute(p3 + Pi.z - static_cast<T>(1));
    const vec<3, T> p32 = __detail::permute(p3 + Pi.z);
    const vec<3, T> p33 = __detail::permute(p3 + Pi.z + static_cast<T>(1));

    // Calculate all distance vectors (manual unroll instead of macro)
    const vec<3, T> ox11 = fract(p11 * K) - Ko;
    const vec<3, T> oy11 = __detail::mod7(floor(p11 * K)) * K - Ko;
    const vec<3, T> oz11 = floor(p11 * K2) * Kz - Kzo;
    const vec<3, T> dx11 = Pfx + jitter * ox11;
    const vec<3, T> dy11 = Pfy.x + jitter * oy11;
    const vec<3, T> dz11 = Pfz.x + jitter * oz11;
    const vec<3, T> d11 = dx11 * dx11 + dy11 * dy11 + dz11 * dz11;

    const vec<3, T> ox12 = fract(p12 * K) - Ko;
    const vec<3, T> oy12 = __detail::mod7(floor(p12 * K)) * K - Ko;
    const vec<3, T> oz12 = floor(p12 * K2) * Kz - Kzo;
    const vec<3, T> dx12 = Pfx + jitter * ox12;
    const vec<3, T> dy12 = Pfy.x + jitter * oy12;
    const vec<3, T> dz12 = Pfz.y + jitter * oz12;
    const vec<3, T> d12 = dx12 * dx12 + dy12 * dy12 + dz12 * dz12;

    const vec<3, T> ox13 = fract(p13 * K) - Ko;
    const vec<3, T> oy13 = __detail::mod7(floor(p13 * K)) * K - Ko;
    const vec<3, T> oz13 = floor(p13 * K2) * Kz - Kzo;
    const vec<3, T> dx13 = Pfx + jitter * ox13;
    const vec<3, T> dy13 = Pfy.x + jitter * oy13;
    const vec<3, T> dz13 = Pfz.z + jitter * oz13;
    const vec<3, T> d13 = dx13 * dx13 + dy13 * dy13 + dz13 * dz13;

    const vec<3, T> ox21 = fract(p21 * K) - Ko;
    const vec<3, T> oy21 = __detail::mod7(floor(p21 * K)) * K - Ko;
    const vec<3, T> oz21 = floor(p21 * K2) * Kz - Kzo;
    const vec<3, T> dx21 = Pfx + jitter * ox21;
    const vec<3, T> dy21 = Pfy.y + jitter * oy21;
    const vec<3, T> dz21 = Pfz.x + jitter * oz21;
    const vec<3, T> d21 = dx21 * dx21 + dy21 * dy21 + dz21 * dz21;

    const vec<3, T> ox22 = fract(p22 * K) - Ko;
    const vec<3, T> oy22 = __detail::mod7(floor(p22 * K)) * K - Ko;
    const vec<3, T> oz22 = floor(p22 * K2) * Kz - Kzo;
    const vec<3, T> dx22 = Pfx + jitter * ox22;
    const vec<3, T> dy22 = Pfy.y + jitter * oy22;
    const vec<3, T> dz22 = Pfz.y + jitter * oz22;
    const vec<3, T> d22 = dx22 * dx22 + dy22 * dy22 + dz22 * dz22;

    const vec<3, T> ox23 = fract(p23 * K) - Ko;
    const vec<3, T> oy23 = __detail::mod7(floor(p23 * K)) * K - Ko;
    const vec<3, T> oz23 = floor(p23 * K2) * Kz - Kzo;
    const vec<3, T> dx23 = Pfx + jitter * ox23;
    const vec<3, T> dy23 = Pfy.y + jitter * oy23;
    const vec<3, T> dz23 = Pfz.z + jitter * oz23;
    const vec<3, T> d23 = dx23 * dx23 + dy23 * dy23 + dz23 * dz23;

    const vec<3, T> ox31 = fract(p31 * K) - Ko;
    const vec<3, T> oy31 = __detail::mod7(floor(p31 * K)) * K - Ko;
    const vec<3, T> oz31 = floor(p31 * K2) * Kz - Kzo;
    const vec<3, T> dx31 = Pfx + jitter * ox31;
    const vec<3, T> dy31 = Pfy.z + jitter * oy31;
    const vec<3, T> dz31 = Pfz.x + jitter * oz31;
    const vec<3, T> d31 = dx31 * dx31 + dy31 * dy31 + dz31 * dz31;

    const vec<3, T> ox32 = fract(p32 * K) - Ko;
    const vec<3, T> oy32 = __detail::mod7(floor(p32 * K)) * K - Ko;
    const vec<3, T> oz32 = floor(p32 * K2) * Kz - Kzo;
    const vec<3, T> dx32 = Pfx + jitter * ox32;
    const vec<3, T> dy32 = Pfy.z + jitter * oy32;
    const vec<3, T> dz32 = Pfz.y + jitter * oz32;
    const vec<3, T> d32 = dx32 * dx32 + dy32 * dy32 + dz32 * dz32;

    const vec<3, T> ox33 = fract(p33 * K) - Ko;
    const vec<3, T> oy33 = __detail::mod7(floor(p33 * K)) * K - Ko;
    const vec<3, T> oz33 = floor(p33 * K2) * Kz - Kzo;
    const vec<3, T> dx33 = Pfx + jitter * ox33;
    const vec<3, T> dy33 = Pfy.z + jitter * oy33;
    const vec<3, T> dz33 = Pfz.z + jitter * oz33;
    const vec<3, T> d33 = dx33 * dx33 + dy33 * dy33 + dz33 * dz33;

    // Sorting: left unchanged — difficult to refactor const safely
    vec<3, T> d1a = min(d11, d12);
    d12 = max(d11, d12);
    d11 = min(d1a, d13);
    d13 = max(d1a, d13);
    d12 = min(d12, d13);

    vec<3, T> d2a = min(d21, d22);
    d22 = max(d21, d22);
    d21 = min(d2a, d23);
    d23 = max(d2a, d23);
    d22 = min(d22, d23);

    vec<3, T> d3a = min(d31, d32);
    d32 = max(d31, d32);
    d31 = min(d3a, d33);
    d33 = max(d3a, d33);
    d32 = min(d32, d33);

    vec<3, T> da = min(d11, d21);
    d21 = max(d11, d21);
    d11 = min(da, d31);
    d31 = max(da, d31);

    d11 = vec<3, T>(min(d11.x, d11.y), max(d11.x, d11.y), d11.z);
    d11 = vec<3, T>(min(d11.x, d11.z), d11.y, max(d11.x, d11.z));

    d12 = min(d12, d21);
    d12 = min(d12, d22);
    d12 = min(d12, d31);
    d12 = min(d12, d32);
    d11 = vec<3, T>(d11.x, min(d11.y, d12.x), min(d11.z, d12.y));
    d11.y = min(d11.y, d12.z);
    d11.y = min(d11.y, d11.z);

    return sqrt(vec<2, T>(d11.x, d11.y)); // F1, F2
}

////////////////////////////////////////////////////////////////////////////////
// 2x2
////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> cellular_noise_2x2(const vec<2, T>& P) noexcept
{
    constexpr T K = static_cast<T>(0.142857142857);   // 1/7
    constexpr T K2 = static_cast<T>(0.0714285714285);  // K/2
    constexpr T jitter = static_cast<T>(0.8);              // jitter < 1.0 improves accuracy

    const vec<2, T> Pi = __detail::mod289(floor(P));
    const vec<2, T> Pf = fract(P);

    const vec<4, T> Pfx = Pf.x + vec<4, T>(
        static_cast<T>(-0.5), static_cast<T>(-1.5),
        static_cast<T>(-0.5), static_cast<T>(-1.5)
    );

    const vec<4, T> Pfy = Pf.y + vec<4, T>(
        static_cast<T>(-0.5), static_cast<T>(-0.5),
        static_cast<T>(-1.5), static_cast<T>(-1.5)
    );

    const vec<4, T> px = Pi.x + vec<4, T>(0, 1, 0, 1);
    const vec<4, T> py = Pi.y + vec<4, T>(0, 0, 1, 1);

    const vec<4, T> p = __detail::permute(__detail::permute(px) + py);

    const vec<4, T> ox = __detail::mod7(p) * K + K2;
    const vec<4, T> oy = __detail::mod7(floor(p * K)) * K + K2;

    const vec<4, T> dx = Pfx + jitter * ox;
    const vec<4, T> dy = Pfy + jitter * oy;

    vec<4, T> d = dx * dx + dy * dy; // squared distances

#if 0
    // Only compute F1 (fast, not accurate)
    d = vec<4, T>(min(d.x, d.z), min(d.y, d.w), d.z, d.w);
    d.x = min(d.x, d.y);
    return vec<2, T>(sqrt(d.x)); // F1 duplicated
#else
    // Compute F1 and F2 properly
    d = vec<4, T>(min(d.x, d.y), max(d.x, d.y), d.z, d.w);
    d = vec<4, T>(min(d.x, d.z), d.y, max(d.x, d.z), d.w);
    d = vec<4, T>(min(d.x, d.w), d.y, d.z, max(d.x, d.w));

    d.y = min(d.y, d.z);
    d.y = min(d.y, d.w);

    return sqrt(vec<2, T>(d.x, d.y)); // F1, F2
#endif
}

////////////////////////////////////////////////////////////////////////////////
// 2x2x2
////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> cellular_noise_2x2x2(const vec<3, T>& P) noexcept
{
    constexpr T K = static_cast<T>(0.142857142857); // 1/7
    constexpr T Ko = static_cast<T>(0.428571428571); // 1/2-K/2
    constexpr T K2 = static_cast<T>(0.020408163265306); // 1/(7*7)
    constexpr T Kz = static_cast<T>(0.166666666667); // 1/6
    constexpr T Kzo = static_cast<T>(0.416666666667); // 1/2-1/6*2
    constexpr T jitter = static_cast<T>(0.8); // smaller jitter gives less errors in F2

    const vec<3, T> Pi = __detail::mod289(floor(P));
    const vec<3, T> Pf = fract(P);

    const vec<4, T> Pfx = Pf.x + vec<4, T>(T(0), T(-1), T(0), T(-1));
    const vec<4, T> Pfy = Pf.y + vec<4, T>(T(0), T(0), T(-1), T(-1));

    const vec<4, T> px = Pi.x + vec<4, T>(T(0), T(1), T(0), T(1));
    const vec<4, T> py = Pi.y + vec<4, T>(T(0), T(0), T(1), T(1));
    const vec<4, T> p = __detail::permute(__detail::permute(px) + py);

    const vec<4, T> p1 = __detail::permute(p + Pi.z);          // z + 0
    const vec<4, T> p2 = __detail::permute(p + Pi.z + T(1));   // z + 1

    const vec<4, T> ox1 = fract(p1 * K) - Ko;
    const vec<4, T> oy1 = __detail::mod7(floor(p1 * K)) * K - Ko;
    const vec<4, T> oz1 = floor(p1 * K2) * Kz - Kzo;

    const vec<4, T> ox2 = fract(p2 * K) - Ko;
    const vec<4, T> oy2 = __detail::mod7(floor(p2 * K)) * K - Ko;
    const vec<4, T> oz2 = floor(p2 * K2) * Kz - Kzo;

    const vec<4, T> dx1 = Pfx + jitter * ox1;
    const vec<4, T> dy1 = Pfy + jitter * oy1;
    const vec<4, T> dz1 = Pf.z + jitter * oz1;

    const vec<4, T> dx2 = Pfx + jitter * ox2;
    const vec<4, T> dy2 = Pfy + jitter * oy2;
    const vec<4, T> dz2 = Pf.z - T(1) + jitter * oz2;

    const vec<4, T> d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
    const vec<4, T> d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;

#if 0
    // Only compute F1 (fast, not accurate)
    vec<4, T> d = min(d1, d2);
    d = vec<4, T>(min(d.x, d.w), min(d.y, d.z), d.z, d.w);
    d.x = min(d.x, d.y);
    return vec<2, T>(sqrt(d.x));
#else
    // Full F1 and F2 computation
    vec<4, T> d = min(d1, d2);  // F1 in d
    const vec<4, T> d2b = max(d1, d2);  // keep candidates for F2

    // Bubble sort logic for F1 to d.x
    d = vec<4, T>(min(d.x, d.y), max(d.x, d.y), d.z, d.w);
    d = vec<4, T>(min(d.x, d.z), d.y, max(d.x, d.z), d.w);
    d = vec<4, T>(min(d.x, d.w), d.y, d.z, max(d.x, d.w));

    // Min F2 candidates
    d = vec<4, T>(d.x,
        min(d.y, d2b.y),
        min(d.z, d2b.z),
        min(d.w, d2b.w));

    d.y = min(d.y, d.z);
    d.y = min(d.y, d.w);
    d.y = min(d.y, d2b.x);

    return sqrt(vec<2, T>(d.x, d.y)); // F1 and F2
#endif
}

} // namespace math
} // namespace vx