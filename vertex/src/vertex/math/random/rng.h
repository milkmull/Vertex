#pragma once

#include <random>
#include <chrono>

#include "weights.h"

namespace vx {
namespace math {

// For now, rng results may varry accross platforms due to implimentation differences

/**
 * @class rng
 * @brief Random Number Generator (RNG) class for generating pseudo-random numbers.
 *
 * The rng class provides a set of functions for generating random numbers of different types and distributions.
 * It supports seeding, generating integers, floats, booleans, and performing various container operations like shuffling.
 *
 * @note The default constructor automatically seeds the RNG using the current system clock.
 *
 * @note The RNG class uses the Mersenne Twister engine (std::mt19937) for random number generation.
 */
class rng
{
public:

    // =============== constructors ===============

    rng() { seed(); }
    rng(uint32_t seed) { m_rng.seed(seed); }

    rng(const rng&) = default;
    rng(rng&&) noexcept = default;

    // =============== destructor ===============

    ~rng() = default;

    // =============== assignment ===============

    rng& operator=(const rng&) = default;
    rng& operator=(rng&&) noexcept = default;

    // =============== seeding ===============

    /**
     * @brief Seeds the RNG with a specified value.
     * 
     * @param seed The seed value.
     */
    inline void seed(uint32_t seed)
    {
        m_rng.seed(seed);
    }

    /**
     * @brief Seeds the RNG using the current system clock and returns the generated seed.
     * 
     * @return The generated seed value.
     */
    inline uint32_t seed()
    {
        const uint32_t s = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
        seed(s);
        return s;
    }

    // =============== generators ===============

    /**
     * @brief Generates a random integer.
     * 
     * @return The generated random integer.
     */
    inline uint32_t randi()
    {
        return m_rng();
    }

    /**
     * @brief Generates a random floating-point number between 0.0 and 1.0.
     * 
     * @return The generated random float.
     */
    inline float randf()
    {
        return randf_range(0.0f, 1.0f);
    }

    /**
     * @brief Generates a random boolean value.
     * 
     * @return The generated random boolean.
     */
    inline bool randb()
    {
        return static_cast<bool>(randi_range(0, 1));
    }

    /**
     * @brief Generates a random integer within the specified range.
     * 
     * @param min The minimum value of the range.
     * @param max The maximum value of the range.
     * @return The generated random integer within the specified range.
     */
    template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
    inline T randi_range(T min, T max)
    {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(m_rng);
    }

    /**
     * @brief Generates a random floating-point number within the specified range.
     * 
     * @param min The minimum value of the range.
     * @param max The maximum value of the range.
     * @return The generated random floating-point number within the specified range.
     */
    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
    inline T randf_range(T min, T max)
    {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(m_rng);
    }

    /**
     * @brief Generates a random number from a normal distribution with specified mean and deviation.
     * 
     * @param mean The mean value of the normal distribution (default is 0).
     * @param deviation The standard deviation of the normal distribution (default is 1).
     * @return The generated random number from the normal distribution.
     */
    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
    inline float rand_norm(T mean = static_cast<T>(0), T deviation = static_cast<T>(1))
    {
        std::normal_distribution<T> dist(mean, deviation);
        return dist(m_rng);
    }

    // =============== container operations ===============

    /**
     * @brief Shuffles the elements in the specified range using the RNG.
     * 
     * @param begin Iterator to the beginning of the range.
     * @param end Iterator to the end of the range.
     */
    template <typename IT, typename std::enable_if<::vx::tools::is_iterator<IT>::value, bool>::type = true>
    inline void shuffle(IT begin, IT end)
    {
        std::shuffle(begin, end, m_rng);
    }

    /**
     * @brief Selects a random element from the specified range using the RNG.
     * 
     * @param first Iterator to the beginning of the range.
     * @param last Iterator to the end of the range.
     * @return A reference to the randomly selected element.
     */
    template <typename IT, typename std::enable_if<::vx::tools::is_iterator<IT>::value, bool>::type = true>
    inline typename std::iterator_traits<IT>::value_type& choice(IT first, IT last)
    {
        using diff_type = typename std::iterator_traits<IT>::difference_type;

        assert(first != last);

        const diff_type size = std::distance(first, last);
        std::uniform_int_distribution<diff_type> dist(0, size - 1);

        const diff_type i = dist(m_rng);
        std::advance(first, i);

        return *first;
    }

    /**
     * @brief Selects multiple random elements from one range and assigns them to another range using the RNG.
     * 
     * @param first1 Iterator to the beginning of the source range.
     * @param last1 Iterator to the end of the source range.
     * @param first2 Iterator to the beginning of the destination range.
     * @param last2 Iterator to the end of the destination range.
     * @return Iterator pointing to the end of the destination range after assignment.
     */
    template <typename IT1, typename IT2,
        typename std::enable_if<
        ::vx::tools::is_iterator<IT1>::value &&
        ::vx::tools::is_iterator<IT2>::value,
        bool>::type = true>
    inline IT2 choices(
        IT1 first1,
        IT1 last1,
        IT2 first2,
        IT2 last2
    )
    {
        using diff_type = typename std::iterator_traits<IT1>::difference_type;

        assert(first1 != last1);
        assert(first2 != last2);

        const diff_type size1 = std::distance(first1, last1);
        const diff_type size2 = std::distance(first2, last2);

        std::uniform_int_distribution<diff_type> dist(0, size1 - 1);

        for (diff_type i = 0; i < size2; i++)
        {
            IT1 it = first1;
            const diff_type j = dist(m_rng);

            std::advance(it, j);
            *first2++ = *it;
        }

        return first2;
    }

    /**
     * @brief Chooses a single element from the specified range based on weights.
     * 
     * @param first Iterator to the beginning of the range.
     * @param last Iterator to the end of the range.
     * @param w Weights object specifying the weights for each element.
     * @return A const reference to the selected element.
     */
    template <typename IT, typename std::enable_if<::vx::tools::is_iterator<IT>::value, bool>::type = true>
    inline const typename std::iterator_traits<IT>::value_type& weighted_choice(
        IT first,
        IT last,
        const weights& w
    )
    {
        using diff_type = typename std::iterator_traits<IT>::difference_type;
        using weight_type = typename weights::weight_type;

        assert(first != last);

        const diff_type size = std::distance(first, last);

        assert(size == static_cast<diff_type>(w.size()));

        std::uniform_real_distribution<weight_type> dist(static_cast<weight_type>(0), w.back());
        const weight_type r = dist(m_rng);
        auto it = std::upper_bound(w.begin(), w.end(), r);

        // If we fail here it means the weights are all <= 0
        assert(it != w.end());

        const diff_type i = std::distance(w.begin(), it);
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
     * @param w Weights object specifying the weights for each element.
     * @return Iterator pointing to the end of the destination range after assignment.
     */
    template <typename IT1, typename IT2,
        typename std::enable_if<
        ::vx::tools::is_iterator<IT1>::value &&
        ::vx::tools::is_iterator<IT2>::value,
        bool>::type = true>
    inline IT2 weighted_choices(
        IT1 first1,
        IT1 last1,
        IT2 first2,
        IT2 last2,
        const weights& w
    )
    {
        using diff_type = typename std::iterator_traits<IT1>::difference_type;
        using weight_type = typename weights::weight_type;

        assert(first1 != last1);
        assert(first2 != last2);

        const diff_type size2 = std::distance(first2, last2);

        assert(std::distance(first1, last1) == static_cast<diff_type>(w.size()));

        std::uniform_real_distribution<weight_type> dist(static_cast<weight_type>(0), w.back());

        for (diff_type i = 0; i < size2; i++)
        {
            const weight_type r = dist(m_rng);
            auto wit = std::upper_bound(w.begin(), w.end(), r);

            // If we fail here it means the weights are all <= 0
            assert(wit != w.end());

            IT1 it = first1;
            const diff_type j = std::distance(w.begin(), wit);;

            std::advance(it, j);
            *first2++ = *it;
        }

        return first2;
    }

private:

    std::mt19937 m_rng;

};

}
}