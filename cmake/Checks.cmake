
# https://github.com/libsdl-org/SDL/blob/6b13d69105721ac54cd7224b0493065c57776c89/cmake/sdlchecks.cmake#L45

function(vx_check_dlopen TARGET_NAME)

    # Check for dlopen in libc first
    check_symbol_exists(dlopen "dlfcn.h" HAVE_DLOPEN_IN_LIBC)

    # If dlopen is not in libc, check in libdl
    if(NOT HAVE_DLOPEN_IN_LIBC)
        check_symbol_exists(dlopen "dlfcn.h" HAVE_DLOPEN_IN_LIBDL)
    endif()

    # If found in libdl, link the dl library to the target
    if(HAVE_DLOPEN_IN_LIBC OR HAVE_DLOPEN_IN_LIBDL)
    
        target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_DLOPEN)
        if(HAVE_DLOPEN_IN_LIBDL)
            target_link_libraries(${TARGET_NAME} PRIVATE dl)
        endif()
        
    else()
        # Optional: Handle case when dlopen is not found in either library
        message(WARNING "dlopen not found in libc or libdl for ${TARGET_NAME}")
    endif()

endfunction()

#--------------------------------------------------------------------

function(vx_check_available_functions TARGET_NAME)

    if(VX_CMAKE_UNIX_SYS)
    
        vx_check_function_exists(${TARGET_NAME} localtime_r     HAVE_GMTIME_R)
        vx_check_function_exists(${TARGET_NAME} localtime_r     HAVE_LOCALTIME_R)
        vx_check_function_exists(${TARGET_NAME} clock_gettime   HAVE_CLOCK_GETTIME)
        vx_check_function_exists(${TARGET_NAME} nanosleep       HAVE_NANOSLEEP)
        
        vx_check_dlopen(${TARGET_NAME})
    
    endif()

endfunction()

#--------------------------------------------------------------------