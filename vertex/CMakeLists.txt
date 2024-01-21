# Files
file(GLOB_RECURSE ALL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

# Define library
if (VX_BUILD_SHARED_LIBS)
    message(STATUS "Building shared libraries")
    add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
else()
    message(STATUS "Building static libraries")
    add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
endif()

# Properties
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Add include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
