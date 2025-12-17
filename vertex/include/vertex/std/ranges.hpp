#pragma once

#include <iterator>

namespace vx {
namespace range {

template <typename It>
auto distance(It first, It last)
{
    return last - first;
}

template <class It>
void advance(It& it,
    typename std::iterator_traits<It>::difference_type n)
{

}

} // namespace range
} // namespace vx
