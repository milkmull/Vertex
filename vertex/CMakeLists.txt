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
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/config.h"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/system/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/math/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/image/*.h"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/system/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/math/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/image/*.h"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/system/*.cpp"
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

    # Vertex App
    file(GLOB_RECURSE VX_APP_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vertex/app/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/app/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/app/*.cpp"
    )
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VX_APP_FILES})
    
    target_sources(${PROJECT_NAME} PRIVATE ${VX_APP_FILES})

    if(VX_OS_WINDOWS)
    
        file(GLOB_RECURSE VX_WINDOWS_APP_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/src/win32_impl/app/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/win32_impl/app/*.cpp"
        )
        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VX_WINDOWS_APP_FILES})
        
        target_sources(${PROJECT_NAME} PRIVATE ${VX_WINDOWS_APP_FILES})
    
    endif()
    
endif()

#--------------------------------------------------------------------
# Install
#--------------------------------------------------------------------