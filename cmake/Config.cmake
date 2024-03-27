#--------------------------------------------------------------------
# Detect OS
#--------------------------------------------------------------------

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

    set(VX_OS_WINDOWS 1)
    message(STATUS "OS: Windows")
    
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

    set(VX_OS_UNIX 1)
    message(STATUS "OS: Linux")

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

    set(VX_OS_MACOS 1)
    message(STATUS "OS: Mac")

else()

    message(FATAL_ERROR "Unsupported operating system")
    return()
    
endif()

#--------------------------------------------------------------------
# Detect Compiler
#--------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    set(VX_COMPILER_MSVC 1)
    message(STATUS "Compiler: MSVC")
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    set(VX_COMPILER_CLANG 1)
    message(STATUS "Compiler: Clang")
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    set(VX_COMPILER_GCC 1)
    message(STATUS "Compiler: GNU")
    
else()

    message(WARNING "Unrecognized compiler: ${CMAKE_CXX_COMPILER_ID}.")
    return()
    
endif()