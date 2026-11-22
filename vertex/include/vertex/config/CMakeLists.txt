# Source files for vertex/include/vertex/system
file(GLOB VX_SYSTEM_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/macro.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/feature_detection.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/version_number.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/compiler.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/os.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/architecture.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/endian.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/language.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/build.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/attributes.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cxx_features.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/warnings.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/export.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source_location.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/intrinsics.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/scoped_enum.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/flags.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assert.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp"
)

target_sources(Vertex PRIVATE ${VX_SYSTEM_HEADER_FILES})
