#pragma once

#include <cstdint>
#include <initializer_list>
#include <utility>
#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/pointer_iterator.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/iterator.hpp"
#include "vertex/std/memory.hpp"
#include "vertex/std/vector_traits.hpp"

namespace vx {

//=========================================================================
// storage
//=========================================================================

namespace _static_vector_priv {

template <size_t N, typename T>
struct static_vector_storage_trivial
{
    T ptr[N];
    size_t size = 0;
};

template <size_t N, typename T>
struct static_vector_storage_nontrivial
{
    T ptr[N];
    size_t size = 0;

    ~static_vector_storage_nontrivial()
    {
        if (size)
        {
            mem::destroy_range(ptr, size);
        }
    }
};

template <size_t N, typename T>
using static_vector_storage = typename std::conditional<
    std::is_trivially_destructible<T>::value,
    static_vector_storage_trivial<N, T>,
    static_vector_storage_nontrivial<N, T>>::type;

} // namespace _static_vector_priv

template <size_t N, typename T>
class static_vector
{
    //=========================================================================
    // member types
    //=========================================================================

private:

    VX_STATIC_ASSERT_MSG(N > 0, "N must not be 0");

    using data_type = _static_vector_priv::static_vector_storage<N, T>;

    template <typename IT>
    using is_my_iterator = _priv::is_my_pointer_iterator<IT, static_vector>;

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

    data_type m_data;

    //=========================================================================
    // construction helpers
    //=========================================================================

    template <construct_method M, bool Fits, typename... Args>
    constexpr success construct_n(size_type count, Args&&... args)
    {
        VX_RET_ERR_IF(!count, success{});

        VX_IF_CONSTEXPR (!Fits)
        {
            VX_RET_ERR_IF(count > max_size(), err::size_error);
        }

        auto ptr = m_data.ptr;
        auto& size = m_data.size;

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
            // used by the move constructor / create(static_vector&&): the
            // destination is a fresh (or freshly-cleared) buffer belonging
            // to a different object, so it can never overlap the source.
            // copy_or_move uses memcpy when T is trivially copyable, and
            // falls back to a real move otherwise.
            mem::copy_or_move_uninitialized_range(ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            // copy elements from the source to my static_vector, memcpy is safe
            mem::copy_uninitialized_range(ptr, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_or_move_uninitialized_range(ptr, std::forward<Args>(args)...);
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
        const auto ok = construct_n<construct_method::default_range, false>(count);
        VX_VERIFY(ok);
    }

    constexpr static_vector(const size_type count, const T& value)
    {
        const auto ok = construct_n<construct_method::fill_range, false>(count, value);
        VX_VERIFY(ok);
    }

    constexpr static_vector(std::initializer_list<T> init)
    {
        const auto ok = construct_n<construct_method::copy_range, false>(init.size(), init.begin());
        VX_VERIFY(ok);
    }

    constexpr static_vector(const static_vector& other)
    {
        construct_n<construct_method::copy_range, true>(
            other.m_data.size,
            other.m_data.ptr);
    }

    constexpr static_vector(static_vector&& other) noexcept
    {
        construct_n<construct_method::move_range, true>(other.m_data.size, other.m_data.ptr);
        other.destroy_range();
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr static_vector(IT first, IT last) noexcept
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        success ok;

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            constexpr bool fits = is_my_iterator<IT>::value;
            ok = construct_n<construct_method::copy_range, fits>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            ok = construct_n<construct_method::copy_range, false>(count, first);
        }
        else
        {
            ok = construct_n<construct_method::iterator_range, false>(count, first, last);
        }

        VX_VERIFY(ok);
    }

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr explicit static_vector(const static_vector<M, T>& v)
    {
        const auto ok = construct_n<construct_method::copy_range, true>(v.size(), v.data());
        VX_VERIFY(ok);
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
        const auto ok = v.template construct_n<construct_method::default_range, false>(count);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    static constexpr expected<static_vector, error> create(size_type count, const T& value)
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::fill_range, false>(count, value);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    static constexpr expected<static_vector, error> create(std::initializer_list<T> init)
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::copy_range, false>(init.size(), init.begin());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    static constexpr expected<static_vector, error> create(const static_vector& other)
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::copy_range, true>(
            other.m_data.size,
            other.m_data.ptr);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    static constexpr expected<static_vector, error> create(static_vector&& other) noexcept
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::move_range, true>(
            other.m_data.size,
            other.m_data.ptr);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);

        other.destroy_range();
        return v;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static constexpr expected<static_vector, error> create(IT first, IT last)
    {
        static_vector v(uninitialized_tag{});

        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        success ok;
        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            constexpr bool fits = is_my_iterator<IT>::value;
            ok = v.template construct_n<construct_method::copy_range, fits>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            ok = v.template construct_n<construct_method::copy_range, false>(count, first);
        }
        else
        {
            ok = v.template construct_n<construct_method::iterator_range, false>(count, first, last);
        }

        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    template <size_t M, VX_REQUIRES(M <= N)>
    static constexpr expected<static_vector, error> create(const static_vector<M, T>& other)
    {
        static_vector v(uninitialized_tag{});
        const auto ok = v.template construct_n<construct_method::copy_range, true>(other.size(), other.data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

private:

    //=========================================================================
    // destructor helpers
    //=========================================================================

    constexpr void destroy_range()
    {
        auto& size = m_data.size;

        if (size > 0)
        {
            mem::destroy_range(m_data.ptr, size);
            size = 0;
        }
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~static_vector() = default;

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, bool Fits, typename Arg>
    constexpr success assign_from(const size_type count, Arg&& arg)
    {
        VX_IF_CONSTEXPR (!Fits)
        {
            VX_RET_ERR_IF(count > max_size(), err::size_error);
        }

        auto ptr = m_data.ptr;
        auto& size = m_data.size;

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
                // This function should never be called with any self referential
                // data so it should be fine to copy trivial types on move.
                auto mid = mem::copy_or_move_range(ptr, arg, size);
                mem::copy_or_move_uninitialized_range(mid, arg + size, tail_count);
            }
            else // VX_IF_CONSTEXPR (M == construct_method::copy_range)
            {
                auto mid = mem::copy_range(ptr, arg, size);
                mem::copy_uninitialized_range(mid, arg + size, tail_count);
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
                mid = mem::copy_or_move_range(ptr, arg, count);
            }
            else // copy_range
            {
                mid = mem::copy_range(ptr, arg, count);
            }

            mem::destroy_range(mid, size - count);
        }

        size = count;
        return success{};
    }

    template <construct_method M, bool Fits, typename IT1, typename IT2>
    constexpr success assign_from(const size_type count, IT1 first, IT2 last)
    {
        VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");

        VX_IF_CONSTEXPR (!Fits)
        {
            VX_RET_ERR_IF(count > max_size(), err::size_error);
        }

        auto ptr = m_data.ptr;
        auto& size = m_data.size;

        if (count > size)
        {
            const auto mid = std::next(first, static_cast<difference_type>(size));
            mem::copy_range(ptr, first, mid);
            mem::copy_uninitialized_range(ptr + size, mid, last);
        }
        else
        {
            const auto mid = std::next(first, static_cast<difference_type>(count));
            mem::copy_range(ptr, first, mid);
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
        if (this == &other)
        {
            return *this;
        }

        assign_from<construct_method::copy_range, true>(other.m_data.size, other.m_data.ptr);
        return *this;
    }

    constexpr static_vector& operator=(static_vector&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        assign_from<construct_method::move_range, true>(other.m_data.size, other.m_data.ptr);
        other.destroy_range();
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    constexpr static_vector& operator=(std::initializer_list<T> init)
    {
        const auto ok = assign_from<construct_method::copy_range, false>(init.size(), init.begin());
        VX_VERIFY(ok);
        return *this;
    }

    constexpr success assign(const static_vector& other)
    {
        if (this == &other)
        {
            return success{};
        }
        return assign_from<construct_method::copy_range, true>(other.m_data.size, other.m_data.ptr);
    }

    constexpr success assign(static_vector&& other) noexcept
    {
        operator=(std::move(other));
        return success{};
    }

    constexpr success assign(std::initializer_list<T> init)
    {
        return assign_from<construct_method::copy_range, false>(init.size(), init.begin());
    }

    constexpr success assign(const pointer ptr, size_type count)
    {
        return assign_from<construct_method::copy_range, false>(count, ptr);
    }

    constexpr success assign(size_type count, const T& value)
    {
        return assign_from<construct_method::fill_range, false>(count, value);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr success assign(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            constexpr bool fits = is_my_iterator<IT>::value;
            return assign_from<construct_method::copy_range, fits>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return assign_from<construct_method::copy_range, false>(count, first);
        }
        else
        {
            return assign_from<construct_method::iterator_range, false>(count, first, last);
        }
    }

    //=========================================================================
    // element access
    //=========================================================================

    constexpr expected<T&, error> front() noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return *m_data.ptr;
    }

    constexpr expected<const T&, error> front() const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return *m_data.ptr;
    }

    constexpr expected<T&, error> back() noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return m_data.ptr[m_data.size - 1];
    }

    constexpr expected<const T&, error> back() const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return m_data.ptr[m_data.size - 1];
    }

    constexpr pointer data() noexcept
    {
        return m_data.ptr;
    }

    constexpr const_pointer data() const noexcept
    {
        return m_data.ptr;
    }

    constexpr T& operator[](size_type i) noexcept
    {
        VX_ASSERT(i < m_data.size);
        return m_data.ptr[i];
    }

    constexpr const T& operator[](size_type i) const noexcept
    {
        VX_ASSERT(i < m_data.size);
        return m_data.ptr[i];
    }

    constexpr expected<T&, error> at(size_type i) noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(i >= m_data.size, err::out_of_range);
        return operator[](i);
    }

    constexpr expected<const T&, error> at(size_type i) const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(i >= m_data.size, err::out_of_range);
        return operator[](i);
    }

    //=========================================================================
    // iterators
    //=========================================================================

    constexpr iterator begin() noexcept
    {
        return iterator(m_data.ptr);
    }

    constexpr const_iterator begin() const noexcept
    {
        return const_iterator(m_data.ptr);
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr iterator end() noexcept
    {
        return iterator(m_data.ptr + m_data.size);
    }

    constexpr const_iterator end() const noexcept
    {
        return const_iterator(m_data.ptr + m_data.size);
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
        mem::destroy_range(m_data.ptr, m_data.size);
        m_data.size = 0;
    }

    constexpr void clear_and_deallocate()
    {
        destroy_range();
    }

    static constexpr success shrink_to_fit()
    {
        return success{};
    }

    constexpr void swap(static_vector& other) noexcept
    {
        auto ptr = m_data.ptr;
        auto other_ptr = other.m_data.ptr;
        auto& size = m_data.size;
        auto& other_size = other.m_data.size;

        const size_type min_size = size < other_size ? size : other_size;

        // swap the elements that exist on both sides
        mem::swap_range(ptr, other_ptr, min_size);

        if (size > other_size)
        {
            // move the extra tail from *this into other's uninitialized
            // storage, then destroy it here since it has been moved from.
            // The two buffers belong to different objects so they can
            // never overlap: copy_or_move uses memcpy when safe.
            const size_type tail_count = size - other_size;
            mem::copy_or_move_uninitialized_range(other_ptr + min_size, ptr + min_size, tail_count);
            mem::destroy_range(ptr + min_size, tail_count);
        }
        else if (other_size > size)
        {
            const size_type tail_count = other_size - size;
            mem::copy_or_move_uninitialized_range(ptr + min_size, other_ptr + min_size, tail_count);
            mem::destroy_range(other_ptr + min_size, tail_count);
        }

        std::swap(size, other_size);
    }

    //=========================================================================
    // size
    //=========================================================================

    constexpr bool empty() const noexcept
    {
        return m_data.size == 0;
    }

    constexpr bool full() const noexcept
    {
        return m_data.size == max_size();
    }

    constexpr size_type size() const noexcept
    {
        return m_data.size;
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

    static constexpr success reserve(size_type new_capacity) noexcept
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

        auto ptr = m_data.ptr;
        auto& size = m_data.size;

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
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

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
            // use copy_or_move since this slice never overlaps [pos, back), memcpy is safe
            mem::copy_or_move_uninitialized_range(last, pos, affected);
        }
        else
        {
            // there is no spill over of inserted elements so we can move the existing
            // elements first

            // use copy_or_move since this slice never overlaps [pos, back - count), memcpy is safe
            pointer src = back - count;
            mem::copy_or_move_uninitialized_range(back, src, count);

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

    template <construct_method M, typename... Args>
    constexpr expected<iterator, error> insert_checked(size_type off, size_type count, Args&&... args)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > m_data.size, err::out_of_range);
        pointer ptr = m_data.ptr + off;

        const auto ok = insert_n<M>(ptr, count, std::forward<Args>(args)...);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok.error());
        return iterator(ok.value());
    }

    template <construct_method M, typename... Args>
    constexpr iterator insert_unchecked(const_iterator pos, size_type count, Args&&... args)
    {
        VX_PRIV_ASSERT_CONTIG_INSERTABLE_POSITION(pos);
        auto ptr = const_cast<pointer>(pos.ptr());

        const auto ok = insert_n<M>(ptr, count, std::forward<Args>(args)...);
        VX_VERIFY(ok);
        return iterator(ok.value());
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
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            return insert_checked<construct_method::copy_range>(off, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return insert_checked<construct_method::copy_range>(off, count, first);
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
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            return insert_unchecked<construct_method::copy_range>(pos, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return insert_unchecked<construct_method::copy_range>(pos, count, first);
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
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

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
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        if (count == 0)
        {
            return pos;
        }

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type tail_count = size - off - count;
        const size_type new_size = size - count;

        mem::move_range(pos, pos + count, tail_count);
        mem::destroy_range(ptr + new_size, count);

        size = new_size;
        return pos;
    }

public:

    constexpr expected<pointer, error> erase(size_type off)
    {
        VX_RET_UNEXPECTED_ERR_IF(off >= size(), err::out_of_range);
        auto ptr = m_data.ptr + off;
        return erase_n(ptr, 1);
    }

    constexpr expected<pointer, error> erase(size_type off, size_type count)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > size() || count > size() - off, err::out_of_range);
        auto ptr = m_data.ptr + off;
        return erase_n(ptr, count);
    }

    constexpr iterator erase(const_iterator pos)
    {
        VX_PRIV_ASSERT_CONTIG_ERASABLE_POSITION(pos);
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = erase_n(ptr, 1);
        return iterator(ptr);
    }

    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        auto ptr = const_cast<pointer>(first.ptr());
        const size_type count = static_cast<size_type>(last.ptr() - first.ptr());
        ptr = erase_n(ptr, count);
        return iterator(ptr);
    }

    //=========================================================================

    constexpr void pop_back()
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

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
