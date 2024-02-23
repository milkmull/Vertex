#include <utility>

namespace vx {
namespace math {
namespace detail {

template<typename T, typename U>
struct result_of
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, "T and U must be arithmetic types");
    using type = decltype(std::declval<T>() * std::declval<U>());
};

}
}
}