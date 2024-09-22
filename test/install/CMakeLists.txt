cmake_minimum_required(VERSION 3.10)
project(VertexInstallTest)

# Find the Vertex package
find_package(Vertex REQUIRED)

# Add an executable or library
add_executable(VertexInstallTest main.cpp)

# Define an option to choose between static and shared libraries
option(USE_STATIC_VERTEX "Link against the static version of Vertex" OFF)

# Check if the headers were found
if(VERTEX_HEADERS_FOUND)
    message(STATUS "Vertex headers found at ${VERTEX_INCLUDE_DIR}")
    target_include_directories(VertexInstallTest PRIVATE "${VERTEX_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Vertex headers not found.")
endif()

# Link against Vertex shared or static libraries based on the option
if(USE_STATIC_VERTEX)
    if(VERTEX_STATIC_LIBRARIES)
        message(STATUS "Linking against Vertex static library.")
        target_link_libraries(VertexInstallTest PRIVATE "${VERTEX_STATIC_LIBRARIES}")
    else()
        message(FATAL_ERROR "Static libraries for Vertex are not available.")
    endif()
else()
    if(VERTEX_SHARED_LIBRARIES)
        message(STATUS "Linking against Vertex shared library.")
        target_link_libraries(VertexInstallTest PRIVATE "${VERTEX_SHARED_LIBRARIES}")
    else()
        message(FATAL_ERROR "Shared libraries for Vertex are not available.")
    endif()
endif()