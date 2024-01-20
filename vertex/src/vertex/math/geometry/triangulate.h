#pragma once

#include <vector>

#include "../math/enum.h"
#include "../math/detail/type_traits.h"
#include "../math/detail/base_type_defs.h"

namespace vx {
namespace math {

template <typename T>
inline constexpr std::vector<uint32_t> triangulate(const std::vector<detail::vecx<2, T>>& contour)
{
    const size_t num_points = contour.size();

    if (num_points <= 3)
    {
        std::vector<uint32_t> indices(num_points);
        for (uint32_t i = 0; i < num_points; i++)
        {
            indices[i] = i;
        }
        return indices;
    }

    clock_direction direction = (polygon_area(contour) < 0.0f) ? clock_direction::CLOCKWISE : clock_direction::COUNTER_CLOCKWISE;

    const size_t num_indices = 3 * (num_points - 2);
    std::vector<uint32_t> indices(num_indices);

    size_t current_index = 0;
    std::vector<uint32_t> index_list(num_points);
    for (uint32_t i = 0; i < num_points; i++)
    {
        index_list[i] = i;
    }

    bool found_ear = false;
    bool include_borders = false;

    while (index_list.size() > 3)
    {
        for (size_t i = 0; i < index_list.size(); i++)
        {
            const uint32_t ia = index_list[i];
            const uint32_t ib = index_list[(i > 0) ? (i - 1) : (index_list.size() - 1)];
            const uint32_t ic = index_list[(i < index_list.size() - 1) ? (i + 1) : 0];

            const detail::vecx<2, T> a(contour[ia]);
            const detail::vecx<2, T> b(contour[ib]);
            const detail::vecx<2, T> c(contour[ic]);

            const detail::vecx<2, T> ab(b - a);
            const detail::vecx<2, T> ac(c - a);

            // Check to see if angle formed by a, b, c is inwards

            const float cross = math::cross(ab, ac);

            if ((direction == clock_direction::CLOCKWISE		 && cross < 0.0f) ||
                (direction == clock_direction::COUNTER_CLOCKWISE && cross > 0.0f))
                continue;

            bool is_ear = true;

            for (size_t j = 0; j < contour.size(); j++)
            {
                if (j == ia || j == ib || j == ic)
                    continue;

                const detail::vecx<2, T> p(contour[j]);
                if (point_in_triangle(p, a, b, c, include_borders))
                {
                    is_ear = false;
                    break;
                }
            }

            if (is_ear)
            {
                indices[current_index + 0] = ib;
                indices[current_index + 1] = ia;
                indices[current_index + 2] = ic;
                current_index += 3;

                index_list.erase(index_list.begin() + i);
                found_ear = true;

                break;
            }
        }

        if (!found_ear)
        {
            if (!include_borders)
            {
                // If we go a whole loop without finding an ear, the polygon is likely 
                // degenerate. We can relax the ear finding criteria by including the 
                // borders when checking if a point lies inside a polygon. This can solve
                // cases where there may be collinear points that form 0 area triangles.

                include_borders = true;
                found_ear = false;
                continue;
            }
            else
            {
                throw std::runtime_error("Polygon could not be triangulated");
            }
        }
        else
        {
            found_ear = false;
            continue;
        }
    }

    indices[current_index + 0] = index_list[0];
    indices[current_index + 1] = index_list[1];
    indices[current_index + 2] = index_list[2];

    return indices;
}

// https://www.theoremoftheday.org/GeometryAndTrigonometry/Shoelace/TotDShoelace.pdf

template <typename T>
inline constexpr auto polygon_area(
    const std::vector<detail::vecx<2, T>>& contour,
    clock_direction direction = clock_direction::COUNTER_CLOCKWISE
)
{
    using FT = typename detail::vecx<2, T>::float_value_type;

    const size_t num_points = contour.size();
    if (num_points < 3)
    {
        return static_cast<FT>(0);
    }

    FT area = static_cast<T>(0);

    for (size_t i = 0; i < num_points; i++)
    {
        const detail::vecx<2, T> p0(contour[i]);
        const detail::vecx<2, T> p1(contour[(i < num_points - 1) ? (i + 1) : 0]);

        area += static_cast<FT>(math::cross(p0, p1));
    }

    if (direction == clock_direction::CLOCKWISE)
    {
        area *= static_cast<FT>(-1);
    }

    return area * static_cast<FT>(0.5);
}

template <typename T>
inline constexpr bool point_in_triangle(
    const detail::vecx<2, T>& p,
    const detail::vecx<2, T>& a,
    const detail::vecx<2, T>& b,
    const detail::vecx<2, T>& c
)
{
    const detail::vecx<2, T> ab(b - a);
    const detail::vecx<2, T> bc(c - b);
    const detail::vecx<2, T> ca(a - c);

    const detail::vecx<2, T> ap(p - a);
    const detail::vecx<2, T> bp(p - b);
    const detail::vecx<2, T> cp(p - c);

    const T cross1 = math::cross(ab, ap);
    const T cross2 = math::cross(bc, bp);
    const T cross3 = math::cross(ca, cp);

    return (cross1 >= static_cast<T>(0) && cross2 >= static_cast<T>(0) && cross3 >= static_cast<T>(0));
}

template <typename T>
inline constexpr bool point_in_triangle(
    const detail::vecx<2, T>& p,
    const detail::vecx<2, T>& a,
    const detail::vecx<2, T>& b,
    const detail::vecx<2, T>& c,
    bool include_borders
)
{
    if (!include_borders)
    {
        return point_in_triangle(p, a, b, c);
    }

    const detail::vecx<2, T> ab(b - a);
    const detail::vecx<2, T> bc(c - b);
    const detail::vecx<2, T> ca(a - c);

    const detail::vecx<2, T> ap(p - a);
    const detail::vecx<2, T> bp(p - b);
    const detail::vecx<2, T> cp(p - c);

    const T cross1 = math::cross(ab, ap);
    const T cross2 = math::cross(bc, bp);
    const T cross3 = math::cross(ca, cp);

    return (cross1 >= static_cast<T>(0) && cross2 >= static_cast<T>(0) && cross3 >= static_cast<T>(0));
}

}
}
