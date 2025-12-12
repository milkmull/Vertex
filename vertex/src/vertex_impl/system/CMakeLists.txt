# Source files for vertex/src/vertex_impl/system
file(GLOB VX_SYSTEM_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profiler.cpp"
)

target_sources(Vertex PRIVATE ${VX_SYSTEM_SOURCE_FILES})
