add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/system")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/util")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/os")

if(VX_IMAGE_ENABLED)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/image")
endif()

if(VX_APP_ENABLED)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/app")
endif()
