# Source files for vertex/src/vertex_impl/std
file(GLOB VX_STD_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/error.cpp"
)

target_sources(Vertex PRIVATE ${VX_STD_SOURCE_FILES})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simd_algorithms")
