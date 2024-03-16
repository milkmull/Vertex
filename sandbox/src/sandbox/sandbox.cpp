#include "sandbox/sandbox.h"

#include "vertex/math/math/type/vec2.h"
#include "vertex/math/math/type/vec3.h"
#include "vertex/math/math/type/vec4.h"

#include "vertex/math/math/type/quat.h"

#include "vertex/math/math/type/mat2x2.h"
#include "vertex/math/math/type/mat2x3.h"
#include "vertex/math/math/type/mat2x4.h"

#include "vertex/math/math/type/mat3x2.h"
#include "vertex/math/math/type/mat3x3.h"
#include "vertex/math/math/type/mat3x4.h"

#include "vertex/math/math/type/mat4x2.h"
#include "vertex/math/math/type/mat4x3.h"
#include "vertex/math/math/type/mat4x4.h"

#include "vertex/math/math/util/to_string.h"

int main()
{
    using namespace vx;

    std::cout << math::to_string(math::vec2()) << std::endl << std::endl;
    std::cout << math::to_string(math::vec3()) << std::endl << std::endl;
    std::cout << math::to_string(math::vec4()) << std::endl << std::endl;

    std::cout << math::to_string(math::quat()) << std::endl << std::endl;

    std::cout << math::to_string(math::mat2x2()) << std::endl << std::endl;
    std::cout << math::to_string(math::mat2x3()) << std::endl << std::endl;
    std::cout << math::to_string(math::mat2x4()) << std::endl << std::endl;

    std::cout << math::to_string(math::mat3x2()) << std::endl << std::endl;
    std::cout << math::to_string(math::mat3x3()) << std::endl << std::endl;
    std::cout << math::to_string(math::mat3x4()) << std::endl << std::endl;

    std::cout << math::to_string(math::mat4x2()) << std::endl << std::endl;
    std::cout << math::to_string(math::mat4x3()) << std::endl << std::endl;
    std::cout << math::to_string(math::mat4x4()) << std::endl << std::endl;


    return 0;
}