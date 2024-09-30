# Vertex cmake project-config input for CMakeLists.txt script

include(FeatureSummary)
set_package_properties(Vertex PROPERTIES
    URL "https://github.com/milkmull/Vertex"
    DESCRIPTION "low level cross platform game development framework"
)


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was VertexConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set(VERTEX_FOUND TRUE)

# Check if the header targets are available
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/VertexHeaderTargets.cmake" AND NOT TARGET Vertex::Headers)
    include("${CMAKE_CURRENT_LIST_DIR}/VertexHeaderTargets.cmake")
    set(VERTEX_HEADERS_FOUND TRUE)
    message(STATUS "Vertex header targets are available.")
else()
    set(VERTEX_HEADERS_FOUND FALSE)
    message(STATUS "Vertex header targets are not available.")
endif()

# Check if the shared library target is available
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/VertexSharedTargets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/VertexSharedTargets.cmake")
    set(VERTEX_FOUND TRUE)
    set(VERTEX_SHARED_LIBRARIES Vertex::Vertex_Shared)
    message(STATUS "Vertex shared library target is available.")
else()
    set(VERTEX_SHARED_LIBRARIES)
    message(STATUS "Vertex shared library target is not available.")
endif()

# Check if the static library target is available
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/VertexStaticTargets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/VertexStaticTargets.cmake")
    set(VERTEX_FOUND TRUE)
    set(VERTEX_STATIC_LIBRARIES Vertex::Vertex_Static)
    message(STATUS "Vertex static library target is available.")
else()
    set(VERTEX_STATIC_LIBRARIES)
    message(STATUS "Vertex static library target is not available.")
endif()

# Error out if neither shared nor static libraries are found
if(NOT VERTEX_FOUND)
    message(FATAL_ERROR "Vertex library was not found. Please ensure it is installed correctly.")
endif()

# Set the include directory if headers are found
if(VERTEX_FOUND)
    set(VERTEX_INCLUDE_DIR "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/include")
else()
    set(VERTEX_INCLUDE_DIR)
endif()
