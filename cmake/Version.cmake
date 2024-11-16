
# Read the file content
file(READ "vertex/include/vertex/system/version.hpp" VX_VERSION_HEADER_CONTENT)

# Extract the major, minor, and patch versions using regex
string(REGEX MATCH "#define[ \t]+VX_VERSION_MAJOR[ \t]+([0-9]+)" _ ${VX_VERSION_HEADER_CONTENT})
set(VX_MAJOR_VERSION ${CMAKE_MATCH_1})

string(REGEX MATCH "#define[ \t]+VX_VERSION_MINOR[ \t]+([0-9]+)" _ ${VX_VERSION_HEADER_CONTENT})
set(VX_MINOR_VERSION ${CMAKE_MATCH_1})

string(REGEX MATCH "#define[ \t]+VX_VERSION_PATCH[ \t]+([0-9]+)" _ ${VX_VERSION_HEADER_CONTENT})
set(VX_PATCH_VERSION ${CMAKE_MATCH_1})

set(VX_PROJECT_VERSION "${VX_MAJOR_VERSION}.${VX_MINOR_VERSION}.${VX_PATCH_VERSION}")