#pragma once

#include <initializer_list>
#include <ratio>

#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"

namespace vx {
namespace _priv {

template <typename T, typename Allocator, typename Growth = std::ratio<3, 2>>
class dynamic_array_base
{
public:

    using allocator = Allocator;

    using growth_rate = Growth;
    VX_STATIC_ASSERT(growth_rate::num >= 0 && growth_rate::den > 0, "Growth rate must be positive");
    VX_STATIC_ASSERT(growth_rate::num >= growth_rate::den, "Growth rate must be greater or equal to 1");

    dynamic_array_base() noexcept
    {}

    ~dynamic_array_base() noexcept
    {
        if (m_buffer.data)
        {
            mem::destroy_array_elements(m_buffer.data, m_buffer.size);
            allocator::free(m_buffer.data);
        }
    }

    template <typename... Args>
    dynamic_array_base(size_t count, Args&&... args) noexcept
    {
        m_buffer.data = static_cast<T*>(allocator::alloc(count * sizeof(T)));
        if (!m_buffer.data)
        {
            VX_ERR(err::out_of_memory);
            return;
        }

        m_buffer.capacity = count;
        mem::construct_array_elements(m_buffer.data, count, std::forward<Args>(args)...);
        m_buffer.size = count;
    }

    dynamic_array_base(std::initializer_list<T> init) noexcept
    {
        m_buffer.data = static_cast<T*>(allocator::alloc(init.size() * sizeof(T)));
        if (!m_buffer.data)
        {
            VX_ERR(err::out_of_memory);
            return;
        }

        m_buffer.capacity = init.size();
        mem::copy_array_elements(m_buffer.data, init.begin(), init.size());
        m_buffer.size = init.size();
    }

    dynamic_array_base(const dynamic_array_base& other) noexcept
    {
        m_buffer.data = static_cast<T*>(allocator::alloc(other.m_buffer.size * sizeof(T)));
        if (!m_buffer.data)
        {
            VX_ERR(err::out_of_memory);
            return;
        }

        m_buffer.capacity = other.m_buffer.size;
        mem::copy_array_elements(m_buffer.data, other.data(), other.m_buffer.size);
        m_buffer.size = other.m_buffer.size;
    }

    VX_FORCE_INLINE dynamic_array_base(dynamic_array_base&& other) noexcept
        : m_buffer(std::exchange(other.m_buffer, buffer{}))
    {}

    dynamic_array_base& operator=(const dynamic_array_base& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        if (other.m_buffer.size <= m_buffer.capacity)
        {
            mem::destroy_array_elements(m_buffer.data, m_buffer.size);
            mem::copy_array_elements(m_buffer.data, other.m_buffer.data, other.m_buffer.size);
            m_buffer.size = other.m_buffer.size;
        }
        else
        {
            T* new_data;

            VX_IF_CONSTEXPR(std::is_trivially_destructible<T>::value && std::is_trivially_copyable<T>::value)
            {
                // Use realloc optimization for trivial types
                new_data = static_cast<T*>(allocator::realloc(m_buffer.data, other.m_buffer.size * sizeof(T)));
                if (!new_data)
                {
                    VX_ERR(err::out_of_memory);
                    return *this;
                }

                mem::copy_array_elements(new_data, other.m_buffer.data, other.m_buffer.size);
            }
            else
            {
                new_data = static_cast<T*>(allocator::alloc(other.m_buffer.size * sizeof(T)));
                if (!new_data)
                {
                    VX_ERR(err::out_of_memory);
                    return *this;
                }

                mem::copy_array_elements(new_data, other.m_buffer.data, other.m_buffer.size);

                if (m_buffer.data)
                {
                    mem::destroy_array_elements(m_buffer.data, m_buffer.size);
                    allocator::free(m_buffer.data);
                }
            }

            m_buffer.data = new_data;
            m_buffer.capacity = other.m_buffer.size;
            m_buffer.size = other.m_buffer.size;
        }

        return *this;
    }

    dynamic_array_base& operator=(dynamic_array_base&& other) noexcept
    {
        if (this == std::addressof(other))
        {
            return *this;
        }

        if (m_buffer.data)
        {
            mem::destroy_array_elements(m_buffer.data, m_buffer.size);
            allocator::free(m_buffer.data);
        }

        m_buffer = std::exchange(other.m_buffer, buffer{});

        return *this;
    }

    size_t capacity() const noexcept
    {
        return m_buffer.capacity;
    }

    size_t size() const noexcept
    {
        return m_buffer.size;
    }

    const T* data() const noexcept
    {
        return m_buffer.data;
    }

    T* data() noexcept
    {
        return m_buffer.data;
    }

    void clear() noexcept
    {
        if (m_buffer.data)
        {
            mem::destroy_array_elements(m_buffer.data, m_buffer.size);
            allocator::free(m_buffer.data);
        }

        m_buffer.capacity = 0;
        m_buffer.size = 0;
        m_buffer.data = nullptr;
    }

    T* release() noexcept
    {
        T* ptr = m_buffer.data;

        m_buffer.capacity = 0;
        m_buffer.size = 0;
        m_buffer.data = nullptr;

        return ptr;
    }

    bool reserve(size_t capacity) noexcept
    {
        if (capacity < m_buffer.capacity)
        {
            return true;
        }

        const size_t new_capacity = grow_capacity(capacity);

        T* new_data;

        VX_IF_CONSTEXPR(std::is_trivially_destructible<T>::value && std::is_trivially_copyable<T>::value)
        {
            // Use realloc optimization for trivial types
            new_data = static_cast<T*>(allocator::realloc(m_buffer.data, new_capacity * sizeof(T)));
            if (!new_data)
            {
                VX_ERR(err::out_of_memory);
                return false;
            }
        }
        else
        {
            new_data = static_cast<T*>(allocator::alloc(new_capacity * sizeof(T)));
            if (!new_data)
            {
                VX_ERR(err::out_of_memory);
                return false;
            }

            mem::copy_array_elements(new_data, m_buffer.data, m_buffer.size);

            if (m_buffer.data)
            {
                mem::destroy_array_elements(m_buffer.data, m_buffer.size);
                allocator::free(m_buffer.data);
            }
        }

        m_buffer.data = new_data;
        m_buffer.capacity = new_capacity;
        return true;
    }

    template <typename... Args>
    bool emplace_back(Args&&... args) noexcept
    {
        if (m_buffer.size == m_buffer.capacity)
        {
            if (!reserve(m_buffer.size + 1))
            {
                VX_ERR(err::out_of_memory);
                return false;
            }
        }

        mem::construct_in_place(m_buffer.data + m_buffer.size, std::forward<Args>(args)...);
        ++m_buffer.size;
        return true;
    }

    void push_back(const T& value) noexcept
    {
        emplace_back(value);
    }

    void push_back(T&& value) noexcept
    {
        emplace_back(std::move(value));
    }

private:

    static void* allocate(size_t size) noexcept
    {
        return allocator::alloc(size);
    }

    size_t grow_capacity(size_t required_capacity) const noexcept
    {
        const size_t old_capacity = m_buffer.capacity ? m_buffer.capacity : 1;
        size_t new_capacity;

        VX_IF_CONSTEXPR(growth_rate::num == 3 && growth_rate::den == 2)
        {
            new_capacity = old_capacity + old_capacity / 2;
        }
        else
        {
            new_capacity = (old_capacity * growth_rate::num + growth_rate::den - 1) / growth_rate::den;
        }

        if (new_capacity < required_capacity)
        {
            new_capacity = required_capacity;
        }

        return new_capacity;
    }

private:

    struct buffer
    {
        size_t capacity;
        size_t size;
        T* data;
    };

    buffer m_buffer = {};
};

} // namespace _priv
} // namespace vx
