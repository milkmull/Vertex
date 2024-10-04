#pragma once

#include <iostream>

namespace vx {
namespace io {

inline void print()
{
    std::cout << std::endl;
}

template <typename Arg, typename... Args>
inline void print(Arg&& arg, Args&&... args)
{
    std::cout << std::forward<Arg>(arg) << ' ';
    print(std::forward<Args>(args)...);
}

inline void print_err()
{
    std::cout << std::endl;
}

template <typename Arg, typename... Args>
inline void print_err(Arg&& arg, Args&&... args)
{
    std::cerr << std::forward<Arg>(arg) << ' ';
    print(std::forward<Args>(args)...);
}

template <typename T>
void read_input(T& value)
{
    std::cin >> value;
}

}
}