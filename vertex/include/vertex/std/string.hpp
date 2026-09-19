#pragma once

#include <sstream>

#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/_tools/dynamic_array_base.hpp"
#include "vertex/std/algorithm.hpp"
#include "vertex/std/char_traits.hpp"
#include "vertex/std/cstring_view.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/growth_policy.hpp"
#include "vertex/std/iterator.hpp"
#include "vertex/std/string_view.hpp"

namespace vx {
namespace str {

template <typename T, typename Allocator = mem::default_allocator<T>, typename Growth = ratio_growth_policy<2>>
class basic_string
{
    //=========================================================================
    // member types
    //=========================================================================

private:

    VX_STATIC_ASSERT_MSG(type_traits::is_char<T>::value, "T must be character type");
    VX_STATIC_ASSERT_MSG(
        (std::is_same<T, typename Allocator::value_type>::value),
        "Allocator value type must match T");

    template <typename S>
    struct is_compatible_string
    {
        static constexpr bool value = is_string_view<S>::value && is_string_of<S, T>::value;
    };

    template <typename Allocator2, typename Growth2>
    struct is_compatible_string<basic_string<T, Allocator2, Growth2>> : std::true_type
    {};

    using data_type = _dynamic_array_base_priv::dynamic_array_data<T>;

    template <typename IT>
    using is_my_iterator = _priv::is_my_pointer_iterator<IT, basic_string>;

public:

    using traits_type = char_traits<T>;

    using allocator_type = Allocator;
    using growth_policy = ratio_growth_policy<2>;

    using value_type = typename data_type::value_type;
    using pointer = typename data_type::pointer;
    using const_pointer = typename data_type::const_pointer;
    using reference = typename data_type::reference;
    using const_reference = typename data_type::const_reference;
    using size_type = typename data_type::size_type;
    using difference_type = typename data_type::difference_type;

    using iterator = vx::_priv::pointer_iterator<basic_string, T>;
    using const_iterator = vx::_priv::pointer_iterator<basic_string, const T>;
    using reverse_iterator = vx::_priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = vx::_priv::reverse_pointer_iterator<const_iterator>;

    static constexpr auto npos{ static_cast<size_type>(-1) };

private:

    // holds the allocator alongside the buffer_type; empty-base-optimized when
    // allocator_type is stateless, so sizeof(basic_string) is unaffected in that case
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

private:

    //=========================================================================
    // allocation helpers
    //=========================================================================

    static void destroy_size(T* ptr, size_type size)
    {
        range::destroy(ptr, size + 1);
    }

    // no longer static: freeing memory requires the instance's allocator
    void deallocate_capacity(T* ptr, size_type capacity)
    {
        m_allocator().deallocate(ptr, capacity + 1);
    }

    // does not reassign static buffer, only call if you are about to assign a new pointer
    void destroy_and_deallocate(T* ptr, size_type size, size_type capacity)
    {
        if (!is_static_buffer())
        {
            destroy_size(ptr, size);
            deallocate_capacity(ptr, capacity);
        }
    }

    void destroy_range()
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (!is_static_buffer())
        {
            range::destroy(ptr, size + 1);
            m_allocator().deallocate(ptr, capacity + 1); // +1 for null terminator slot
        }

        construct_empty();
    }

    //=========================================================================
    // constructor helpers
    //=========================================================================

    enum class construct_method
    {
        from_char,
        from_char_count,
        from_pointer,
        from_string,
        from_iterator_range
    };

    bool is_static_buffer() const noexcept
    {
        return m_data().ptr == _string_traits_priv::empty_string<T>();
    }

    void construct_empty() noexcept
    {
        m_data().ptr = const_cast<pointer>(_string_traits_priv::empty_string<T>());
        m_data().size = 0;
        m_data().capacity = 0;
    }

    template <construct_method M, typename... Args>
    success construct_n(size_type count, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (count == 0)
        {
            construct_empty();
            return success{};
        }

        VX_RET_ERR_IF(count > max_size(), err::size_error);

        const size_type alloc_count = count + 1;
        auto new_ptr = m_allocator().allocate(alloc_count);
        VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

        ptr = new_ptr;
        size = count;
        capacity = count;

        range::construct_maybe_trivial(ptr, alloc_count);

        VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(ptr, count, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _char_traits_priv::copy_batch(ptr, std::forward<Args>(args)..., count);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_string)
        {
            traits_type::copy(ptr, std::forward<Args>(args)..., alloc_count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");

            traits_type::copy_range(ptr, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }

        return success{};
    }

    struct uninitialized_tag
    {};

    basic_string(uninitialized_tag, const allocator_type& alloc) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {}

public:

    //=========================================================================
    // constructors
    //=========================================================================

    basic_string(std::nullptr_t) = delete;

    basic_string()
        : m_storage(_priv::zero_then_variadic_args_tag{})
    {
        construct_empty();
    }

    explicit basic_string(const allocator_type& alloc)
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        construct_empty();
    }

    basic_string(const basic_string& other)
        : m_storage(_priv::one_then_variadic_args_tag{}, other.m_allocator())
    {
        const auto ok = construct_n<construct_method::from_string>(other.size(), other.data());
        VX_VERIFY(ok);
    }

    // copy with an explicitly supplied allocator
    basic_string(const basic_string& other, const allocator_type& alloc)
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const auto ok = construct_n<construct_method::from_string>(other.size(), other.data());
        VX_VERIFY(ok);
    }

    // move takes over the source's allocator along with its buffer_type, since
    // the buffer_type must always be freed by the allocator that produced it
    basic_string(basic_string&& other) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, std::move(other.m_allocator()))
    {
        m_data().acquire(other.m_data());
    }

    basic_string(basic_string&& other, const allocator_type& alloc) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (alloc == other.get_allocator())
        {
            m_data().acquire(other.m_data());
        }
        else
        {
            const auto ok = construct_n<construct_method::from_pointer>(other.size(), other.data());
            VX_VERIFY(ok);
        }
    }

    //=========================================================================

    basic_string(const basic_string& other, size_type off, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            construct_empty();
            return;
        }

        const auto ok = construct_n<construct_method::from_pointer>(other.size() - off, other.data() + off);
        VX_VERIFY(ok);
    }

    basic_string(const basic_string& other, size_type off, size_type count, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            construct_empty();
            return;
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        const auto ok = construct_n<construct_method::from_pointer>(count, other.data() + off);
        VX_VERIFY(ok);
    }

    //=========================================================================

    basic_string(size_type count, const T value, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const auto ok = construct_n<construct_method::from_char_count>(count, value);
        VX_VERIFY(ok);
    }

    //=========================================================================

    basic_string(const T* const ptr, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        const auto ok = construct_n<construct_method::from_pointer>(count, ptr);
        VX_VERIFY(ok);
    }

    basic_string(const T* const ptr, size_type count, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const auto ok = construct_n<construct_method::from_pointer>(count, ptr);
        VX_VERIFY(ok);
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string(IT first, IT last, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        success ok;

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            ok = construct_n<construct_method::from_pointer>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            ok = construct_n<construct_method::from_pointer>(count, first);
        }
        else
        {
            ok = construct_n<construct_method::from_iterator_range>(count, first, last);
        }

        VX_VERIFY(ok);
    }

    //=========================================================================

    basic_string(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const size_type count = static_cast<size_type>(init.size());
        const auto ok = construct_n<construct_method::from_pointer>(count, init.begin());
        VX_VERIFY(ok);
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_string(const S& other, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        const size_type count = static_cast<size_type>(other.size());
        const auto ok = construct_n<construct_method::from_pointer>(count, other.data());
        VX_VERIFY(ok);
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_string(const S& other, size_type off, size_type count = npos, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            construct_empty();
            return;
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        const auto ok = construct_n<construct_method::from_pointer>(count, other.data() + off);
        VX_VERIFY(ok);
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<basic_string, error> create(std::nullptr_t) = delete;

    static expected<basic_string, error> create(const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);
        s.construct_empty();
        return s;
    }

    static expected<basic_string, error> create(const basic_string& other, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);
        const auto ok = s.template construct_n<construct_method::from_string>(other.size(), other.data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    static expected<basic_string, error> create(basic_string&& other, const allocator_type& alloc = allocator_type()) noexcept
    {
        basic_string s(uninitialized_tag{}, alloc);
        if (alloc == other.get_allocator())
        {
            s.m_data().acquire(other.m_data());
            return s;
        }

        const auto ok = s.construct_n<construct_method::from_pointer>(other.size(), other.data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    static expected<basic_string, error> create(const basic_string& other, size_type off, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);

        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            s.construct_empty();
            return s;
        }

        const auto ok = s.template construct_n<construct_method::from_pointer>(other.size() - off, other.data() + off);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    static expected<basic_string, error> create(const basic_string& other, size_type off, size_type count, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);

        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            s.construct_empty();
            return s;
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        const auto ok = s.template construct_n<construct_method::from_pointer>(count, other.data() + off);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    static expected<basic_string, error> create(size_type count, const T value, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);
        const auto ok = s.template construct_n<construct_method::from_char_count>(count, value);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    static expected<basic_string, error> create(const T* const ptr, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);

        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        const auto ok = s.template construct_n<construct_method::from_pointer>(count, ptr);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    static expected<basic_string, error> create(const T* const ptr, size_type count, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);
        const auto ok = s.template construct_n<construct_method::from_pointer>(count, ptr);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static expected<basic_string, error> create(IT first, IT last, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);
        const size_type count = static_cast<size_type>(std::distance(first, last));
        success ok;

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            ok = s.template construct_n<construct_method::from_pointer>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            ok = s.template construct_n<construct_method::from_pointer>(count, first);
        }
        else
        {
            ok = s.template construct_n<construct_method::from_iterator_range>(count, first, last);
        }

        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    static expected<basic_string, error> create(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);
        const size_type count = static_cast<size_type>(init.size());
        const auto ok = s.template construct_n<construct_method::from_pointer>(count, init.begin());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    static expected<basic_string, error> create(const S& other, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);
        const size_type count = static_cast<size_type>(other.size());
        const auto ok = s.template construct_n<construct_method::from_pointer>(count, other.data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    static expected<basic_string, error> create(const S& other, size_type off, size_type count = npos, const allocator_type& alloc = allocator_type())
    {
        basic_string s(uninitialized_tag{}, alloc);
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            s.construct_empty();
            return s;
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        const auto ok = s.template construct_n<construct_method::from_pointer>(count, other.data() + off);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~basic_string()
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

    operator basic_string_view<T>() const noexcept
    {
        return basic_string_view<T>(data(), size());
    }

    operator basic_cstring_view<T>() const noexcept
    {
        return basic_cstring_view<T>(*this);
    }

#if VX_HAVE_STD_STRING_VIEW

    template <typename Traits>
    operator std::basic_string_view<T, Traits>() const noexcept
    {
        return std::basic_string_view<T, Traits>(data(), size());
    }

#endif // VX_HAVE_STD_STRING_VIEW

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    success assign_from(const size_type count, Args&&... args)
    {
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
            pointer new_ptr = m_allocator().allocate(count + 1);
            VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

            range::construct_maybe_trivial(new_ptr, count + 1);
            destroy_and_deallocate(ptr, size, capacity);

            ptr = new_ptr;
            capacity = count;
        }
        else if (count > size)
        {
            range::construct_maybe_trivial(ptr + size + 1, count - size);
        }
        else // if (count < size)
        {
            range::destroy(ptr + count + 1, size - count);
        }

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(*ptr, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(ptr, count, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _char_traits_priv::copy_batch(ptr, std::forward<Args>(args)..., count);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_string)
        {
            traits_type::copy(ptr, std::forward<Args>(args)..., count + 1);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");

            traits_type::copy_range(ptr, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }

        size = count;
        return success{};
    }

public:

    //=========================================================================
    // assignment operators
    //=========================================================================

    basic_string& operator=(const basic_string& other)
    {
        if (this == &other)
        {
            return *this;
        }

        const auto ok = assign_from<construct_method::from_string>(other.size(), other.data());
        VX_VERIFY(ok);
        return *this;
    }

    basic_string& operator=(basic_string&& other) noexcept
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

    basic_string& operator=(const T c)
    {
        const auto ok = assign_from<construct_method::from_char>(1, c);
        VX_VERIFY(ok);
        return *this;
    }

    basic_string& operator=(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        const auto ok = assign_from<construct_method::from_pointer>(count, ptr);
        VX_VERIFY(ok);
        return *this;
    }

    basic_string& operator=(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        const auto ok = assign_from<construct_method::from_pointer>(count, init.begin());
        VX_VERIFY(ok);
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_string& operator=(const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        const auto ok = assign_from<construct_method::from_pointer>(count, other.data());
        VX_VERIFY(ok);
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    success assign(const basic_string& other)
    {
        if (this == &other)
        {
            return success{};
        }
        return assign_from<construct_method::from_string>(other.size(), other.data());
    }

    success assign(basic_string&& other) noexcept
    {
        operator=(std::move(other));
        return success{};
    }

    //=========================================================================

    success assign(const basic_string& other, size_type off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            clear();
            return success{};
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return assign_from<construct_method::from_pointer>(count, other.data() + off);
    }

    //=========================================================================

    success assign(const T c)
    {
        return assign_from<construct_method::from_char>(1, c);
    }

    success assign(const size_type count, const T c)
    {
        return assign_from<construct_method::from_char_count>(count, c);
    }

    //=========================================================================

    success assign(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        return assign_from<construct_method::from_pointer>(count, ptr);
    }

    success assign(const T* const ptr, size_type count)
    {
        return assign_from<construct_method::from_pointer>(count, ptr);
    }

    //=========================================================================

    success assign(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return assign_from<construct_method::from_pointer>(count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    success assign(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            return assign_from<construct_method::from_pointer>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return assign_from<construct_method::from_pointer>(count, first);
        }
        else
        {
            return assign_from<construct_method::from_iterator_range>(count, first, last);
        }
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    success assign(const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        return assign_from<construct_method::from_pointer>(count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    success assign(const S& other, size_type off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            clear();
            return success{};
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return assign_from<construct_method::from_pointer>(count, other.data() + off);
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

    T* data() noexcept
    {
        return m_data().ptr;
    }

    const T* data() const noexcept
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
        VX_RET_UNEXPECTED_ERR_IF(i >= m_data().size, err::out_of_range);
        return operator[](i);
    }

    expected<const T&, error> at(size_type i) const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(i >= m_data().size, err::out_of_range);
        return operator[](i);
    }

    const T* c_str() const noexcept
    {
        return m_data().ptr;
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

private:

    //=========================================================================
    // append helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    void append_capacity(size_type count, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        T* const dst = ptr + size;

        // we increase the size early so we can easily assign the null terminator at the end
        size += count;
        range::construct_maybe_trivial(dst + 1, count);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(*dst, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(dst, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _char_traits_priv::copy_batch(dst, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(dst, std::forward<Args>(args)...);
        }

        traits_type::assign(ptr[size], T());
    }

    template <typename op_growth_policy, construct_method M, typename... Args>
    success append_reallocate(size_type count, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        VX_RET_ERR_IF(count > max_size() - size, err::size_error);
        const size_type new_size = size + count;
        const size_type new_capacity = op_growth_policy::next_capacity(new_size, capacity, max_size());
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = m_allocator().allocate(new_capacity + 1);
        VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

        range::construct_maybe_trivial(new_ptr, new_size + 1);

        // copy prefix [ptr, ptr + size) to [new_ptr, ...), then construct suffix [ptr + size, ...)
        traits_type::copy(new_ptr, ptr, size);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(new_ptr[size], std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(new_ptr + size, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _char_traits_priv::copy_batch(new_ptr + size, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(new_ptr + size, std::forward<Args>(args)...);
        }

        traits_type::assign(new_ptr[new_size], T());

        // destroy original range
        destroy_and_deallocate(ptr, size, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_capacity;

        return success{};
    }

    template <typename op_growth_policy, construct_method M, typename... Args>
    success append_n(const size_type count, Args&&... args)
    {
        const size_type available = m_data().capacity - m_data().size;

        if (count <= available)
        {
            append_capacity<M>(count, std::forward<Args>(args)...);
            return success{};
        }
        else
        {
            return append_reallocate<op_growth_policy, M>(count, std::forward<Args>(args)...);
        }
    }

public:

    //=========================================================================
    // append
    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    success append(const basic_string& other)
    {
        return append_n<op_growth_policy, construct_method::from_pointer>(other.size(), other.data());
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    success append(const basic_string& other, size_type off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            return success{};
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return append_n<op_growth_policy, construct_method::from_pointer>(count, other.data() + off);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    success append(const T c)
    {
        return append_n<op_growth_policy, construct_method::from_char>(1, c);
    }

    template <typename op_growth_policy = growth_policy>
    success append(size_type count, const T c)
    {
        return append_n<op_growth_policy, construct_method::from_char_count>(count, c);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    success append(const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        return append_n<op_growth_policy, construct_method::from_pointer>(count, s);
    }

    template <typename op_growth_policy = growth_policy>
    success append(const T* const s, const size_type count)
    {
        return append_n<op_growth_policy, construct_method::from_pointer>(count, s);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    success append(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return append_n<op_growth_policy, construct_method::from_pointer>(count, init.begin());
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    success append(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));
        return append_n<op_growth_policy, construct_method::from_iterator_range>(count, first, last);
    }

    //=========================================================================

    template <typename S, typename op_growth_policy = growth_policy, VX_REQUIRES(is_compatible_string<S>::value)>
    success append(const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        return append_n<op_growth_policy, construct_method::from_pointer>(count, other.data());
    }

    template <typename S, typename op_growth_policy = growth_policy, VX_REQUIRES(is_compatible_string<S>::value)>
    success append(const S& other, size_type off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            return success{};
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return append_n<op_growth_policy, construct_method::from_pointer>(count, other.data() + off);
    }

    //=========================================================================

    basic_string& operator+=(const basic_string& other)
    {
        const auto ok = append(other);
        VX_VERIFY(ok);
        return *this;
    }

    basic_string& operator+=(const T c)
    {
        const auto ok = append(c);
        VX_VERIFY(ok);
        return *this;
    }

    basic_string& operator+=(const T* const s)
    {
        const auto ok = append(s);
        VX_VERIFY(ok);
        return *this;
    }

    basic_string& operator+=(std::initializer_list<T> init)
    {
        const auto ok = append(init);
        VX_VERIFY(ok);
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_string& operator+=(const S& other)
    {
        const auto ok = append(other);
        VX_VERIFY(ok);
        return *this;
    }

private:

    //=========================================================================
    // insert helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    T* insert_capacity(T* pos, size_type count, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        // initialize the new elements that will be moved into uninitialized memory
        const pointer back = ptr + size;
        range::construct_maybe_trivial(back + 1, count);

        // move the tail backward to make room for the new elements
        const size_type tail_count = static_cast<size_type>(back - pos) + 1;
        _char_traits_priv::move_batch(pos + count, pos, tail_count);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(*pos, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(pos, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _char_traits_priv::copy_batch(pos, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(pos, std::forward<Args>(args)...);
        }

        size += count;
        return pos;
    }

    template <typename op_growth_policy, construct_method M, typename... Args>
    expected<T*, error> insert_reallocate(T* pos, size_type count, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        VX_RET_UNEXPECTED_ERR_IF(count > max_size() - size, err::size_error);
        const size_type new_size = size + count;
        const size_type new_capacity = op_growth_policy::next_capacity(new_size, capacity, max_size());
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = m_allocator().allocate(new_capacity + 1);
        VX_RET_UNEXPECTED_ERR_IF(!new_ptr, err::out_of_memory);

        const size_type off = static_cast<size_type>(pos - ptr);

        range::construct_maybe_trivial(new_ptr, new_size + 1);
        // copy first range
        traits_type::copy(new_ptr, ptr, off);

        pointer dst = new_ptr + off;

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(*dst, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(dst, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _char_traits_priv::copy_batch(dst, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(dst, std::forward<Args>(args)...);
        }

        // copy second range (includes null terminator)
        traits_type::copy(dst + count, pos, (size - off) + 1);

        // destroy original range
        destroy_and_deallocate(ptr, size, capacity);

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

    //=========================================================================
    // insert
    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, const basic_string& other)
    {
        VX_ASSERT(this != &other);
        return insert<op_growth_policy>(off, other.data(), other.size());
    }

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, const basic_string& other, size_type other_off, size_type count = npos)
    {
        VX_ASSERT(this != &other);
        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            VX_RET_UNEXPECTED_ERR_IF(off > m_data().size, err::out_of_range);
            return iterator(m_data().ptr + off);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count));
        return insert_checked<op_growth_policy, construct_method::from_pointer>(off, count, other.data() + other_off);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, const T c)
    {
        return insert_checked<op_growth_policy, construct_method::from_char>(off, 1, c);
    }

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, size_type count, const T c)
    {
        return insert_checked<op_growth_policy, construct_method::from_char_count>(off, count, c);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        return insert_checked<op_growth_policy, construct_method::from_pointer>(off, count, s);
    }

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, const T* const s, size_type count)
    {
        return insert_checked<op_growth_policy, construct_method::from_pointer>(off, count, s);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    expected<iterator, error> insert(size_type off, std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return insert_checked<op_growth_policy, construct_method::from_pointer>(off, count, init.begin());
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    expected<iterator, error> insert(size_type off, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            return insert_checked<op_growth_policy, construct_method::from_pointer>(off, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return insert_checked<op_growth_policy, construct_method::from_pointer>(off, count, first);
        }
        else
        {
            return insert_checked<op_growth_policy, construct_method::from_iterator_range>(off, count, first, last);
        }
    }

    //=========================================================================

    template <typename S, typename op_growth_policy = growth_policy, VX_REQUIRES(is_compatible_string<S>::value)>
    expected<iterator, error> insert(size_type off, const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        return insert_checked<op_growth_policy, construct_method::from_pointer>(off, count, other.data());
    }

    template <typename S, typename op_growth_policy = growth_policy, VX_REQUIRES(is_compatible_string<S>::value)>
    expected<iterator, error> insert(size_type off, const S& other, size_type other_off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            VX_RET_UNEXPECTED_ERR_IF(off > m_data().size, err::out_of_range);
            return iterator(m_data().ptr + off);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count));
        return insert_checked<op_growth_policy, construct_method::from_pointer>(off, count, other.data() + other_off);
    }

    //=========================================================================
    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, const basic_string& other)
    {
        VX_ASSERT(this != &other);
        return insert<op_growth_policy>(pos, other.data(), other.size());
    }

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, const basic_string& other, size_type other_off, size_type count = npos)
    {
        VX_ASSERT(this != &other);
        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            return iterator(pos);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count));
        return insert_unchecked<op_growth_policy, construct_method::from_pointer>(pos, count, other.data() + other_off);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, const T c)
    {
        return insert_unchecked<op_growth_policy, construct_method::from_char>(pos, 1, c);
    }

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, const size_type count, const T c)
    {
        return insert_unchecked<op_growth_policy, construct_method::from_char_count>(pos, count, c);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, const T* const s, size_type count)
    {
        return insert_unchecked<op_growth_policy, construct_method::from_pointer>(pos, count, s);
    }

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        return insert_unchecked<op_growth_policy, construct_method::from_pointer>(pos, count, s);
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return insert_unchecked<op_growth_policy, construct_method::from_pointer>(pos, count, init.begin());
    }

    //=========================================================================

    template <typename op_growth_policy = growth_policy, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            return insert_unchecked<op_growth_policy, construct_method::from_pointer>(pos, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            return insert_unchecked<op_growth_policy, construct_method::from_pointer>(pos, count, first);
        }
        else
        {
            return insert_unchecked<op_growth_policy, construct_method::from_iterator_range>(pos, count, first, last);
        }
    }

    //=========================================================================

    template <typename S, typename op_growth_policy = growth_policy, VX_REQUIRES(is_compatible_string<S>::value)>
    iterator insert(const_iterator pos, const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        return insert_unchecked<op_growth_policy, construct_method::from_pointer>(pos, count, other.data());
    }

    template <typename S, typename op_growth_policy = growth_policy, VX_REQUIRES(is_compatible_string<S>::value)>
    iterator insert(const_iterator pos, const S& other, size_type other_off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            return iterator(pos);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count));
        return insert_unchecked<op_growth_policy, construct_method::from_pointer>(pos, count, other.data() + other_off);
    }

public:

    //=========================================================================
    // memory
    //=========================================================================

    void clear()
    {
        if (!is_static_buffer())
        {
            auto& ptr = m_data().ptr;
            auto& size = m_data().size;

            range::destroy(ptr, size);
            size = 0;
            traits_type::assign(*ptr, T());
        }
    }

    void clear_and_deallocate()
    {
        destroy_and_deallocate(m_data().ptr, m_data().size, m_data().capacity);
        construct_empty();
    }

    success shrink_to_fit()
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (size == 0)
        {
            destroy_range();
            return success{};
        }

        if (capacity > size)
        {
            pointer new_ptr = m_allocator().allocate(size + 1);
            VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

            VX_ASSERT(size > 0);
            range::construct_maybe_trivial(new_ptr, size + 1);
            traits_type::copy(new_ptr, ptr, size + 1);
            destroy_and_deallocate(ptr, size, capacity);

            ptr = new_ptr;
            capacity = size;
        }

        return success{};
    }

    T* release() noexcept
    {
        if (is_static_buffer())
        {
            return nullptr;
        }

        T* old_ptr = m_data().ptr;
        construct_empty();
        return old_ptr;
    }

    success acquire(T* new_ptr)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        const size_type count = static_cast<size_type>(traits_type::length(new_ptr));
        VX_RET_ERR_IF(count > max_size(), err::size_error);

        destroy_and_deallocate(ptr, size, capacity);

        ptr = new_ptr;
        size = count;
        capacity = count;

        return success{};
    }

    // swap keeps allocator and buffer_type glued together, same reasoning as move:
    // each buffer_type must stay paired with the allocator that produced it
    void swap(basic_string& other) noexcept
    {
        vx::swap(m_storage, other.m_storage);
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

    size_type length() const noexcept
    {
        return size();
    }

    size_type size_bytes() const noexcept
    {
        return size() * sizeof(T);
    }

    constexpr size_type max_size() const noexcept
    {
        const size_type alloc_max = static_cast<size_type>(
            std::allocator_traits<allocator_type>::max_size(m_allocator()));

        if (alloc_max == 0)
        {
            return 0;
        }

        return (std::min)(static_cast<size_type>(std::numeric_limits<difference_type>::max()),
            static_cast<size_type>(alloc_max - 1));
    }

    size_type capacity() const noexcept
    {
        return m_data().capacity;
    }

    //=========================================================================
    // reserve
    //=========================================================================

    success reserve(size_type new_capacity)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (new_capacity <= capacity)
        {
            return success{};
        }

        VX_RET_ERR_IF(new_capacity > max_size(), err::size_error);
        const size_type alloc_capacity = new_capacity + 1;
        pointer new_ptr = m_allocator().allocate(alloc_capacity);
        VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

        range::construct_maybe_trivial(new_ptr, alloc_capacity);
        traits_type::copy(new_ptr, ptr, size + 1);
        destroy_and_deallocate(ptr, size, capacity);

        ptr = new_ptr;
        capacity = new_capacity;

        return success{};
    }

    //=========================================================================
    // resize
    //=========================================================================

    success resize(size_type new_size, const T c = T())
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        if (new_size == 0)
        {
            destroy_range();
            return success{};
        }

        if (new_size <= size)
        {
            const size_type shrink_count = size - new_size;
            pointer end_ptr = ptr + new_size;
            range::destroy(end_ptr + 1, shrink_count);
            traits_type::assign(*end_ptr, T());
            m_data().size = new_size;
            return success{};
        }

        const size_type count = new_size - size;
        return append_n<ratio_growth_policy<1, 1>, construct_method::from_char_count>(count, c);
    }

    //=========================================================================
    // push back
    //=========================================================================

    template <typename op_growth_policy = growth_policy>
    success push_back(const T c)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        if (size < capacity)
        {
            T* const dst = ptr + size;
            mem::construct_in_place(dst);
            traits_type::assign(dst[0], c);
            traits_type::assign(dst[1], T());
            ++size;
            return success{};
        }

        return append_reallocate<op_growth_policy, construct_method::from_char>(1, c);
    }

    //=========================================================================
    // erase
    //=========================================================================

private:

    T* erase_n(T* pos, size_type count)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        if (count == 0)
        {
            return pos;
        }

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type new_size = size - count;
        const size_type tail_count = size - off - count;

        // Move the tail plus the null terminator
        // old end:  ptr[size] == '\0'
        // new end:  ptr[new_size] must become '\0'
        _char_traits_priv::move_batch(ptr + off, ptr + off + count, tail_count + 1);

        // Destroy removed objects
        range::destroy(ptr + new_size, count);

        size = new_size;
        return pos;
    }

public:

    expected<pointer, error> erase(size_type off = 0, size_type count = npos)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > size(), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        auto ptr = m_data().ptr + off;
        return erase_n(ptr, count);
    }

    iterator erase(const_iterator pos)
    {
        VX_PRIV_ASSERT_CONTIG_ERASABLE_POSITION(pos);
        auto ptr = const_cast<T*>(pos.ptr());
        return iterator(erase_n(ptr, 1));
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        auto ptr = const_cast<T*>(first.ptr());
        return iterator(erase_n(ptr, count));
    }

    //=========================================================================
    // pop_back
    //=========================================================================

    void pop_back()
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        if (size > 0)
        {
            mem::destroy_in_place(ptr + size);
            --size;
            traits_type::assign(ptr[size], T());
        }
    }

    //=========================================================================
    // copy
    //=========================================================================

    size_type copy(T* dst, size_type count, size_type off = 0) const
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return 0;
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        traits_type::copy(dst, m_data().ptr + off, count);
        return count;
    }

    basic_string substr(size_type off = 0, size_type count = npos) const
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return basic_string(m_allocator());
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return basic_string(m_data().ptr + off, count, m_allocator());
    }

    basic_string_view<T> view(size_type off = 0, size_type count = npos) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return basic_string_view<T>();
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return basic_string_view<T>(m_data().ptr + off, count);
    }

    //=========================================================================
    // replace
    //=========================================================================

private:

    template <construct_method M, typename... Args>
    void replace_capacity(pointer pos, size_type in_count, size_type out_count, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;

        if (in_count > out_count)
        {
            const size_type diff = in_count - out_count;
            const pointer back = ptr + size + 1;
            range::construct_maybe_trivial(back, diff);

            const size_type tail_count = static_cast<size_type>(back - (pos + out_count));
            _char_traits_priv::move_batch(pos + in_count, pos + out_count, tail_count);

            size += diff;
        }

        if (in_count < out_count)
        {
            const size_type diff = out_count - in_count;
            const pointer back = ptr + size + 1;
            const size_type tail_count = static_cast<size_type>(back - (pos + out_count));
            _char_traits_priv::move_batch(pos + in_count, pos + out_count, tail_count);

            range::destroy(back - diff, diff);

            size -= diff;
        }

        VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(pos, in_count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _char_traits_priv::copy_batch(pos, std::forward<Args>(args)..., in_count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(pos, std::forward<Args>(args)...);
        }
    }

    template <construct_method M, typename... Args>
    success replace_reallocate(T* pos, size_type in_count, size_type out_count, size_type new_capacity, Args&&... args)
    {
        auto& ptr = m_data().ptr;
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        pointer new_ptr = m_allocator().allocate(new_capacity + 1);
        VX_RET_ERR_IF(!new_ptr, err::out_of_memory);

        const size_type off = static_cast<size_type>(pos - ptr);
        pointer dst = new_ptr + off;

        // copy prefix [ptr, ptr + size) to [new_ptr, ...), then construct suffix [ptr + size, ...)
        range::construct_maybe_trivial(new_ptr, new_capacity + 1);
        traits_type::copy(new_ptr, ptr, off);

        VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(dst, in_count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _char_traits_priv::copy_batch(dst, std::forward<Args>(args)..., in_count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(dst, std::forward<Args>(args)...);
        }

        // copy second range
        traits_type::copy(dst + in_count, pos + out_count, (size - off - out_count) + 1);

        // destroy original range
        destroy_and_deallocate(ptr, size, capacity);

        ptr = new_ptr;
        size = new_capacity;
        capacity = new_capacity;

        return success{};
    }

    template <construct_method M, typename... Args>
    success replace_n(const T* pos, size_type in_count, size_type out_count, Args&&... args)
    {
        auto& size = m_data().size;
        auto& capacity = m_data().capacity;

        // out_count is always <= size here (callers clamp it via
        // clamp_suffix_size), so this subtraction can't underflow.
        VX_ASSERT(out_count <= size);
        const size_type base = size - out_count;

        // overflow-safe: if in_count were added to base directly, a huge
        // in_count could wrap size_type back into range and slip past a
        // post-hoc "new_size > max_size()" check. Checking against the
        // remaining headroom first avoids ever forming the wrapped value.
        VX_RET_ERR_IF(in_count > max_size() - base, err::size_error);
        const size_type new_size = base + in_count;

        auto ptr = const_cast<T*>(pos);

        if (new_size <= capacity)
        {
            replace_capacity<M>(ptr, in_count, out_count, std::forward<Args>(args)...);
            return success{};
        }

        return replace_reallocate<M>(ptr, in_count, out_count, new_size, std::forward<Args>(args)...);
    }

public:

    success replace(size_type off, size_type count, const basic_string& other)
    {
        VX_ASSERT(&other != this);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return replace_n<construct_method::from_pointer>(m_data().ptr + off, other.size(), count, other.data());
    }

    success replace(size_type off, size_type count, const basic_string& other, size_type other_off, size_type count2 = npos)
    {
        VX_ASSERT(&other != this);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), other_off), err::out_of_range);

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count2));
        return replace_n<construct_method::from_pointer>(m_data().ptr + off, count2, count, other.data() + other_off);
    }

    //=========================================================================

    success replace(size_type off, size_type count, size_type count2, const T c)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return replace_n<construct_method::from_char_count>(m_data().ptr + off, count2, count, c);
    }

    //=========================================================================

    success replace(size_type off, size_type count, const T* const s)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type count2 = static_cast<size_type>(traits_type::length(s));
        return replace_n<construct_method::from_pointer>(m_data().ptr + off, count2, count, s);
    }

    success replace(size_type off, size_type count, const T* const s, size_type count2)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return replace_n<construct_method::from_pointer>(m_data().ptr + off, count2, count, s);
    }

    //=========================================================================

    success replace(size_type off, size_type count, std::initializer_list<T> init)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type count2 = static_cast<size_type>(init.size());
        return replace_n<construct_method::from_pointer>(m_data().ptr + off, count2, count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    success replace(size_type off, size_type count, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type count2 = static_cast<size_type>(std::distance(first, last));
        return replace_n<construct_method::from_iterator_range>(m_data().ptr + off, count2, count, first, last);
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    success replace(size_type off, size_type count, const S& other)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type count2 = static_cast<size_type>(other.size());
        return replace_n<construct_method::from_pointer>(m_data().ptr + off, count2, count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    success replace(size_type off, size_type count, const S& other, size_type other_off, size_type count2 = npos)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), other_off), err::out_of_range);

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count2));
        return replace_n<construct_method::from_pointer>(m_data().ptr + off, count2, count, other.data() + other_off);
    }

    //=========================================================================
    //=========================================================================

    success replace(const_iterator first, const_iterator last, const basic_string& other)
    {
        VX_ASSERT(&other != this);
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        return replace_n<construct_method::from_pointer>(first.ptr(), other.size(), count, other.data());
    }

    success replace(const_iterator first, const_iterator last, const basic_string& other, size_type other_off, size_type count2 = npos)
    {
        VX_ASSERT(&other != this);
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            return success{};
        }

        const size_type count = static_cast<size_type>(std::distance(first, last));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count2));
        return replace_n<construct_method::from_pointer>(first.ptr(), count2, count, other.data() + other_off);
    }

    //=========================================================================

    success replace(const_iterator first, const_iterator last, size_type count2, const T c)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        return replace_n<construct_method::from_char_count>(first.ptr(), count2, count, c);
    }

    //=========================================================================

    success replace(const_iterator first, const_iterator last, const T* const s)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(traits_type::length(s));
        return replace_n<construct_method::from_pointer>(first.ptr(), count2, count, s);
    }

    success replace(const_iterator first, const_iterator last, const T* const s, size_type count2)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        return replace_n<construct_method::from_pointer>(first.ptr(), count2, count, s);
    }

    //=========================================================================

    success replace(const_iterator first, const_iterator last, std::initializer_list<T> init)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(init.size());
        return replace_n<construct_method::from_pointer>(first.ptr(), count2, count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    success replace(const_iterator first, const_iterator last, IT first2, IT last2)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first2, last2);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(std::distance(first2, last2));
        return replace_n<construct_method::from_iterator_range>(first.ptr(), count2, count, first2, last2);
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    success replace(const_iterator first, const_iterator last, const S& other)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(other.size());
        return replace_n<construct_method::from_pointer>(first.ptr(), count2, count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    success replace(const_iterator first, const_iterator last, const S& other, size_type other_off, size_type count2 = npos)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            return success{};
        }

        const size_type count = static_cast<size_type>(std::distance(first, last));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count2));
        return replace_n<construct_method::from_pointer>(first.ptr(), count2, count, other.data() + other_off);
    }

    //=========================================================================
    // searching
    //=========================================================================

    bool contains(const basic_string& other) const noexcept
    {
        return find(other) != npos;
    }

    bool contains(const T c) const noexcept
    {
        return find(c) != npos;
    }

    bool contains(const T* const s) const noexcept
    {
        return find(s) != npos;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    bool contains(const S& other) const noexcept
    {
        return find(other) != npos;
    }

    //=========================================================================

    size_type find(const basic_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    size_type find(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_ch<traits_type>(
            m_data().ptr, m_data().size, off, c));
    }

    size_type find(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find<traits_type>(
            m_data().ptr, m_data().size, off, s, s_len));
    }

    size_type find(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find<traits_type>(
            m_data().ptr, m_data().size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find(const S& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    //=========================================================================

    size_type rfind(const basic_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    size_type rfind(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind_ch<traits_type>(
            m_data().ptr, m_data().size, off, c));
    }

    size_type rfind(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_rfind<traits_type>(
            m_data().ptr, m_data().size, off, s, s_len));
    }

    size_type rfind(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind<traits_type>(
            m_data().ptr, m_data().size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type rfind(const S& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    //=========================================================================

    size_type find_first_of(const basic_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_of<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    size_type find_first_of(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_ch<traits_type>(
            m_data().ptr, m_data().size, off, c));
    }

    size_type find_first_of(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find_first_of<traits_type>(
            m_data().ptr, m_data().size, off, s, s_len));
    }

    size_type find_first_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_of<traits_type>(
            m_data().ptr, m_data().size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find_first_of(const S& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_of<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    //=========================================================================

    size_type find_last_of(const basic_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_of<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    size_type find_last_of(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind_ch<traits_type>(
            m_data().ptr, m_data().size, off, c));
    }

    size_type find_last_of(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find_last_of<traits_type>(
            m_data().ptr, m_data().size, off, s, s_len));
    }

    size_type find_last_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_of<traits_type>(
            m_data().ptr, m_data().size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find_last_of(const S& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_of<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    //=========================================================================

    size_type find_first_not_of(const basic_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_not_of<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    size_type find_first_not_of(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_not_ch<traits_type>(
            m_data().ptr, m_data().size, off, c));
    }

    size_type find_first_not_of(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find_first_not_of<traits_type>(
            m_data().ptr, m_data().size, off, s, s_len));
    }

    size_type find_first_not_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_not_of<traits_type>(
            m_data().ptr, m_data().size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find_first_not_of(const S& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_not_of<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    //=========================================================================

    size_type find_last_not_of(const basic_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_not_of<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    size_type find_last_not_of(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind_not_ch<traits_type>(
            m_data().ptr, m_data().size, off, c));
    }

    size_type find_last_not_of(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find_last_not_of<traits_type>(
            m_data().ptr, m_data().size, off, s, s_len));
    }

    size_type find_last_not_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_not_of<traits_type>(
            m_data().ptr, m_data().size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find_last_not_of(const S& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_not_of<traits_type>(
            m_data().ptr, m_data().size, off, other.data(), other.size()));
    }

    //=========================================================================
    // comparison
    //=========================================================================

    int compare(const basic_string& other) const noexcept
    {
        return _char_traits_priv::traits_compare<traits_type>(
            m_data().ptr, m_data().size,
            other.data(), other.size());
    }

    int compare(size_type off, size_type count, const basic_string& other) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return 1;
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data().ptr + off, count,
            other.data(), other.size());
    }

    int compare(size_type off1, size_type count1, const basic_string& other, size_type off2, size_type count2 = npos) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off1) || !_char_traits_priv::check_offset(other.size(), off2))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off1, count1));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off2, count2));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data().ptr + off1, count1,
            other.data() + off2, count2);
    }

    //=========================================================================

    int compare(const T* const s) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data().ptr, m_data().size,
            s, s_len);
    }

    int compare(size_type off, size_type count, const T* const s) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return 1;
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return compare(off, count, s, s_len);
    }

    int compare(size_type off, size_type count1, const T* const s, size_type count2) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count1));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data().ptr + off, count1,
            s, count2);
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    int compare(const S& other) const noexcept
    {
        const size_type other_len = static_cast<size_type>(other.size());
        return _char_traits_priv::traits_compare<traits_type>(
            m_data().ptr, m_data().size,
            other.data(), other_len);
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    int compare(size_type off1, size_type count1, const S& other, size_type off2, size_type count2 = npos) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off1) || !_char_traits_priv::check_offset(other.size(), off2))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off1, count1));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off2, count2));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data().ptr + off1, count1,
            other.data() + off2, count2);
    }
};

//=========================================================================
// binary + operators
//=========================================================================

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(const basic_string<T, Allocator, Growth>& lhs, const basic_string<T, Allocator, Growth>& rhs)
{
    basic_string<T, Allocator, Growth> result(lhs);
    return result.operator+=(rhs);
}

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(basic_string<T, Allocator, Growth>&& lhs, basic_string<T, Allocator, Growth>&& rhs)
{
    return std::move(lhs).operator+=(rhs);
}

//=========================================================================

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(const basic_string<T, Allocator, Growth>& lhs, const T rhs)
{
    basic_string<T, Allocator, Growth> result(lhs);
    return result.operator+=(rhs);
}

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(const T lhs, const basic_string<T, Allocator, Growth>& rhs)
{
    basic_string<T, Allocator, Growth> result(1, lhs, rhs.get_allocator());
    return result.operator+=(rhs);
}

//=========================================================================

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(const basic_string<T, Allocator, Growth>& lhs, const T* const rhs)
{
    basic_string<T, Allocator, Growth> result(lhs);
    return result.operator+=(rhs);
}

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(const T* const lhs, const basic_string<T, Allocator, Growth>& rhs)
{
    basic_string<T, Allocator, Growth> result(lhs, rhs.get_allocator());
    return result.operator+=(rhs);
}

//=========================================================================

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(basic_string<T, Allocator, Growth>&& lhs, const basic_string<T, Allocator, Growth>& rhs)
{
    return std::move(lhs.operator+=(rhs));
}

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(const basic_string<T, Allocator, Growth>& lhs, basic_string<T, Allocator, Growth>&& rhs)
{
    return basic_string<T, Allocator, Growth>(lhs).operator+=(std::move(rhs));
}

//=========================================================================

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(basic_string<T, Allocator, Growth>&& lhs, const T rhs)
{
    lhs.push_back(rhs);
    return std::move(lhs);
}

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(const T lhs, basic_string<T, Allocator, Growth>&& rhs)
{
    return basic_string<T, Allocator, Growth>(1, lhs, rhs.get_allocator()).operator+=(std::move(rhs));
}

//=========================================================================

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(basic_string<T, Allocator, Growth>&& lhs, const T* const rhs)
{
    return std::move(lhs.operator+=(rhs));
}

template <typename T, typename Allocator, typename Growth>
basic_string<T, Allocator, Growth> operator+(const T* const lhs, basic_string<T, Allocator, Growth>&& rhs)
{
    return basic_string<T, Allocator, Growth>(lhs, rhs.get_allocator()).operator+=(std::move(rhs));
}

//=========================================================================
// comparison operators
//=========================================================================

template <typename T, typename Allocator, typename Growth>
bool operator==(const basic_string<T, Allocator, Growth>& lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator==(const basic_string<T, Allocator, Growth>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator==(const T* const lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return rhs.compare(lhs) == 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator!=(const basic_string<T, Allocator, Growth>& lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator!=(const basic_string<T, Allocator, Growth>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator!=(const T* const lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return rhs.compare(lhs) != 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator<(const basic_string<T, Allocator, Growth>& lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator<(const basic_string<T, Allocator, Growth>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator<(const T* const lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return rhs.compare(lhs) > 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator>(const basic_string<T, Allocator, Growth>& lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator>(const basic_string<T, Allocator, Growth>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator>(const T* const lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return rhs.compare(lhs) < 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator<=(const basic_string<T, Allocator, Growth>& lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator<=(const basic_string<T, Allocator, Growth>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator<=(const T* const lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return rhs.compare(lhs) >= 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator>=(const basic_string<T, Allocator, Growth>& lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator>=(const basic_string<T, Allocator, Growth>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <typename T, typename Allocator, typename Growth>
bool operator>=(const T* const lhs, const basic_string<T, Allocator, Growth>& rhs) noexcept
{
    return rhs.compare(lhs) <= 0;
}

//=========================================================================
// stream operators
//=========================================================================

template <typename T, typename Allocator, typename Growth, typename Traits2>
std::basic_istream<T, Traits2>& operator>>(
    std::basic_istream<T, Traits2>& iss,
    basic_string<T, Allocator, Growth>& s)
{
    std::string is;
    iss >> is;
    s = std::move(is);
    return iss;
}

template <typename T, typename Allocator, typename Growth, typename Traits2>
std::basic_ostream<T, Traits2>& operator<<(
    std::basic_ostream<T, Traits2>& oss,
    const basic_string<T, Allocator, Growth>& s)
{
    std::string os(s.data(), s.size());
    oss << os;
    return oss;
}

} // namespace str

//=========================================================================

using string = str::basic_string<char>;
using wstring = str::basic_string<wchar_t>;
#if VX_HAVE_STD_CHAR8_T
using u8string = str::basic_string<char8_t>;
#endif // VX_HAVE_STD_CHAR8_T
using u16string = str::basic_string<char16_t>;
using u32string = str::basic_string<char32_t>;

} // namespace vx

//=========================================================================
// hashing
//=========================================================================

namespace vx {

template <typename T>
struct hash;

template <typename T, typename Allocator, typename Growth>
struct hash<str::basic_string<T, Allocator, Growth>>
{
    size_t operator()(const vx::str::basic_string<T, Allocator, Growth>& s) const noexcept
    {
        using traits = typename vx::str::basic_string<T, Allocator, Growth>::traits_type;
        return traits::hash(s.data(), s.size());
    }
};

} // namespace vx

namespace std {

template <typename T, typename Allocator, typename Growth>
struct hash<vx::str::basic_string<T, Allocator, Growth>>
{
    size_t operator()(const vx::str::basic_string<T, Allocator, Growth>& s) const noexcept
    {
        return vx::hash<vx::str::basic_string<T, Allocator, Growth>>{}(s);
    }
};

} // namespace std
