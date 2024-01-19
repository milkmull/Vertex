#pragma once

#include <random>
#include <chrono>
#include <cassert>

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
	inline constexpr T randi_range(T min, T max)
	{
		std::uniform_int_distribution<T> dist(min, max);
		return dist(m_rng);
	}

	template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
	inline constexpr T randf_range(T min, T max)
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

	template <typename IT>
	inline constexpr void shuffle(IT begin, IT end)
	{
		std::shuffle(begin, end, m_rng);
	}

	template <typename IT>
	inline constexpr typename std::iterator_traits<IT>::value_type choice(IT begin, IT end)
	{
		using diff_t = typename std::iterator_traits<IT>::difference_type;
		assert(begin != end);

		const diff_t size = std::distance(begin, end);
		std::uniform_int_distribution<diff_t> dist(0, size - 1);

		const diff_t i = dist(m_rng);
		std::advance(begin, i);

		return *begin;
	}

	template <typename IT>
	inline constexpr std::vector<typename std::iterator_traits<IT>::value_type> choices(
		IT begin,
		IT end,
		size_t k
	)
	{
		using diff_t = typename std::iterator_traits<IT>::difference_type;
		assert(begin != end);

		const diff_t size = std::distance(begin, end);
		std::uniform_int_distribution<diff_t> dist(
			static_cast<diff_t>(0),
			size - static_cast<diff_t>(1)
		);

		std::vector<typename std::iterator_traits<IT>::value_type> choices;
		choices.reserve(k);

		for (size_t i = 0; i < k; i++)
		{
			IT it = begin;
			const diff_t j = dist(m_rng);

			std::advance(it, j);
			choices.push_back(*it);
		}

		return choices;
	}

	template <typename IT, typename WIT>
	inline constexpr void choices(IT begin, IT end, WIT wbegin, WIT wend, size_t k)
	{
		using weight_type = typename std::iterator_traits<WIT>::value_type;
		assert(begin != end);
		assert(wbegin != wend);

		const size_t size = std::distance(begin, end);
		const size_t wsize = std::distance(wbegin, wend);

		if (size != wsize)
		{
			throw std::invalid_argument("Sizes are not equal");
		}

		weight_type totalWeight = static_cast<weight_type>(0);
		for (WIT wit = wbegin; wit != wend; wit++)
        {
            totalWeight += *wit;
        }

		std::uniform_int_distribution<size_t> dist(0, totalWeight - 1);

		for (size_t i = 0; i < k; i++)
		{
			size_t rnd = dist(m_rng);

			for (size_t j = 0; j < size; j++)
			{
				WIT wit = wbegin;
				std::advance(wit, j);

				if (rnd < *wit)
				{
					IT it = begin;
					std::advance(it, j);
					choices.push_back(*it);

					break;
				}

				rnd -= *wit;
			}
		}
	}

private:

	std::mt19937 m_rng;

};

}
}