#include "vertex/math/types.hpp"

#include <type_traits>

using namespace vx::math;

int main(int argc, char* argv[])
{
    constexpr bool x = std::is_same<vec2b, vec2b::type>::value;
}
