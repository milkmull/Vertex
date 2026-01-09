#pragma once

#include <initializer_list>
#include <ratio>
#include <algorithm>
#include <limits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"
#include "vertex/std/_priv/pointer_iterator.hpp"

namespace vx {
namespace _priv {

// Testing shows that allocating memory aligned to the element type is often faster than using no explicit alignment.
// Using an optimal alignment can provide additional speedups by taking advantage of vectorization. Any alignment
// introduces a small amount of wasted memory at the start of the allocation. In typical use cases the performance
// gains are negligible, but for large vectors aligned memory is beneficial, as it enables more effective SIMD
// vectorization.

template <typename T, typename Allocator = default_allocator<T, alignof(T)>>
class dynamic_array_base
{
public:

    using value_type      = T;
    using allocator_type  = Allocator;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    using iterator               = pointer_iterator<T*>;
    using const_iterator         = pointer_iterator<const T*>;
    using reverse_iterator       = reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = reverse_pointer_iterator<const_iterator>;

private:

    template <typename T>
    struct is_native_iterator : std::false_type {};

    template <>
    struct is_native_iterator<iterator> : std::true_type {};

    template <>
    struct is_native_iterator<const_iterator> : std::true_type {};

    template <>
    struct is_native_iterator<reverse_iterator> : std::true_type {};

    template <>
    struct is_native_iterator<const_reverse_iterator> : std::true_type {};

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

    buffer release_buffer()
    {
        auto& b = m_buffer;
        auto& ptr = b.ptr;
        auto& size = b.size;
        auto& capacity = b.capacity;

        buffer old_buffer = std::move(b);

        ptr = nullptr;
        size = 0;
        capacity = 0;

        return old_buffer;
    }

    struct construct_range_tag {};
    struct move_range_tag {};
    struct iterator_range_tag {};

    template <typename Tag, typename... Args>
    inline void construct_n(size_type count, Args&&... args)
    {
        if (!count)
        {
            return;
        }

        auto& b = m_buffer;
        auto& ptr = b.ptr;
        auto& size = b.size;
        auto& capacity = b.capacity;

        VX_UNLIKELY_COLD_PATH(count > max_size(),
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

        VX_IF_CONSTEXPR(sizeof...(Args) == 0)
        {
            VX_STATIC_ASSERT((std::is_same<Tag, construct_range_tag>::value), "Invalid tag");
            mem::construct_range(new_ptr, count);
        }
        else VX_IF_CONSTEXPR(sizeof...(Args) == 1)
        {
            VX_IF_CONSTEXPR((std::is_same<Tag, construct_range_tag>::value))
            {
                mem::construct_range(new_ptr, count, std::forward<Args>(args)...);
            }
            else
            {
                VX_STATIC_ASSERT((std::is_same<Tag, move_range_tag>::value), "Invalid tag");
                // vectors should never overlap so it is safe to use memmove (faster than memcpy)
                mem::move_range(ptr, std::forward<Args>(args)..., count);
            }
        }
        else
        {
            VX_STATIC_ASSERT(sizeof...(Args) == 2, "Invalid argument count");
            VX_STATIC_ASSERT((std::is_same<Tag, iterator_range_tag>::value), "Invalid tag");
            mem::_priv::construct_from_range(ptr, std::forward<Args>(args)...);
        }
    }

public:

    dynamic_array_base()
    {}

    dynamic_array_base(size_type count) noexcept
    {
        construct_n<construct_range_tag>(count);
    }

    template <typename U>
    dynamic_array_base(const size_type count, const U& value)
    {
        construct_n<construct_range_tag>(count, value);
    }

    dynamic_array_base(std::initializer_list<T> init)
    {
        construct_n<move_range_tag>(init.size(), init.begin());
    }

    dynamic_array_base(const dynamic_array_base& other)
    {
        construct_n<move_range_tag>(
            other.m_buffer.size,
            other.m_buffer.ptr
        );
    }

    dynamic_array_base(dynamic_array_base&& other) noexcept
        : m_buffer(other.release_buffer())
    {}

    template <typename IT1, typename IT2>
    dynamic_array_base(IT1 first, IT2 last) noexcept
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR(is_native_iterator<IT1>::value && is_native_iterator<IT2>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            construct_n<move_range_tag>(count, first.ptr());
        }
        else
        {
            construct_n<iterator_range_tag>(count, std::move(first), std::move(last));
        }
    }

    //=========================================================================
    // destructor
    //=========================================================================

private:

    void destroy_range() noexcept
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        if (!ptr)
        {
            return;
        }

        mem::destroy_range(ptr, size);
        allocator_type::deallocate(ptr, capacity);

        ptr = nullptr;
        size = 0;
        capacity = 0;
    }

public:

    ~dynamic_array_base() noexcept
    {
        destroy_range();
    }

    //=========================================================================
    // assignment
    //=========================================================================

private:

    void assign_from(const T* data, const size_type new_count) noexcept
    {
        auto& b = m_buffer;
        auto& ptr = b.ptr;
        auto& size = b.size;
        auto& capacity = b.capacity;

        VX_UNLIKELY_COLD_PATH(new_count > max_size(),
        {
            VX_ERR(err::size_error);
            return;
        });

        mem::destroy_range(ptr, size);

        if (new_count > capacity)
        {
            VX_UNLIKELY_COLD_PATH(!reallocate(new_count),
            {
                return;
            });
        }

        size = new_count;

        // vectors should never overlap so it is safe to use memmove (faster than memcpy)
        mem::move_range(ptr, data, new_count);
    }

public:

    dynamic_array_base& operator=(const dynamic_array_base& other) noexcept
    {
        if (this == std::addressof(other))
        {
            return *this;
        }

        const auto& other_buffer = other.m_buffer;
        const auto other_ptr = other_buffer.ptr;
        const auto other_size = other_buffer.size;

        assign_from(other_ptr, other_size);
        return *this;
    }

    dynamic_array_base& operator=(std::initializer_list<T> init)
    {
        const auto& other_ptr = init.begin();
        const auto& other_size = init.size();

        assign_from(other_ptr, other_size);
        return *this;
    }

    dynamic_array_base& operator=(dynamic_array_base&& other) noexcept
    {
        if (this == std::addressof(other))
        {
            return *this;
        }

        destroy_range();

        auto& buffer = m_buffer;
        buffer = other.release_buffer();

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
        return m_buffer.ptr[i];
    }

    const T& operator[](size_type i) const
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.ptr[i];
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin()
    {
        return iterator(m_buffer.ptr);
    }

    const_iterator begin() const
    {
        return const_iterator(m_buffer.ptr);
    }

    const_iterator cbegin() const
    {
        return begin();
    }

    iterator end()
    {
        return iterator(m_buffer.ptr + m_buffer.size);
    }

    const_iterator end() const
    {
        return const_iterator(m_buffer.ptr + m_buffer.size);
    }

    const_iterator cend() const
    {
        return end();
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end() - 1);
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end() - 1);
    }

    const_reverse_iterator crbegin() const
    {
        return rbegin();
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin() - 1);
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin() - 1);
    }

    const_reverse_iterator crend() const
    {
        return rend();
    }

    //=========================================================================
    // capacity
    //=========================================================================

    bool empty() const
    {
        return m_buffer.ptr == nullptr;
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

    template <typename growth_rate>
    size_type grow_capacity(size_type required_capacity, size_type current_capacity) const
    {
        constexpr size_type max_capacity = max_size();
        current_capacity = current_capacity ? current_capacity : 1;
        
        size_type new_capacity;
        
        VX_IF_CONSTEXPR(growth_rate::num == 3 && growth_rate::den == 2)
        {
            if (current_capacity > max_capacity - current_capacity / 2)
            {
                return max_capacity;
            }
        
            new_capacity = current_capacity + current_capacity / 2;
        }
        else
        {
            // Guard against multiplication overflow: old_capacity * num
            if (current_capacity > max_capacity / growth_rate::num)
            {
                return max_capacity;
            }
        
            const size_type added_capacity = current_capacity * growth_rate::num / growth_rate::den;
            // Guard against addition overflow: old_capacity + added_capacity
            if (current_capacity > max_capacity - added_capacity)
            {
                return max_capacity;
            }
        
            new_capacity = current_capacity + added_capacity;
        }
        
        if (new_capacity < required_capacity)
        {
            new_capacity = required_capacity;
        }
        
        return new_capacity;
    }

    bool reallocate_shrink(size_type new_capacity)
    {
        const size_type bytes = m_buffer.size * sizeof(T);
        constexpr size_type reallocate_threshold = 96000;

        if (bytes < reallocate_threshold)
        {
            return reallocate(new_capacity);
        }
        else
        {
            return reallocate<true>(new_capacity);
        }
    }

    template <bool shrinking = false>
    bool reallocate(size_type new_capacity) noexcept
    {
        auto& buffer = m_buffer;
        auto& ptr = buffer.ptr;
        auto& size = buffer.size;
        auto& capacity = buffer.capacity;

        VX_ASSERT((shrinking && new_capacity < size) || (new_capacity > size));
        pointer new_ptr;

        VX_IF_CONSTEXPR(shrinking && std::is_trivially_destructible<T>::value && std::is_trivially_copyable<T>::value)
        {
            // Use realloc optimization for trivial types only when shrinking
            new_ptr = allocator_type::reallocate(ptr, new_capacity);
            VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return false;
            });
        }
        else
        {
            new_ptr = allocator_type::allocate(new_capacity);
            VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return false;
            });

            if (ptr)
            {
                VX_IF_CONSTEXPR(shrinking)
                {
                    mem::move_range(new_ptr, ptr, new_capacity);
                }
                else
                {
                    mem::move_range(new_ptr, ptr, size);
                }

                mem::destroy_range(ptr, size);
                allocator_type::deallocate(ptr, capacity);
            }
        }

        ptr = new_ptr;
        VX_IF_CONSTEXPR(shrinking)
        {
            size = new_capacity;
        }
        capacity = new_capacity;

        return true;
    }

    // reallocate the vector data, split at pos and shift back by shift
    pointer reallocate_grow_shift(pointer pos, size_type shift) noexcept
    {
        pointer ptr = m_buffer.ptr;
        size_type size = m_buffer.size;
        size_type capacity = m_buffer.capacity;

        const size_type off = pos - ptr;
        const size_type new_capacity = capacity + shift;
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = allocator_type::allocate(new_capacity);
        VX_UNLIKELY_COLD_PATH(!new_ptr,
        {
            return nullptr;
        });

        if (ptr)
        {
            pointer dst = new_ptr;
            pointer src = ptr;

            // copy first range
            mem::move_range(dst, src, off);

            // copy second range
            dst += off + shift;
            src += off;
            mem::move_range(dst, src, size - off);

            // dstroy original range
            mem::destroy_range(ptr, size);
            allocator_type::deallocate(ptr, capacity);
        }

        m_buffer.ptr = new_ptr;
        m_buffer.capacity = new_capacity;

        pointer new_pos = new_ptr + off;
        return new_pos;
    }

    template <typename... Args>
    bool resize_impl(const size_type new_size, Args&&... args)
    {
        auto& buffer = m_buffer;
        auto& ptr = buffer.ptr;
        auto& size = buffer.size;

        VX_UNLIKELY_COLD_PATH(new_size > max_size(),
        {
            VX_ERR(err::size_error);
            return false;
        });

        // trim
        if (new_size < size)
        {
            const size_type shrink_count = size - new_size;
            pointer end_ptr = ptr + new_size;
            mem::destroy_range(end_ptr, shrink_count);
        }

        if (new_size > size)
        {
            VX_UNLIKELY_COLD_PATH(!reallocate(new_size),
            {
                return false;
            });

            const size_type grow_count = new_size - size;
            pointer end_ptr = ptr + size;
            mem::construct_range(end_ptr, grow_count, std::forward<Args>(args)...);
        }

        size = new_size;
        return true;
    }

    template <typename growth_rate>
    bool grow_if_needed(size_type new_size)
    {
        VX_STATIC_ASSERT(growth_rate::num >= 0 && growth_rate::den > 0, "Growth rate must be positive");
        VX_STATIC_ASSERT(growth_rate::num >= growth_rate::den, "Growth rate must be greater or equal to 1");

        auto& buffer = m_buffer;
        const auto capacity = buffer.capacity;

        if (new_size <= capacity)
        {
            return true;
        }

        const size_type new_capacity = grow_capacity<growth_rate>(new_size, capacity);
        VX_UNLIKELY_COLD_PATH(!reserve(new_capacity),
        {
            return false;
        });

        return true;
    }

    pointer insert_n(pointer pos, size_type count)
    {
        pointer ptr = m_buffer.ptr;
        size_type size = m_buffer.size;
        size_type capacity = m_buffer.capacity;

        const size_type available = capacity - size;
        if (count <= available)
        {
            pointer back = ptr + size;

            if (pos != back)
            {
                // initialize the object past the end
                pointer last = back - count;
                mem::construct_in_place(back, std::move(*last));
                // move the existing range back until we get to the insertion
                const size_type end_count = last - pos;
                mem::_priv::move_range_back(back, last, end_count);
            }
        }
        else
        {
            VX_ASSERT(size == capacity);

            pos = reallocate_grow_shift(pos, count);
            VX_UNLIKELY_COLD_PATH(!pos,
            {
                return nullptr;
            });
        }

        m_buffer.size += count;
        return pos;
    }

    pointer erase_n(pointer pos, size_type count)
    {
        auto& buffer = m_buffer;
        auto& ptr = buffer.ptr;
        auto& size = buffer.size;
        auto& capacity = buffer.capacity;

        mem::move_range(pos, pos + count, count);
        mem::destroy_range(ptr + size - count, count);

        capacity += count;
        size -= count;

        return pos;
    }

public:

    bool reserve(size_type new_capacity)
    {
        auto& buffer = m_buffer;
        const auto capacity = buffer.capacity;

        VX_UNLIKELY_COLD_PATH(new_capacity > max_size(),
        {
            VX_ERR(err::size_error);
            return false;
        });

        if (new_capacity <= capacity)
        {
            return true;
        }

        return reallocate(new_capacity);
    }

    bool shrink_to_fit()
    {
        auto& buffer = m_buffer;
        const auto size = buffer.size;
        const auto capacity = buffer.capacity;

        if (size == capacity)
        {
            return true;
        }

        return reallocate_shrink(size);
    }

    //=========================================================================
    // modifiers
    //=========================================================================

    void clear() noexcept
    {
        auto& b = m_buffer;
        auto& ptr = b.ptr;
        auto& size = b.size;

        if (!size)
        {
            return;
        }

        mem::destroy_range(ptr, size);
        size = 0;
    }

    void clear_and_shrink() noexcept
    {
        destroy_range();
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

    template <typename growth_rate = std::ratio<3, 2>, typename... Args>
    bool emplace_back(Args&&... args)
    {
        auto& buffer = m_buffer;
        auto& ptr = buffer.ptr;
        auto& size = buffer.size;

        VX_UNLIKELY_COLD_PATH(!grow_if_needed<growth_rate>(size + 1),
        {
            return false;
        });
        
        mem::construct_in_place(ptr + size, std::forward<Args>(args)...);
        ++size;
        return true;
    }

    template <typename growth_rate = std::ratio<3, 2>>
    void push_back(const T& value)
    {
        emplace_back<growth_rate>(value);
    }

    template <typename growth_rate = std::ratio<3, 2>>
    void push_back(T&& value)
    {
        emplace_back<growth_rate>(std::move(value));
    }

    void pop_back()
    {
        auto& buffer = m_buffer;
        auto& ptr = buffer.ptr;
        auto& size = buffer.size;

        if (size)
        {
            mem::destroy_in_place(ptr + size - 1);
            --size;
        }
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n(ptr.ptr(), 1);
        VX_UNLIKELY_COLD_PATH(!ptr,
        {
            return iterator(nullptr);
        });

        // emplace the value
        mem::construct_in_place(ptr, std::forward<Args>(args)...);
        return iterator(ptr);
    }

    iterator insert(const_iterator pos, const T& value)
    {
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value)
    {
        return emplace(pos, std::move(value));
    }

    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n(ptr, count);
        VX_UNLIKELY_COLD_PATH(!ptr,
        {
            return iterator(nullptr);
        });

        // emplace the value
        mem::construct_range(ptr, count, value);
        return iterator(ptr);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n(ptr, init.size());
        VX_UNLIKELY_COLD_PATH(!ptr,
        {
            return iterator(nullptr);
        });

        mem::move_range(ptr, init.begin(), init.size());
        return iterator(ptr);
    }

    template <typename IT1, typename IT2>
    iterator insert(const_iterator pos, IT1 first, IT2 last)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        const size_type count = static_cast<size_type>(std::distance(first, last));

        ptr = insert_n(ptr, count);
        VX_UNLIKELY_COLD_PATH(!ptr,
        {
            return iterator(nullptr);
        });

        VX_IF_CONSTEXPR(is_native_iterator<IT1>::value && is_native_iterator<IT2>::value)
        {
            mem::move_range(ptr, first.ptr(), count);
        }
        else
        {
            mem::_priv::construct_from_range(ptr, std::move(first), std::move(last));
        }

        return iterator(ptr);
    }

    iterator erase(const_iterator pos)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = erase_n(ptr, 1);
        return iterator(ptr);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        auto ptr = const_cast<pointer>(first.ptr());
        const size_type count = static_cast<size_type>(std::distance(first, last));
        ptr = erase_n(ptr, count);
        return iterator(ptr);
    }

public:

    template <typename U>
    bool resize(const size_type count, const U& value)
    {
        resize_impl(count, value);
    }

    bool resize(const size_type count)
    {
        return resize_impl(count);
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
