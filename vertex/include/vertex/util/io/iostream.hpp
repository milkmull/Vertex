#pragma once

#include <iostream>

namespace vx {
namespace io {

/**
 * @brief Base case for variadic print functions, does nothing.
 */
inline void print() noexcept {}

/**
 * @brief Prints a series of arguments to standard output with a space separator.
 *
 * This function uses perfect forwarding to efficiently handle various argument types.
 * It recursively prints each argument followed by a space, stopping when all arguments are printed.
 *
 * @tparam Arg The type of the first argument.
 * @tparam Args The types of the remaining arguments.
 * @param arg The first argument to print.
 * @param args The remaining arguments to print.
 */
template <typename Arg, typename... Args>
inline void print(Arg&& arg, Args&&... args)
{
    std::cout << std::forward<Arg>(arg) << ' ';
    print(std::forward<Args>(args)...);
}

/**
 * @brief Prints a newline to standard output.
 */
inline void println()
{
    std::cout << std::endl;
}

/**
 * @brief Prints a series of arguments to standard output with a space separator, followed by a newline.
 *
 * This function uses perfect forwarding to efficiently handle various argument types.
 * It recursively prints each argument followed by a space, and after all arguments are printed, a newline is output.
 *
 * @tparam Arg The type of the first argument.
 * @tparam Args The types of the remaining arguments.
 * @param arg The first argument to print.
 * @param args The remaining arguments to print.
 */
template <typename Arg, typename... Args>
inline void println(Arg&& arg, Args&&... args)
{
    std::cout << std::forward<Arg>(arg) << ' ';
    println(std::forward<Args>(args)...);
}

/**
 * @brief Reads a single input value from standard input.
 *
 * This function reads a value of type T from the standard input stream and stores it in the provided reference.
 *
 * @tparam T The type of the input value.
 * @param value The variable where the input will be stored.
 */
template <typename T>
inline void read_input(T& value)
{
    std::cin >> value;
}

} // namespace io
} // namespace vx