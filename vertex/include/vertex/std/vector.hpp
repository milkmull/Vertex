#pragma once

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_memory/allocator.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/_tools/dynamic_array_base.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/growth_policy.hpp"
#include "vertex/std/iterator.hpp"

namespace vx {

template <typename T, typename Allocator = mem::default_allocator<T>, typename Growth = ratio_growth_policy<2>>
class vector
{
    //=========================================================================
    // member types
    //=========================================================================

private:

    VX_STATIC_ASSERT_MSG(
        (std::is_same<T, typename Allocator::value_type>::value),
        "Allocator value type must match T");

    using data_type = _dynamic_array_base_priv::dynamic_array_data<T>;

public:

    using allocator_type = Allocator;
    using growth_policy = Growth;

    using value_type = typename data_type::value_type;
    using pointer = typename data_type::pointer;
    using const_pointer = typename data_type::const_pointer;
    using reference = typename data_type::reference;
    using const_reference = typename data_type::const_reference;
    using size_type = typename data_type::size_type;
    using difference_type = typename data_type::difference_type;

    using iterator = _priv::pointer_iterator<vector, T>;
    using const_iterator = _priv::pointer_iterator<vector, const T>;
    using reverse_iterator = _priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

private:

    enum class construct_method
    {
        single,        // construct a single value
        default_range, // construct from size
        fill_range,    // fill range
        copy_range,    // copy range (no overlap, source stays valid)
        move_range,    // move range (overlap allowed, source stays valid/moved-from)
        iterator_range // construct from iterator range
    };

    // holds the allocator alongside the data_type; empty-base-optimized when
    // allocator_type is stateless, so sizeof(vector) is unaffected in that case
    _priv::compressed_pair<allocator_type, data_type> m_storage;

    allocator_type& m_allocator() noexcept
    {
        return m_storage.first();
    }
    const allocator_type& m_allocator() const noexcept
    {
        return m_storage.first();
    }

    data_type& m_data() noexcept
    {
        return m_storage.second;
    }
    const data_type& m_data() const noexcept
    {
        return m_storage.second;
    }

    //=========================================================================
    // construction helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    success construct_n(size_type count, Args&&... args)
    {
        VX_RET_ERR_IF(!count, success{});
        VX_RET_ERR_IF(count > max_size(), err::size_error);

        pointer new_ptr = m_allocator().allocate(count);
        VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

        VX_IF_CONSTEXPR (M == construct_method::default_range)
        {
            mem::construct_range_maybe_trivial(new_ptr, count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_uninitialized_range(new_ptr, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            // Used by vector(vector&&, alloc) when allocators differ. The
            // destination is a brand new allocation, so it can never
            // overlap with the source: copy_or_move uses memcpy when T is
            // trivially copyable, and falls back to a real move otherwise
            // (the source vector still exists and will run its own
            // destructor later, so ownership must still be transferred).
            mem::copy_or_move_uninitialized_range(new_ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            // copy elements from the source to my vector, memcpy is safe
            mem::copy_uninitialized_range(new_ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_or_move_uninitialized_range(new_ptr, std::forward<Args>(args)...);
        }

        m_data().ptr = new_ptr;
        m_data().size = count;
        m_data().capacity = count;

        return success{};
    }

    struct uninitialized_tag
    {};

    vector(uninitialized_tag, const allocator_type& alloc) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {}

public:

    //=========================================================================
    // constructors
    //=========================================================================

    vector() noexcept(noexcept(allocator_type()))
        : m_storage(_priv::zero_then_variadic_args_tag{})
    {}

    explicit vector(const allocator_type& alloc) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {}

    explicit vector(size_type count, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const auto ok = construct_n<construct_method::default_range>(count);
        VX_VERIFY(ok);
    }

    vector(const size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const auto ok = construct_n<construct_method::fill_range>(count, value);
        VX_VERIFY(ok);
    }

    vector(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const auto ok = construct_n<construct_method::copy_range>(init.size(), init.begin());
        VX_VERIFY(ok);
    }

    vector(const vector& other)
        : m_storage(_priv::one_then_variadic_args_tag{}, other.m_allocator())
    {
        const auto ok = construct_n<construct_method::copy_range>(
            other.m_data().size,
            other.m_data().ptr);
        VX_VERIFY(ok);
    }

    vector(const vector& other, const allocator_type& alloc)
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const auto ok = construct_n<construct_method::copy_range>(
            other.m_data().size,
            other.m_data().ptr);
        VX_VERIFY(ok);
    }

    // move takes over the source's allocator along with its data_type, since
    // the data_type must always be freed by the allocator that produced it
    vector(vector&& other) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, std::move(other.m_allocator()))
    {
        m_data().acquire(other.m_data());
    }

    vector(vector&& other, const allocator_type& alloc) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (alloc == other.get_allocator())
        {
            m_data().acquire(other.m_data());
        }
        else
        {
            const auto ok = construct_n<construct_method::move_range>(other.size(), other.data());
            VX_VERIFY(ok);
        }
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    vector(IT first, IT last, const allocator_type& alloc = allocator_type()) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        success ok;

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            ok = construct_n<construct_method::copy_range>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            ok = construct_n<construct_method::copy_range>(count, first);
        }
        else
        {
            ok = construct_n<construct_method::iterator_range>(count, first, last);
        }

        VX_VERIFY(ok);
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<vector, error> create(const allocator_type& alloc = allocator_type())
    {
        return vector(uninitialized_tag{}, alloc);
    }

    static expected<vector, error> create(size_type count, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const auto ok = v.template construct_n<construct_method::default_range>(count);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    static expected<vector, error> create(size_type count, const T& value, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const auto ok = v.template construct_n<construct_method::fill_range>(count, value);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    static expected<vector, error> create(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const auto ok = v.template construct_n<construct_method::copy_range>(init.size(), init.begin());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    static expected<vector, error> create(const vector& other, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const auto ok = v.template construct_n<construct_method::copy_range>(
            other.m_data().size,
            other.m_data().ptr);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    static expected<vector, error> create(vector&& other, const allocator_type& alloc = allocator_type()) noexcept
    {
        vector v(uninitialized_tag{}, alloc);

        if (alloc == other.get_allocator())
        {
            v.m_data().acquire(other.m_data());
            return v;
        }

        const auto ok = v.template construct_n<construct_method::move_range>(other.size(), other.data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static expected<vector, error> create(IT first, IT last, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);

        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        success ok;
        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            ok = v.template construct_n<construct_method::copy_range>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            ok = v.template construct_n<construct_method::copy_range>(count, first);
        }
        else
        {
            ok = v.template construct_n<construct_method::iterator_range>(count, first, last);
        }

        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return v;
    }

private:

    //=========================================================================
    // destructor helpers
    //=========================================================================

    void destroy_range()
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (ptr)
        {
            mem::destroy_range(ptr, size);
            m_allocator().deallocate(ptr, capacity);
        }

        ptr = nullptr;
        size = 0;
        capacity = 0;
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~vector()
    {
        destroy_range();
    }

    //=========================================================================
    // allocator
    //=========================================================================

    allocator_type get_allocator() const noexcept
    {
        return m_allocator();
    }

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, typename Arg>
    success assign_from(const size_type count, Arg&& arg)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (count > capacity)
        {
            VX_RET_ERR_IF(count > max_size(), err::size_error);
            pointer new_ptr = m_allocator().allocate(count);
            VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

            mem::destroy_range(ptr, size);
            m_allocator().deallocate(ptr, capacity);

            ptr = new_ptr;
            capacity = count;
            size = count;

            VX_IF_CONSTEXPR (M == construct_method::fill_range)
            {
                mem::fill_uninitialized_range(ptr, count, arg);
            }
            else VX_IF_CONSTEXPR (M == construct_method::move_range)
            {
                VX_ASSERT(false);
                //mem::copy_or_move_uninitialized_range(ptr, arg, count);
            }
            else // VX_IF_CONSTEXPR (M == construct_method::copy_range)
            {
                mem::copy_uninitialized_range(ptr, arg, count);
            }

            return success{};
        }

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
                VX_ASSERT(false);
                //auto mid = mem::move_range(ptr, arg, size);
                //mem::copy_or_move_uninitialized_range(mid, arg, tail_count);
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
                VX_ASSERT(false);
                //mid = mem::copy_or_move_range(ptr, arg, count);
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

    template <construct_method M, typename IT1, typename IT2>
    success assign_from(const size_type count, IT1 first, IT2 last)
    {
        VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");

        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (count == 0)
        {
            clear();
            return success{};
        }

        if (count > capacity)
        {
            VX_RET_ERR_IF(count > max_size(), err::size_error);
            pointer new_ptr = m_allocator().allocate(count);
            VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

            mem::destroy_range(ptr, size);
            m_allocator().deallocate(ptr, capacity);

            ptr = new_ptr;
            capacity = count;
            size = count;

            // The buffer is brand new and fully uninitialized here, so
            // the whole range must be placement-constructed via
            // copy_uninitialized_range.
            mem::copy_uninitialized_range(ptr, first, last);

            return success{};
        }

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

    vector& operator=(const vector& other)
    {
        if (this == &other)
        {
            return *this;
        }

        const auto ok = assign_from<construct_method::copy_range>(other.m_data().size, other.m_data().ptr);
        VX_VERIFY(ok);
        return *this;
    }

    vector& operator=(vector&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        destroy_range();
        m_allocator() = std::move(other.m_allocator());
        m_data().acquire(other.m_data());

        return *this;
    }

    vector& operator=(std::initializer_list<T> init)
    {
        const auto ok = assign_from<construct_method::copy_range>(init.size(), init.begin());
        VX_VERIFY(ok);
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    success assign(const vector& other)
    {
        if (this == &other)
        {
            return success{};
        }
        return assign_from<construct_method::copy_range>(other.m_data().size, other.m_data().ptr);
    }

    success assign(vector&& other) noexcept
    {
        operator=(std::move(other));
        return success{};
    }

    success assign(std::initializer_list<T> init)
    {
        return assign_from<construct_method::copy_range>(init.size(), init.begin());
    }

    success assign(const pointer ptr, size_type count)
    {
        return assign_from<construct_method::copy_range>(count, ptr);
    }

    success assign(size_type count, const T& value)
    {
        return assign_from<construct_method::fill_range>(count, value);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    success assign(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            return assign_from<construct_method::copy_range>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return assign_from<construct_method::copy_range>(count, first);
        }
        else
        {
            return assign_from<construct_method::iterator_range>(count, first, last);
        }
    }

    //=========================================================================
    // element access
    //=========================================================================

    expected<T&, error> front() noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return *m_data().ptr;
    }

    expected<const T&, error> front() const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return *m_data().ptr;
    }

    expected<T&, error> back() noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return m_data().ptr[m_data().size - 1];
    }

    expected<const T&, error> back() const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return m_data().ptr[m_data().size - 1];
    }

    pointer data() noexcept
    {
        return m_data().ptr;
    }

    const_pointer data() const noexcept
    {
        return m_data().ptr;
    }

    reference operator[](size_type i) noexcept
    {
        VX_ASSERT(i < m_data().size);
        return m_data().ptr[i];
    }

    const_reference operator[](size_type i) const noexcept
    {
        VX_ASSERT(i < m_data().size);
        return m_data().ptr[i];
    }

    expected<reference, error> at(size_type i) noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(i >= m_data().size, err::out_of_range);
        return operator[](i);
    }

    expected<const_reference, error> at(size_type i) const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(i >= m_data().size, err::out_of_range);
        return operator[](i);
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_data().ptr);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_data().ptr);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(m_data().ptr + m_data().size);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_data().ptr + m_data().size);
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
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        mem::destroy_range(ptr, size);
        size = 0;
    }

    void clear_and_deallocate()
    {
        destroy_range();
    }

    success shrink_to_fit()
    {
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (size == capacity)
        {
            return success{};
        }

        return reallocate<true>(size);
    }

    pointer release() noexcept
    {
        return m_data().release().ptr;
    }

    success acquire(pointer ptr, size_type count) noexcept
    {
        VX_RET_ERR_IF(count > max_size(), err::size_error);
        destroy_range();

        m_data().ptr = ptr;
        m_data().size = count;
        m_data().capacity = count;

        return success{};
    }

    void swap(vector& other) noexcept
    {
        mem::swap(m_storage, other.m_storage);
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_data().size == 0;
    }

    bool full() const noexcept
    {
        return m_data().size == max_size();
    }

    size_type size() const noexcept
    {
        return m_data().size;
    }

    size_type size_bytes() const noexcept
    {
        return size() * sizeof(T);
    }

    constexpr size_type max_size() const noexcept
    {
        return static_cast<size_type>(std::allocator_traits<allocator_type>::max_size(m_allocator()));
    }

    //=========================================================================
    // capacity
    //=========================================================================

    size_type capacity() const noexcept
    {
        return m_data().capacity;
    }

private:

    //=========================================================================
    // reallocate
    //=========================================================================

    template <bool shrinking>
    success reallocate(size_type new_capacity)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        pointer new_ptr = m_allocator().allocate(new_capacity);
        VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

        // use copy_or_move since we have a brand new buffer
        VX_IF_CONSTEXPR (shrinking)
        {
            VX_ASSERT(size > 0);
            mem::copy_or_move_uninitialized_range(new_ptr, ptr, new_capacity);
        }
        else
        {
            mem::copy_or_move_uninitialized_range(new_ptr, ptr, size);
        }

        mem::destroy_range(ptr, size);
        m_allocator().deallocate(ptr, capacity);

        ptr = new_ptr;
        VX_IF_CONSTEXPR (shrinking)
        {
            size = new_capacity;
        }
        capacity = new_capacity;

        return success{};
    }

public:

    //=========================================================================
    // reserve
    //=========================================================================

    success reserve(size_type new_capacity)
    {
        if (new_capacity > m_data().capacity)
        {
            VX_RET_ERR_IF(new_capacity > max_size(), err::size_error);
            return reallocate<false>(new_capacity);
        }

        return success{};
    }

private:

    //=========================================================================
    // resize
    //=========================================================================

    template <typename... Args>
    success resize_reallocate(const size_type new_size, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        VX_RET_ERR_IF(new_size > max_size(), err::size_error);
        pointer new_ptr = m_allocator().allocate(new_size);
        VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

        const size_type grow_count = new_size - size;
        pointer end_ptr = new_ptr + size;

        VX_IF_CONSTEXPR (sizeof...(Args) == 0)
        {
            mem::construct_range_maybe_trivial(end_ptr, grow_count);
        }
        else // VX_IF_CONSTEXPR(sizeof...(Args) == 1)
        {
            VX_STATIC_ASSERT_MSG(sizeof...(Args) == 1, "Invalid arguments");
            mem::fill_uninitialized_range(end_ptr, grow_count, std::forward<Args>(args)...);
        }

        // use copy_or_move since we have a brand new buffer
        mem::copy_or_move_uninitialized_range(new_ptr, ptr, size);

        mem::destroy_range(ptr, size);
        m_allocator().deallocate(ptr, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_size;

        return success{};
    }

    template <typename... Args>
    success resize_impl(const size_type new_size, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        // trim
        if (new_size < size)
        {
            const size_type shrink_count = size - new_size;
            pointer end_ptr = ptr + new_size;
            mem::destroy_range(end_ptr, shrink_count);
        }

        if (new_size > size)
        {
            if (new_size > capacity)
            {
                // seems to be slightly faster to call this rather than inlining
                return resize_reallocate(new_size, std::forward<Args>(args)...);
            }
            else
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
        }

        size = new_size;
        return success{};
    }

public:

    template <typename U>
    success resize(const size_type count, const U& value)
    {
        return resize_impl(count, value);
    }

    success resize(const size_type count)
    {
        return resize_impl(count);
    }

private:

    //=========================================================================
    // insert
    //=========================================================================

    template <construct_method M, typename... Args>
    pointer insert_capacity(pointer pos, size_type count, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        pointer back = ptr + size;
        const size_type affected = static_cast<size_type>(back - pos);

        if (count > affected)
        {
            // new stuff spills over
            //
            // initialize the new elements that will spill over into uninitialized memory
            pointer last = mem::construct_range_maybe_trivial(back, count - affected);
            // use copy_or_move since self insert is not allowed, memcpy is safe
            mem::copy_or_move_uninitialized_range(last, pos, affected);
        }
        else
        {
            // there is no spill over of inserted elements so we can move the existing
            // elements first

            pointer src = back - count;
            // use copy_or_move since self insert is not allowed, memcpy is safe
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
            VX_ASSERT(false);
            //mem::copy_or_move_range(pos, std::forward<Args>(args)..., count);
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

    template <typename op_growth_policy, construct_method M, typename... Args>
    expected<pointer, error> insert_reallocate(pointer pos, size_type count, Args&&... args) noexcept
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        VX_RET_UNEXPECTED_ERR_IF(count > max_size() - size, err::size_error);
        const size_type new_size = size + count;
        const size_type new_capacity = op_growth_policy::next_capacity(new_size, capacity, max_size());
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = m_allocator().allocate(new_capacity);
        VX_RET_UNEXPECTED_ERR_IF(!new_ptr, err::out_of_memory);

        const size_type off = static_cast<size_type>(pos - ptr);
        pointer dst = new_ptr + off;

        // use copy_or_move since we have a brand new buffer
        mem::copy_or_move_uninitialized_range(new_ptr, ptr, off);

        VX_IF_CONSTEXPR (M == construct_method::single)
        {
            mem::construct_in_place_maybe_trivial(dst, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_uninitialized_range(dst, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            VX_ASSERT(false);
            //mem::copy_or_move_uninitialized_range(dst, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            mem::copy_uninitialized_range(dst, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_uninitialized_range(dst, std::forward<Args>(args)...);
        }

        // second split range, same reasoning as above
        mem::copy_or_move_uninitialized_range(dst + count, pos, size - off);

        // destroy original range
        mem::destroy_range(ptr, size);
        m_allocator().deallocate(ptr, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_capacity;

        return dst;
    }

    // Offsett based insertion with runtime range checks.
    template <typename op_growth_policy, construct_method M, typename... Args>
    expected<iterator, error> insert_checked(size_type off, size_type count, Args&&... args)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > m_data().size, err::out_of_range);
        pointer ptr = m_data().ptr + off;

        const size_type available = m_data().capacity - m_data().size;
        if (count <= available)
        {
            ptr = insert_capacity<M>(ptr, count, std::forward<Args>(args)...);
            return ptr;
        }
        else
        {
            const auto ok = insert_reallocate<op_growth_policy, M>(ptr, count, std::forward<Args>(args)...);
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok.error());
            return ok.value();
        }
    }

    // Iterator based insertion with no runtime checks (only assert)
    template <typename op_growth_policy, construct_method M, typename... Args>
    iterator insert_unchecked(const_iterator pos, size_type count, Args&&... args)
    {
        VX_PRIV_ASSERT_CONTIG_INSERTABLE_POSITION(pos);
        auto ptr = const_cast<pointer>(pos.ptr());

        const size_type available = m_data().capacity - m_data().size;
        if (count <= available)
        {
            ptr = insert_capacity<M>(ptr, count, std::forward<Args>(args)...);
            return iterator(ptr);
        }
        else
        {
            const auto ok = insert_reallocate<op_growth_policy, M>(ptr, count, std::forward<Args>(args)...);
            VX_VERIFY(ok);
            return iterator(ok.value());
        }
    }

public:

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, const T& value)
    {
        return emplace<op_growth_policy>(off, value);
    }

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, T&& value) noexcept
    {
        return emplace<op_growth_policy>(off, std::move(value));
    }

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, size_type count, const T& value)
    {
        return insert_checked<op_growth_policy, construct_method::fill_range>(off, count, value);
    }

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, std::initializer_list<T> init)
    {
        return insert_checked<op_growth_policy, construct_method::copy_range>(off, init.size(), init.begin());
    }

    template <typename op_growth_policy = growth_policy, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    expected<iterator, error> insert(size_type off, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            return insert_checked<op_growth_policy, construct_method::copy_range>(off, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return insert_checked<op_growth_policy, construct_method::copy_range>(off, count, first);
        }
        else
        {
            return insert_checked<op_growth_policy, construct_method::iterator_range>(off, count, first, last);
        }
    }

    //=========================================================================
    // insert
    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, const T& value)
    {
        return emplace<op_growth_policy>(pos, value);
    }

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, T&& value) noexcept
    {
        return emplace<op_growth_policy>(pos, std::move(value));
    }

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        return insert_unchecked<op_growth_policy, construct_method::fill_range>(pos, count, value);
    }

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        return insert_unchecked<op_growth_policy, construct_method::copy_range>(pos, init.size(), init.begin());
    }

    template <typename op_growth_policy = growth_policy, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            return insert_unchecked<op_growth_policy, construct_method::copy_range>(pos, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return insert_unchecked<op_growth_policy, construct_method::copy_range>(pos, count, first);
        }
        else
        {
            return insert_unchecked<op_growth_policy, construct_method::iterator_range>(pos, count, first, last);
        }
    }

    //=========================================================================
    // emplace
    //=========================================================================

    template <typename op_growth_policy = growth_policy, typename... Args>
    expected<iterator, error> emplace_back(Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (size == capacity)
        {
            const size_type count = size + 1;
            VX_RET_UNEXPECTED_ERR_IF(count > max_size() - size, err::size_error);

            const size_type new_capacity = op_growth_policy::next_capacity(count, capacity, max_size());
            const auto ok = reallocate<false>(new_capacity);
            VX_RET_UNEXPECTED_ERR_IF(!ok, err::out_of_memory);
        }

        pointer dst = ptr + size;
        mem::construct_in_place(dst, std::forward<Args>(args)...);
        ++size;

        return iterator(dst);
    }

    template <typename op_growth_policy = growth_policy, typename... Args>
    expected<iterator, error> emplace(size_type off, Args&&... args)
    {
        return insert_checked<op_growth_policy, construct_method::single>(off, 1, std::forward<Args>(args)...);
    }

    template <typename op_growth_policy = growth_policy, typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        return insert_unchecked<op_growth_policy, construct_method::single>(pos, 1, std::forward<Args>(args)...);
    }

    //=========================================================================
    // push back
    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> push_back(const T& value)
    {
        return emplace_back<op_growth_policy>(value);
    }

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> push_back(T&& value) noexcept
    {
        return emplace_back<op_growth_policy>(std::move(value));
    }

    //=========================================================================
    // erase
    //=========================================================================

private:

    pointer erase_n(pointer pos, size_type count)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        if (count == 0)
        {
            return pos;
        }

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type tail_count = size - off - count;
        const size_type new_size = size - count;

        // must move because ranges may overlap
        mem::move_range(pos, pos + count, tail_count);
        mem::destroy_range(ptr + new_size, count);

        size = new_size;
        return pos;
    }

public:

    expected<pointer, error> erase(size_type off)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > size(), err::out_of_range);
        auto ptr = m_data().ptr + off;
        return erase_n(ptr, 1);
    }

    expected<pointer, error> erase(size_type off, size_type count)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > size() || count > size() - off, err::out_of_range);
        auto ptr = m_data().ptr + off;
        return erase_n(ptr, count);
    }

    iterator erase(const_iterator pos)
    {
        VX_PRIV_ASSERT_CONTIG_ERASABLE_POSITION(pos);
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = erase_n(ptr, 1);
        return iterator(ptr);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        auto ptr = const_cast<pointer>(first.ptr());
        const size_type count = static_cast<size_type>(last.ptr() - first.ptr());
        ptr = erase_n(ptr, count);
        return iterator(ptr);
    }

    //=========================================================================

    void pop_back()
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        if (size > 0)
        {
            --size;
            mem::destroy_in_place(ptr + size);
        }
    }
};

//=========================================================================
// comparison
//=========================================================================

template <typename T, typename Allocator>
bool operator==(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
{
    return mem::compare_range(lhs.data(), lhs.size(), rhs.data(), rhs.size()) == 0;
}

template <typename T, typename Allocator>
bool operator!=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename Allocator>
bool operator<(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
{
    return mem::compare_range(lhs.data(), lhs.size(), rhs.data(), rhs.size()) < 0;
}

template <typename T, typename Allocator>
bool operator>(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
{
    return (rhs < lhs);
}

template <typename T, typename Allocator>
bool operator<=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
{
    return !(rhs < lhs);
}

template <typename T, typename Allocator>
bool operator>=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
{
    return !(lhs < rhs);
}

} // namespace vx
