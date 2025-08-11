#pragma once

#include <utility> // std::move

#include "vertex/config/language_config.hpp"
#include "vertex/os/handle.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// shared_library
///////////////////////////////////////////////////////////////////////////////

namespace _priv {

struct shared_library_impl;

} // namespace _priv

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Class for dynamically loading and interacting with shared libraries.
 *
 * The `shared_library` class provides functionality for loading, accessing
 * symbols, and unloading shared libraries at runtime. It provides methods to
 * check if a library is loaded, retrieve functions from the library, and manage
 * the library's lifecycle.
 *
 * This class abstracts the OS-specific details of working with shared libraries,
 * ensuring compatibility across platforms.
 */
class shared_library
{
public:

    // https://beta.boost.org/doc/libs/1_61_0/doc/html/boost/dll/shared_library.html

    /**
     * @brief Returns the platform-specific file extension for shared libraries.
     *
     * This static method provides the appropriate file extension for shared
     * libraries depending on the platform.
     *
     * - Windows: `.dll`
     * - Apple (macOS): `.dylib`
     * - Linux and other Unix-based OS: `.so`
     *
     * @return The shared library file extension.
     */
    static constexpr const char* suffix() noexcept
    {
#   if defined(VX_OS_WINDOWS)
        return ".dll";
#   elif defined(VX_OS_APPLE)
        return ".dylib";
#   else
        return ".so";
#   endif
    }

public:

    shared_library() = default;
    shared_library(const char* name) { load(name); }
    ~shared_library() { free(); }

    shared_library(const shared_library&) = default;
    shared_library& operator=(const shared_library&) = default;

    shared_library(shared_library&& other) noexcept : m_handle(other.m_handle)
    {
        other.free();
    }

    shared_library& operator=(shared_library&& other) noexcept
    {
        if (this != &other)
        {
            free();
            m_handle = other.m_handle;
            other.free();
        }

        return *this;
    }

    void swap(shared_library& other) noexcept { std::swap(m_handle, other.m_handle); }

public:

    /**
     * @brief Checks if the shared library is successfully loaded.
     *
     * @return `true` if the library is loaded, `false` otherwise.
     */
    bool is_loaded() const noexcept { return m_handle != nullptr; }

    /**
     * @brief Loads the shared library from the specified path.
     *
     * This method attempts to load the shared library located at the given `lib` path.
     * If a library is already loaded, it sets an error.
     *
     * @param lib The path to the shared library to load.
     * @return `true` if the library was successfully loaded, `false` if an error occurred.
     */
    VX_API bool load(const char* lib);

    /**
     * @brief Unloads the currently loaded shared library.
     */
    VX_API void free() noexcept;

public:

    /**
     * @brief Checks if a symbol exists in the loaded library.
     *
     * @param symbol_name The name of the symbol to search for.
     * @return `true` if the symbol is found, `false` otherwise.
     */
    bool has(const char* symbol_name) const noexcept
    {
        return get_void(symbol_name) != nullptr;
    }

    /**
     * @brief Retrieves a function pointer to a symbol in the loaded library.
     *
     * This method casts the retrieved symbol into a function pointer of the specified type.
     *
     * @param symbol_name The name of the symbol to retrieve.
     * @return A function pointer to the symbol, or `nullptr` if the symbol doesn't exist.
     */
    template <typename Func>
    Func get(const char* symbol_name) const noexcept
    {
        return reinterpret_cast<Func>(get_void(symbol_name));
    }

    /**
     * @brief Retrieves a void pointer to a symbol in the loaded library.
     *
     * This method provides a raw pointer to the symbol, which can then be cast to
     * the appropriate type.
     *
     * @param symbol_name The name of the symbol to retrieve.
     * @return A pointer to the symbol, or `nullptr` if the symbol doesn't exist.
     */
    VX_API void* get_void(const char* symbol_name) const noexcept;

private:

    friend _priv::shared_library_impl;

    void* m_handle = nullptr;
};

} // namespace os
} // namespace vx