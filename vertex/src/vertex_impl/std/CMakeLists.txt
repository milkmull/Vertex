# Source files for vertex/src/vertex_impl/std
file(GLOB VX_STD_SOURCE_FILES

    # Error
    "${CMAKE_CURRENT_SOURCE_DIR}/error/fatal.cpp"
)

target_sources(Vertex PRIVATE ${VX_STD_SOURCE_FILES})
