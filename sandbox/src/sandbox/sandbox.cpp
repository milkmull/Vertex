#include <vertex/math/math.h>
#include <iostream>

int main()
{
    // Create vectors
    vx::vec2 v2(1.0f, 2.0f);
    vx::vec3 v3(1.0f, 2.0f, 3.0f);
    vx::vec4 v4(1.0f, 2.0f, 3.0f, 4.0f);

    // Print vector
    std::cout << v2.to_string() << std::endl;

    // Normalize vector
    v2 = vx::math::normalize(v2);

    // See if the magnitude is 1.
    if (vx::math::is_equal_approx(v2.magnitude(), 1.0f))
    {
        std::cout << "vector " << v2.to_string() << " is normalized." << std::endl;
    }

    // Check for approximate equallity
    vx::vec3 v32 = v3 + 0.000001f;
    if (vx::math::is_equal_approx(v3, v32))
    {
        std::cout << v3.to_string() << " and " << v32.to_string() << " are approximately equal." << std::endl;
    }
}