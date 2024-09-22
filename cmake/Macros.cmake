
# Define a macro that helps define an option
macro(vx_set_option var type default docstring)

    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
    
endmacro()

macro(vx_check_add_debug_flag FLAG SUFFIX)

    check_c_compiler_flag(${FLAG} HAS_C_FLAG_${SUFFIX})
    if(HAS_C_FLAG_${SUFFIX})
        string(APPEND CMAKE_C_FLAGS_DEBUG " ${FLAG}")
    endif()

    check_cxx_compiler_flag(${FLAG} HAS_CXX_${SUFFIX})
    if(HAS_CXX_${SUFFIX})
        string(APPEND CMAKE_CXX_FLAGS_DEBUG " ${FLAG}")
    endif()

endmacro()

macro(vx_sanatizer_check_add_debug_flag FLAG)

    vx_check_add_debug_flag("-fsanitize=${FLAG}" "${FLAG}")
  
endmacro()