#pragma once

#include <numeric>

#include "rng.h"

namespace vx {
namespace math {

/**
 * @class weighted_selection
 * @brief Class for weighted random selection from a range.
 *
 * The weighted_selection class allows for weighted random selection from a range of elements.
 * It supports both single and multiple selections with weights provided during construction.
 */
class weighted_selection
{
public:

    using weight_type = double;

    // =============== constructors ===============

    /**
     * @brief Constructor with weights range.
     * 
     * Initializes the class with weights and computes cumulative weights for efficient selection.
     * 
     * @param wfirst Iterator to the beginning of the weights range.
     * @param wlast Iterator to the end of the weights range.
     */
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

    // =============== destructor ===============

    ~weighted_selection() = default;

    // =============== assignment ===============

    weighted_selection& operator=(const weighted_selection&) = default;
    weighted_selection& operator=(weighted_selection&&) noexcept = default;

    // =============== container operations ===============

    /**
     * @brief Chooses a single element from the specified range based on weights.
     * 
     * @param first Iterator to the beginning of the range.
     * @param last Iterator to the end of the range.
     * @param gen Reference to the RNG for random number generation.
     * @return A const reference to the selected element.
     */
    template <typename IT, std::enable_if_t<detail::is_iterator<IT>::value, bool> = true>
    inline const typename std::iterator_traits<IT>::value_type& choice(
        IT first,
        IT last,
        rng& gen
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

    /**
     * @brief Chooses multiple elements from one range and assigns them to another range based on weights.
     * 
     * @param first1 Iterator to the beginning of the source range.
     * @param last1 Iterator to the end of the source range.
     * @param first2 Iterator to the beginning of the destination range.
     * @param last2 Iterator to the end of the destination range.
     * @param gen Reference to the RNG for random number generation.
     * @return Iterator pointing to the end of the destination range after assignment.
     */
    template <typename IT1, typename IT2,
        std::enable_if_t<
        detail::is_iterator<IT1>::value &&
        detail::is_iterator<IT2>::value,
        bool> = true>
    inline IT2 choices(
        IT1 first1,
        IT1 last1,
        IT2 first2,
        IT2 last2,
        rng& gen
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