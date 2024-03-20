# Files

# =============================================== Vertex ===============================================

file(GLOB_RECURSE VX_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VX_FILES})

# =============================================== stb_image ===============================================

file(GLOB_RECURSE STB_IMAGE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/*.h
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${STB_IMAGE_FILES})

# ======================================================================================================

# Define library
if (VX_BUILD_SHARED_LIBS)
    message(STATUS "Building shared libraries")
    add_library(${PROJECT_NAME} SHARED ${VX_FILES} ${STB_IMAGE_FILES} ${VULKAN_FILES})
else()
    message(STATUS "Building static libraries")
    add_library(${PROJECT_NAME} STATIC ${VX_FILES} ${STB_IMAGE_FILES} ${VULKAN_FILES})
endif()

# Properties
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Add include directories

# =============================================== Vertex ===============================================

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# =============================================== stb_image ===============================================

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/include")