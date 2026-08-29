#pragma once

#include <ratio>

#include "vertex/config/language_config.hpp"

namespace vx {

//=========================================================================
// growth policy
//=========================================================================

template <size_t N, size_t D = 1>
struct ratio_growth_policy
{
private:

    using ratio = std::ratio<N, D>;
    VX_STATIC_ASSERT_MSG(ratio::num >= ratio::den, "Growth rate must be >= 1");

public:

    template <typename ST>
    static ST next_capacity(
        ST current_capacity,
        ST required_capacity,
        ST max_capacity) noexcept
    {
        // Fast path: growth factor of 1
        VX_IF_CONSTEXPR (ratio::num == 1 && ratio::den == 1)
        {
            if (required_capacity > max_capacity)
            {
                return max_capacity;
            }

            return required_capacity;
        }
        else
        {
            current_capacity = current_capacity ? current_capacity : 1;

            ST new_capacity;

            // Fast path: ratios of the form (den + 1) / den  (3/2, 4/3, 5/4, ...)
            VX_IF_CONSTEXPR (ratio::num == ratio::den + 1)
            {
                constexpr ST den = static_cast<ST>(ratio::den);

                if (current_capacity > max_capacity - current_capacity / den)
                {
                    return max_capacity;
                }

                new_capacity = current_capacity + current_capacity / den;
            }
            else
            {
                // general case
                if (current_capacity > max_capacity / ratio::num)
                {
                    return max_capacity;
                }

                new_capacity = current_capacity * ratio::num / ratio::den;
            }

            if (new_capacity < required_capacity)
            {
                new_capacity = required_capacity;
            }

            return new_capacity;
        }
    }
};

} // namespace vx
