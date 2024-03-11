#pragma once

#include <cstdint>
#include <type_traits>

namespace vx {
namespace math {

using bscale = bool;
using dscale = double;
using fscale = float;
using iscale = int32_t;
using uscale = uint32_t;

template <typename T> struct is_scaler : std::false_type {};

template <> struct is_scaler<bscale> : std::true_type {};
template <> struct is_scaler<dscale> : std::true_type {};
template <> struct is_scaler<fscale> : std::true_type {};
template <> struct is_scaler<iscale> : std::true_type {};
template <> struct is_scaler<uscale> : std::true_type {};

template <typename T> struct is_numeric : std::false_type {};

template <> struct is_numeric<dscale> : std::true_type {};
template <> struct is_numeric<fscale> : std::true_type {};
template <> struct is_numeric<iscale> : std::true_type {};
template <> struct is_numeric<uscale> : std::true_type {};

template <typename T> struct is_integral : std::false_type {};

template <> struct is_integral<iscale> : std::true_type {};
template <> struct is_integral<uscale> : std::true_type {};

template <typename T> struct is_floating_point : std::false_type {};

template <> struct is_floating_point<dscale> : std::true_type {};
template <> struct is_floating_point<fscale> : std::true_type {};

}
}