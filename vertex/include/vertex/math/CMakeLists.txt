
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/math
file(GLOB VX_MATH_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/type_traits.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec2b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec2i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec2u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec2f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec2d.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec3b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec3i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec3u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec3f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec3d.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec4b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec4i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec4u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec4f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec/vec4d.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/fn/common.hpp"
)

target_sources(Vertex PRIVATE ${VX_MATH_HEADER_FILES})