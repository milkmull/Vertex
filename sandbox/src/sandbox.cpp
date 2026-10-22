#include <cassert>
#include <cstring>
#include <forward_list>
#include <memory>

#include "vertex/std/hash_set.hpp"

using namespace vx;

int main()
{
    vx::unordered_set<int> set;
    set.insert(5);

    const bool x1 = set.contains(5);
    set.erase(5);
    const bool x2 = set.contains(5);

    set.clear();
    set.clear_and_deallocate();

    return 0;
}
