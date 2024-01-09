#pragma once

#include <sstream>

#include "base_type_defs.h"

namespace vx {
namespace math {
namespace detail {

template <typename T>
struct vec<2, T, vec_t::vec, val_t::integral>
{
	static_assert(std::is_integral<T>::value, "type T must be integral type");

	// =============== meta ===============

private:

	using FT = typename detail::to_float_type<T>::type;

public:

	using value_type = T;
	using float_value_type = FT;

	using type = vec<2, T, vec_t::vec, val_t::integral>;
	using float_type = vec<2, FT, vec_t::vec, val_t::floating_point>;

	using vec2_type = vec<2, T, vec_t::vec, val_t::integral>;
	using vec3_type = vec<3, T, vec_t::vec, val_t::integral>;
	using vec4_type = vec<4, T, vec_t::vec, val_t::integral>;

	using size_type = length_type;
    static inline constexpr size_type size() noexcept;

	// =============== data ===============

	T x, y;

	// =============== implicit constructors ===============

    inline constexpr vec() noexcept;
    inline constexpr vec(const type& v) noexcept;
	inline constexpr vec(type&&) noexcept = default;

	// =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler) noexcept;
    inline constexpr vec(T x, T y) noexcept;

	// =============== conversion vector constructors ===============

	template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr explicit vec(U scaler);

	template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr vec(U x, U y);

	template <typename U>
    inline constexpr explicit vec(const vecx<2, U>& v);

	template <typename U>
    inline constexpr explicit vec(const vecx<3, U>& v);

	template <typename U>
    inline constexpr explicit vec(const vecx<4, U>& v);

    inline constexpr float_type to_float() const;

	// =============== destructor ===============

	~vec() noexcept = default;

	// =============== assignment operators ===============

    inline constexpr type& operator=(const type& v) noexcept;
	inline constexpr type& operator=(type&&) noexcept = default;

	// =============== accessors ===============

    inline constexpr T& operator[](size_type i);
    inline constexpr const T& operator[](size_type i) const;

	// =============== unary constant operators ===============

    inline constexpr type operator+() const;
    inline constexpr type operator-() const;

	// =============== incrememnt and decrement operators ===============

	// increment (++)

    inline constexpr type& operator++();
    inline constexpr type operator++(int);

	// decrement (--)

    inline constexpr type& operator--();
    inline constexpr type operator--(int);

	// =============== unary arithmetic operators ===============

	// addition (+=)

    inline constexpr type& operator+=(T scaler);
    inline constexpr type& operator+=(const type& v);

	// subtraction (-=)

    inline constexpr type& operator-=(T scaler);
    inline constexpr type& operator-=(const type& v);

	// multiplication (*=)

    inline constexpr type& operator*=(T scaler);
    inline constexpr type& operator*=(const type& v);

	// division (/=)

    inline constexpr type& operator/=(T scaler);
    inline constexpr type& operator/=(const type& v);

	// modulo (%=)

    inline constexpr type& operator%=(T scaler);
    inline constexpr type& operator%=(const type& v);

	// =============== unary bit operators ===============

	// and (&=)

    inline constexpr type& operator&=(T scaler);
    inline constexpr type& operator&=(const type& v);

	// or (|=)

    inline constexpr type& operator|=(T scaler);
    inline constexpr type& operator|=(const type& v);

	// xor (^=)

    inline constexpr type& operator^=(T scaler);
    inline constexpr type& operator^=(const type& v);

	// left shift (<<=)

    inline constexpr type& operator<<=(T scaler);
    inline constexpr type& operator<<=(const type& v);

	// right shift (>>=)

    inline constexpr type& operator>>=(T scaler);
    inline constexpr type& operator>>=(const type& v);

	// =============== iterator ===============

    inline T* begin() noexcept;
    inline const T* begin() const noexcept;
    inline T* end() noexcept;
    inline const T* end() const noexcept;
    inline const T* cbegin() const noexcept;
    inline const T* cend() const noexcept;
    inline auto rbegin() noexcept;
    inline const auto rbegin() const noexcept;
    inline auto rend() noexcept;
    inline const auto rend() const noexcept;
    inline auto crbegin() const noexcept;
    inline auto crend() const noexcept;

	// =============== swizzle ===============

    inline constexpr T swizzle(size_type i) const;
    inline constexpr vec2_type swizzle(size_type i, size_type j) const;
    inline constexpr vec3_type swizzle(size_type i, size_type j, size_type k) const;
    inline constexpr vec4_type swizzle(size_type i, size_type j, size_type k, size_type l) const;

	// =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const;

	// =============== comparison and testing ===============

    inline constexpr void set(T nxy);
    inline constexpr void set(T nx, T ny);

    inline constexpr T min() const;
    inline constexpr T max() const;

    inline constexpr T width() const;
    inline constexpr T height() const;

    inline constexpr FT aspect() const;
    inline constexpr FT angle() const;

	// =============== magnitude ===============

    inline constexpr FT magnitude_squared() const;
    inline constexpr FT magnitude() const;

	// =============== constants ===============

    static inline constexpr type ZERO();
    static inline constexpr type ONE();

    static inline constexpr type RIGHT();
    static inline constexpr type LEFT();

    static inline constexpr type UP();
    static inline constexpr type DOWN();

};

// =============== comparison operators ===============

template <typename T>
inline constexpr bool operator==(const veci<2, T>& v1, const veci<2, T>& v2);

template <typename T>
inline constexpr bool operator!=(const veci<2, T>& v1, const veci<2, T>& v2);

template <typename T>
inline constexpr bool operator<(const veci<2, T>& v1, const veci<2, T>& v2);

template <typename T>
inline constexpr bool operator>(const veci<2, T>& v1, const veci<2, T>& v2);

template <typename T>
inline constexpr bool operator<=(const veci<2, T>& v1, const veci<2, T>& v2);

template <typename T>
inline constexpr bool operator>=(const veci<2, T>& v1, const veci<2, T>& v2);

// =============== binary arithmetic operators ===============

// addition (+)

template <typename T>
inline constexpr veci<2, T> operator+(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator+(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator+(const veci<2, T>& v1, const veci<2, T>& v2);

// subtraction (-)

template <typename T>
inline constexpr veci<2, T> operator-(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator-(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator-(const veci<2, T>& v1, const veci<2, T>& v2);

// multiplication (*)

template <typename T>
inline constexpr veci<2, T> operator*(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator*(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator*(const veci<2, T>& v1, const veci<2, T>& v2);

// division (/)

template <typename T>
inline constexpr veci<2, T> operator/(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator/(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator/(const veci<2, T>& v1, const veci<2, T>& v2);

// modulo (%)

template <typename T>
inline constexpr veci<2, T> operator%(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator%(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator%(const veci<2, T>& v1, const veci<2, T>& v2);

// =============== binary bit operators ===============

// and (&)

template <typename T>
inline constexpr veci<2, T> operator&(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator&(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator&(const veci<2, T>& v1, const veci<2, T>& v2);

// or (|)

template <typename T>
inline constexpr veci<2, T> operator|(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator|(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator|(const veci<2, T>& v1, const veci<2, T>& v2);

// xor (^)

template <typename T>
inline constexpr veci<2, T> operator^(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator^(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator^(const veci<2, T>& v1, const veci<2, T>& v2);

// left shift (<<)

template <typename T>
inline constexpr veci<2, T> operator<<(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator<<(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator<<(const veci<2, T>& v1, const veci<2, T>& v2);

// right shift (>>)

template <typename T>
inline constexpr veci<2, T> operator>>(const veci<2, T>& v, T scaler);

template <typename T>
inline constexpr veci<2, T> operator>>(T scaler, const veci<2, T>& v);

template <typename T>
inline constexpr veci<2, T> operator>>(const veci<2, T>& v1, const veci<2, T>& v2);

// not (~)

template <typename T>
inline constexpr veci<2, T> operator~(const veci<2, T>& v);

}
}
}

#include "_impl/vec2i_type.inl"