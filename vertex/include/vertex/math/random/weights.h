#pragma once

#include <vector>
#include <numeric>

namespace vx {
namespace math {

////////////////////////////////////////////////////////////////////////////////
/// @brief Class for managing a range of weights.
///
/// The weights class provides a convenient way to manage a range of weights.
/// It supports various constructors, accessors, and comparison operators.
////////////////////////////////////////////////////////////////////////////////
class weights
{
public:

    using weight_type = double;

    using value_type = weight_type;
    using size_type = size_t;

    using iterator = typename std::vector<value_type>::const_iterator;
    using const_iterator = iterator;
    using reverse_iterator = typename std::vector<value_type>::const_reverse_iterator;
    using const_reverse_iterator = reverse_iterator;

    ////////////////////////////////////////////////////////////////////////////////
    // constructors
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Constructor with weights range specified by iterators.
    ///
    /// Initializes the class with weights and computes cumulative weights for efficient access.
    ///
    /// @param wfirst Iterator to the beginning of the weights range.
    /// @param wlast Iterator to the end of the weights range.
    ////////////////////////////////////////////////////////////////////////////////
    template <typename IT>
    weights(IT wfirst, IT wlast) : m_weights(std::distance(wfirst, wlast))
    {
        std::partial_sum(wfirst, wlast, m_weights.begin());
    }

    weights(std::initializer_list<value_type> ilist) : weights(ilist.begin(), ilist.end()) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Constructor with weights range specified by an STL container.
    ///
    /// Initializes the class with weights from the specified container and computes
    /// cumulative weights.
    ///
    /// @param w STL container containing weights.
    ////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    weights(const T& w) : weights(w.begin(), w.end()) {}

    weights(const weights&) = default;
    weights(weights&&) noexcept = default;

    ////////////////////////////////////////////////////////////////////////////////
    // destructors
    ////////////////////////////////////////////////////////////////////////////////

    ~weights() = default;

    ////////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ////////////////////////////////////////////////////////////////////////////////

    weights& operator=(const weights&) = default;
    weights& operator=(weights&&) noexcept = default;

    ////////////////////////////////////////////////////////////////////////////////
    // index operators
    ////////////////////////////////////////////////////////////////////////////////

    inline const value_type& operator[](size_type i) const
    {
        return m_weights[i];
    }

    ////////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ////////////////////////////////////////////////////////////////////////////////

    friend inline bool operator==(const weights& w1, const weights& w2)
    {
        return w1.m_weights == w2.m_weights;
    }

    friend inline bool operator!=(const weights& w1, const weights& w2)
    {
        return !(w1 == w2);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // attributes
    ////////////////////////////////////////////////////////////////////////////////

    inline size_type size() const noexcept { return m_weights.size(); }
    inline bool empty() const noexcept { return m_weights.empty(); }

    inline const value_type& back() const noexcept { return m_weights.back(); }
    inline const value_type& front() const noexcept { return m_weights.front(); }

    inline value_type max() const noexcept { return back(); }
    inline value_type min() const noexcept { return front(); }

    ////////////////////////////////////////////////////////////////////////////////
    // iterators
    ////////////////////////////////////////////////////////////////////////////////

    inline const_iterator begin() const noexcept
    {
        return m_weights.cbegin();
    }

    inline const_iterator end() const noexcept
    {
        return m_weights.cend();
    }

    inline const_reverse_iterator rbegin() const noexcept
    {
        return m_weights.crbegin();
    }

    inline const_reverse_iterator rend() const noexcept
    {
        return m_weights.crend();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // stats
    ////////////////////////////////////////////////////////////////////////////////

    inline const_iterator upper_bound(weight_type r) const
    {
        return std::upper_bound(begin(), end(), r);
    }

private:

    std::vector<value_type> m_weights;

};

}
}