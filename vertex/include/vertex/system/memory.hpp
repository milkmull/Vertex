#pragma once

#if defined(VX_STREAM_MEMORY)

#include <iostream>

void* operator new(std::size_t size)
{
    std::cout << "Allocating " << size << " bytes\n";
    return std::malloc(size);
}

void operator delete(void* ptr, std::size_t size) noexcept
{
    std::cout << "Deallocating " << size << " bytes\n";
    std::free(ptr);
}

#endif // VX_STREAM_MEMORY