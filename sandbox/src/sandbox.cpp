#include <cassert>
#include <cstring>
#include <forward_list>

#include "vertex/std/io.hpp"
#include "vertex/std/array.hpp"

using namespace vx;

int main()
{
    array<int, 5> arr = { 0, 1, 2, 3, 4 };
    auto x = arr.at(2);
    if (x)
    {
        x.value() = -x.value();
        println("arr[2]: ", x.value());
    }
    else
    {
        println("arr[2] error: ", (int)x.error());
    }

    return 0;
}
