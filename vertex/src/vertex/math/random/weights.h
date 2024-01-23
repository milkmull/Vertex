#pragma once

#include <vector>
#include <numeric>

#include "vertex/detail/setup.h"
#include "vertex/detail/type_traits.h"

namespace vx {
namespace math {

/**
 * @class weights
 * @brief Class for managing a range of weights.
 *
 * The weights class provides a convenient way to manage a range of weights.
 * It supports various constructors, accessors, and comparison operators.
 */
class weights
{
public:

    using weight_type = double;

    using value_type = weight_type;
    using size_type = size_t;

    using iterator = typename std::vector<value_type>::iterator;
    using const_iterator = typename std::vector<value_type>::const_iterator;
    using reverse_iterator = typename std::vector<value_type>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<value_type>::const_reverse_iterator;

    // =============== constructors ===============

    /**
     * @brief Constructor with weights range specified by iterators.
     * 
     * Initializes the class with weights and computes cumulative weights for efficient access.
     * 
     * @param wfirst Iterator to the beginning of the weights range.
     * @param wlast Iterator to the end of the weights range.
     */
    template <typename IT,
        typename std::enable_if<
        ::vx::detail::is_iterator<IT>::value&&
        std::is_arithmetic<typename std::iterator_traits<IT>::value_type>::value,
        bool>::type = true>
    weights(IT wfirst, IT wlast)
        : m_weights(std::distance(wfirst, wlast))
    {
        assert(wfirst != wlast);
        std::partial_sum(wfirst, wlast, m_weights.begin());
    }

    /**
     * @brief Constructor with weights range specified by an STL container.
     * 
     * Initializes the class with weights from the specified container and computes cumulative weights.
     * 
     * @param w STL container containing weights.
     */
    template <typename T,
        typename std::enable_if<
        ::vx::detail::is_stl_container<T>::value &&
        std::is_arithmetic<typename T::value_type>::value,
        bool>::type = true>
    weights(const T& w)
        : weights(w.begin(), w.end()) {}

    weights(const weights&) = default;

    weights(weights&& other) noexcept
        : m_weights(std::move(other.m_weights)) {}

    // =============== destructor ===============

    ~weights() = default;

    // =============== assignment ===============

    weights& operator=(const weights&) = default;
    weights& operator=(weights&&) noexcept = default;

    // =============== accessors ===============

    inline value_type& operator[](size_type i)
    {
        assert(i < m_weights.size());
        return m_weights[i];
    }

    // =============== comparison ===============

    friend inline bool operator==(
        const weights& w1,
        const weights& w2
    )
    {
        return w1.m_weights == w2.m_weights;
    }

    friend inline bool operator!=(
        const weights& w1,
        const weights& w2
    )
    {
        return !(w1 == w2);
    }

    // =============== attributes ===============

    inline size_type size() const noexcept { return m_weights.size(); }
    inline bool empty() const noexcept { return m_weights.empty(); }

    inline const value_type& back() const noexcept { return m_weights.back(); }
    inline const value_type& front() const noexcept { return m_weights.back(); }

    inline value_type max() const noexcept { return back(); }
    inline value_type min() const noexcept { return front(); }

    // =============== iterator ===============

    inline iterator begin() noexcept
    {
        return m_weights.begin();
    }

    inline const_iterator begin() const noexcept
    {
        return m_weights.cbegin();
    }

    inline iterator end() noexcept
    {
        return m_weights.end();
    }

    inline const_iterator end() const noexcept
    {
        return m_weights.cend();
    }

    inline const_iterator cbegin() const noexcept
    {
        return m_weights.cbegin();
    }

    inline const_iterator cend() const noexcept
    {
        return m_weights.cend();
    }

    inline reverse_iterator rbegin() noexcept
    {
        return m_weights.rbegin();
    }

    inline const_reverse_iterator rbegin() const noexcept
    {
        return m_weights.crbegin();
    }

    inline reverse_iterator rend() noexcept
    {
        return m_weights.rend();
    }

    inline const_reverse_iterator rend() const noexcept
    {
        return m_weights.crend();
    }

    inline const_reverse_iterator crbegin() const noexcept
    {
        return m_weights.crbegin();
    }

    inline const_reverse_iterator crend() const noexcept
    {
        return m_weights.crend();
    }

private:

    std::vector<value_type> m_weights;

};

}
}