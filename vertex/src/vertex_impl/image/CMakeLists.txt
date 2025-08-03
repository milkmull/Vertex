
#--------------------------------------------------------------------
# stb_image files
#--------------------------------------------------------------------

# stb_image
target_include_directories(Vertex PRIVATE "${VX_THIRD_PARTY_DIR}/stb_image/include")

file(GLOB STB_IMAGE_FILES
    "${VX_THIRD_PARTY_DIR}/stb_image/include/stb_image/stb_image.h"
    "${VX_THIRD_PARTY_DIR}/stb_image/include/stb_image/stb_image_write.h"
)

target_sources(Vertex PRIVATE ${STB_IMAGE_FILES})

#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/image
file(GLOB VX_IMAGE_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/util.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/load.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/write.cpp"
)

target_sources(Vertex PRIVATE ${VX_IMAGE_SOURCE_FILES})