#include <cassert>
#include <cstring>
#include <forward_list>
#include <memory>

#include "vertex/std/io.hpp"
#include "vertex/std/array.hpp"
#include "vertex/std/shared_ptr.hpp"

using namespace vx;

int main()
{
    auto res = vx::make_shared<int>(42);
    assert(res);

    auto ptr = res.value();
    constexpr auto x = sizeof(vx::shared_ptr<int>);

    return 0;
}
