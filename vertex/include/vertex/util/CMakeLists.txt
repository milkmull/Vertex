# Source files for vertex/include/vertex/util
file(GLOB VX_UTIL_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/bit/bit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bit/endian.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bit/enum_bitwise_operators.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bit.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto/FNV1a.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto/MD5.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto/SHA1.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto/SHA256.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/encode/base64.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/base64.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/encode/utf.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utf.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/io/iostream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/io/quoted.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/io.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/iterator/proxy.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/random/algorithm.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/bernoulli_distribution.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/discrete_distribution.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/generate_canonical.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/normal_distribution.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/pcg.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/rng.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/uniform_int_distribution.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/uniform_real_distribution.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/string/regex.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/string.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/string_cast.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/string_compare.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/string_compare.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/string_type.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/type_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/regex.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/timer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/type_traits.hpp"
)

target_sources(Vertex PRIVATE ${VX_UTIL_HEADER_FILES})