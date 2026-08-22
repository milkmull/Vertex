#pragma once

#include <cstdint>
#include <initializer_list>
#include <ratio>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_memory/allocator.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/_tools/dynamic_array_base.hpp"
#include "vertex/std/_tools/pointer_iterator.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/vector_traits.hpp"

//#define VX_VECTOR_DISABLE_MAX_SIZE_CHECK 1

namespace vx {

template <typename T, typename Allocator = mem::default_allocator<T>>
class vector
{
    //=========================================================================
    // member types
    //=========================================================================

private:

    template <typename V>
    struct is_compatible_vector : is_vector_of<V, T>
    {};

    VX_STATIC_ASSERT_MSG(
        (std::is_same<T, typename Allocator::value_type>::value),
        "Allocator value type must match T");

    using data_type = _dynamic_array_base_priv::dynamic_array_data<T>;

public:

    template <intmax_t N, intmax_t D = 1>
    using growth_rate_type = std::ratio<N, D>;
    using default_growth_rate = growth_rate_type<3, 2>;

    using allocator_type = Allocator;

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
        copy_range,    // copy range (no overlap)
        move_range,    // move range (no overlap)
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
        VX_UNLIKELY_COLD_PATH(!count,
            {
                return make_error(err::none);
            });

#if !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size(),
            {
                return make_error(err::size_error);
            });

#endif // !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

        auto new_ptr = m_allocator().allocate(count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return make_error(err::out_of_memory);
            });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

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
            // move the range out of the source, ranges will never overlap since we just allocated out memory
            mem::move_uninitialized_range(new_ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            // copy elements from the source to my vector, memcpy is safe
            mem::copy_move_uninitialized_range(new_ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_move_uninitialized_range(new_ptr, std::forward<Args>(args)...);
        }

        m_data().ptr = new_ptr;
        m_data().size = count;
        m_data().capacity = count;

        return make_error(err::none);
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
        if (!construct_n<construct_method::default_range>(count))
        {
            err::fast_fail();
        }
    }

    vector(const size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (!construct_n<construct_method::fill_range>(count, value))
        {
            err::fast_fail();
        }
    }

    vector(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (!construct_n<construct_method::copy_range>(init.size(), init.begin()))
        {
            err::fast_fail();
        }
    }

    vector(const vector& other)
        : m_storage(_priv::one_then_variadic_args_tag{}, other.m_allocator())
    {
        if (!construct_n<construct_method::copy_range>(
                other.m_data().size,
                other.m_data().ptr))
        {
            err::fast_fail();
        }
    }

    // copy with an explicitly supplied allocator
    vector(const vector& other, const allocator_type& alloc)
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (!construct_n<construct_method::copy_range>(
                other.m_data().size,
                other.m_data().ptr))
        {
            err::fast_fail();
        }
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
        m_data().acquire(other.m_data());
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    vector(IT first, IT last, const allocator_type& alloc = allocator_type()) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
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

        if (!e)
        {
            err::fast_fail();
        }
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    vector(const V& v, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (!construct_n<construct_method::copy_range>(v.size(), v.data()))
        {
            err::fast_fail();
        }
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<vector, error> construct(const allocator_type& alloc = allocator_type())
    {
        return vector(uninitialized_tag{}, alloc);
    }

    static expected<vector, error> construct(size_type count, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const auto e = v.template construct_n<construct_method::default_range>(count);
        return e ? make_unexpected(e) : v;
    }

    static expected<vector, error> construct(size_type count, const T& value, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const error e = v.template construct_n<construct_method::fill_range>(count, value);
        return e ? make_unexpected(e) : v;
    }

    static expected<vector, error> construct(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const error e = v.template construct_n<construct_method::copy_range>(init.size(), init.begin());
        return e ? make_unexpected(e) : v;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static expected<vector, error> construct(IT first, IT last, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        error e;
        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            e = v.template construct_n<construct_method::copy_range>(count, first.ptr());
        }
        else
        {
            e = v.template construct_n<construct_method::iterator_range>(count, std::move(first), std::move(last));
        }

        return e ? make_unexpected(e) : v;
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    static expected<vector, error> construct(const V& other, const allocator_type& alloc = allocator_type())
    {
        vector v(uninitialized_tag{}, alloc);
        const error e = v.template construct_n<construct_method::copy_range>(other.size(), other.data());
        return e ? make_unexpected(e) : v;
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

    template <construct_method M, typename Arg>
    success assign_from(const size_type count, Arg&& arg)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (count > capacity)
        {
#if !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

            VX_UNLIKELY_COLD_PATH(count > max_size(),
                {
                    return make_error(err::size_error);
                });

#endif // !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

            pointer new_ptr = m_allocator().allocate(count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            VX_UNLIKELY_COLD_PATH(!new_ptr,
                {
                    return make_error(err::out_of_memory);
                });
#endif // !defined(VX_ALLOCATE_FAIL_FAST)

            mem::destroy_range(ptr, size);
            m_allocator().deallocate(ptr, capacity);

            ptr = new_ptr;
            capacity = count;

            VX_IF_CONSTEXPR (M == construct_method::fill_range)
            {
                mem::fill_uninitialized_range(ptr, count, arg);
            }
            else VX_IF_CONSTEXPR (M == construct_method::move_range)
            {
                mem::move_uninitialized_range(ptr, arg, count);
            }
            else // VX_IF_CONSTEXPR (M == construct_method::copy_range)
            {
                mem::copy_uninitialized_range(ptr, arg, count);
            }

            size = count;
            return make_error(err::none);
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
                auto mid = mem::move_range(ptr, arg, size);
                mem::move_uninitialized_range(mid, arg, tail_count);
            }
            else // VX_IF_CONSTEXPR (M == construct_method::copy_range)
            {
                auto mid = mem::copy_range(ptr, arg, size);
                mem::copy_uninitialized_range(mid, arg, tail_count);
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
                mid = mem::copy_range(ptr, arg, count);
            }

            mem::destroy_range(mid, size - count);
        }

        size = count;
        return make_error(err::none);
    }

    template <construct_method M, typename IT1, typename IT2>
    success assign_from(const size_type count, IT1 first, IT2 last)
    {
        VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");

        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (count > capacity)
        {
#if !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

            VX_UNLIKELY_COLD_PATH(count > max_size(),
                {
                    return make_error(err::size_error);
                });

#endif // !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

            pointer new_ptr = m_allocator().allocate(count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            VX_UNLIKELY_COLD_PATH(!new_ptr,
                {
                    return make_error(err::out_of_memory);
                });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

            mem::destroy_range(ptr, size);
            m_allocator().deallocate(ptr, capacity);

            ptr = new_ptr;
            capacity = count;
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
        return make_error(err::none);
    }

public:

    //=========================================================================
    // assignment operators
    //=========================================================================

    vector& operator=(const vector& other)
    {
        if (!assign_from<construct_method::copy_range>(other.m_data().size, other.m_data().ptr))
        {
            err::fast_fail();
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept
    {
        if (this != &other)
        {
            destroy_range();
            m_allocator() = std::move(other.m_allocator());
            m_data().acquire(other.m_data());
        }
        return *this;
    }

    vector& operator=(std::initializer_list<T> init)
    {
        if (!assign_from<construct_method::copy_range>(init.size(), init.begin()))
        {
            err::fast_fail();
        }
        return *this;
    }

    template <typename V, VX_REQUIRES(is_compatible_vector<V>::value)>
    vector& operator=(const V& v)
    {
        if (!assign_from<construct_method::copy_range>(v.size(), v.data()))
        {
            err::fast_fail();
        }
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    success assign(const vector& other)
    {
        if (this == &other)
        {
            return true;
        }
        return assign_from<construct_method::copy_range>(other.m_data().size, other.m_data().ptr);
    }

    success assign(vector&& other) noexcept
    {
        operator=(std::move(other));
        return make_error(err::none);
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
    success assign(const V& v)
    {
        return assign_from<construct_method::copy_range>(v.size(), v.data());
    }

    //=========================================================================
    // element access
    //=========================================================================

    expected<T&, error> front() noexcept
    {
        if (empty())
        {
            return make_unexpected(make_error(err::out_of_range));
        }
        return *m_data().ptr;
    }

    expected<const T&, error> front() const noexcept
    {
        if (empty())
        {
            return make_unexpected(make_error(err::out_of_range));
        }
        return *m_data().ptr;
    }

    expected<T&, error> back() noexcept
    {
        if (empty())
        {
            return make_unexpected(make_error(err::out_of_range));
        }
        return m_data().ptr[m_data().size - 1];
    }

    expected<const T&, error> back() const noexcept
    {
        if (empty())
        {
            return make_unexpected(make_error(err::out_of_range));
        }
        return m_data().ptr[m_data().size - 1];
    }

    pointer data() noexcept
    {
        return m_data().ptr;
    }

    const pointer data() const noexcept
    {
        return m_data().ptr;
    }

    T& operator[](size_type i) noexcept
    {
        VX_ASSERT(i < m_data().size);
        return m_data().ptr[i];
    }

    const T& operator[](size_type i) const noexcept
    {
        VX_ASSERT(i < m_data().size);
        return m_data().ptr[i];
    }

    expected<T&, error> at(size_type i) noexcept
    {
        if (i >= m_data().size)
        {
            return make_unexpected(make_error(err::out_of_range));
        }
        return operator[](i);
    }

    expected<const T&, error> at(size_type i) const noexcept
    {
        if (i >= m_data().size)
        {
            return make_unexpected(make_error(err::out_of_range));
        }
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
            return true;
        }

        return reallocate_shrink(size);
    }

    pointer release() noexcept
    {
        return m_data().release().ptr;
    }

    success acquire(pointer ptr, size_type count) noexcept
    {
#if !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size(),
            {
                return make_error(err::size_error);
            });

#endif // !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

        destroy_range();

        m_data().ptr = ptr;
        m_data().size = count;
        m_data().capacity = count;

        return make_error(err::none);
    }

    // swap keeps allocator and data_type glued together because each
    // data_type must stay paired with the allocator that produced it
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

    success reallocate_shrink(size_type new_capacity)
    {
        const size_type bytes = m_data().size * sizeof(T);
        constexpr size_type reallocate_threshold = 96000;

        if (bytes < reallocate_threshold)
        {
            return reallocate<true, false>(new_capacity);
        }
        else
        {
            return reallocate<true, true>(new_capacity);
        }
    }

    template <bool shrinking = false, bool try_reallocate = false>
    success reallocate(size_type new_capacity)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        pointer new_ptr;

        VX_IF_CONSTEXPR (try_reallocate && std::is_trivially_destructible<T>::value && std::is_trivially_copyable<T>::value)
        {
            new_ptr = m_allocator().reallocate(ptr, new_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            if (!new_ptr)
            {
                return make_error(err::out_of_memory);
            }

#endif // !defined(VX_ALLOCATE_FAIL_FAST)
        }
        else
        {
            new_ptr = m_allocator().allocate(new_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            if (!new_ptr)
            {
                return make_error(err::out_of_memory);
            }

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

            VX_IF_CONSTEXPR (shrinking)
            {
                VX_ASSERT(size > 0);
                mem::move_uninitialized_range(new_ptr, ptr, new_capacity);
            }
            else
            {
                mem::move_uninitialized_range(new_ptr, ptr, size);
            }

            mem::destroy_range(ptr, size);
            m_allocator().deallocate(ptr, capacity);
        }

        ptr = new_ptr;
        VX_IF_CONSTEXPR (shrinking)
        {
            size = new_capacity;
        }
        capacity = new_capacity;

        return make_error(err::none);
    }

public:

    //=========================================================================
    // reserve
    //=========================================================================

    success reserve(size_type new_capacity)
    {
        if (new_capacity > m_data().capacity)
        {
#if !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

            VX_UNLIKELY_COLD_PATH(new_capacity > max_size(),
                {
                    return make_error(err::size_error);
                });

#endif // !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

            return reallocate(new_capacity);
        }

        return make_error(err::none);
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

#if !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(new_size > max_size(),
            {
                return make_error(err::size_error);
            });

#endif // !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

        pointer new_ptr = m_allocator().allocate(new_size);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return make_error(err::out_of_memory);
            });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

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

        mem::move_uninitialized_range(new_ptr, ptr, size);

        mem::destroy_range(ptr, size);
        m_allocator().deallocate(ptr, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_size;

        return make_error(err::none);
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
        return make_error(err::none);
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

    // reallocate the vector data, split at pos and shift back by shift
    // caller should ensure
    template <typename growth_rate, construct_method M, typename... Args>
    expected<pointer, error> insert_reallocate(pointer pos, size_type count, Args&&... args) noexcept
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

#if !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size() - size,
            {
                return make_unexpected(make_error(err::size_error));
            });

#endif // !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

        const size_type new_size = size + count;
        const size_type new_capacity = _dynamic_array_base_priv::grow_capacity<growth_rate>(new_size, capacity, max_size());
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = m_allocator().allocate(new_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return make_unexpected(make_error(err::out_of_memory));
            });

#endif // defined(VX_ALLOCATE_FAIL_FAST)

        const size_type off = static_cast<size_type>(pos - ptr);
        pointer dst = new_ptr + off;

        // copy first range
        mem::move_uninitialized_range(new_ptr, ptr, off);

        VX_IF_CONSTEXPR (M == construct_method::single)
        {
            mem::construct_in_place_maybe_trivial(dst, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_range(dst, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            mem::move_uninitialized_range(dst, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            mem::copy_uninitialized_range(dst, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_range(dst, std::forward<Args>(args)...);
        }

        // copy second range
        mem::move_uninitialized_range(dst + count, pos, size - off);

        // destroy original range
        mem::destroy_range(ptr, size);
        m_allocator().deallocate(ptr, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_capacity;

        return dst;
    }

    expected<pointer, error> checked_offset_ptr(size_type off) noexcept
    {
        if (off > m_data().size)
        {
            return make_unexpected(make_error(err::out_of_range));
        }
        return m_data().ptr + off;
    }


    template <typename growth_rate, construct_method M, typename... Args>
    expected<pointer, error> insert_n(pointer pos, size_type count, Args&&... args)
    {
        const size_type available = m_data().capacity - m_data().size;

        if (count <= available)
        {
            return insert_capacity<M>(pos, count, std::forward<Args>(args)...);
        }
        else
        {
            return insert_reallocate<growth_rate, M>(pos, count, std::forward<Args>(args)...);
        }
    }

    template <typename growth_rate, construct_method M, typename... Args>
    expected<iterator, error> insert_checked(size_type off, size_type count, Args&&... args)
    {
        auto p = checked_offset_ptr(off);
        if (!p)
        {
            return make_unexpected(p.error());
        }

        const auto res = insert_n<growth_rate, M>(p.value(), count, std::forward<Args>(args)...);
        if (!res)
        {
            return make_unexpected(res.error());
        }
        return iterator(res.value());
    }

    template <typename growth_rate, construct_method M, typename... Args>
    iterator insert_unchecked(const_iterator pos, size_type count, Args&&... args)
    {
        VX_ASSERT(pos >= cbegin() && pos <= cend());
        auto ptr = const_cast<pointer>(pos.ptr());

        const auto res = insert_n<growth_rate, M>(ptr, count, std::forward<Args>(args)...);
        if (!res)
        {
            err::fast_fail();
        }
        return iterator(res.value());
    }

public:

    template <typename growth_rate = default_growth_rate>
    expected<iterator, error> insert(size_type off, const T& value)
    {
        return emplace<growth_rate>(off, value);
    }

    template <typename growth_rate = default_growth_rate>
    expected<iterator, error> insert(size_type off, T&& value) noexcept
    {
        return emplace<growth_rate>(off, std::move(value));
    }

    template <typename growth_rate = default_growth_rate>
    expected<iterator, error> insert(size_type off, size_type count, const T& value)
    {
        return insert_checked<growth_rate, construct_method::fill_range>(off, count, value);
    }

    template <typename growth_rate = default_growth_rate>
    expected<iterator, error> insert(size_type off, std::initializer_list<T> init)
    {
        return insert_checked<growth_rate, construct_method::copy_range>(off, init.size(), init.begin());
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    expected<iterator, error> insert(size_type off, IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            return insert_checked<growth_rate, construct_method::copy_range>(off, count, first.ptr());
        }
        else
        {
            return insert_checked<growth_rate, construct_method::iterator_range>(off, count, first, last);
        }
    }

    //=========================================================================
    //=========================================================================

    //=========================================================================
    // insert (const_iterator, std-compatible)
    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const T& value)
    {
        return emplace<growth_rate>(pos, value);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, T&& value) noexcept
    {
        return emplace<growth_rate>(pos, std::move(value));
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        return insert_unchecked<growth_rate, construct_method::fill_range>(pos, count, value);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        return insert_unchecked<growth_rate, construct_method::copy_range>(pos, init.size(), init.begin());
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
        {
            return insert_unchecked<growth_rate, construct_method::copy_range>(pos, count, first.ptr());
        }
        else
        {
            return insert_unchecked<growth_rate, construct_method::iterator_range>(pos, count, first, last);
        }
    }

    //=========================================================================
    // emplace
    //=========================================================================

    template <typename growth_rate = default_growth_rate, typename... Args>
    expected<iterator, error> emplace_back(Args&&... args)
    {
        VX_STATIC_ASSERT_MSG(growth_rate::num >= 0 && growth_rate::den > 0, "Growth rate must be positive");
        VX_STATIC_ASSERT_MSG(growth_rate::num >= growth_rate::den, "Growth rate must be greater or equal to 1");

        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (size == capacity)
        {
            const size_type count = size + 1;

#if !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

            VX_UNLIKELY_COLD_PATH(count > max_size() - size,
                {
                    return make_unexpected(make_error(err::size_error));
                });

#endif // !defined(VX_VECTOR_DISABLE_MAX_SIZE_CHECK)

            const size_type new_capacity = _dynamic_array_base_priv::grow_capacity<growth_rate>(count, capacity, max_size());
            VX_UNLIKELY_COLD_PATH(!reallocate(new_capacity),
                {
                    return make_unexpected(make_error(err::out_of_memory));
                });
        }

        pointer dst = ptr + size;
        mem::construct_in_place_maybe_trivial(dst, std::forward<Args>(args)...);
        ++size;

        return iterator(dst);
    }

    template <typename growth_rate = default_growth_rate, typename... Args>
    expected<iterator, error> emplace(size_type off, Args&&... args)
    {
        return insert_checked<growth_rate, construct_method::single>(off, 1, std::forward<Args>(args)...);
    }

    template <typename growth_rate = default_growth_rate, typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        return insert_unchecked<growth_rate, construct_method::single>(pos, 1, std::forward<Args>(args)...);
    }

    //=========================================================================
    // push back
    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    expected<iterator, error> push_back(const T& value)
    {
        return emplace_back<growth_rate>(value);
    }

    template <typename growth_rate = default_growth_rate>
    expected<iterator, error> push_back(T&& value) noexcept
    {
        return emplace_back<growth_rate>(std::move(value));
    }

    //=========================================================================
    // erase
    //=========================================================================

private:

    pointer erase_n(pointer pos, size_type count)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type tail_count = size - off - count;
        const size_type new_size = size - count;

        mem::move_range(pos, pos + count, tail_count);
        mem::destroy_range(ptr + new_size, count);

        size = new_size;
        return pos;
    }

public:

    success erase(size_type off)
    {
        if (off >= size())
        {
            return make_error(err::out_of_range);
        }
        auto ptr = m_data().ptr + off;
        erase_n(ptr, 1);
        return make_error(err::none);
    }

    success erase(size_type off, size_type count)
    {
        if (off > size() || count > size() - off)
        {
            return make_error(err::out_of_range);
        }
        auto ptr = m_data().ptr + off;
        erase_n(ptr, count);
        return make_error(err::none);
    }

    iterator erase(const_iterator pos)
    {
        VX_ASSERT(pos >= cbegin() && pos < cend());
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = erase_n(ptr, 1);
        return iterator(ptr);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        VX_ASSERT(first >= cbegin() && last <= cend() && first <= last);
        auto ptr = const_cast<pointer>(first.ptr());
        const size_type count = static_cast<size_type>(last.ptr() - first.ptr());
        ptr = erase_n(ptr, count);
        return iterator(ptr);
    }

    //=========================================================================

    success pop_back()
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        if (size == 0)
        {
            return make_error(err::out_of_range);
        }

        --size;
        mem::destroy_in_place(ptr + size);
        return make_error(err::none);
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
