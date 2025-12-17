#pragma once

#include <initializer_list>
#include <ratio>

#include "vertex/config/language_config.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"
#include <algorithm>
#include <limits>
#include <utility>

namespace vx {
namespace _priv {

template <typename T, typename Allocator>
T* uninit_value_construct(T* first, size_t count)
{
    for (; 0 < count; --count)
    {
        mem::construct_in_place(first);
        ++first;
    }

    return first;
}

template <typename T, typename Allocator>
VX_NO_DISCARD T* allocate_at_least(size_t count)
{
    return static_cast<T*>(Allocator::allocate(count * sizeof(T)));
}

template <typename T, typename Allocator = default_allocator<T>, typename Growth = std::ratio<3, 2>>
class dynamic_array_base
{
public:

    using value_type      = T;
    using allocator_type  = Allocator;
    using pointer         = T*;
    using const_pointer   = const pointer;
    using reference       = T&;
    using const_reference = const reference;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    using growth_rate = Growth;
    VX_STATIC_ASSERT(growth_rate::num >= 0 && growth_rate::den > 0, "Growth rate must be positive");
    VX_STATIC_ASSERT(growth_rate::num >= growth_rate::den, "Growth rate must be greater or equal to 1");

private:

    struct buffer
    {
        pointer ptr;
        size_type size;
        size_type capacity;
    };

public:

    //=========================================================================
    // member types
    //=========================================================================

    //=========================================================================
    // construction
    //=========================================================================

private:

    static T* allocate_n(size_type count)
    {
        T* ptr = nullptr;

        if (count == 0)
        {
            return nullptr;
        }

        if (count > max_size())
        {
            //VX_ERR(err::size_error);
            return nullptr;
        }

        T* ptr = allocator_type::allocate(count);
        if (!ptr)
        {
            //VX_ERR(err::out_of_memory);
            return nullptr;
        }

        return ptr;

        allocation_error:
        {
            err::set(err::out_of_memory);
        }
    }

    buffer release_buffer()
    {
        buffer old_buffer = std::move(m_buffer);

        m_buffer.ptr = nullptr;
        m_buffer.size = 0;
        m_buffer.capacity = 0;

        return old_buffer;
    }

    void load_error()
    {

    }

public:

    dynamic_array_base()
    {}

    dynamic_array_base(size_type count) noexcept
    {
        if (!count)
        {
            return;
        }

        auto& b = m_buffer;
        auto& ptr = b.ptr;
        auto& size = b.size;
        auto& capacity = b.capacity;

        VX_UNLIKELY_COLD_PATH(count > mem::_priv::get_max_count<T>(),
        {
            VX_ERR(err::size_error);
            return;
        });

        auto new_ptr = Allocator::allocate(count);
        VX_UNLIKELY_COLD_PATH(!new_ptr,
        {
            return;
        });

        ptr = new_ptr;
        size = count;
        capacity = count;

        mem::construct_range(new_ptr, count);
    }

    template <typename U>
    dynamic_array_base(const size_type count, const U& value)
    {
        T* ptr = allocate_n(count);
        if (ptr)
        {
            mem::construct_range(m_buffer.ptr, count, value);
            m_buffer.ptr = ptr;
            m_buffer.size = count;
            m_buffer.capacity = count;
        }
    }

    dynamic_array_base(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        T* ptr = allocate_n(count);
        if (ptr)
        {
            mem::move_range(ptr, init.begin(), count);
            m_buffer.ptr = ptr;
            m_buffer.size = count;
            m_buffer.capacity = count;
        }
    }

    dynamic_array_base(const dynamic_array_base& other)
    {
        const size_type count = other.m_buffer.size;
        T* ptr = allocate_n(count);
        if (ptr)
        {
            // vectors should never overlap so it is safe to use memmove (faster than memcpy)
            mem::move_range(ptr, other.m_buffer.ptr, count);
            m_buffer.ptr = ptr;
            m_buffer.size = count;
            m_buffer.capacity = count;
        }
    }

    dynamic_array_base(dynamic_array_base&& other)
        : m_buffer(other.release_buffer())
    {}

    //=========================================================================
    // destructor
    //=========================================================================

private:

    void destroy_range()
    {
        VX_ASSERT(m_buffer.ptr);
        mem::destroy_range(m_buffer.ptr, m_buffer.size);
        allocator_type::deallocate(m_buffer.ptr, m_buffer.capacity);
    }

public:

    ~dynamic_array_base()
    {
        if (m_buffer.ptr)
        {
            destroy_range();
        }
    }

    //=========================================================================
    // assignment
    //=========================================================================

private:

    bool assign_from(const T* data, const size_type new_count)
    {
        if (!data || new_count == 0)
        {
            clear();
            return true;
        }

        if (new_count > max_size())
        {
            VX_ERR(err::size_error);
            return false;
        }

        const size_type old_count = size();
        if (old_count)
        {
            mem::destroy_range(m_buffer.ptr, old_count);
        }

        const size_type current_capacity = capacity();
        if (new_count > current_capacity)
        {
            T* new_data = allocator_type::reallocate(m_buffer.ptr, new_count);
            if (!new_data)
            {
                VX_ERR(err::out_of_memory);
                return false;
            }

            m_buffer.ptr = new_data;
            m_buffer.size = new_count;
            m_buffer.capacity = new_count;
        }

        // vectors should never overlap so it is safe to use memmove (faster than memcpy)
        mem::move_range(m_buffer.ptr, data, new_count);
        return true;
    }

public:

    dynamic_array_base& operator=(const dynamic_array_base& other)
    {
        if (this == std::addressof(other))
        {
            return *this;
        }

        assign_from(other.m_buffer.ptr, other.m_buffer.size);
        return *this;
    }

    dynamic_array_base& operator=(std::initializer_list<T> init)
    {
        assign_from(init.begin(), init.size());
        return *this;
    }

    dynamic_array_base& operator=(dynamic_array_base&& other)
    {
        if (this == std::addressof(other))
        {
            return *this;
        }

        if (m_buffer.ptr)
        {
            destroy_range();
        }

        m_buffer = other.release_buffer();
        return *this;
    }

    //=========================================================================
    // element access
    //=========================================================================

    T* front()
    {
        return m_buffer.ptr;
    }

    const T* front() const
    {
        return m_buffer.ptr;
    }

    T* back()
    {
        return m_buffer.ptr + m_buffer.size;
    }

    const T* back() const
    {
        return m_buffer.ptr + m_buffer.size;
    }

    const T* data() const
    {
        return m_buffer.ptr;
    }

    T* data()
    {
        return m_buffer.ptr;
    }

    T& operator[](size_type i)
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.first[i];
    }

    const T& operator[](size_type i) const
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.first[i];
    }

    //=========================================================================
    // capacity
    //=========================================================================

    bool empty() const
    {
        return m_buffer.first == nullptr;
    }

    size_type size() const
    {
        return m_buffer.size;
    }

    size_type data_size() const
    {
        return size() * sizeof(T);
    }

    static constexpr size_type max_size()
    {
        return std::min(std::numeric_limits<size_type>::max(), mem::_priv::get_max_count<T>());
    }

    size_type capacity() const
    {
        return m_buffer.capacity;
    }

private:

    // grow to a new capacity
    bool reserve_grow(size_type new_capacity)
    {
        //VX_ASSERT(m_buffer.capacity < new_capacity);
        //
        //auto& ptr = m_buffer.ptr;
        //auto& capacity = m_buffer.capacity;
        //
        //pointer new_ptr;
        //
        //VX_IF_CONSTEXPR((std::is_trivially_destructible<T>::value && std::is_trivially_copyable<T>::value))
        //{
        //    // Use reallocate optimization for trivial types
        //    new_ptr = allocator_type::reallocate(ptr, new_capacity);
        //    if (!new_ptr)
        //    {
        //        VX_ERR(err::out_of_memory);
        //        return false;
        //    }
        //
        //    ptr = new_ptr;
        //    capacity = new_capacity;
        //}
        //else
        //{
        //    new_ptr = allocator_type::allocate(new_capacity);
        //    if (!new_ptr)
        //    {
        //        VX_ERR(err::out_of_memory);
        //        return false;
        //    }
        //
        //    auto count = m_buffer.size;
        //
        //    // should never overlap so it is safe to use memmove (faster than memcpy)
        //    VX_IF_CONSTEXPR(type_traits::memmove_is_safe<T*>::value)
        //    {
        //        mem::move(new_ptr, ptr, count * sizeof(T));
        //    }
        //    else
        //    {
        //        //for (pointer tmp = ptr; 0 < count; --count)
        //        //{
        //        //    mem::construct_in_place(new_ptr, std::move(*tmp));
        //        //    ++new_ptr;
        //        //    ++tmp;
        //        //}
        //        //
        //        //ptr = 
        //    }
        //
        //    mem::destroy_range(first, count);
        //    allocator_type::deallocate(first, count);
        //    first = ptr;
        //}

        return true;
    }

private:

    size_type grow_capacity(size_type required_capacity, size_type current_capacity) const
    {
        //constexpr size_type max_capacity = max_size();
        //current_capacity = current_capacity ? current_capacity : 1;
        //
        //size_type new_capacity;
        //
        //VX_IF_CONSTEXPR(growth_rate::num == 3 && growth_rate::den == 2)
        //{
        //    if (current_capacity > max_capacity - current_capacity / 2)
        //    {
        //        return max_capacity;
        //    }
        //
        //    new_capacity = current_capacity + current_capacity / 2;
        //}
        //else
        //{
        //    // Guard against multiplication overflow: old_capacity * num
        //    if (current_capacity > max_capacity / growth_rate::num)
        //    {
        //        return max_capacity;
        //    }
        //
        //    const size_type added_capacity = current_capacity * growth_rate::num / growth_rate::den;
        //    // Guard against addition overflow: old_capacity + added_capacity
        //    if (current_capacity > max_capacity - added_capacity)
        //    {
        //        return max_capacity;
        //    }
        //
        //    new_capacity = current_capacity + added_capacity;
        //}
        //
        //if (new_capacity < required_capacity)
        //{
        //    new_capacity = required_capacity;
        //}
        //
        //return new_capacity;
    }

public:

    bool reserve(size_type new_capacity)
    {
        //pointer& first = m_buffer.first;
        //pointer& end = m_buffer.end;
        //
        //const size_type current_capacity = static_cast<size_type>(end - first);
        //if (new_capacity < current_capacity)
        //{
        //    return true;
        //}
        //
        //new_capacity = grow_capacity(new_capacity, current_capacity);
        //return reserve_grow(new_capacity);
    }

    bool shrink_to_fit()
    {
        //if (m_buffer.last != m_buffer.end)
        //{
        //    const size_type count = size();
        //    T* new_data = allocator_type::reallocate(m_buffer.first, count);
        //    if (!new_data)
        //    {
        //        //VX_ERR(err::bad_allocation);
        //        return false;
        //    }
        //
        //    m_buffer.first = new_data;
        //    m_buffer.last = new_data + count;
        //    m_buffer.end = m_buffer.last;
        //}
        //
        //return true;
    }

    //=========================================================================
    // modifiers
    //=========================================================================

    void clear()
    {
        if (m_buffer.ptr)
        {
            destroy_range();
        }

        m_buffer.ptr = nullptr;
        m_buffer.size = 0;
        m_buffer.capacity = 0;
    }

    T* release()
    {
        const buffer old_buffer = release_buffer();
        return old_buffer.ptr;
    }

    bool acquire(T* ptr, size_type count)
    {
        if (count > max_size())
        {
            VX_ERR(err::size_error);
            return false;
        }

        if (m_buffer.ptr)
        {
            destroy_range();
        }

        m_buffer.ptr = ptr;
        m_buffer.size = count;
        m_buffer.capacity = count;
        return true;
    }

    template <typename... Args>
    bool emplace_back(Args&&... args)
    {
        //pointer& first = m_buffer.first;
        //pointer& last = m_buffer.last;
        //pointer& end = m_buffer.end;
        //const size_type count = static_cast<size_type>(last - first);
        //
        //if (last == end)
        //{
        //    const size_type new_capacity = count + 1;
        //    if (!reserve(new_capacity))
        //    {
        //        VX_ERR(err::out_of_memory);
        //        return false;
        //    }
        //}
        //
        //mem::construct_in_place(last, std::forward<Args>(args)...);
        //++last;
        return true;
    }

    void push_back(const T& value)
    {
        emplace_back(value);
    }

    void push_back(T&& value)
    {
        emplace_back(std::move(value));
    }

    void pop_back()
    {
        //if (m_buffer.first)
        //{
        //    mem::destroy_in_place(m_buffer.last);
        //    --m_buffer.last;
        //}
    }

private:

    struct value_init_tag {};

    template <typename U>
    bool resize_impl(const size_type new_size, const U& value)
    {
        //const size_type old_size = size();
        //
        //if (new_size == old_size)
        //{
        //    return true;
        //}
        //
        //// trim
        //if (new_size < old_size)
        //{
        //    const size_type trim_count = old_size - new_size;
        //    T* new_last = m_buffer.last - trim_count;
        //    mem::destroy_range(new_last, trim_count);
        //    m_buffer.last = new_last;
        //}
        //
        //// append
        //if (new_size > old_size)
        //{
        //    const size_type old_capacity = capacity();
        //    if (new_size > old_capacity)
        //    {
        //        // reallocate
        //        if (!reserve(new_size))
        //        {
        //            return false;
        //        }
        //
        //        const size_type grow_count = new_size - old_size;
        //
        //        VX_IF_CONSTEXPR((std::is_same<U, value_init_tag>::value))
        //        {
        //            mem::construct_range(m_buffer.first + old_size, grow_count);
        //        }
        //        else
        //        {
        //            mem::construct_range(m_buffer.first + old_size, grow_count, value);
        //        }
        //    }
        //}

        return true;
    }

public:

    bool resize(const size_type count)
    {
        return resize_impl(count, value_init_tag{});
    }

    template <typename U>
    bool resize(const size_type count, const U& value)
    {
        return resize_impl(count, value);
    }

    //=========================================================================
    // comparison
    //=========================================================================

    friend bool operator==(const dynamic_array_base& lhs, const dynamic_array_base& rhs)
    {
        const size_type lhs_count = lhs.m_buffer.size;
        const size_type rhs_count = rhs.m_buffer.size;

        if (lhs_count != rhs_count)
        {
            return false;
        }

        if (lhs_count == 0)
        {
            return true;
        }

        return mem::compare_range(lhs.m_buffer.ptr, rhs.m_buffer.ptr, lhs_count);
    }

    friend bool operator!=(const dynamic_array_base& lhs, const dynamic_array_base& rhs)
    {
        return !operator==(lhs, rhs);
    }

private:

    buffer m_buffer = {};
};

} // namespace _priv
} // namespace vx
