# Install script for directory: C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/vertex

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/vertex/include/" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/lib/Vertex_d.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/bin/Vertex_d.dll")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/lib/Vertex_d.pdb")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexConfigVersion.cmake;C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexConfig.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake" TYPE FILE FILES
    "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/vertex/VertexConfigVersion.cmake"
    "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/vertex/VertexConfig.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexHeaderTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexHeaderTargets.cmake"
         "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/vertex/CMakeFiles/Export/8f7f269710eba612354578be1bf97c15/VertexHeaderTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexHeaderTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexHeaderTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexHeaderTargets.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake" TYPE FILE FILES "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/vertex/CMakeFiles/Export/8f7f269710eba612354578be1bf97c15/VertexHeaderTargets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexSharedTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexSharedTargets.cmake"
         "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/vertex/CMakeFiles/Export/8f7f269710eba612354578be1bf97c15/VertexSharedTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexSharedTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexSharedTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexSharedTargets.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake" TYPE FILE FILES "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/vertex/CMakeFiles/Export/8f7f269710eba612354578be1bf97c15/VertexSharedTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake/VertexSharedTargets-debug.cmake")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    file(INSTALL DESTINATION "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/install/x64-Debug/cmake" TYPE FILE FILES "C:/Users/Owner/Desktop/Milk_Stuff/2024/code/cpp/Vertex/out/build/x64-Debug/vertex/CMakeFiles/Export/8f7f269710eba612354578be1bf97c15/VertexSharedTargets-debug.cmake")
  endif()
endif()

