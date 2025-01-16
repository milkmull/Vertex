#pragma once

#if defined(VX_LOG_MEMORY)

#define VX_ENABLE_LOGGING 1
#include "vertex/system/log.hpp"

void* operator new(std::size_t size)
{
    VX_LOG_TRACE << "Allocating " << size << " bytes\n";
    return std::malloc(size);
}

void operator delete(void* ptr, std::size_t size) noexcept
{
    VX_LOG_TRACE << "Deallocating " << size << " bytes\n";
    std::free(ptr);
}

#endif // VX_STREAM_MEMORY