# Source files for vertex/include/vertex/_test
file(GLOB VX_TEST_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/test.hpp"
)

target_sources(Vertex PRIVATE ${VX_TEST_HEADER_FILES})