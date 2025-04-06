
# https://github.com/libsdl-org/SDL/blob/6b13d69105721ac54cd7224b0493065c57776c89/cmake/sdlchecks.cmake#L45

function(vx_check_dlopen TARGET_NAME)

    # Check if the dlopen symbol exists in libc (usually in dlfcn.h)
    check_symbol_exists(dlopen "dlfcn.h" HAVE_DLOPEN_IN_LIBC)
    
    if(HAVE_DLOPEN_IN_LIBC)
        
        # No need for any extra links
        target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_DLOPEN)
        
    elseif(CMAKE_DL_LIBS)
    
        # Link against the library that contains dlopen
        target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_DL_LIBS})
        target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_DLOPEN)
    
    else()
    
        message(WARNING "dlopen not found in CMAKE_DL_LIBS for target ${TARGET_NAME}. Dynamic loading may not be supported.")
        
    endif()

endfunction()

#--------------------------------------------------------------------

function(vx_check_available_functions TARGET_NAME)

    if(VX_CMAKE_PLATFORM_UNIX)
    
        vx_check_function_exists(${TARGET_NAME} localtime_r     HAVE_GMTIME_R)
        vx_check_function_exists(${TARGET_NAME} localtime_r     HAVE_LOCALTIME_R)
        vx_check_function_exists(${TARGET_NAME} clock_gettime   HAVE_CLOCK_GETTIME)
        vx_check_function_exists(${TARGET_NAME} nanosleep       HAVE_NANOSLEEP)
        
        vx_check_dlopen(${TARGET_NAME})
    
    endif()

endfunction()

#--------------------------------------------------------------------