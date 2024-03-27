#--------------------------------------------------------------------
# Library
#--------------------------------------------------------------------

# Define library
if(VX_BUILD_SHARED_LIBS)
    message(STATUS "Building shared libraries")
    add_library(${PROJECT_NAME} SHARED)
else()
    message(STATUS "Building static libraries")
    add_library(${PROJECT_NAME} STATIC)
endif()

# Properties
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

#--------------------------------------------------
# Vertex Core
#--------------------------------------------------

file(GLOB_RECURSE VX_CORE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/detail/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/tools/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/math/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/image/*.h"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/detail/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/tools/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/math/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/image/*.h"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/detail/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/tools/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/math/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/image/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VX_CORE_FILES})

target_sources(${PROJECT_NAME} PRIVATE ${VX_CORE_FILES})

# stb_image
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/include")

file(GLOB_RECURSE STB_IMAGE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image_impl/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${STB_IMAGE_FILES})

target_sources(${PROJECT_NAME} PRIVATE ${STB_IMAGE_FILES})

#--------------------------------------------------------------------
# Window
#--------------------------------------------------------------------

if(VX_BUILD_WINDOW)

    if(VX_OS_WINDOWS)
    
    
    
    endif()
    
endif()

#--------------------------------------------------------------------
# Install
#--------------------------------------------------------------------