#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_MAIN_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/main_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_sources(Vertex PRIVATE ${VX_APP_MAIN_SOURCE_FILES})

#--------------------------------------------------------------------
# Platform specific files
#--------------------------------------------------------------------

if(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_APP_MAIN_PLATFORM_SOURCE_FILES

        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/generic/generic_main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_run_app.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_APP_MAIN_PLATFORM_SOURCE_FILES})
    
else()

    # Source files for generic modules
    file(GLOB VX_APP_MAIN_PLATFORM_SOURCE_FILES

        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/generic/generic_main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/generic/generic_run_app.cpp"
    )

    target_sources(Vertex PRIVATE ${VX_APP_MAIN_PLATFORM_SOURCE_FILES})
    
endif()