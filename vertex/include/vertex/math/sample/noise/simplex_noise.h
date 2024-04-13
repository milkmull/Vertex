#pragma once

// Copyright (C) 2011 Ashima Arts. All rights reserved.
// Distributed under the MIT License. See LICENSE file.
// https://github.com/ashima/webgl-noise
// https://github.com/stegu/webgl-noise

// Based on the work of Stefan Gustavson and Ashima Arts:
// https://github.com/ashima/webgl-noise/blob/master/src/noise2D.glsl
// https://github.com/ashima/webgl-noise/blob/master/src/noise3D.glsl
// https://github.com/ashima/webgl-noise/blob/master/src/noise4D.glsl

// https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/simplexnoise.pdf

#include "_priv/fn_noise.h"

namespace vx {
namespace math {

/**
 * @brief Generates simplex noise for 2D coordinates.
 *
 * This function calculates simplex noise for given 2D coordinates.
 *
 * @tparam T The type of the coordinates.
 * @param v The 2D coordinates for which simplex noise is calculated.
 * @return The simplex noise value for the given coordinates, ranging from -1 to 1.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T simplex_noise(const vec<2, T>& v)
{
    const vec<4, T> C(
        static_cast<T>(+0.211324865405187), // (3.0 - sqrt(3.0)) / 6.0
        static_cast<T>(+0.366025403784439), // 0.5 * (sqrt(3.0) - 1.0)
        static_cast<T>(-0.577350269189626), // -1.0 + 2.0 * c.x
        static_cast<T>(+0.024390243902439)  // 1.0 / 41.0
    );

    // First corner
    vec<2, T> i  = floor(v + dot(v, vec<2, T>(C.y)));
    vec<2, T> x0 = v -   i + dot(i, vec<2, T>(C.x));

    if (x0.x == x0.y)
    {
        i *= 1.0f;
    }

    // Other corners
    // i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1 : 0
    // i1.y = 1 - i1.x;
    vec<2, T> i1 = (x0.x > x0.y)
        ? vec<2, T>(static_cast<T>(1), static_cast<T>(0))
        : vec<2, T>(static_cast<T>(0), static_cast<T>(1));

    // x0 = x0 - 0 + 0 * c.xx ;
    // x1 = x0 - i1 + 1 * c.xx ;
    // x2 = x0 - 1 + 2 * c.xx ;
    vec<4, T> x12 = vec<4, T>(x0.x, x0.y, x0.x, x0.y) + vec<4, T>(C.x, C.x, C.z, C.z);
    x12.x -= i1.x;
    x12.y -= i1.y;

    // Permutations
    i = _priv::mod289(i); // Avoid truncation effects in permutation
    vec<3, T> p = _priv::permute(_priv::permute(
        i.y + vec<3, T>(static_cast<T>(0), i1.y, static_cast<T>(1))) +
        i.x + vec<3, T>(static_cast<T>(0), i1.x, static_cast<T>(1))
    );

    vec<3, T> m = max(static_cast<T>(0.5) - vec<3, T>(
        dot(x0, x0),
        dot(vec<2, T>(x12.x, x12.y), vec<2, T>(x12.x, x12.y)),
        dot(vec<2, T>(x12.z, x12.w), vec<2, T>(x12.z, x12.w))
    ), static_cast<T>(0));
    m = m * m;
    m = m * m;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    vec<3, T> x = static_cast<T>(2) * fract(p * vec<3, T>(C.w)) - static_cast<T>(1);
    vec<3, T> h = abs(x) - static_cast<T>(0.5);
    vec<3, T> ox = floor(x + static_cast<T>(0.5));
    vec<3, T> a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt(a0 * a0 + h * h);
    m *= static_cast<T>(1.79284291400159) - static_cast<T>(0.85373472095314) * (a0 * a0 + h * h);

    // Compute final noise value at P
    vec<3, T> g(
        a0.x *  x0.x + h.x *  x0.y,
        a0.y * x12.x + h.y * x12.y,
        a0.z * x12.z + h.z * x12.w
    );

    return static_cast<T>(130) * dot(m, g);
}

/**
 * @brief Generates simplex noise for 3D coordinates.
 *
 * This function calculates simplex noise for given 3D coordinates.
 *
 * @tparam T The type of the coordinates.
 * @param v The 3D coordinates for which simplex noise is calculated.
 * @return The simplex noise value for the given coordinates, ranging from -1 to 1.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T simplex_noise(const vec<3, T>& v)
{
    const vec<2, T> C(static_cast<T>(1.0) / static_cast<T>(6.0), static_cast<T>(1.0) / static_cast<T>(3.0));
    const vec<4, T> D(static_cast<T>(0.0),  static_cast<T>(0.5), static_cast<T>(1.0),  static_cast<T>(2.0));

    // First corner
    vec<3, T> i = floor(v + dot(v, vec<3, T>(C.y)));
    vec<3, T> x0 = v -  i + dot(i, vec<3, T>(C.x));

    // Other corners
    vec<3, T> g = step(vec<3, T>(x0.y, x0.z, x0.x), x0);
    g.z -= step(static_cast<T>(3), (g.x + g.y + g.z)); // Avoids singularities when g == (1, 1, 1)
    vec<3, T> l = static_cast<T>(1) - g;
    vec<3, T> i1 = min(g, vec<3, T>(l.z, l.x, l.y));
    vec<3, T> i2 = max(g, vec<3, T>(l.z, l.x, l.y));

    // x0 = x0 - 0.0 + 0.0 * C.xxx;
    // x1 = x0 - i1  + 1.0 * C.xxx;
    // x2 = x0 - i2  + 2.0 * C.xxx;
    // x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec<3, T> x1(x0 - i1 + C.x);
    vec<3, T> x2(x0 - i2 + C.y); //  2.0 * C.x = 1 / 3 = C.y
    vec<3, T> x3(x0 -      D.y); // -1.0 + 3.0 * C.x = -0.5 = -D.y

    // Permutations
    i = _priv::mod289(i);
    vec<4, T> p = _priv::permute(_priv::permute(_priv::permute(
        i.z + vec<4, T>(static_cast<T>(0), i1.z, i2.z, static_cast<T>(1))) +
        i.y + vec<4, T>(static_cast<T>(0), i1.y, i2.y, static_cast<T>(1))) +
        i.x + vec<4, T>(static_cast<T>(0), i1.x, i2.x, static_cast<T>(1))
    );

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    const T n_ = static_cast<T>(0.142857142857); // 1.0 / 7.0
    vec<3, T> ns = n_ * vec<3, T>(D.w, D.y, D.z) - vec<3, T>(D.x, D.z, D.x);

    vec<4, T> j = p - static_cast<T>(49) * floor(p * ns.z * ns.z); // mod(p, 7 * 7)

    vec<4, T> x_ = floor(j * ns.z);
    vec<4, T> y_ = floor(j - static_cast<T>(7) * x_); // mod(j, N)

    vec<4, T> x(x_ * ns.x + ns.y);
    vec<4, T> y(y_ * ns.x + ns.y);
    vec<4, T> h = static_cast<T>(1) - abs(x) - abs(y);

    vec<4, T> b0(x.x, x.y, y.x, y.y);
    vec<4, T> b1(x.z, x.w, y.z, y.w);

    // vec4 s0 = vec4(lessThan(b0, 0.0)) * 2.0 - 1.0;
    // vec4 s1 = vec4(lessThan(b1, 0.0)) * 2.0 - 1.0;
    vec<4, T> s0 = floor(b0) * static_cast<T>(2) + static_cast<T>(1);
    vec<4, T> s1 = floor(b1) * static_cast<T>(2) + static_cast<T>(1);
    vec<4, T> sh = -step(h, vec<4, T>(static_cast<T>(0)));

    vec<4, T> a0 = vec<4, T>(b0.x, b0.z, b0.y, b0.w) + vec<4, T>(s0.x, s0.z, s0.y, s0.w) * vec<4, T>(sh.x, sh.x, sh.y, sh.y);
    vec<4, T> a1 = vec<4, T>(b1.x, b1.z, b1.y, b1.w) + vec<4, T>(s1.x, s1.z, s1.y, s1.w) * vec<4, T>(sh.z, sh.z, sh.w, sh.w);

    vec<3, T> p0 (a0.x, a0.y, h.x);
    vec<3, T> p1 (a0.z, a0.w, h.y);
    vec<3, T> p2 (a1.x, a1.y, h.z);
    vec<3, T> p3 (a1.z, a1.w, h.w);

    // Normalise gradients
    vec<4, T> norm = _priv::taylor_inv_sqrt(vec<4, T>(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec<4, T> m = max(static_cast<T>(0.5) - vec<4, T>(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), static_cast<T>(0));
    m = m * m;

    return static_cast<T>(105) * dot(m * m, vec<4, T>(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}

/**
 * @brief Generates simplex noise for 4D coordinates.
 *
 * This function calculates simplex noise for given 4D coordinates.
 *
 * @tparam T The type of the coordinates.
 * @param v The 4D coordinates for which simplex noise is calculated.
 * @return The simplex noise value for the given coordinates, ranging from -1 to 1.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T simplex_noise(const vec<4, T>& v)
{
    const T F4 = static_cast<T>(0.309016994374947451);

    const vec<4, T> C(
        static_cast<T>(+0.138196601125011),  // (5 - sqrt(5))/20  G4
        static_cast<T>(+0.276393202250021),  // 2 * G4
        static_cast<T>(+0.414589803375032),  // 3 * G4
        static_cast<T>(-0.447213595499958)   // -1 + 4 * G4
    );

    // First corner
    vec<4, T> i = floor(v + dot(v, vec<4, T>(F4)));
    vec<4, T> x0 = v -  i + dot(i, vec<4, T>(C.x));

    // Other corners

    // Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
    vec<3, T> is_x  = step(vec<3, T>(x0.y, x0.z, x0.w), vec<3, T>(x0.x));
    vec<3, T> is_yz = step(vec<3, T>(x0.z, x0.w, x0.w), vec<3, T>(x0.y, x0.y, x0.z));
    // i0.x = dot(isX, vec3(1.0));
    // i0.x = isX.x + isX.y + isX.z;
    // i0.yzw = static_cast<T>(1) - isX;
    vec<4, T> i0 (is_x.x + is_x.y + is_x.z, static_cast<T>(1) - is_x);
    // i0.y += dot(isYZ.xy, vec2(1.0));
    i0.y += is_yz.x + is_yz.y;
    // i0.zw += 1.0 - vec<2, T, Q>(isYZ.x, isYZ.y);
    i0.z += static_cast<T>(1) - is_yz.x;
    i0.w += static_cast<T>(1) - is_yz.y;
    i0.z += is_yz.z;
    i0.w += static_cast<T>(1) - is_yz.z;

    // i0 now contains the unique values 0,1,2,3 in each channel
    vec<4, T> i3 = clamp(i0,                     static_cast<T>(0), static_cast<T>(1));
    vec<4, T> i2 = clamp(i0 - static_cast<T>(1), static_cast<T>(0), static_cast<T>(1));
    vec<4, T> i1 = clamp(i0 - static_cast<T>(2), static_cast<T>(0), static_cast<T>(1));

    // x0 = x0 - 0.0 + 0.0 * C.xxxx
    // x1 = x0 - i1  + 1.0 * C.xxxx
    // x2 = x0 - i2  + 2.0 * C.xxxx
    // x3 = x0 - i3  + 3.0 * C.xxxx
    // x4 = x0 - 1.0 + 4.0 * C.xxxx
    vec<4, T> x1 = x0 - i1 + C.x;
    vec<4, T> x2 = x0 - i2 + C.y;
    vec<4, T> x3 = x0 - i3 + C.z;
    vec<4, T> x4 = x0      + C.w;

    // Permutations
    i = _priv::mod289(i);
    T j0 = _priv::permute(_priv::permute(_priv::permute(_priv::permute(i.w) + i.z) + i.y) + i.x);
    vec<4, T> j1 = _priv::permute(_priv::permute(_priv::permute(_priv::permute(
        i.w + vec<4, T>(i1.w, i2.w, i3.w, static_cast<T>(1))) +
        i.z + vec<4, T>(i1.z, i2.z, i3.z, static_cast<T>(1))) +
        i.y + vec<4, T>(i1.y, i2.y, i3.y, static_cast<T>(1))) +
        i.x + vec<4, T>(i1.x, i2.x, i3.x, static_cast<T>(1))
    );

    // Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope
    // 7*7*6 = 294, which is close to the ring size 17*17 = 289.
    vec<4, T> ip(
        static_cast<T>(1) / static_cast<T>(294),
        static_cast<T>(1) / static_cast<T>(49),
        static_cast<T>(1) / static_cast<T>(7),
        static_cast<T>(0)
    );

    vec<4, T> p0 = _priv::grad4(j0,   ip);
    vec<4, T> p1 = _priv::grad4(j1.x, ip);
    vec<4, T> p2 = _priv::grad4(j1.y, ip);
    vec<4, T> p3 = _priv::grad4(j1.z, ip);
    vec<4, T> p4 = _priv::grad4(j1.w, ip);

    // Normalise gradients
    vec<4, T> norm = _priv::taylor_inv_sqrt(vec<4, T>(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    p4 *= _priv::taylor_inv_sqrt(dot(p4, p4));

    // Mix contributions from the five corners
    vec<3, T> m0 = max(static_cast<T>(0.6) - vec<3, T>(dot(x0, x0), dot(x1, x1), dot(x2, x2)), static_cast<T>(0));
    vec<2, T> m1 = max(static_cast<T>(0.6) - vec<2, T>(dot(x3, x3), dot(x4, x4)             ), static_cast<T>(0));
    m0 = m0 * m0;
    m1 = m1 * m1;
    return static_cast<T>(49) * (
        dot(m0 * m0, vec<3, T>(dot(p0, x0), dot(p1, x1), dot(p2, x2))) +
        dot(m1 * m1, vec<2, T>(dot(p3, x3),              dot(p4, x4)))
    );
}

}
}