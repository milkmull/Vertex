#include "sandbox/sandbox.h"

#include "vertex/math/math/type/vec2.h"
#include "vertex/math/math/type/mat2x2.h"
#include "vertex/math/math/type/mat2x4.h"
#include "vertex/math/math/type/mat4x2.h"
#include "vertex/math/math/type/mat4x4.h"
#include "vertex/math/math/util/to_string.h"

int main()
{
    using namespace vx;

    math::mat2x4 m1;
    math::mat4x2 m2;
    math::mat4x4 m3;

    std::cout << math::to_string(m1) << std::endl << std::endl;
    std::cout << math::to_string(m2) << std::endl << std::endl;
    std::cout << math::to_string(m3) << std::endl << std::endl;


    return 0;
}