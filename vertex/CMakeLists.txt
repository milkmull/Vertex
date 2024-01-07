# Files
file(GLOB_RECURSE ALL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

# Define library
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

# Properties
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
