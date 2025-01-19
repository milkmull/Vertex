# Source files for sandbox/src/sandbox_impl
file(GLOB SB_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/sandbox.cpp"
)

target_sources(Sandbox PRIVATE ${SB_SOURCE_FILES})