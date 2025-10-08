# Source files for vertex/include/vertex/app/video
file(GLOB VX_APP_VIDEO_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/video.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/window.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_VIDEO_HEADER_FILES})