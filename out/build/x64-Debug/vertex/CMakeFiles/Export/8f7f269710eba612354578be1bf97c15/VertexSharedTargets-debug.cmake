#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Vertex::Vertex_Shared" for configuration "Debug"
set_property(TARGET Vertex::Vertex_Shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Vertex::Vertex_Shared PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/Vertex_d.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/Vertex_d.dll"
  )

list(APPEND _cmake_import_check_targets Vertex::Vertex_Shared )
list(APPEND _cmake_import_check_files_for_Vertex::Vertex_Shared "${_IMPORT_PREFIX}/lib/Vertex_d.lib" "${_IMPORT_PREFIX}/bin/Vertex_d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
