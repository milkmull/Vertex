#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/os/shared_library.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_cast.hpp"

namespace vx {
namespace os {

class shared_library
{
public:

    shared_library() = default;

    shared_library(const char* name)
    {
        load(name);
    }

    ~shared_library()
    {
        free();
    }

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

    void swap(shared_library& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
    }

public:

    /**
     * @brief Checks if the shared library is successfully loaded.
     *
     * @return `true` if the library is loaded, `false` otherwise.
     */
    bool is_loaded() const noexcept
    {
        return m_handle != nullptr;
    }

    template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
    success load(const C* lib)
    {
        VX_IF_CONSTEXPR (std::is_same<C, os::native_char>::value)
        {
            return os::load_shared_library(m_handle, lib);
        }
        else
        {
            str::basic_string<os::native_char> tmp;
            str::string_cast
        }
    }

    /**
     * @brief Unloads the currently loaded shared library.
     */
    void free() noexcept;

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
    void* get_void(const char* symbol_name) const noexcept;

private:

    os::shared_library_handle m_handle = nullptr;
};

} // namespace os
} // namespace vx
