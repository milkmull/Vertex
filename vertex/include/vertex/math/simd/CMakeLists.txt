
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/math
file(GLOB VX_MATH_SIMD_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/arch/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/arch/vec_default.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/arch/mat_default.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/vec4f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mat4f.hpp"
    
    #--------------------------------------
    # x86
    #--------------------------------------
    
    "${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/base_types.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/vec4f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/mat4f.hpp"
)

target_sources(Vertex PRIVATE ${VX_MATH_SIMD_HEADER_FILES})