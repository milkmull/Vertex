add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/config")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/system")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/util")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/os")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/math")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pixel")

if(VX_BUILD_IMAGE)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/image")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/app")