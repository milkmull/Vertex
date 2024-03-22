#--------------------------------------------------------------------
# Library
#--------------------------------------------------------------------

# Define library
if (VX_BUILD_SHARED_LIBS)
    message(STATUS "Building shared libraries")
    add_library(${PROJECT_NAME} SHARED)
else()
    message(STATUS "Building static libraries")
    add_library(${PROJECT_NAME} STATIC)
endif()

# Properties
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

#--------------------------------------------------
# Vertex Math
#--------------------------------------------------

file(GLOB_RECURSE VX_MATH_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex/math/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex/math/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex/math/*.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VX_MATH_FILES})

target_sources(${PROJECT_NAME} PUBLIC ${VX_MATH_FILES})

#--------------------------------------------------
# Vertex Image
#--------------------------------------------------

# stb_image
file(GLOB_RECURSE STB_IMAGE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/*.h
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${STB_IMAGE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/include")
target_sources(${PROJECT_NAME} PRIVATE ${STB_IMAGE_FILES})

file(GLOB_RECURSE VX_IMAGE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex/image/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex/image/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex/image/*.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VX_IMAGE_FILES})

target_sources(${PROJECT_NAME} PRIVATE ${VX_IMAGE_FILES})

#--------------------------------------------------------------------
# App
#--------------------------------------------------------------------

if (NOT VX_BACKEND STREQUAL "None")

    # GLFW
    if (VX_WINDOWING STREQUAL "GLFW")

        if (VX_BUILD_SHARED_LIBS)
            set(GLFW_LIBRARY_TYPE "SHARED")
        else()
            set(GLFW_LIBRARY_TYPE "STATIC")
        endif()
        
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw)
        
        target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include")
        target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
        
        file(GLOB_RECURSE VX_GLFW_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex/app/window/platform/glfw_window.cpp"
        )
        
        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VX_GLFW_FILES})
        target_sources(${PROJECT_NAME} PRIVATE ${VX_GLFW_FILES})
        
    endif()
    
    # Vertex App
    file(GLOB_RECURSE VX_APP_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex/app/*.h
    )
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VX_APP_FILES})
    
    target_sources(${PROJECT_NAME} PRIVATE ${VX_APP_FILES})

endif()