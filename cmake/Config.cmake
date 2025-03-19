#--------------------------------------------------------------------
# Detect OS
#--------------------------------------------------------------------

if(NOT VX_BUILD_DUMMY_OS)

    # Windows
    if(WIN32)

        set(VX_CMAKE_PLATFORM_WINDOWS 1)
        message(STATUS "Detected platform: Windows")
        
    # macOS or iOS
    elseif(APPLE)

        if(CMAKE_SYSTEM_NAME MATCHES ".*(Darwin|MacOS).*")
        
            set(VX_CMAKE_PLATFORM_MACOS 1)
            message(STATUS "Detected platform: MacOS")
            
        elseif(CMAKE_SYSTEM_NAME MATCHES ".*iOS.*")
        
            set(VX_CMAKE_PLATFORM_IOS 1)
            message(STATUS "Detected platform: iOS")
            
        else()
        
            message(FATAL_ERROR "Unknown Apple platform: \"${CMAKE_SYSTEM_NAME}\"")
            return()
            
        endif()
        
    # Linux
    elseif(CMAKE_SYSTEM_NAME MATCHES ".*Linux")

        set(VX_CMAKE_PLATFORM_LINUX 1)
        message(STATUS "Detected platform: Linux")
        
    # Android
    elseif(CMAKE_SYSTEM_NAME MATCHES "Android.*")

        set(VX_CMAKE_PLATFORM_ANDROID 1)
        message(STATUS "Detected platform: Android")

    else()

        message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
        return()
        
    endif()

    # Detect Unix-based systems (excluding Android, Apple, and RISC OS)
    # This ensures that Unix-specific options are applied only to general Unix systems.
    if(UNIX AND NOT ANDROID AND NOT APPLE AND NOT RISCOS)

        set(VX_CMAKE_UNIX_SYS 1)
        message(STATUS "Platform is Unix")
        
    else()

        set(VX_CMAKE_UNIX_SYS 0)
        
    endif()

endif()

#--------------------------------------------------------------------
# Detect Compiler
#--------------------------------------------------------------------

# Note: The detection is order is important because:
# - Visual Studio can both use MSVC and Clang
# - GNUCXX can still be set on macOS when using Clang

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|IntelLLVM")

    if(MSVC)
        
        # MSVC with Clang
        set(VX_CMAKE_COMPILER_MSVC_CLANG 1)
        message(STATUS "Detected compiler: MSVC-Clang")
        
    else()
    
        # General Clang compiler
        set(VX_CMAKE_COMPILER_CLANG 1)
        message(STATUS "Detected compiler: Clang")
        
    endif()
    
elseif(MSVC)

    # Microsoft Visual Studio compiler
    set(VX_CMAKE_COMPILER_MSVC 1)
    message(STATUS "Detected compiler: MSVC")
    
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    # GCC (GNU Compiler Collection)
    set(VX_CMAKE_COMPILER_GCC 1)
    message(STATUS "Detected compiler: GCC")
    
else()

    # Handle unsupported compilers
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
    return()
    
endif()

#--------------------------------------------------------------------
# Detect Architecture
#--------------------------------------------------------------------

if(CMAKE_SIZEOF_VOID_P EQUAL 8)

    set(VX_CMAKE_ARCH_64BIT 1)
    message(STATUS "Detected architecture: 64-bit")
    
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)

    set(VX_CMAKE_ARCH_32BIT 1)
    message(STATUS "Detected architecture: 32-bit")
    
else()

    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SIZEOF_VOID_P}-bit")
    
endif()

#--------------------------------------------------------------------

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")

    set(VX_CMAKE_ARCH_X86_64 1)
    message(STATUS "Detected processor: x86_64")
    
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86")

    set(VX_CMAKE_ARCH_X86 1)
    message(STATUS "Detected processor: x86")
    
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7l|armv8|aarch64")

    set(VX_CMAKE_ARCH_ARM 1)
    
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    
        set(VX_CMAKE_ARCH_ARM64 1)
        message(STATUS "Detected processor: ARM64")
        
    else()
    
        message(STATUS "Detected processor: ARM")
        
    endif()
    
#elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc64|powerpc64|ppc64le")
#
#    set(VX_CMAKE_ARCH_POWERPC64 1)
#    message(STATUS "Detected processor: PowerPC64")
#    
#elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc|powerpc")
#
#    set(VX_CMAKE_ARCH_POWERPC 1)
#    message(STATUS "Detected processor: PowerPC")
#    
#elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "m68k")
#
#    set(VX_CMAKE_ARCH_M68K 1)
#    message(STATUS "Detected processor: Motorola 68000")
#    
#elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "sparc|sparcv9")
#
#    set(VX_CMAKE_ARCH_SPARC 1)
#    if(CMAKE_SYSTEM_PROCESSOR MATCHES "sparcv9")
#        set(VX_CMAKE_ARCH_SPARC64 1)
#        message(STATUS "Detected processor: SPARC64")
#    else()
#        message(STATUS "Detected processor: SPARC")
#    endif()
    
else()

    message(FATAL_ERROR "Unsupported processor architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    
endif()