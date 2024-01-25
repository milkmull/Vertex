# Files
file(GLOB_RECURSE SB_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SB_FILES})

# Define executable
add_executable(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox/sandbox.cpp" ${SB_FILES})

# Add link libraries
target_link_libraries(Sandbox PRIVATE Vertex)

# Add include directories
target_include_directories(Sandbox PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")