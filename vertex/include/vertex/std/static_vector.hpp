#pragma once

#include <cstdint>
#include <initializer_list>
#include <ratio>
#include <utility>
#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_priv/pointer_iterator.hpp"
#include "vertex/std/vector_traits.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

template <size_t N, typename T>
class static_vector
{
private:

    VX_STATIC_ASSERT_MSG(N > 0, "N must be greater than 0");

    template <typename V>
    struct is_compatible_vector : is_vector_of<V, T>
    {};

public:

    //=========================================================================
    // member types
    //=========================================================================

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using iterator = _priv::pointer_iterator<static_vector, T>;
    using const_iterator = _priv::pointer_iterator<static_vector, const T>;
    using reverse_iterator = _priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

private:

    enum class construct_method
    {
        single,        // construct a single value
        default_range, // construct from size
        fill_range,    // fill range
        copy_range,    // copy range (no overlap)
        move_range,    // move range (no overlap)
        iterator_range // construct from iterator range
    };

    struct buffer
    {
        T ptr[N];
        size_type size;
    };

    buffer m_buffer = {};

    //=========================================================================
    // construction helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    void construct_n(size_type count, Args&&... args)
    {
        auto ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (!count)
        {
            return;
        }

        if (count > max_size())
        {
            err::set(err::size_error);
            return;
        }

        VX_IF_CONSTEXPR (M == construct_method::default_range)
        {
            mem::construct_range_maybe_trivial(ptr, count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_uninitialized_range(ptr, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            // move the range out of the source, ranges will never overlap since we just allocated out memory
            mem::move_uninitialized_range(ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            // copy elements from the source to my static_vector, memcpy is safe
            mem::copy_uninitialized_range(ptr, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_uninitialized_range(ptr, std::forward<Args>(args)...);
        }

        size = count;
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    static_vector() noexcept
    {}

    static_vector(size_type count)
    {
        construct_n<construct_method::default_range>(count);
    }

    static_vector(const size_type count, const T& value)
    {
        construct_n<construct_method::fill_range>(count, value);
    }

    static_vector(std::initializer_list<T> init)
    {
        construct_n<construct_method::copy_range>(init.size(), init.begin());
    }

    static_vector(const static_vector& other)
    {
        construct_n<construct_method::copy_range>(
            other.m_buffer.size,
            other.m_buffer.ptr);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static_vector(IT first, IT last) noexcept
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            construct_n<construct_method::copy_range>(count, first.ptr());
        }
        else
        {
            construct_n<construct_method::iterator_range>(count, std::move(first), std::move(last));
        }
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    static_vector(const V& v)
    {
        construct_n<construct_method::copy_range>(v.size(), v.data());
    }

private:

    //=========================================================================
    // destructor helpers
    //=========================================================================

    void destroy_range()
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (size > 0)
        {
            mem::destroy_range(ptr, size);
            size = 0;
        }
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~static_vector()
    {
        destroy_range();
    }

    //=========================================================================
    // operators
    //=========================================================================

    template <typename Allocator2>
    operator std::vector<T, Allocator2>() const
    {
        return std::vector<T, Allocator2>(begin(), end());
    }

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    bool assign_from(const size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (count > max_size())
        {
            err::set(err::size_error);
            return false;
        }

        if (count > size)
        {
            mem::construct_range_maybe_trivial(ptr + size + 1, count - size);
        }
        if (count < size)
        {
            mem::destroy_range(ptr + count + 1, size - count);
        }

        VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_range(ptr, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            mem::move_range(ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            mem::copy_range(ptr, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_range(ptr, std::forward<Args>(args)...);
        }

        size = count;
        return true;
    }

public:

    //=========================================================================
    // assignment operators
    //=========================================================================

    static_vector& operator=(const static_vector& other)
    {
        if (this != std::addressof(other))
        {
            assign_from<construct_method::copy_range>(other.m_buffer.size, other.m_buffer.ptr);
        }

        return *this;
    }

    static_vector& operator=(std::initializer_list<T> init)
    {
        assign_from<construct_method::copy_range>(init.size(), init.begin());
        return *this;
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    static_vector& operator=(const V& v)
    {
        assign_from<construct_method::copy_range>(v.size(), v.data());
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    bool assign(const static_vector& other)
    {
        if (this == std::addressof(other))
        {
            return true;
        }

        return assign_from<construct_method::copy_range>(other.m_buffer.size, other.m_buffer.ptr);
    }

    bool assign(std::initializer_list<T> init)
    {
        return assign_from<construct_method::copy_range>(init.size(), init.begin());
    }

    bool assign(const T* ptr, size_type count)
    {
        return assign_from<construct_method::copy_range>(count, ptr);
    }

    bool assign(size_type count, const T& value)
    {
        return assign_from<construct_method::fill_range>(count, value);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    bool assign(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            return assign_from<construct_method::copy_range>(count, first.ptr());
        }
        else
        {
            return assign_from<construct_method::iterator_range>(count, std::move(first), std::move(last));
        }
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    static_vector& asssign(const V& v)
    {
        assign_from<construct_method::copy_range>(v.size(), v.data());
        return *this;
    }

    //=========================================================================
    // element access
    //=========================================================================

    T& front() noexcept
    {
        VX_ASSERT(m_buffer.ptr && m_buffer.size);
        return *m_buffer.ptr;
    }

    const T& front() const noexcept
    {
        VX_ASSERT(m_buffer.ptr && m_buffer.size);
        return *m_buffer.ptr;
    }

    T& back() noexcept
    {
        VX_ASSERT(m_buffer.ptr && m_buffer.size);
        return m_buffer.ptr[m_buffer.size - 1];
    }

    const T& back() const noexcept
    {
        VX_ASSERT(m_buffer.ptr && m_buffer.size);
        return m_buffer.ptr[m_buffer.size - 1];
    }

    T* data() noexcept
    {
        return m_buffer.ptr;
    }

    const T* data() const noexcept
    {
        return m_buffer.ptr;
    }

    T& operator[](size_type i) noexcept
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.ptr[i];
    }

    const T& operator[](size_type i) const noexcept
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.ptr[i];
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_buffer.ptr);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_buffer.ptr);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(m_buffer.ptr + m_buffer.size);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_buffer.ptr + m_buffer.size);
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    //=========================================================================
    // memory
    //=========================================================================

    void clear()
    {
        mem::destroy_range(m_buffer.ptr, m_buffer.size);
        m_buffer.size = 0;
    }

    void clear_and_deallocate()
    {
        destroy_range();
    }

    static bool shrink_to_fit()
    {
        return true;
    }

    void swap(static_vector& other) noexcept
    {
        std::swap(m_buffer.ptr, other.m_buffer.ptr);
        std::swap(m_buffer.size, other.m_buffer.size);
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_buffer.size == 0;
    }

    bool full() const noexcept
    {
        return m_buffer.size == max_size();
    }

    size_type size() const noexcept
    {
        return m_buffer.size;
    }

    size_type size_bytes() const noexcept
    {
        return size() * sizeof(T);
    }

    static constexpr size_type max_size() noexcept
    {
        return N;
    }

    //=========================================================================
    // capacity
    //=========================================================================

    static constexpr size_type capacity() noexcept
    {
        return N;
    }

    //=========================================================================
    // reserve
    //=========================================================================

    static bool reserve(size_type new_capacity)
    {
        if (new_capacity > max_size())
        {
            err::set(err::size_error);
            return false;
        }

        return true;
    }

private:

    //=========================================================================
    // resize
    //=========================================================================

    template <typename... Args>
    bool resize_impl(const size_type new_size, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (new_size > N)
        {
            err::set(err::size_error);
            return false;
        }

        // trim
        if (new_size < size)
        {
            const size_type shrink_count = size - new_size;
            pointer end_ptr = ptr + new_size;
            mem::destroy_range(end_ptr, shrink_count);
        }

        if (new_size > size)
        {
            const size_type grow_count = new_size - size;
            pointer end_ptr = ptr + size;

            VX_IF_CONSTEXPR (sizeof...(Args) == 0)
            {
                mem::construct_range_maybe_trivial(end_ptr, grow_count);
            }
            else // VX_IF_CONSTEXPR(sizeof...(Args) == 1)
            {
                VX_STATIC_ASSERT_MSG(sizeof...(Args) == 1, "Invalid arguments");
                mem::fill_uninitialized_range(end_ptr, grow_count, std::forward<Args>(args)...);
            }
        }

        size = new_size;
        return true;
    }

public:

    template <typename U>
    bool resize(const size_type count, const U& value)
    {
        return resize_impl(count, value);
    }

    bool resize(const size_type count)
    {
        return resize_impl(count);
    }

private:

    //=========================================================================
    // insert
    //=========================================================================

    template <construct_method M, typename... Args>
    pointer insert_n(pointer pos, size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        const size_type available = N - size;
        if (count > available)
        {
            err::set(err::size_error);
            return nullptr;
        }

        pointer back = ptr + size;
        const size_type affected = static_cast<size_type>(back - pos);

        if (count > affected)
        {
            // new stuff spills over
            //
            // initialize the new elements that will spill over into uninitialized memory
            pointer last = mem::construct_range_maybe_trivial(back, count - affected);
            // move the existing elements that will be moved into uninitialized memory
            mem::move_uninitialized_range(last, pos, affected);
        }
        else
        {
            // there is no spill over of inserted elements so we can move the existing
            // elements first

            // move the values that will spill over into uninitialized memory
            pointer src = back - count;
            mem::move_uninitialized_range(back, src, count);

            // move the values that will be moved into already initialized memory

            VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
            {
                const size_type off = static_cast<size_type>(pos - ptr);
                const size_type tail_count = size - off - count;
                // pointer point to the last element in the range, so adjust
                mem::move(pos + count, pos, tail_count * sizeof(T));
            }
            else
            {
                --back;
                --src;

                while (back != pos)
                {
                    *back = std::move(*src);
                    --src;
                    --back;
                }
            }
        }

        VX_IF_CONSTEXPR (M == construct_method::single)
        {
            *pos = T(std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_range(pos, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            mem::move_range(pos, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            mem::copy_range(pos, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_range(pos, std::forward<Args>(args)...);
        }

        size += count;
        return pos;
    }

public:

    static_vector& insert(size_type off, const T& value)
    {
        return emplace(off, value);
    }

    static_vector& insert(size_type off, T&& value) noexcept
    {
        return emplace(off, std::move(value));
    }

    static_vector& insert(size_type off, size_type count, const T& value)
    {
        VX_ASSERT(off <= size());
        auto ptr = m_buffer.ptr + off;
        insert_n<construct_method::fill_range>(ptr, count, value);
        return *this;
    }

    static_vector& insert(size_type off, std::initializer_list<T> init)
    {
        VX_ASSERT(off <= size());
        auto ptr = m_buffer.ptr + off;
        insert_n<construct_method::copy_range>(ptr, init.size(), init.begin());
        return *this;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static_vector& insert(size_type off, IT first, IT last)
    {
        VX_ASSERT(off <= size());
        auto ptr = m_buffer.ptr + off;
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            insert_n<construct_method::copy_range>(ptr, count, first.ptr());
        }
        else
        {
            insert_n<construct_method::iterator_range>(ptr, count, first, last);
        }

        return *this;
    }

    //=========================================================================
    //=========================================================================

    iterator insert(const_iterator pos, const T& value)
    {
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value) noexcept
    {
        return emplace(pos, std::move(value));
    }

    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n<construct_method::fill_range>(ptr, count, value);
        return iterator(ptr);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n<construct_method::copy_range>(ptr, init.size(), init.begin());
        return iterator(ptr);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            ptr = insert_n<construct_method::copy_range>(ptr, count, first.ptr());
        }
        else
        {
            ptr = insert_n<construct_method::iterator_range>(ptr, count, first, last);
        }

        return iterator(ptr);
    }

    //=========================================================================
    // emplace
    //=========================================================================

    template <typename... Args>
    pointer emplace_back(Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (size == N)
        {
            err::set(err::size_error);
            return nullptr;
        }

        pointer dst = ptr + size;
        mem::construct_in_place_maybe_trivial(dst, std::forward<Args>(args)...);
        ++size;

        return dst;
    }

    template <typename... Args>
    static_vector& emplace(size_type off, Args&&... args)
    {
        VX_ASSERT(off < size());
        auto ptr = m_buffer.ptr + off;
        insert_n<construct_method::single>(ptr, 1, std::forward<Args>(args)...);
        return *this;
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n<construct_method::single>(ptr, 1, std::forward<Args>(args)...);
        return iterator(ptr);
    }

    //=========================================================================
    // push back
    //=========================================================================

    void push_back(const T& value)
    {
        emplace_back(value);
    }

    void push_back(T&& value) noexcept
    {
        emplace_back(std::move(value));
    }

    //=========================================================================
    // erase
    //=========================================================================

private:

    pointer erase_n(pointer pos, size_type count)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type tail_count = size - off - count;
        const size_type new_size = size - count;

        mem::move_range(pos, pos + count, tail_count);
        mem::destroy_range(ptr + new_size, count);

        size = new_size;
        return pos;
    }

public:

    static_vector& erase(size_type off)
    {
        VX_ASSERT(off < size());
        auto ptr = m_buffer.ptr + off;
        erase_n(ptr, 1);
        return *this;
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
        const size_type count = static_cast<size_type>(last.ptr() - first.ptr());
        ptr = erase_n(ptr, count);
        return iterator(ptr);
    }

    void pop_back()
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (size)
        {
            --size;
            mem::destroy_in_place(ptr + size);
        }
    }
};

//=========================================================================
// comparison
//=========================================================================

template <size_t N, typename T>
bool operator==(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return mem::compare_range(lhs.data(), lhs.size(), rhs.data(), rhs.size()) == 0;
}

template <size_t N, typename T>
bool operator!=(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return !(lhs == rhs);
}

template <size_t N, typename T>
bool operator<(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return mem::compare_range(lhs.data(), lhs.size(), rhs.data(), rhs.size()) < 0;
}

template <size_t N, typename T>
bool operator>(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return (rhs < lhs);
}

template <size_t N, typename T>
bool operator<=(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return !(rhs < lhs);
}

template <size_t N, typename T>
bool operator>=(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return !(lhs < rhs);
}

} // namespace vx
