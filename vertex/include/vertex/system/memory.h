#pragma once

#include "error.h"

namespace vx {
namespace mem {

inline void* malloc(size_t size)
{
    void* block = std::malloc(size);
    if (!block) VX_ERROR_DEFAULT(error::error_code::OUT_OF_MEMORY);
    return block;
}

inline void free(void* block)
{
    std::free(block);
}

}
}