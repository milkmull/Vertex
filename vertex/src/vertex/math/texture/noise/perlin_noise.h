#pragma once

#include "../../math/math.h"
#include "detail/fn_noise.h"

// Based on the work of Stefan Gustavson and Ashima Arts:
// https://github.com/ashima/webgl-noise
// https://github.com/ashima/webgl-noise/blob/master/src/classicnoise2D.glsl
// https://github.com/ashima/webgl-noise/blob/master/src/classicnoise3D.glsl
// https://github.com/ashima/webgl-noise/blob/master/src/classicnoise4D.glsl

namespace vx {
namespace math {

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T perlin_noise(const vec<2, T>& p)
{
    vec<4, T> pi = floor(vec<4, T>(p.x, p.y, p.x, p.y)) + vec<4, T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(1));
    vec<4, T> pf = fract(vec<4, T>(p.x, p.y, p.x, p.y)) - vec<4, T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(1));
    pi = mod(pi, static_cast<T>(289)); // To avoid truncation effects in permutation
    vec<4, T> ix(pi.x, pi.z, pi.x, pi.z);
    vec<4, T> iy(pi.y, pi.y, pi.w, pi.w);
    vec<4, T> fx(pf.x, pf.z, pf.x, pf.z);
    vec<4, T> fy(pf.y, pf.y, pf.w, pf.w);

    vec<4, T> i = detail::permute(detail::permute(ix) + iy);

    vec<4, T> gx = static_cast<T>(2) * fract(i * (static_cast<T>(1) / static_cast<T>(41))) - static_cast<T>(1);
    vec<4, T> gy = abs(gx) - static_cast<T>(0.5);
    vec<4, T> tx = floor(gx + static_cast<T>(0.5));
    gx -= tx;

    vec<2, T> g00 (gx.x, gy.x);
    vec<2, T> g10 (gx.y, gy.y);
    vec<2, T> g01 (gx.z, gy.z);
    vec<2, T> g11 (gx.w, gy.w);

    vec<4, T> norm = detail::taylor_inv_sqrt(vec<4, T>(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;

    T n00 = dot(g00, vec<2, T>(fx.x, fy.x));
    T n10 = dot(g10, vec<2, T>(fx.y, fy.y));
    T n01 = dot(g01, vec<2, T>(fx.z, fy.z));
    T n11 = dot(g11, vec<2, T>(fx.w, fy.w));

    vec<2, T> fade_xy = detail::fade(vec<2, T>(pf.x, pf.y));
    vec<2, T> n_x = mix(vec<2, T>(n00, n01), vec<2, T>(n10, n11), fade_xy.x);
    T n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return static_cast<T>(2.3) * n_xy;
}

template <typename T>
T perlin_noise(const vec<3, T>& p)
{
    vec<3, T> pi0 = floor(p); // Integer part for indexing
    vec<3, T> pi1 = pi0 + static_cast<T>(1); // Integer part + 1
    pi0 = detail::mod289(pi0);
    pi1 = detail::mod289(pi1);
    vec<3, T> pf0 = fract(p); // Fractional part for interpolation
    vec<3, T> pf1 = pf0 - static_cast<T>(1); // Fractional part - 1
    vec<4, T> ix(pi0.x, pi1.x, pi0.x, pi1.x);
    vec<4, T> iy(pi0.y, pi0.y, pi1.y, pi1.y);
    vec<4, T> iz0(pi0.z);
    vec<4, T> iz1(pi1.z);

    vec<4, T> ixy = detail::permute(detail::permute(ix) + iy);
    vec<4, T> ixy0 = detail::permute(ixy + iz0);
    vec<4, T> ixy1 = detail::permute(ixy + iz1);

    vec<4, T> gx0 = ixy0 * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gy0 = fract(floor(gx0) * (static_cast<T>(1) / static_cast<T>(7))) - static_cast<T>(0.5);
    gx0 = fract(gx0);
    vec<4, T> gz0 = vec<4, T>(0.5) - abs(gx0) - abs(gy0);
    vec<4, T> sz0 = step(gz0, vec<4, T>(0));
    gx0 -= sz0 * (step(static_cast<T>(0), gx0) - static_cast<T>(0.5));
    gy0 -= sz0 * (step(static_cast<T>(0), gy0) - static_cast<T>(0.5));

    vec<4, T> gx1 = ixy1 * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gy1 = fract(floor(gx1) * (static_cast<T>(1) / static_cast<T>(7))) - static_cast<T>(0.5);
    gx1 = fract(gx1);
    vec<4, T> gz1 = vec<4, T>(0.5) - abs(gx1) - abs(gy1);
    vec<4, T> sz1 = step(gz1, vec<4, T>(0));
    gx1 -= sz1 * (step(static_cast<T>(0), gx1) - static_cast<T>(0.5));
    gy1 -= sz1 * (step(static_cast<T>(0), gy1) - static_cast<T>(0.5));

    vec<3, T> g000(gx0.x, gy0.x, gz0.x);
    vec<3, T> g100(gx0.y, gy0.y, gz0.y);
    vec<3, T> g010(gx0.z, gy0.z, gz0.z);
    vec<3, T> g110(gx0.w, gy0.w, gz0.w);
    vec<3, T> g001(gx1.x, gy1.x, gz1.x);
    vec<3, T> g101(gx1.y, gy1.y, gz1.y);
    vec<3, T> g011(gx1.z, gy1.z, gz1.z);
    vec<3, T> g111(gx1.w, gy1.w, gz1.w);

    vec<4, T> norm0 = detail::taylor_inv_sqrt(vec<4, T>(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    vec<4, T> norm1 = detail::taylor_inv_sqrt(vec<4, T>(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    T n000 = dot(g000, pf0);
    T n100 = dot(g100, vec<3, T>(pf1.x, pf0.y, pf0.z));
    T n010 = dot(g010, vec<3, T>(pf0.x, pf1.y, pf0.z));
    T n110 = dot(g110, vec<3, T>(pf1.x, pf1.y, pf0.z));
    T n001 = dot(g001, vec<3, T>(pf0.x, pf0.y, pf1.z));
    T n101 = dot(g101, vec<3, T>(pf1.x, pf0.y, pf1.z));
    T n011 = dot(g011, vec<3, T>(pf0.x, pf1.y, pf1.z));
    T n111 = dot(g111, pf1);

    vec<3, T> fade_xyz = detail::fade(pf0);
    vec<4, T> n_z = mix(vec<4, T>(n000, n100, n010, n110), vec<4, T>(n001, n101, n011, n111), fade_xyz.z);
    vec<2, T> n_yz = mix(vec<2, T>(n_z.x, n_z.y), vec<2, T>(n_z.z, n_z.w), fade_xyz.y);
    T n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return static_cast<T>(2.2) * n_xyz;
}

template <typename T>
T perlin_noise(const vec<4, T>& p)
{
    vec<4, T> pi0 = floor(p); // Integer part for indexing
    vec<4, T> pi1 = pi0 + static_cast<T>(1); // Integer part + 1
    pi0 = detail::mod289(pi0);
    pi1 = detail::mod289(pi1);
    vec<4, T> pf0 = fract(p); // Fractional part for interpolation
    vec<4, T> pf1 = pf0 - static_cast<T>(1); // Fractional part - 1
    vec<4, T> ix(pi0.x, pi1.x, pi0.x, pi1.x);
    vec<4, T> iy(pi0.y, pi0.y, pi1.y, pi1.y);
    vec<4, T> iz0(pi0.z);
    vec<4, T> iz1(pi1.z);
    vec<4, T> iw0(pi0.w);
    vec<4, T> iw1(pi1.w);

    vec<4, T> ixy = detail::permute(detail::permute(ix) + iy);
    vec<4, T> ixy0 = detail::permute(ixy + iz0);
    vec<4, T> ixy1 = detail::permute(ixy + iz1);
    vec<4, T> ixy00 = detail::permute(ixy0 + iw0);
    vec<4, T> ixy01 = detail::permute(ixy0 + iw1);
    vec<4, T> ixy10 = detail::permute(ixy1 + iw0);
    vec<4, T> ixy11 = detail::permute(ixy1 + iw1);

    vec<4, T> gx00 = ixy00 * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gy00 = floor(gx00) * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gz00 = floor(gy00) * (static_cast<T>(1) / static_cast<T>(6));
    gx00 = fract(gx00) - static_cast<T>(0.5);
    gy00 = fract(gy00) - static_cast<T>(0.5);
    gz00 = fract(gz00) - static_cast<T>(0.5);
    vec<4, T> gw00 = static_cast<T>(0.75) - abs(gx00) - abs(gy00) - abs(gz00);
    vec<4, T> sw00 = step(gw00, vec<4, T>(0));
    gx00 -= sw00 * (step(static_cast<T>(0), gx00) - static_cast<T>(0.5));
    gy00 -= sw00 * (step(static_cast<T>(0), gy00) - static_cast<T>(0.5));

    vec<4, T> gx01 = ixy01 * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gy01 = floor(gx01) * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gz01 = floor(gy01) * (static_cast<T>(1) / static_cast<T>(6));
    gx01 = fract(gx01) - static_cast<T>(0.5);
    gy01 = fract(gy01) - static_cast<T>(0.5);
    gz01 = fract(gz01) - static_cast<T>(0.5);
    vec<4, T> gw01 = static_cast<T>(0.75) - abs(gx01) - abs(gy01) - abs(gz01);
    vec<4, T> sw01 = step(gw01, vec<4, T>(0));
    gx01 -= sw01 * (step(static_cast<T>(0), gx01) - static_cast<T>(0.5));
    gy01 -= sw01 * (step(static_cast<T>(0), gy01) - static_cast<T>(0.5));

    vec<4, T> gx10 = ixy10 * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gy10 = floor(gx10) * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gz10 = floor(gy10) * (static_cast<T>(1) / static_cast<T>(6));
    gx10 = fract(gx10) - static_cast<T>(0.5);
    gy10 = fract(gy10) - static_cast<T>(0.5);
    gz10 = fract(gz10) - static_cast<T>(0.5);
    vec<4, T> gw10 = static_cast<T>(0.75) - abs(gx10) - abs(gy10) - abs(gz10);
    vec<4, T> sw10 = step(gw10, vec<4, T>(0));
    gx10 -= sw10 * (step(static_cast<T>(0), gx10) - static_cast<T>(0.5));
    gy10 -= sw10 * (step(static_cast<T>(0), gy10) - static_cast<T>(0.5));

    vec<4, T> gx11 = ixy11 * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gy11 = floor(gx11) * (static_cast<T>(1) / static_cast<T>(7));
    vec<4, T> gz11 = floor(gy11) * (static_cast<T>(1) / static_cast<T>(6));
    gx11 = fract(gx11) - static_cast<T>(0.5);
    gy11 = fract(gy11) - static_cast<T>(0.5);
    gz11 = fract(gz11) - static_cast<T>(0.5);
    vec<4, T> gw11 = static_cast<T>(0.75) - abs(gx11) - abs(gy11) - abs(gz11);
    vec<4, T> sw11 = step(gw11, vec<4, T>(0));
    gx11 -= sw11 * (step(static_cast<T>(0), gx11) - static_cast<T>(0.5));
    gy11 -= sw11 * (step(static_cast<T>(0), gy11) - static_cast<T>(0.5));

    vec<4, T> g0000(gx00.x, gy00.x, gz00.x, gw00.x);
    vec<4, T> g1000(gx00.y, gy00.y, gz00.y, gw00.y);
    vec<4, T> g0100(gx00.z, gy00.z, gz00.z, gw00.z);
    vec<4, T> g1100(gx00.w, gy00.w, gz00.w, gw00.w);
    vec<4, T> g0010(gx10.x, gy10.x, gz10.x, gw10.x);
    vec<4, T> g1010(gx10.y, gy10.y, gz10.y, gw10.y);
    vec<4, T> g0110(gx10.z, gy10.z, gz10.z, gw10.z);
    vec<4, T> g1110(gx10.w, gy10.w, gz10.w, gw10.w);
    vec<4, T> g0001(gx01.x, gy01.x, gz01.x, gw01.x);
    vec<4, T> g1001(gx01.y, gy01.y, gz01.y, gw01.y);
    vec<4, T> g0101(gx01.z, gy01.z, gz01.z, gw01.z);
    vec<4, T> g1101(gx01.w, gy01.w, gz01.w, gw01.w);
    vec<4, T> g0011(gx11.x, gy11.x, gz11.x, gw11.x);
    vec<4, T> g1011(gx11.y, gy11.y, gz11.y, gw11.y);
    vec<4, T> g0111(gx11.z, gy11.z, gz11.z, gw11.z);
    vec<4, T> g1111(gx11.w, gy11.w, gz11.w, gw11.w);

    vec<4, T> norm00 = detail::taylor_inv_sqrt(vec<4, T>(dot(g0000, g0000), dot(g0100, g0100), dot(g1000, g1000), dot(g1100, g1100)));
    g0000 *= norm00.x;
    g0100 *= norm00.y;
    g1000 *= norm00.z;
    g1100 *= norm00.w;

    vec<4, T> norm01 = detail::taylor_inv_sqrt(vec<4, T>(dot(g0001, g0001), dot(g0101, g0101), dot(g1001, g1001), dot(g1101, g1101)));
    g0001 *= norm01.x;
    g0101 *= norm01.y;
    g1001 *= norm01.z;
    g1101 *= norm01.w;

    vec<4, T> norm10 = detail::taylor_inv_sqrt(vec<4, T>(dot(g0010, g0010), dot(g0110, g0110), dot(g1010, g1010), dot(g1110, g1110)));
    g0010 *= norm10.x;
    g0110 *= norm10.y;
    g1010 *= norm10.z;
    g1110 *= norm10.w;

    vec<4, T> norm11 = detail::taylor_inv_sqrt(vec<4, T>(dot(g0011, g0011), dot(g0111, g0111), dot(g1011, g1011), dot(g1111, g1111)));
    g0011 *= norm11.x;
    g0111 *= norm11.y;
    g1011 *= norm11.z;
    g1111 *= norm11.w;

    T n0000 = dot(g0000, pf0);
    T n1000 = dot(g1000, vec<4, T>(pf1.x, pf0.y, pf0.z, pf0.w));
    T n0100 = dot(g0100, vec<4, T>(pf0.x, pf1.y, pf0.z, pf0.w));
    T n1100 = dot(g1100, vec<4, T>(pf1.x, pf1.y, pf0.z, pf0.w));
    T n0010 = dot(g0010, vec<4, T>(pf0.x, pf0.y, pf1.z, pf0.w));
    T n1010 = dot(g1010, vec<4, T>(pf1.x, pf0.y, pf1.z, pf0.w));
    T n0110 = dot(g0110, vec<4, T>(pf0.x, pf1.y, pf1.z, pf0.w));
    T n1110 = dot(g1110, vec<4, T>(pf1.x, pf1.y, pf1.z, pf0.w));
    T n0001 = dot(g0001, vec<4, T>(pf0.x, pf0.y, pf0.z, pf1.w));
    T n1001 = dot(g1001, vec<4, T>(pf1.x, pf0.y, pf0.z, pf1.w));
    T n0101 = dot(g0101, vec<4, T>(pf0.x, pf1.y, pf0.z, pf1.w));
    T n1101 = dot(g1101, vec<4, T>(pf1.x, pf1.y, pf0.z, pf1.w));
    T n0011 = dot(g0011, vec<4, T>(pf0.x, pf0.y, pf1.z, pf1.w));
    T n1011 = dot(g1011, vec<4, T>(pf1.x, pf0.y, pf1.z, pf1.w));
    T n0111 = dot(g0111, vec<4, T>(pf0.x, pf1.y, pf1.z, pf1.w));
    T n1111 = dot(g1111, pf1);

    vec<4, T> fade_xyzw = detail::fade(pf0);
    vec<4, T> n_0w = mix(vec<4, T>(n0000, n1000, n0100, n1100), vec<4, T>(n0001, n1001, n0101, n1101), fade_xyzw.w);
    vec<4, T> n_1w = mix(vec<4, T>(n0010, n1010, n0110, n1110), vec<4, T>(n0011, n1011, n0111, n1111), fade_xyzw.w);
    vec<4, T> n_zw = mix(n_0w, n_1w, fade_xyzw.z);
    vec<2, T> n_yzw = mix(vec<2, T>(n_zw.x, n_zw.y), vec<2, T>(n_zw.z, n_zw.w), fade_xyzw.y);
    T n_xyzw = mix(n_yzw.x, n_yzw.y, fade_xyzw.x);
    return static_cast<T>(2.2) * n_xyzw;
}

}
}