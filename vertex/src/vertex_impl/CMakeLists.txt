add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/system")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/util")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/os")

if(VX_BUILD_IMAGE)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/image")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/app")
