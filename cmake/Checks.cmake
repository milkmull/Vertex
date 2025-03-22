
# https://github.com/libsdl-org/SDL/blob/6b13d69105721ac54cd7224b0493065c57776c89/cmake/sdlchecks.cmake#L45

function(vx_check_dlopen TARGET_NAME)

    # Check if CMAKE_DL_LIBS contains a library for dynamic loading
    if(CMAKE_DL_LIBS)
        
        if(NOT CMAKE_DL_LIBS STREQUAL "c")
            target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_DL_LIBS})
            target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_DLOPEN)
        endif()
        
    else()
        # Message if dlopen is not found (i.e., no dynamic loading library needed)
        message(WARNING "dlopen not found in CMAKE_DL_LIBS for target ${TARGET_NAME}. Dynamic loading may not be supported.")
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