# Source files for vertex/src/vertex_impl/util
file(GLOB VX_UTIL_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto/MD5.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto/SHA1.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto/SHA256.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/encode/base64.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/time/datetime.cpp"
)

target_sources(Vertex PRIVATE ${VX_UTIL_SOURCE_FILES})