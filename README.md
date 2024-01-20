# Vertex Framework

Vertex is a C++ game development framework that provides essential building blocks for developing games.
Vertex comes with many helpful tools:
* Math library including essential functions and structures for 2D and 3D development such as vectors, quaternions, and matrices.
* (coming soon) Image library for image io and manipulation oprtations.
* (coming soon) Logging and profiling tools to help streamline the development process.
* (coming soon) Windowing tools for cross platform window generation and input handling with GLSL backend.
* (coming soon) Cross platform graphcs api with support for both Vulkan and OpenGL backend.

## Getting Started

### Prerequisites

Before you start, make sure you have CMake installed on your system. You can download it from [CMake's official website](https://cmake.org/download/).

### Clone the Repository

Clone the Vertex repository to your local machine:

```bash
git clone https://github.com/milkmull/Vertex.git
cd Vertex
```

### Configure and Generate the Project Files

Create a build directory and run CMake to configure and build the project:

```bash
cmake -DVX_BUILD_SANDBOX=ON -B build .
cmake --build build
```

#### CMake Parameters

You can customize the build process using the following CMake parameters:

- **VX_BUILD_SHARED_LIBS** (Default: OFF)
  - Set to ON to build shared libraries instead of static libraries.

- **VX_BUILD_EXAMPLES** (Default: OFF)
  - Set to ON to include building examples.

- **VX_BUILD_TESTS** (Default: OFF)
  - Set to ON to include building tests.

- **VX_BUILD_SANDBOX** (Default: ON)
  - Set to OFF to exclude building Sandbox.

### Sandbox

Vertex comes with the option to build an internal project called sandbox:

https://github.com/milkmull/Vertex/tree/main/sandbox

Sandbox is an easy way to get started playing around with features without having to build your own project. Sandbox will be compiled along with Vertex and is configured to include and link against Vertex automatically. An executable will be generated for sandbox in the **bin** directory.

## Installation

coming soon

# Vertex Math

The math library in Vertex provides a set of functions and structures for handling mathematical operations in game development. It includes features such as vector operations, matrix transformations, and more.

### Usage

Here are some basic examples demonstrating the usage of the Vertex math library:

```cpp
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
```