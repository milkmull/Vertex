# Vertex Framework

**Vertex** is a modern C++ systems framework designed for high-performance application and game development. It provides clean, portable abstractions for working with the operating system, utilities, and foundational components.

## Key Features

- **Cross-platform file I/O** and filesystem utilities.
- **Process module** for running and monitoring child processes.
- **Random number generation** with a robust RNG module.
- **Basic string manipulation utilities.**
- *(Upcoming)* Abstractions for windowing, input, and audio.

---

## Prerequisites

Before building Vertex, make sure you have:

- **CMake** (3.15 or newer)  
  [https://cmake.org/download/](https://cmake.org/download/)

- **A C++17-capable compiler**  
  Examples:
  - GCC 9+ — [https://gcc.gnu.org](https://gcc.gnu.org)
  - Clang 10+ — [https://clang.llvm.org](https://clang.llvm.org)
  - MSVC (Visual Studio 2019 or newer) — [https://visualstudio.microsoft.com](https://visualstudio.microsoft.com)

- *(Optional)* **CMake GUI** – available on Windows, macOS, and Linux.

---

## Cloning the Repository

To get started, clone the Vertex repository to your local machine:

```bash
git clone https://github.com/milkmull/Vertex.git
cd Vertex
```

---

## Building the Library

Vertex uses **CMake** for configuration. You can build it from the command line or with CMake GUI.

### Command Line (Cross-Platform)

#### Linux / macOS

```bash
# Create and enter a build directory
mkdir build && cd build

# Configure with default settings (Debug build with Sandbox enabled)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
cmake --build .
```

#### Windows (Command Prompt or PowerShell)

```powershell
# Create and enter a build directory
mkdir build; cd build

# Configure for Visual Studio 2022
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug

# Build using MSBuild
cmake --build . --config Debug
```

---

### CMake GUI (All Platforms)

1. Open CMake GUI.
2. Set **source folder** to the root of the Vertex repo.
3. Set **build folder** (e.g. `Vertex/build`).
4. Click **Configure** and select your compiler/toolchain.
5. Adjust build options as needed (see below).
6. Click **Generate**, then open/build the project.

---

## CMake Configuration Options

These options can be set via `-D<option>=<value>` on the command line, or toggled in CMake GUI.

| Option                 | Default | Description                                             |
|------------------------|---------|---------------------------------------------------------|
| `CMAKE_BUILD_TYPE`     | Debug   | Set to `Release`, `RelWithDebInfo`, etc.                |
| `VX_BUILD_SANDBOX`     | ON      | Build the sandbox test application.                     |
| `VX_BUILD_TESTS`       | OFF     | Build unit tests.                                       |
| `VX_INSTALL_TESTS`     | OFF     | Install test executables during `cmake --install`.      |
| `VX_DUMMY_PLATFORM`    | OFF     | Use the dummy OS backend (for low-level testing only).  |

Example build with release mode and tests:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DVX_BUILD_TESTS=ON
cmake --build .
```

---

## Sandbox Application

When `VX_BUILD_SANDBOX=ON` (default), the build includes the **sandbox** app:

- Located in [`sandbox/`](https://github.com/milkmull/Vertex/tree/main/sandbox)
- Automatically links to the Vertex library
- Compiles into `bin/` on build
- A fast way to explore and test features interactively

---

## Installation

Install the built library and headers using:

```bash
cmake --install . --prefix <install_path> --config <Debug\Release>
```

### Example:

```bash
# Linux/macOS
cmake --install . --prefix /usr/local --config Debug
cmake --install . --prefix /usr/local --config Release
# Windows
cmake --install . --prefix "C:\Program Files\Vertex" --config Debug
cmake --install . --prefix "C:\Program Files\Vertex" --config Release
```

This installs:

- `lib/` — static/shared library
- `include/` — public headers
- `cmake/` — package config for `find_package(Vertex REQUIRED)`

---

## License

[MIT](./LICENSE)
