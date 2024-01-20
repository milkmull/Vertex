# Files
file(GLOB_RECURSE ALL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

# Define executable
add_executable(Sandbox ${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox/sandbox.cpp)

# Properties
set_target_properties(Sandbox PROPERTIES LINKER_LANGUAGE CXX)

# Add include directories
target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}/vertex/src)

# Add link libraries
target_link_libraries(Sandbox Vertex)
