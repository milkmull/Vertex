# Files
file(GLOB_RECURSE ALL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

# Define executable
add_executable(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox/sandbox.cpp")

# Add link libraries
target_link_libraries(Sandbox PRIVATE Vertex)
