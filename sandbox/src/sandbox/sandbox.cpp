#include "sandbox/sandbox.h"

#include "vertex/math/math/type/vec2.h"
#include "vertex/math/math/util/hash.h"

#include <unordered_set>

int main()
{
    using namespace vx;

    math::vec2 v1;
    math::vec2 v2;

    std::unordered_set<math::vec2> set;
    set.insert({ 0, 0 });
    set.insert({ 1, 0 });
    set.insert({ 0, 1 });
    set.insert({ 1, 1 });

    std::cout << set.size();


    return 0;
}