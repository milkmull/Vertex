#--------------------------------------------------------------------
# std Algorithm Tests
#--------------------------------------------------------------------

# Adds test_std_algorithm_<name> from <name>.cpp plus the shared header.
function(vx_add_std_algorithm_test name)
    vx_add_test(
        "test_std_algorithm_${name}"
        "std/algorithm"
        "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp;${CMAKE_CURRENT_SOURCE_DIR}/algorithm_tools.hpp"
    )
endfunction()

vx_add_std_algorithm_test(minmax)
vx_add_std_algorithm_test(lifetime)
vx_add_std_algorithm_test(fill)
# vx_add_std_algorithm_test(find)
