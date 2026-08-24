#pragma once

#include <cstdint>
#include <initializer_list>
#include <utility>
#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/pointer_iterator.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/memory.hpp"
#include "vertex/std/vector_traits.hpp"

namespace vx {

template <size_t N, typename T>
class static_vector
{
    //=========================================================================
    // member types
    //=========================================================================

private:

    VX_STATIC_ASSERT_MSG(N > 0, "N must be greater than 0");

    template <typename V>
    struct is_compatible_vector : is_vector_of<V, T>
    {};

public:

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

    struct buffer_type
    {
        T ptr[N];
        size_type size;
    };

    // fixed-capacity storage; unlike vector there is no allocator or
    // pointer to own, so size is the only thing that ever changes here
    buffer_type m_buffer = {};

    //=========================================================================
    // construction helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    constexpr success construct_n(size_type count, Args&&... args)
    {
        VX_RET_ERR_IF(!count, success{});
        VX_RET_ERR_IF(count > max_size(), err::size_error);

        auto ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

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
            // move the range out of the source, ranges will never overlap since this is fresh storage
            mem::move_uninitialized_range(ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            // copy elements from the source to my static_vector, memcpy is safe
            mem::copy_move_uninitialized_range(ptr, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_move_uninitialized_range(ptr, std::forward<Args>(args)...);
        }

        size = count;
        return success{};
    }

    struct uninitialized_tag
    {};

    constexpr static_vector(uninitialized_tag) noexcept
    {}

public:

    //=========================================================================
    // constructors
    //=========================================================================

    constexpr static_vector() noexcept
    {}

    explicit constexpr static_vector(size_type count)
    {
        const auto e = construct_n<construct_method::default_range>(count);
        VX_VERIFY(e);
    }

    constexpr static_vector(const size_type count, const T& value)
    {
        const auto e = construct_n<construct_method::fill_range>(count, value);
        VX_VERIFY(e);
    }

    constexpr static_vector(std::initializer_list<T> init)
    {
        const auto e = construct_n<construct_method::copy_range>(init.size(), init.begin());
        VX_VERIFY(e);
    }

    constexpr static_vector(const static_vector& other)
    {
        const auto e = construct_n<construct_method::copy_range>(
            other.m_buffer.size,
            other.m_buffer.ptr);
        VX_VERIFY(e);
    }

    // there is no pointer to steal, so "move" just moves each element and
    // empties the source, same observable effect as vector's move ctor
    constexpr static_vector(static_vector&& other) noexcept
    {
        const auto e = construct_n<construct_method::move_range>(other.m_buffer.size, other.m_buffer.ptr);
        VX_VERIFY(e);
        other.destroy_range();
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr static_vector(IT first, IT last) noexcept
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));
        success e;

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            e = construct_n<construct_method::copy_range>(count, first.ptr());
        }
        else
        {
            e = construct_n<construct_method::iterator_range>(count, std::move(first), std::move(last));
        }

        VX_VERIFY(e);
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    constexpr static_vector(const V& v)
    {
        const auto e = construct_n<construct_method::copy_range>(v.size(), v.data());
        VX_VERIFY(e);
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static constexpr expected<static_vector, error> create()
    {
        return static_vector(uninitialized_tag{});
    }

    static constexpr expected<static_vector, error> create(size_type count)
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::default_range>(count);
        return ok ? v : make_unexpected(error{ ok });
    }

    static constexpr expected<static_vector, error> create(size_type count, const T& value)
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::fill_range>(count, value);
        return ok ? v : make_unexpected(error{ ok });
    }

    static constexpr expected<static_vector, error> create(std::initializer_list<T> init)
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::copy_range>(init.size(), init.begin());
        return ok ? v : make_unexpected(error{ ok });
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static constexpr expected<static_vector, error> create(IT first, IT last)
    {
        static_vector v(uninitialized_tag{});
        const size_type count = static_cast<size_type>(std::distance(first, last));

        success ok;
        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            ok = v.template construct_n<construct_method::copy_range>(count, first.ptr());
        }
        else
        {
            ok = v.template construct_n<construct_method::iterator_range>(count, std::move(first), std::move(last));
        }

        return ok ? v : make_unexpected(error{ ok });
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    static constexpr expected<static_vector, error> create(const V& other)
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::copy_range>(other.size(), other.data());
        return ok ? v : make_unexpected(error{ ok });
    }

private:

    //=========================================================================
    // destructor helpers
    //=========================================================================

    constexpr void destroy_range()
    {
        auto& size = m_buffer.size;

        if (size > 0)
        {
            mem::destroy_range(m_buffer.ptr, size);
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
    constexpr operator vector<T, Allocator2>() const
    {
        return vector<T, Allocator2>(begin(), end());
    }

    template <typename Allocator2>
    constexpr operator std::vector<T, Allocator2>() const
    {
        return std::vector<T, Allocator2>(begin(), end());
    }

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, typename Arg>
    constexpr success assign_from(const size_type count, Arg&& arg)
    {
        VX_RET_ERR_IF(count > max_size(), err::size_error);

        auto ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (count > size)
        {
            const size_type tail_count = count - size;

            VX_IF_CONSTEXPR (M == construct_method::fill_range)
            {
                auto mid = mem::fill_range(ptr, size, arg);
                mem::fill_uninitialized_range(mid, tail_count, arg);
            }
            else VX_IF_CONSTEXPR (M == construct_method::move_range)
            {
                auto mid = mem::move_range(ptr, arg, size);
                mem::move_uninitialized_range(mid, arg, tail_count);
            }
            else // VX_IF_CONSTEXPR (M == construct_method::copy_range)
            {
                auto mid = mem::copy_move_range(ptr, arg, size);
                mem::copy_move_uninitialized_range(mid, arg, tail_count);
            }
        }
        else
        {
            pointer mid;

            VX_IF_CONSTEXPR (M == construct_method::fill_range)
            {
                mid = mem::fill_range(ptr, count, arg);
            }
            else VX_IF_CONSTEXPR (M == construct_method::move_range)
            {
                mid = mem::move_range(ptr, arg, count);
            }
            else // copy_range
            {
                mid = mem::copy_move_range(ptr, arg, count);
            }

            mem::destroy_range(mid, size - count);
        }

        size = count;
        return success{};
    }

    template <construct_method M, typename IT1, typename IT2>
    constexpr success assign_from(const size_type count, IT1 first, IT2 last)
    {
        VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");

        VX_RET_ERR_IF(count > max_size(), err::size_error);

        auto ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (count > size)
        {
            const auto mid = std::next(first, static_cast<difference_type>(size));
            mem::copy_move_range(ptr, first, mid);
            mem::copy_move_uninitialized_range(ptr + size, mid, last);
        }
        else
        {
            const auto mid = std::next(first, static_cast<difference_type>(count));
            mem::copy_move_range(ptr, first, mid);
            mem::destroy_range(ptr + count, size - count);
        }

        size = count;
        return success{};
    }

public:

    //=========================================================================
    // assignment operators
    //=========================================================================

    constexpr static_vector& operator=(const static_vector& other)
    {
        if (this != &other)
        {
            const auto e = assign_from<construct_method::copy_range>(other.m_buffer.size, other.m_buffer.ptr);
            VX_VERIFY(e);
        }
        return *this;
    }

    constexpr static_vector& operator=(static_vector&& other) noexcept
    {
        if (this != &other)
        {
            const auto e = assign_from<construct_method::move_range>(other.m_buffer.size, other.m_buffer.ptr);
            VX_VERIFY(e);
            other.destroy_range();
        }
        return *this;
    }

    constexpr static_vector& operator=(std::initializer_list<T> init)
    {
        const auto e = assign_from<construct_method::copy_range>(init.size(), init.begin());
        VX_VERIFY(e);
        return *this;
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    constexpr static_vector& operator=(const V& v)
    {
        const auto e = assign_from<construct_method::copy_range>(v.size(), v.data());
        VX_VERIFY(e);
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    constexpr success assign(const static_vector& other)
    {
        if (this == &other)
        {
            return true;
        }
        return assign_from<construct_method::copy_range>(other.m_buffer.size, other.m_buffer.ptr);
    }

    constexpr void assign(static_vector&& other) noexcept
    {
        operator=(std::move(other));
    }

    constexpr success assign(std::initializer_list<T> init)
    {
        return assign_from<construct_method::copy_range>(init.size(), init.begin());
    }

    constexpr success assign(const pointer ptr, size_type count)
    {
        return assign_from<construct_method::copy_range>(count, ptr);
    }

    constexpr success assign(size_type count, const T& value)
    {
        return assign_from<construct_method::fill_range>(count, value);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr success assign(IT first, IT last)
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
    constexpr success assign(const V& v)
    {
        return assign_from<construct_method::copy_range>(v.size(), v.data());
    }

    //=========================================================================
    // element access
    //=========================================================================

    constexpr expected<T&, error> front() noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return *m_buffer.ptr;
    }

    constexpr expected<const T&, error> front() const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return *m_buffer.ptr;
    }

    constexpr expected<T&, error> back() noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return m_buffer.ptr[m_buffer.size - 1];
    }

    constexpr expected<const T&, error> back() const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return m_buffer.ptr[m_buffer.size - 1];
    }

    constexpr pointer data() noexcept
    {
        return m_buffer.ptr;
    }

    constexpr const_pointer data() const noexcept
    {
        return m_buffer.ptr;
    }

    constexpr T& operator[](size_type i) noexcept
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.ptr[i];
    }

    constexpr const T& operator[](size_type i) const noexcept
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.ptr[i];
    }

    constexpr expected<T&, error> at(size_type i) noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(i >= m_buffer.size, err::out_of_range);
        return operator[](i);
    }

    constexpr expected<const T&, error> at(size_type i) const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(i >= m_buffer.size, err::out_of_range);
        return operator[](i);
    }

    //=========================================================================
    // iterators
    //=========================================================================

    constexpr iterator begin() noexcept
    {
        return iterator(m_buffer.ptr);
    }

    constexpr const_iterator begin() const noexcept
    {
        return const_iterator(m_buffer.ptr);
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr iterator end() noexcept
    {
        return iterator(m_buffer.ptr + m_buffer.size);
    }

    constexpr const_iterator end() const noexcept
    {
        return const_iterator(m_buffer.ptr + m_buffer.size);
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    //=========================================================================
    // memory
    //=========================================================================

    constexpr void clear()
    {
        mem::destroy_range(m_buffer.ptr, m_buffer.size);
        m_buffer.size = 0;
    }

    constexpr void clear_and_deallocate()
    {
        destroy_range();
    }

    static constexpr success shrink_to_fit()
    {
        // fixed capacity: nothing to shrink, always a no-op success
        return success{};
    }

    constexpr void swap(static_vector& other) noexcept
    {
        std::swap(m_buffer.ptr, other.m_buffer.ptr);
        std::swap(m_buffer.size, other.m_buffer.size);
    }

    //=========================================================================
    // size
    //=========================================================================

    constexpr bool empty() const noexcept
    {
        return m_buffer.size == 0;
    }

    constexpr bool full() const noexcept
    {
        return m_buffer.size == max_size();
    }

    constexpr size_type size() const noexcept
    {
        return m_buffer.size;
    }

    constexpr size_type size_bytes() const noexcept
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

    static constexpr success reserve(size_type new_capacity)
    {
        VX_RET_ERR_IF(new_capacity > max_size(), err::size_error);
        return success{};
    }

private:

    //=========================================================================
    // resize
    //=========================================================================

    template <typename... Args>
    constexpr success resize_impl(const size_type new_size, Args&&... args)
    {
        VX_RET_ERR_IF(new_size > max_size(), err::size_error);

        auto ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

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
        return success{};
    }

public:

    template <typename U>
    constexpr success resize(const size_type count, const U& value)
    {
        return resize_impl(count, value);
    }

    constexpr success resize(const size_type count)
    {
        return resize_impl(count);
    }

private:

    //=========================================================================
    // insert
    //=========================================================================

    template <construct_method M, typename... Args>
    constexpr expected<pointer, error> insert_n(pointer pos, size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        const size_type available = N - size;
        VX_RET_UNEXPECTED_ERR_IF(count > available, err::size_error);

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

            VX_IF_CONSTEXPR (type_traits::memmove_is_safe<pointer>::value)
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

    constexpr expected<pointer, error> checked_offset_ptr(size_type off) noexcept
    {
        if (off > m_buffer.size)
        {
            return make_unexpected(error{ err::out_of_range });
        }
        return m_buffer.ptr + off;
    }

    template <construct_method M, typename... Args>
    constexpr expected<iterator, error> insert_checked(size_type off, size_type count, Args&&... args)
    {
        auto p = checked_offset_ptr(off);
        if (!p)
        {
            return make_unexpected(p.error());
        }

        const auto res = insert_n<M>(p.value(), count, std::forward<Args>(args)...);
        if (!res)
        {
            return make_unexpected(res.error());
        }
        return iterator(res.value());
    }

    template <construct_method M, typename... Args>
    constexpr iterator insert_unchecked(const_iterator pos, size_type count, Args&&... args)
    {
        VX_ASSERT(pos >= cbegin() && pos <= cend());
        auto ptr = const_cast<pointer>(pos.ptr());

        const auto e = insert_n<M>(ptr, count, std::forward<Args>(args)...);
        VX_VERIFY(e);

        return iterator(e.value());
    }

public:

    constexpr expected<iterator, error> insert(size_type off, const T& value)
    {
        return emplace(off, value);
    }

    constexpr expected<iterator, error> insert(size_type off, T&& value) noexcept
    {
        return emplace(off, std::move(value));
    }

    constexpr expected<iterator, error> insert(size_type off, size_type count, const T& value)
    {
        return insert_checked<construct_method::fill_range>(off, count, value);
    }

    constexpr expected<iterator, error> insert(size_type off, std::initializer_list<T> init)
    {
        return insert_checked<construct_method::copy_range>(off, init.size(), init.begin());
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr expected<iterator, error> insert(size_type off, IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            return insert_checked<construct_method::copy_range>(off, count, first.ptr());
        }
        else
        {
            return insert_checked<construct_method::iterator_range>(off, count, first, last);
        }
    }

    //=========================================================================
    // insert (const_iterator, std-compatible)
    //=========================================================================

    constexpr iterator insert(const_iterator pos, const T& value)
    {
        return emplace(pos, value);
    }

    constexpr iterator insert(const_iterator pos, T&& value) noexcept
    {
        return emplace(pos, std::move(value));
    }

    constexpr iterator insert(const_iterator pos, size_type count, const T& value)
    {
        return insert_unchecked<construct_method::fill_range>(pos, count, value);
    }

    constexpr iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        return insert_unchecked<construct_method::copy_range>(pos, init.size(), init.begin());
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr iterator insert(const_iterator pos, IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            return insert_unchecked<construct_method::copy_range>(pos, count, first.ptr());
        }
        else
        {
            return insert_unchecked<construct_method::iterator_range>(pos, count, first, last);
        }
    }

    //=========================================================================
    // emplace
    //=========================================================================

    template <typename... Args>
    constexpr expected<iterator, error> emplace_back(Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        VX_RET_UNEXPECTED_ERR_IF(size == N, err::size_error);

        pointer dst = ptr + size;
        mem::construct_in_place_maybe_trivial(dst, std::forward<Args>(args)...);
        ++size;

        return iterator(dst);
    }

    template <typename... Args>
    constexpr expected<iterator, error> emplace(size_type off, Args&&... args)
    {
        return insert_checked<construct_method::single>(off, 1, std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args)
    {
        return insert_unchecked<construct_method::single>(pos, 1, std::forward<Args>(args)...);
    }

    //=========================================================================
    // push back
    //=========================================================================

    constexpr expected<iterator, error> push_back(const T& value)
    {
        return emplace_back(value);
    }

    constexpr expected<iterator, error> push_back(T&& value) noexcept
    {
        return emplace_back(std::move(value));
    }

    //=========================================================================
    // erase
    //=========================================================================

private:

    constexpr pointer erase_n(pointer pos, size_type count)
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

    constexpr success erase(size_type off)
    {
        if (off >= size())
        {
            return error{ err::out_of_range };
        }
        auto ptr = m_buffer.ptr + off;
        erase_n(ptr, 1);
        return success{};
    }

    constexpr success erase(size_type off, size_type count)
    {
        if (off > size() || count > size() - off)
        {
            return error{ err::out_of_range };
        }
        auto ptr = m_buffer.ptr + off;
        erase_n(ptr, count);
        return success{};
    }

    constexpr iterator erase(const_iterator pos)
    {
        VX_ASSERT(pos >= cbegin() && pos < cend());
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = erase_n(ptr, 1);
        return iterator(ptr);
    }

    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        VX_ASSERT(first >= cbegin() && last <= cend() && first <= last);
        auto ptr = const_cast<pointer>(first.ptr());
        const size_type count = static_cast<size_type>(last.ptr() - first.ptr());
        ptr = erase_n(ptr, count);
        return iterator(ptr);
    }

    //=========================================================================

    constexpr success pop_back()
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (size == 0)
        {
            return error{ err::out_of_range };
        }

        --size;
        mem::destroy_in_place(ptr + size);
        return success{};
    }
};

//=========================================================================
// comparison
//=========================================================================

template <size_t N, typename T>
constexpr bool operator==(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return mem::compare_range(lhs.data(), lhs.size(), rhs.data(), rhs.size()) == 0;
}

template <size_t N, typename T>
constexpr bool operator!=(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return !(lhs == rhs);
}

template <size_t N, typename T>
constexpr bool operator<(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return mem::compare_range(lhs.data(), lhs.size(), rhs.data(), rhs.size()) < 0;
}

template <size_t N, typename T>
constexpr bool operator>(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return (rhs < lhs);
}

template <size_t N, typename T>
constexpr bool operator<=(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return !(rhs < lhs);
}

template <size_t N, typename T>
constexpr bool operator>=(const static_vector<N, T>& lhs, const static_vector<N, T>& rhs)
{
    return !(lhs < rhs);
}

} // namespace vx
