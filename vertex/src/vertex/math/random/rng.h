#pragma once

#include <random>
#include <chrono>
#include <cassert>

#include "detail/type_traits.h"

namespace vx {
namespace math {

// For now, rng results may varry accross platforms due to floating point math

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

	inline void seed(uint32_t seed)
	{
		m_rng.seed(seed);
	}

	inline uint32_t seed()
	{
		const uint32_t s = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
		seed(s);
		return s;
	}

	// =============== generators ===============

	inline uint32_t randi()
	{
		return m_rng();
	}

	inline float randf()
	{
		return randf_range(0.0f, 1.0f);
	}

	inline bool randb()
	{
		return static_cast<bool>(randi_range(0, 1));
	}

	template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
	inline T randi_range(T min, T max)
	{
		std::uniform_int_distribution<T> dist(min, max);
		return dist(m_rng);
	}

	template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
	inline T randf_range(T min, T max)
	{
		std::uniform_real_distribution<T> dist(min, max);
		return dist(m_rng);
	}

	template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
	inline float rand_norm(T mean = static_cast<T>(0), T deviation = static_cast<T>(1))
	{
		std::normal_distribution<T> dist(mean, deviation);
		return dist(m_rng);
	}

	// =============== container operations ===============

	template <typename IT, std::enable_if_t<detail::is_iterator<IT>::value, bool> = true>
	inline void shuffle(IT begin, IT end)
	{
		std::shuffle(begin, end, m_rng);
	}

	template <typename IT, std::enable_if_t<detail::is_iterator<IT>::value, bool> = true>
	inline typename std::iterator_traits<IT>::value_type& choice(IT first, IT last)
	{
		using diff_type = typename std::iterator_traits<IT>::difference_type;
        using choice_type = typename std::iterator_traits<IT>::value_type;

        assert(first != last);

		const diff_type size = std::distance(first, last);
		std::uniform_int_distribution<diff_type> dist(0, size - 1);

		const diff_type i = dist(m_rng);
		std::advance(first, i);

		return *first;
	}

    template <typename IT1, typename IT2,
        std::enable_if_t<
        detail::is_iterator<IT1>::value &&
        detail::is_iterator<IT2>::value,
        bool> = true>
    inline IT2 choices(
        IT1 first1,
        IT1 last1,
        IT2 first2,
        IT2 last2
    )
	{
		using diff_type = typename std::iterator_traits<IT1>::difference_type;
        using choice_type = typename std::iterator_traits<IT1>::value_type;

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

private:

	std::mt19937 m_rng;

};

}
}