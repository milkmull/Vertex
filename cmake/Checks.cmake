
# https://github.com/libsdl-org/SDL/blob/6b13d69105721ac54cd7224b0493065c57776c89/cmake/sdlchecks.cmake#L45

function(vx_check_dlopen TARGET_NAME)

    # Check if the dlopen symbol exists in libc (usually in dlfcn.h)
    vx_check_symbol_exists(${TARGET_NAME} "dlopen" "dlfcn.h" PRIVATE HAVE_DLOPEN_IN_LIBC)

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

function(vx_link_threads TARGET_NAME)

    # Find pthreads (sets CMAKE_THREAD_LIBS_INIT and CMAKE_USE_PTHREADS_INIT)
    find_package(Threads REQUIRED)
    target_link_libraries(${TARGET_NAME} PUBLIC Threads::Threads)

    if(CMAKE_USE_PTHREADS_INIT)
        target_compile_definitions(${TARGET_NAME} PUBLIC VX_HAVE_PTHREADS)
        vx_check_symbol_exists( ${TARGET_NAME} "PTHREAD_MUTEX_RECURSIVE" "pthread.h" PUBLIC VX_HAVE_PTHREAD_MUTEX_RECURSIVE)
    endif()

endfunction()

#--------------------------------------------------------------------

function(vx_check_available_features TARGET_NAME)

    vx_link_threads(${TARGET_NAME})

    # signl utils
    vx_check_include_exists(${TARGET_NAME}                                   "signal.h" PRIVATE  HAVE_SIGNAL_H)
    vx_check_symbol_exists( ${TARGET_NAME} "sigaction"                       "signal.h" PRIVATE  HAVE_SIGACTION)

    if(VX_CMAKE_PLATFORM_WINDOWS)

    elseif(VX_CMAKE_PLATFORM_UNIX)

        vx_check_function_exists(${TARGET_NAME} "localtime_r"     PRIVATE HAVE_GMTIME_R)
        vx_check_function_exists(${TARGET_NAME} "localtime_r"     PRIVATE HAVE_LOCALTIME_R)
        vx_check_function_exists(${TARGET_NAME} "clock_gettime"   PRIVATE HAVE_CLOCK_GETTIME)
        vx_check_function_exists(${TARGET_NAME} "nanosleep"       PRIVATE HAVE_NANOSLEEP)
        vx_check_function_exists(${TARGET_NAME} "statx"           PRIVATE HAVE_STATX)

        vx_check_dlopen(${TARGET_NAME})

        # posix_spawn*
        vx_check_symbol_exists(${TARGET_NAME} "posix_spawn_file_actions_addchdir"     "spawn.h"         PRIVATE HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR)
        vx_check_symbol_exists(${TARGET_NAME} "posix_spawn_file_actions_addchdir_np"  "spawn.h"         PRIVATE HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR_NP)

        # other
        vx_check_symbol_exists(${TARGET_NAME} "getrandom"                             "sys/random.h"                PRIVATE HAVE_GETRANDOM)
        vx_check_symbol_exists(${TARGET_NAME} "sysctl"                                "sys/types.h;sys/sysctl.h"    PRIVATE HAVE_SYSCTL)
        vx_check_symbol_exists(${TARGET_NAME} "sysctlbyname"                          "sys/types.h;sys/sysctl.h"    PRIVATE HAVE_SYSCTLBYNAME)

    endif()

endfunction()

#--------------------------------------------------------------------
