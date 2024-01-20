#pragma once

#include <numeric>

#include "rng.h"

namespace vx {
namespace math {

class weighted_selection
{
public:

    using weight_type = double;

    template <typename IT,
        std::enable_if_t<
        detail::is_iterator<IT>::value &&
        std::is_arithmetic<typename std::iterator_traits<IT>::value_type>::value,
        bool> = true>
    weighted_selection(IT wfirst, IT wlast)
        : m_weights(std::distance(wfirst, wlast))
    {
        assert(wfirst != wlast);
        std::partial_sum(wfirst, wlast, m_weights.begin());
    }

    weighted_selection(const weighted_selection&) = default;
    weighted_selection(weighted_selection&&) noexcept = default;

    ~weighted_selection() = default;

    weighted_selection& operator=(const weighted_selection&) = default;
    weighted_selection& operator=(weighted_selection&&) noexcept = default;

    template <typename IT, std::enable_if_t<detail::is_iterator<IT>::value, bool> = true>
    inline const typename std::iterator_traits<IT>::value_type& choice(
        rng& gen,
        IT first,
        IT last
    ) const
    {
        using diff_type = typename std::iterator_traits<IT>::difference_type;
        using choice_type = typename std::iterator_traits<IT>::value_type;

        assert(first != last);

        const diff_type size = std::distance(first, last);

        assert(size == m_weights.size());

        const weight_type r = gen.randf_range(static_cast<weight_type>(0), m_weights.back());
        auto it = std::upper_bound(m_weights.begin(), m_weights.end(), r);

        const diff_type i = std::distance(m_weights.begin(), it);
        std::advance(first, i);

        return *first;
    }

    template <typename IT1, typename IT2,
        std::enable_if_t<
        detail::is_iterator<IT1>::value &&
        detail::is_iterator<IT2>::value,
        bool> = true>
    inline IT2 choices(
        rng& gen,
        IT1 first1,
        IT1 last1,
        IT2 first2,
        IT2 last2
    ) const
    {
        using diff_type = typename std::iterator_traits<IT1>::difference_type;
        using choice_type = typename std::iterator_traits<IT1>::value_type;

        assert(first1 != last1);
        assert(first2 != last2);

        const diff_type size1 = std::distance(first1, last1);
        const diff_type size2 = std::distance(first2, last2);

        assert(size1 == m_weights.size());

        for (diff_type i = 0; i < size2; i++)
        {
            const weight_type r = gen.randf_range(static_cast<weight_type>(0), m_weights.back());
            auto wit = std::upper_bound(m_weights.begin(), m_weights.end(), r);

            IT1 it = first1;
            const diff_type j = std::distance(m_weights.begin(), wit);;

            std::advance(it, j);
            *first2++ = *it;
        }
        
        return first2;
    }

private:

    std::vector<weight_type> m_weights;

};

}
}