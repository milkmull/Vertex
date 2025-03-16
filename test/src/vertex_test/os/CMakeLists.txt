
#--------------------------------------------------------------------
# OS Tests
#--------------------------------------------------------------------

vx_add_test(test_os_system_info   "os" "${CMAKE_CURRENT_SOURCE_DIR}/system_info.cpp")
vx_add_test(test_os_locale        "os" "${CMAKE_CURRENT_SOURCE_DIR}/locale.cpp")
vx_add_test(test_os_system_memory "os" "${CMAKE_CURRENT_SOURCE_DIR}/system_memory.cpp")

file(GLOB VX_OS_PATH_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/path.cpp"
)
vx_add_test(test_os_path         "os" "${VX_OS_PATH_SRC_FILES}")

vx_add_test(test_os_file         "os" "${CMAKE_CURRENT_SOURCE_DIR}/file.cpp")
vx_add_test(test_os_filesystem   "os" "${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp")
vx_add_test(test_os_time         "os" "${CMAKE_CURRENT_SOURCE_DIR}/time.cpp")

vx_add_test(os_child_process     "os" "${CMAKE_CURRENT_SOURCE_DIR}/child_process.cpp")
vx_add_test(test_os_process      "os" "${CMAKE_CURRENT_SOURCE_DIR}/process.cpp")
add_dependencies(test_os_process os_child_process)
vx_add_test(test_os_thread       "os" "${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp")

vx_add_test(test_os_random       "os" "${CMAKE_CURRENT_SOURCE_DIR}/random.cpp")

#--------------------------------------------------------------------
# Shared Library Test
#--------------------------------------------------------------------

add_library(os_shared_library SHARED "${CMAKE_CURRENT_SOURCE_DIR}/shared_library_lib.cpp")
set_target_properties(os_shared_library PROPERTIES FOLDER "Tests/os")
vx_add_common_compiler_flags(os_shared_library)
vx_hide_public_symbols(os_shared_library)
vx_configure_test_install(os_shared_library)

vx_add_test(test_os_shared_library "os" "${CMAKE_CURRENT_SOURCE_DIR}/shared_library.cpp")
add_dependencies(test_os_shared_library os_shared_library)
