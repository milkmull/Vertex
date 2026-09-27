#pragma once

#include <ratio>
#include <sstream>
#include <string>

#include "vertex/std/_tools/static_array_base.hpp"
#include "vertex/std/char_traits.hpp"
#include "vertex/std/string.hpp"

namespace vx {
namespace str {

template <size_t N, typename T>
class basic_static_string : private _static_array_base_priv::static_array_base<N + 1, T>
{
private:

    VX_STATIC_ASSERT_MSG(type_traits::is_char<T>::value, "T must be character type");
    VX_STATIC_ASSERT_MSG(N > 0, "N must be greater than 0");

    using base = _static_array_base_priv::static_array_base<N + 1, T>;
    using data_type = decltype(base::m_data);

    template <typename IT>
    using is_my_iterator = _priv::is_my_pointer_iterator<IT, basic_static_string>;

    template <typename S>
    struct is_compatible_string
    {
        static constexpr bool value = is_string_view<S>::value && is_string_of<S, T>::value;
    };

    template <size_t M>
    struct is_compatible_string<basic_static_string<M, T>> : std::true_type
    {
        static constexpr bool value = (M <= N);
    };

    template <typename IT>
    struct is_fittable_iterator : std::false_type
    {};

    template <size_t M>
    struct is_fittable_iterator<_priv::pointer_iterator<basic_static_string<M, T>, T>>
    {
        static constexpr bool value = (M <= N);
    };

public:

    //=========================================================================
    // member types
    //=========================================================================

    using traits_type = char_traits<T>;

    using value_type = typename data_type::value_type;
    using pointer = typename data_type::pointer;
    using const_pointer = typename data_type::const_pointer;
    using reference = typename data_type::reference;
    using const_reference = typename data_type::const_reference;
    using size_type = typename data_type::size_type;
    using difference_type = typename data_type::difference_type;

    using iterator = _priv::pointer_iterator<basic_static_string, T>;
    using const_iterator = _priv::pointer_iterator<basic_static_string, const T>;
    using reverse_iterator = _priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

    static constexpr auto npos{ static_cast<size_type>(-1) };

private:

    using base::m_data;

    enum class construct_method
    {
        from_char,
        from_char_count,
        from_pointer,
        from_string,
        from_iterator_range
    };

    //=========================================================================
    // allocation helpers
    //=========================================================================

    static constexpr void destroy_size(T* ptr, size_type size)
    {
        range::destroy(ptr, size + 1);
    }

    constexpr void destroy_range()
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        if (size > 0)
        {
            destroy_size(ptr, size);
        }

        // don'other need to construct empty, should be invalid after this
    }

    //=========================================================================
    // constructor helpers
    //=========================================================================

    constexpr void construct_empty()
    {
        m_data.size = 0;
        mem::construct_in_place_maybe_trivial(m_data.ptr);
        traits_type::assign(*m_data.ptr, T());
    }

    template <construct_method M, bool Fits, typename... Args>
    constexpr success construct_n(size_type count, Args&&... args)
    {
        VX_IF_CONSTEXPR (!Fits)
        {
            VX_RET_ERR_IF(count > max_size(), err::size_error);
        }

        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        // +1 to also construct the null-terminator slot
        range::construct_maybe_trivial(ptr, count + 1);

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
            // count + 1 to also copy the source's null terminator
            traits_type::copy(ptr, std::forward<Args>(args)..., count + 1);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");

            traits_type::copy_range(ptr, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }

        size = count;
        return success{};
    }

    struct uninitialized_tag
    {};

    constexpr basic_static_string(uninitialized_tag) noexcept
    {}

public:

    //=========================================================================
    // constructors
    //=========================================================================

    basic_static_string(std::nullptr_t) = delete;

    constexpr basic_static_string()
    {
        construct_empty();
    }

    constexpr basic_static_string(const basic_static_string& other)
    {
        const auto ok = construct_n<construct_method::from_string, true>(other.size(), other.data());
        VX_VERIFY(ok);
    }

    constexpr basic_static_string(basic_static_string&& other) noexcept
    {
        const auto ok = construct_n<construct_method::from_string, true>(other.m_data.size, other.m_data.ptr);
        VX_VERIFY(ok);
        other.destroy_range();
    }

    //=========================================================================

    constexpr basic_static_string(const basic_static_string& other, size_type off)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            construct_empty();
            return;
        }

        const auto ok = construct_n<construct_method::from_pointer, true>(other.size() - off, other.data() + off);
        VX_VERIFY(ok);
    }

    constexpr basic_static_string(const basic_static_string& other, size_type off, size_type count)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            construct_empty();
            return;
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        const auto ok = construct_n<construct_method::from_pointer, true>(count, other.data() + off);
        VX_VERIFY(ok);
    }

    //=========================================================================

    constexpr basic_static_string(size_type count, const T value)
    {
        const auto ok = construct_n<construct_method::from_char_count, false>(count, value);
        VX_VERIFY(ok);
    }

    //=========================================================================

    constexpr basic_static_string(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        const auto ok = construct_n<construct_method::from_pointer, false>(count, ptr);
        VX_VERIFY(ok);
    }

    constexpr basic_static_string(const T* const ptr, size_type count)
    {
        const auto ok = construct_n<construct_method::from_pointer, false>(count, ptr);
        VX_VERIFY(ok);
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr basic_static_string(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        success ok;

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            constexpr bool fits = is_fittable_iterator<IT>::value;
            ok = construct_n<construct_method::from_pointer, fits>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            ok = construct_n<construct_method::from_pointer, false>(count, first);
        }
        else
        {
            ok = construct_n<construct_method::from_iterator_range, false>(count, first, last);
        }

        VX_VERIFY(ok);
    }

    //=========================================================================

    constexpr basic_static_string(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        const auto ok = construct_n<construct_method::from_pointer, false>(count, init.begin());
        VX_VERIFY(ok);
    }

    //=========================================================================

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr basic_static_string(const basic_static_string<M, T>& other)
    {
        const auto ok = construct_n<construct_method::from_string, true>(other.size(), other.data());
        VX_VERIFY(ok);
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr basic_static_string(const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        const auto ok = construct_n<construct_method::from_pointer, false>(count, other.data());
        VX_VERIFY(ok);
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr basic_static_string(const S& other, size_type off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            construct_empty();
            return;
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        const auto ok = construct_n<construct_method::from_pointer, false>(count, other.data() + off);
        VX_VERIFY(ok);
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static constexpr expected<basic_static_string, error> create()
    {
        basic_static_string s(uninitialized_tag{});
        s.construct_empty();
        return s;
    }

    static constexpr expected<basic_static_string, error> create(const basic_static_string& other)
    {
        basic_static_string s(uninitialized_tag{});
        const auto ok = s.template construct_n<construct_method::from_string, true>(other.size(), other.data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    static constexpr expected<basic_static_string, error> create(basic_static_string&& other) noexcept
    {
        basic_static_string s(uninitialized_tag{});
        const auto ok = s.template construct_n<construct_method::from_string, true>(other.m_data.size, other.m_data.ptr);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        other.destroy_range();
        return s;
    }

    //=========================================================================

    static constexpr expected<basic_static_string, error> create(const basic_static_string& other, size_type off)
    {
        basic_static_string s(uninitialized_tag{});

        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            s.construct_empty();
            return s;
        }

        const auto ok = s.template construct_n<construct_method::from_pointer, true>(other.size() - off, other.data() + off);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    static constexpr expected<basic_static_string, error> create(const basic_static_string& other, size_type off, size_type count)
    {
        basic_static_string s(uninitialized_tag{});

        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            s.construct_empty();
            return s;
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        const auto ok = s.template construct_n<construct_method::from_pointer, true>(count, other.data() + off);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    static constexpr expected<basic_static_string, error> create(size_type count, const T value)
    {
        basic_static_string s(uninitialized_tag{});
        const auto ok = s.template construct_n<construct_method::from_char_count, false>(count, value);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    static constexpr expected<basic_static_string, error> create(const T* const ptr)
    {
        basic_static_string s(uninitialized_tag{});
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        const auto ok = s.template construct_n<construct_method::from_pointer, false>(count, ptr);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    static constexpr expected<basic_static_string, error> create(const T* const ptr, size_type count)
    {
        basic_static_string s(uninitialized_tag{});
        const auto ok = s.template construct_n<construct_method::from_pointer, false>(count, ptr);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static constexpr expected<basic_static_string, error> create(IT first, IT last)
    {
        basic_static_string s(uninitialized_tag{});

        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        const size_type count = static_cast<size_type>(std::distance(first, last));

        success ok;
        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            constexpr bool fits = is_fittable_iterator<IT>::value;
            ok = s.template construct_n<construct_method::from_pointer, fits>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            ok = s.template construct_n<construct_method::from_pointer, false>(count, first);
        }
        else
        {
            ok = s.template construct_n<construct_method::from_iterator_range, false>(count, first, last);
        }

        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    static constexpr expected<basic_static_string, error> create(std::initializer_list<T> init)
    {
        basic_static_string s(uninitialized_tag{});
        const size_type count = static_cast<size_type>(init.size());
        const auto ok = s.template construct_n<construct_method::from_pointer, false>(count, init.begin());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    //=========================================================================

    template <size_t M, VX_REQUIRES(M <= N)>
    static constexpr expected<basic_static_string, error> create(const basic_static_string<M, T>& other)
    {
        basic_static_string s(uninitialized_tag{});
        const auto ok = s.template construct_n<construct_method::from_string, true>(other.size(), other.data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    static constexpr expected<basic_static_string, error> create(const S& other)
    {
        basic_static_string s(uninitialized_tag{});
        const size_type count = static_cast<size_type>(other.size());
        const auto ok = s.template construct_n<construct_method::from_pointer, false>(count, other.data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    static constexpr expected<basic_static_string, error> create(const S& other, size_type off, size_type count = npos)
    {
        basic_static_string s(uninitialized_tag{});

        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            s.construct_empty();
            return s;
        }

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        const auto ok = s.template construct_n<construct_method::from_pointer, false>(count, other.data() + off);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return s;
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~basic_static_string() = default;

    //=========================================================================
    // operators
    //=========================================================================

    constexpr operator basic_string_view<T>() const noexcept
    {
        return basic_string_view<T>(data(), size());
    }

    constexpr operator basic_cstring_view<T>() const noexcept
    {
        return basic_cstring_view<T>(*this);
    }

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, bool Fits, bool NoOverlap, typename... Args>
    constexpr success assign_from(const size_type count, Args&&... args)
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        VX_IF_CONSTEXPR (!Fits)
        {
            VX_RET_ERR_IF(count > max_size(), err::size_error);
        }

        if (count > size)
        {
            range::construct_maybe_trivial(ptr + size + 1, count - size);
        }
        else VX_IF_CONSTEXPR (NoOverlap)
        {
            if (count < size)
            {
                range::destroy(ptr + count + 1, size - count);
            }
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
            VX_IF_CONSTEXPR (NoOverlap)
            {
                _char_traits_priv::copy_batch(ptr, std::forward<Args>(args)..., count);
            }
            else
            {
                _char_traits_priv::move_batch(ptr, std::forward<Args>(args)..., count);
            }
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_string)
        {
            VX_IF_CONSTEXPR (NoOverlap)
            {
                traits_type::copy(ptr, std::forward<Args>(args)..., count + 1);
            }
            else
            {
                _char_traits_priv::move_batch(ptr, std::forward<Args>(args)..., count);
                traits_type::assign(ptr[count], T());
            }
        }
        else
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(ptr, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }

        VX_IF_CONSTEXPR (!NoOverlap)
        {
            if (count < size)
            {
                range::destroy(ptr + count + 1, size - count);
            }
        }

        size = count;
        return success{};
    }

public:

    //=========================================================================
    // assignment operators
    //=========================================================================

    constexpr basic_static_string& operator=(const basic_static_string& other)
    {
        if (this == &other)
        {
            return *this;
        }
        const auto ok = assign_from<construct_method::from_string, true, true>(other.size(), other.data());
        VX_VERIFY(ok);
        return *this;
    }

    constexpr basic_static_string& operator=(basic_static_string&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        const auto ok = assign_from<construct_method::from_string, true, true>(other.size(), other.data());
        VX_VERIFY(ok);
        return *this;
    }

    constexpr basic_static_string& operator=(const T c)
    {
        const auto ok = assign_from<construct_method::from_char, true, true>(1, c);
        VX_VERIFY(ok);
        return *this;
    }

    constexpr basic_static_string& operator=(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        const auto ok = assign_from<construct_method::from_pointer, false, false>(count, ptr);
        VX_VERIFY(ok);
        return *this;
    }

    constexpr basic_static_string& operator=(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        const auto ok = assign_from<construct_method::from_pointer, false, true>(count, init.begin());
        VX_VERIFY(ok);
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr basic_static_string& operator=(const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        const auto ok = assign_from<construct_method::from_pointer, false, false>(count, other.data());
        VX_VERIFY(ok);
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    constexpr success assign(const basic_static_string& other)
    {
        if (this == &other)
        {
            return success{};
        }
        return assign_from<construct_method::from_string, true, true>(other.size(), other.data());
    }

    constexpr success assign(basic_static_string&& other) noexcept
    {
        operator=(std::move(other));
        return success{};
    }

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr success assign(const basic_static_string<M, T>& other)
    {
        return assign_from<construct_method::from_string, true, true>(other.size(), other.data());
    }

    //=========================================================================

    constexpr success assign(const T c)
    {
        return assign_from<construct_method::from_char, true, true>(1, c);
    }

    constexpr success assign(const size_type count, const T c)
    {
        return assign_from<construct_method::from_char_count, false, true>(count, c);
    }

    //=========================================================================

    constexpr success assign(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        return assign_from<construct_method::from_pointer, false, false>(count, ptr);
    }

    constexpr success assign_no_overlap(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(ptr, ptr + count);
        return assign_from<construct_method::from_pointer, false, true>(count, ptr);
    }

    //=========================================================================

    constexpr success assign(const T* const ptr, size_type count)
    {
        return assign_from<construct_method::from_pointer, false, false>(count, ptr);
    }

    constexpr success assign_no_overlap(const T* const ptr, size_type count)
    {
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(ptr, ptr + count);
        return assign_from<construct_method::from_pointer, false, true>(count, ptr);
    }

    //=========================================================================

    constexpr success assign(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return assign_from<construct_method::from_pointer, false, true>(count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr success assign(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            constexpr bool fits = is_fittable_iterator<IT>::value;
            const size_type count = static_cast<size_type>(std::distance(first, last));
            return assign_from<construct_method::from_pointer, fits, false>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            return assign_from<construct_method::from_pointer, false, false>(count, first);
        }
        else
        {
            const auto tmp = basic_static_string::create(first, last);
            VX_RET_ERR_IF(!tmp, tmp.error());
            return assign_from<construct_method::from_pointer, true, true>(tmp.value().size(), tmp.value().data());
        }
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr success assign_no_overlap(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            constexpr bool fits = is_fittable_iterator<IT>::value;
            const size_type count = static_cast<size_type>(std::distance(first, last));
            VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
            return assign_from<construct_method::from_pointer, fits, true>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last);
            return assign_from<construct_method::from_pointer, false, true>(count, first);
        }
        else
        {
            const auto tmp = basic_static_string::create(first, last);
            VX_RET_ERR_IF(!tmp, tmp.error());
            return assign_from<construct_method::from_pointer, true, true>(tmp.value().size(), tmp.value().data());
        }
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success assign(const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        return assign_from<construct_method::from_pointer, false, false>(count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success assign_no_overlap(const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(other.data(), other.data() + count);
        return assign_from<construct_method::from_pointer, false, true>(count, other.data());
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success assign(const S& other, size_type off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            clear();
            return success{};
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return assign_from<construct_method::from_pointer, false, false>(count, other.data() + off);
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success assign_no_overlap(const S& other, size_type off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            clear();
            return success{};
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(other.data() + off, other.data() + off + count);
        return assign_from<construct_method::from_pointer, false, true>(count, other.data() + off);
    }

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr success assign(const basic_static_string<M, T>& other, size_type off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), off))
        {
            clear();
            return success{};
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return assign_from<construct_method::from_pointer, true, true>(count, other.data() + off);
    }

    //=========================================================================
    // element access
    //=========================================================================

    constexpr expected<T&, error> front() noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return m_data.ptr[0];
    }

    constexpr expected<const T&, error> front() const noexcept
    {
        VX_RET_UNEXPECTED_ERR_IF(empty(), err::out_of_range);
        return m_data.ptr[0];
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

    constexpr T* data() noexcept
    {
        return m_data.ptr;
    }

    constexpr const T* data() const noexcept
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

    constexpr const T* c_str() const noexcept
    {
        return m_data.ptr;
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_data.ptr);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_data.ptr);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(m_data.ptr + m_data.size);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_data.ptr + m_data.size);
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
    constexpr success append_n(const size_type count, Args&&... args)
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        const size_type available = N - size;
        VX_RET_ERR_IF(count > available, err::size_error);

        T* const dst = ptr + size;

        // increase size early so the null terminator can be assigned at the end
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
        else
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_pointer, "invalid tag");
            _char_traits_priv::copy_batch(dst, std::forward<Args>(args)..., count);
        }

        traits_type::assign(ptr[size], T());
        return success{};
    }

public:

    //=========================================================================
    // append
    //=========================================================================

    constexpr success append(const basic_static_string& other)
    {
        return append_n<construct_method::from_pointer>(other.size(), other.data());
    }

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr success append(const basic_static_string<M, T>& other)
    {
        return append_n<construct_method::from_pointer>(other.size(), other.data());
    }

    //=========================================================================

    constexpr success append(const basic_static_string& other, size_type off, size_type count = npos)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return append_n<construct_method::from_pointer>(count, other.data() + off);
    }

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr success append(const basic_static_string<M, T>& other, size_type off, size_type count = npos)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return append_n<construct_method::from_pointer>(count, other.data() + off);
    }

    //=========================================================================

    constexpr success append(const T c)
    {
        return append_n<construct_method::from_char>(1, c);
    }

    constexpr success append(size_type count, const T c)
    {
        return append_n<construct_method::from_char_count>(count, c);
    }

    //=========================================================================

    constexpr success append(const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        return append_n<construct_method::from_pointer>(count, s);
    }

    constexpr success append(const T* const s, const size_type count)
    {
        return append_n<construct_method::from_pointer>(count, s);
    }

    //=========================================================================

    constexpr success append(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return append_n<construct_method::from_pointer>(count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr success append(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            return append_n<construct_method::from_pointer>(count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            return append_n<construct_method::from_pointer>(count, first);
        }
        else
        {
            const auto tmp = basic_static_string::create(first, last);
            VX_RET_ERR_IF(!tmp, tmp.error());
            return append_n<construct_method::from_pointer>(tmp.value().size(), tmp.value().data());
        }
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success append(const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        return append_n<construct_method::from_pointer>(count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success append(const S& other, size_type off, size_type count = npos)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off, count));
        return append_n<construct_method::from_pointer>(count, other.data() + off);
    }

    //=========================================================================

    constexpr basic_static_string& operator+=(const basic_static_string& other)
    {
        const auto ok = append(other);
        VX_VERIFY(ok);
        return *this;
    }

    constexpr basic_static_string& operator+=(const T c)
    {
        const auto ok = append(c);
        VX_VERIFY(ok);
        return *this;
    }

    constexpr basic_static_string& operator+=(const T* const s)
    {
        const auto ok = append(s);
        VX_VERIFY(ok);
        return *this;
    }

    constexpr basic_static_string& operator+=(std::initializer_list<T> init)
    {
        const auto ok = append(init);
        VX_VERIFY(ok);
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr basic_static_string& operator+=(const S& other)
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
    constexpr T* insert_n_no_overlap(T* pos, const size_type count, Args&&... args)
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        const pointer back = ptr + size;
        range::construct_maybe_trivial(back + 1, count);

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
        else
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_pointer, "invalid tag");
            _char_traits_priv::copy_batch(pos, std::forward<Args>(args)..., count);
        }

        size += count;
        return pos;
    }

    constexpr T* insert_n_pointer_safe(T* pos, const size_type count, const T* src)
    {
        if (count == 0)
        {
            return pos;
        }

        auto& ptr = m_data.ptr;
        auto& size = m_data.size;
        const T* const old_back = ptr + size;

        size_type unshifted;
        if (src + count <= pos || src > old_back)
        {
            unshifted = count;
        }
        else if (pos <= src)
        {
            unshifted = 0;
        }
        else
        {
            unshifted = static_cast<size_type>(pos - src);
        }

        const pointer back = ptr + size;
        range::construct_maybe_trivial(back + 1, count);
        const size_type tail_count = static_cast<size_type>(back - pos) + 1;
        _char_traits_priv::move_batch(pos + count, pos, tail_count);
        size += count;

        _char_traits_priv::copy_batch(pos, src, unshifted);
        _char_traits_priv::copy_batch(pos + unshifted, src + count + unshifted, count - unshifted);

        return pos;
    }

    template <construct_method M, typename... Args>
    constexpr success insert_n(T* pos, const size_type count, Args&&... args)
    {
        const size_type available = N - m_data.size;
        VX_RET_ERR_IF(count > available, err::size_error);

        T* new_pos;
        VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            new_pos = insert_n_pointer_safe(pos, count, std::forward<Args>(args)...);
        }
        else
        {
            new_pos = insert_n_no_overlap<M>(pos, count, std::forward<Args>(args)...);
        }

        return success{};
    }

    template <construct_method M, typename... Args>
    constexpr expected<iterator, error> insert_checked(size_type off, size_type count, Args&&... args)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > m_data.size, err::out_of_range);
        T* pos = m_data.ptr + off;

        const auto ok = insert_n<M>(pos, count, std::forward<Args>(args)...);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok.error());
        return iterator(pos);
    }

    template <construct_method M, typename... Args>
    constexpr iterator insert_unchecked(const_iterator pos, size_type count, Args&&... args)
    {
        VX_PRIV_ASSERT_CONTIG_INSERTABLE_POSITION(pos);
        T* p = const_cast<T*>(pos.ptr());

        const auto ok = insert_n<M>(p, count, std::forward<Args>(args)...);
        VX_VERIFY(ok);
        return iterator(p);
    }

public:

    //=========================================================================
    // insert
    //=========================================================================

    constexpr expected<iterator, error> insert(size_type off, const basic_static_string& other)
    {
        return insert(off, other.data(), other.size());
    }

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr expected<iterator, error> insert(size_type off, const basic_static_string<M, T>& other)
    {
        return insert(off, other.data(), other.size());
    }

    //=========================================================================

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr expected<iterator, error> insert(size_type off, const basic_static_string<M, T>& other, size_type other_off, size_type count = npos)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > m_data.size, err::out_of_range);
        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            return iterator(m_data.ptr + off);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count));
        return insert_checked<construct_method::from_pointer>(off, count, other.data() + other_off);
    }

    constexpr expected<iterator, error> insert(size_type off, const basic_static_string& other, size_type other_off, size_type count = npos)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > m_data.size, err::out_of_range);
        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            return iterator(m_data.ptr + off);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count));
        return insert_checked<construct_method::from_pointer>(off, count, other.data() + other_off);
    }

    //=========================================================================

    constexpr expected<iterator, error> insert(size_type off, const T c)
    {
        return insert_checked<construct_method::from_char>(off, 1, c);
    }

    constexpr expected<iterator, error> insert(size_type off, size_type count, const T c)
    {
        return insert_checked<construct_method::from_char_count>(off, count, c);
    }

    //=========================================================================

    constexpr expected<iterator, error> insert(size_type off, const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        return insert_checked<construct_method::from_pointer>(off, count, s);
    }

    constexpr expected<iterator, error> insert(size_type off, const T* const s, size_type count)
    {
        return insert_checked<construct_method::from_pointer>(off, count, s);
    }

    //=========================================================================

    constexpr expected<iterator, error> insert(size_type off, std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return insert_checked<construct_method::from_pointer>(off, count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr expected<iterator, error> insert(size_type off, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            return insert_checked<construct_method::from_pointer>(off, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            return insert_checked<construct_method::from_pointer>(off, count, first);
        }
        else
        {
            const auto tmp = basic_static_string::create(first, last);
            VX_RET_UNEXPECTED_ERR_IF(!tmp, tmp.error());
            return insert_checked<construct_method::from_pointer>(off, tmp.value().size(), tmp.value().data());
        }
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr expected<iterator, error> insert(size_type off, const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        return insert_checked<construct_method::from_pointer>(off, count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr expected<iterator, error> insert(size_type off, const S& other, size_type t_off, size_type count = npos)
    {
        VX_RET_UNEXPECTED_ERR_IF(off > m_data.size, err::out_of_range);
        if (!_char_traits_priv::check_offset(other.size(), t_off))
        {
            return iterator(m_data.ptr + off);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), t_off, count));
        return insert_checked<construct_method::from_pointer>(off, count, other.data() + t_off);
    }

    //=========================================================================
    //=========================================================================

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr iterator insert(const_iterator pos, const basic_static_string<M, T>& other)
    {
        return insert(pos, other.data(), other.size());
    }

    template <size_t M>
    constexpr iterator insert(const_iterator pos, const basic_static_string<M, T>& other, size_type other_off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), other_off))
        {
            return iterator(pos);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count));
        return insert_unchecked<construct_method::from_pointer>(pos, count, other.data() + other_off);
    }

    //=========================================================================

    constexpr iterator insert(const_iterator pos, const T c)
    {
        return insert_unchecked<construct_method::from_char>(pos, 1, c);
    }

    constexpr iterator insert(const_iterator pos, const size_type count, const T c)
    {
        return insert_unchecked<construct_method::from_char_count>(pos, count, c);
    }

    //=========================================================================

    constexpr iterator insert(const_iterator pos, const T* const s, size_type count)
    {
        return insert_unchecked<construct_method::from_pointer>(pos, count, s);
    }

    constexpr iterator insert(const_iterator pos, const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        return insert_unchecked<construct_method::from_pointer>(pos, count, s);
    }

    //=========================================================================

    constexpr iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return insert_unchecked<construct_method::from_pointer>(pos, count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr iterator insert(const_iterator pos, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            return insert_unchecked<construct_method::from_pointer>(pos, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            return insert_unchecked<construct_method::from_pointer>(pos, count, first);
        }
        else
        {
            const auto tmp = basic_static_string::create(first, last);
            VX_VERIFY(tmp);
            return insert_unchecked<construct_method::from_pointer>(pos, tmp.value().size(), tmp.value().data());
        }
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr iterator insert(const_iterator pos, const S& other)
    {
        const size_type count = static_cast<size_type>(other.size());
        return insert_unchecked<construct_method::from_pointer>(pos, count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr iterator insert(const_iterator pos, const S& other, size_type t_off, size_type count = npos)
    {
        if (!_char_traits_priv::check_offset(other.size(), t_off))
        {
            return iterator(pos);
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), t_off, count));
        return insert_unchecked<construct_method::from_pointer>(pos, count, other.data() + t_off);
    }

public:

    //=========================================================================
    // memory
    //=========================================================================

    constexpr void clear() noexcept
    {
        destroy_size(m_data.ptr, m_data.size);
        m_data.size = 0;
        traits_type::assign(*m_data.ptr, T());
    }

    constexpr void clear_and_deallocate() noexcept
    {
        clear();
    }

    constexpr success shrink_to_fit() noexcept
    {
        return success{};
    }

    constexpr void swap(basic_static_string& other) noexcept
    {
        range::swap(m_data.ptr, other.m_data.ptr, N + 1);
        vx::swap(m_data.size, other.m_data.size);
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
        return m_data.size == N;
    }

    constexpr size_type size() const noexcept
    {
        return m_data.size;
    }

    constexpr size_type length() const noexcept
    {
        return size();
    }

    constexpr size_type size_bytes() const noexcept
    {
        return size() * sizeof(T);
    }

    static constexpr size_type max_size() noexcept
    {
        return N;
    }

    static constexpr size_type capacity() noexcept
    {
        return N;
    }

    //=========================================================================
    // reserve
    //=========================================================================

    constexpr success reserve(size_type new_capacity) noexcept
    {
        VX_RET_ERR_IF(new_capacity > max_size(), err::size_error);
        return success{};
    }

    //=========================================================================
    // resize
    //=========================================================================

    constexpr success resize(size_type new_size, const T c = T())
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        if (new_size <= size)
        {
            const size_type shrink_count = size - new_size;
            pointer end_ptr = ptr + new_size;
            range::destroy(end_ptr + 1, shrink_count);
            traits_type::assign(*end_ptr, T());
            size = new_size;
            return success{};
        }

        const size_type count = new_size - size;
        return append_n<construct_method::from_char_count>(count, c);
    }

    //=========================================================================
    // push back
    //=========================================================================

private:

    template <bool Reserved>
    constexpr success push_back_impl(const T c)
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        VX_IF_CONSTEXPR (Reserved)
        {
            VX_ASSERT(size < N);
        }
        else
        {
            VX_RET_ERR_IF(size >= N, err::size_error);
        }

        T* const dst = ptr + size;
        mem::construct_in_place_maybe_trivial(dst);
        traits_type::assign(dst[0], c);
        traits_type::assign(dst[1], T());
        ++size;
        return success{};
    }

public:

    constexpr success push_back(const T c)
    {
        return push_back_impl<false>(c);
    }

    constexpr success push_back_capacity(const T c)
    {
        return push_back_impl<true>(c);
    }

    //=========================================================================
    // erase
    //=========================================================================

private:

    constexpr T* erase_n(T* pos, size_type count)
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type new_size = size - count;
        const size_type tail_count = size - off - count;

        // Move the tail plus the null terminator
        // old end: ptr[size] == '\0'
        // new end: ptr[new_size] must become '\0'
        _char_traits_priv::move_batch(ptr + off, ptr + off + count, tail_count + 1);
        range::destroy(ptr + new_size, count);

        size = new_size;
        return pos;
    }

public:

    constexpr expected<pointer, error> erase(size_type off = 0, size_type count = npos)
    {
        VX_RET_UNEXPECTED_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        auto pos = m_data.ptr + off;
        return erase_n(pos, count);
    }

    constexpr iterator erase(const_iterator pos)
    {
        VX_PRIV_ASSERT_CONTIG_ERASABLE_POSITION(pos);
        auto ptr = const_cast<T*>(pos.ptr());
        return iterator(erase_n(ptr, 1));
    }

    constexpr iterator erase(const_iterator first, const_iterator last)
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

    constexpr void pop_back()
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        if (size > 0)
        {
            mem::destroy_in_place(ptr + size);
            --size;
            traits_type::assign(ptr[size], T());
        }
    }

    constexpr void pop_back_capacity()
    {
        VX_ASSERT(m_data.size > 0);
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;
        mem::destroy_in_place(ptr + size);
        --size;
        traits_type::assign(ptr[size], T());
    }

    //=========================================================================
    // copy
    //=========================================================================

    constexpr size_type copy(T* dst, size_type count, size_type off = 0) const
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return 0;
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        traits_type::copy(dst, m_data.ptr + off, count);
        return count;
    }

    constexpr size_t copy_in_range(T* dst, size_t count, size_t off = 0) const
    {
        VX_ASSERT(_char_traits_priv::check_offset(size(), off));
        count = static_cast<size_t>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        traits_type::copy(dst, m_data.ptr + off, count);
        return count;
    }

    //=========================================================================
    // substr
    //=========================================================================

    constexpr basic_static_string substr(size_type off = 0, size_type count = npos) const
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return basic_static_string();
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return basic_static_string(m_data.ptr + off, count);
    }

    constexpr basic_static_string substr_in_range(size_type off, size_type count) const
    {
        VX_ASSERT(_char_traits_priv::check_offset(size(), off));
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return basic_static_string(m_data.ptr + off, count);
    }

    //=========================================================================
    // view
    //=========================================================================

    constexpr basic_string_view<T> view(size_type off = 0, size_type count = npos) const
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return basic_string_view<T>();
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return basic_string_view<T>(m_data.ptr + off, count);
    }

    constexpr basic_string_view<T> view_in_range(size_type off, size_type count) const noexcept
    {
        VX_ASSERT(_char_traits_priv::check_offset(size(), off));
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return basic_string_view<T>(m_data.ptr + off, count);
    }

    //=========================================================================
    // replace
    //=========================================================================

private:

    constexpr void replace_n_shift(T* pos, size_type in_count, size_type out_count)
    {
        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        if (in_count > out_count)
        {
            const size_type diff = in_count - out_count;
            const pointer back = ptr + size + 1;
            range::construct_maybe_trivial(back, diff);

            const size_type tail_count = static_cast<size_type>(back - (pos + out_count));
            _char_traits_priv::move_batch(pos + in_count, pos + out_count, tail_count);

            size += diff;
        }
        else if (in_count < out_count)
        {
            const size_type diff = out_count - in_count;
            const pointer back = ptr + size + 1;
            const size_type tail_count = static_cast<size_type>(back - (pos + out_count));
            _char_traits_priv::move_batch(pos + in_count, pos + out_count, tail_count);

            range::destroy(back - diff, diff);
            size -= diff;
        }
    }

    // overlap-safe for from_pointer: source may alias the shifted region
    constexpr void replace_n_pointer_safe(T* pos, size_type in_count, size_type out_count, const T* src)
    {
        if (in_count == 0)
        {
            replace_n_shift(pos, in_count, out_count);
            return;
        }

        auto& ptr = m_data.ptr;
        auto& size = m_data.size;

        const T* const old_terminator = ptr + size;
        const T* const shift_start = pos + out_count;
        const difference_type diff =
            static_cast<difference_type>(in_count) - static_cast<difference_type>(out_count);

        size_type unshifted;
        if (src + in_count <= shift_start || src > old_terminator)
        {
            unshifted = in_count;
        }
        else if (shift_start <= src)
        {
            unshifted = 0;
        }
        else
        {
            unshifted = static_cast<size_type>(shift_start - src);
        }

        replace_n_shift(pos, in_count, out_count);

        // order load-bearing -- see the dynamic string's version for why
        _char_traits_priv::move_batch(pos, src, unshifted);
        _char_traits_priv::move_batch(pos + unshifted, src + unshifted + diff, in_count - unshifted);
    }

    template <construct_method M, typename... Args>
    constexpr success replace_n(T* pos, size_type in_count, size_type out_count, Args&&... args)
    {
        auto& size = m_data.size;

        VX_ASSERT(out_count <= size);
        const size_type base = size - out_count;

        // fixed N is the ceiling here, not max_size()/allocator headroom
        VX_RET_ERR_IF(in_count > N - base, err::size_error);

        VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            replace_n_pointer_safe(pos, in_count, out_count, std::forward<Args>(args)...);
        }
        else
        {
            replace_n_shift(pos, in_count, out_count);

            VX_IF_CONSTEXPR (M == construct_method::from_char_count)
            {
                traits_type::assign(pos, in_count, std::forward<Args>(args)...);
            }
            else
            {
                VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
                traits_type::copy_range(pos, std::forward<Args>(args)...);
            }
        }

        return success{};
    }

public:

    constexpr success replace(size_type off, size_type count, const basic_static_string& other)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, other.size(), count, other.data());
    }

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr success replace(size_type off, size_type count, const basic_static_string<M, T>& other)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, other.size(), count, other.data());
    }

    //=========================================================================

    template <size_t M, VX_REQUIRES(M <= N)>
    constexpr success replace(size_type off, size_type count, const basic_static_string<M, T>& other, size_type other_off, size_type count2 = npos)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), other_off), err::out_of_range);

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count2));
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, other.data() + other_off);
    }

    constexpr success replace(size_type off, size_type count, const basic_static_string& other, size_type other_off, size_type count2 = npos)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), other_off), err::out_of_range);

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count2));
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, other.data() + other_off);
    }

    //=========================================================================

    constexpr success replace(size_type off, size_type count, size_type count2, const T c)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return replace_n<construct_method::from_char_count>(m_data.ptr + off, count2, count, c);
    }

    //=========================================================================

    constexpr success replace(size_type off, size_type count, const T* const s)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type count2 = static_cast<size_type>(traits_type::length(s));
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, s);
    }

    constexpr success replace(size_type off, size_type count, const T* const s, size_type count2)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, s);
    }

    //=========================================================================

    constexpr success replace(size_type off, size_type count, std::initializer_list<T> init)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type count2 = static_cast<size_type>(init.size());
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr success replace(size_type off, size_type count, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            const size_type count2 = static_cast<size_type>(std::distance(first, last));
            return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, first.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            const size_type count2 = static_cast<size_type>(std::distance(first, last));
            return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, first);
        }
        else
        {
            const auto tmp = basic_static_string::create(first, last);
            VX_RET_ERR_IF(!tmp, tmp.error());
            return replace_n<construct_method::from_pointer>(m_data.ptr + off, tmp.value().size(), count, tmp.value().data());
        }
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success replace(size_type off, size_type count, const S& other)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type count2 = static_cast<size_type>(other.size());
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success replace(size_type off, size_type count, const S& other, size_type other_off, size_type count2 = npos)
    {
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(size(), off), err::out_of_range);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), other_off), err::out_of_range);

        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count2));
        return replace_n<construct_method::from_pointer>(m_data.ptr + off, count2, count, other.data() + other_off);
    }

    //=========================================================================
    //=========================================================================

    constexpr success replace(const_iterator first, const_iterator last, const basic_static_string& other)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        return replace_n<construct_method::from_pointer>(const_cast<T*>(first.ptr()), other.size(), count, other.data());
    }

    constexpr success replace(const_iterator first, const_iterator last, const basic_static_string& other, size_type other_off, size_type count2 = npos)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), other_off), err::out_of_range);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), other_off, count2));
        return replace_n<construct_method::from_pointer>(const_cast<T*>(first.ptr()), count2, count, other.data() + other_off);
    }

    //=========================================================================

    constexpr success replace(const_iterator first, const_iterator last, size_type count2, const T c)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        return replace_n<construct_method::from_char_count>(const_cast<T*>(first.ptr()), count2, count, c);
    }

    //=========================================================================

    constexpr success replace(const_iterator first, const_iterator last, const T* const s)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(traits_type::length(s));
        return replace_n<construct_method::from_pointer>(const_cast<T*>(first.ptr()), count2, count, s);
    }

    constexpr success replace(const_iterator first, const_iterator last, const T* const s, size_type count2)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        return replace_n<construct_method::from_pointer>(const_cast<T*>(first.ptr()), count2, count, s);
    }

    //=========================================================================

    constexpr success replace(const_iterator first, const_iterator last, std::initializer_list<T> init)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(init.size());
        return replace_n<construct_method::from_pointer>(const_cast<T*>(first.ptr()), count2, count, init.begin());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    constexpr success replace(const_iterator first, const_iterator last, IT first2, IT last2)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        T* pos = const_cast<T*>(first.ptr());

        VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator_of<IT, T>::value)
        {
            const size_type count2 = static_cast<size_type>(std::distance(first2, last2));
            return replace_n<construct_method::from_pointer>(pos, count2, count, first2.ptr());
        }
        else VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
        {
            const size_type count2 = static_cast<size_type>(std::distance(first2, last2));
            return replace_n<construct_method::from_pointer>(pos, count2, count, first2);
        }
        else
        {
            const auto tmp = basic_static_string::create(first2, last2);
            VX_RET_ERR_IF(!tmp, tmp.error());
            return replace_n<construct_method::from_pointer>(pos, tmp.value().size(), count, tmp.value().data());
        }
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success replace(const_iterator first, const_iterator last, const S& other)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(other.size());
        return replace_n<construct_method::from_pointer>(const_cast<T*>(first.ptr()), count2, count, other.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr success replace(const_iterator first, const_iterator last, const S& other, size_type t_off, size_type count2 = npos)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last);
        VX_RET_ERR_IF(!_char_traits_priv::check_offset(other.size(), t_off), err::out_of_range);

        const size_type count = static_cast<size_type>(std::distance(first, last));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), t_off, count2));
        return replace_n<construct_method::from_pointer>(const_cast<T*>(first.ptr()), count2, count, other.data() + t_off);
    }

    //=========================================================================
    // searching
    //=========================================================================

    constexpr bool contains(const basic_static_string& other) const noexcept
    {
        return find(other) != npos;
    }

    constexpr bool contains(const T c) const noexcept
    {
        return find(c) != npos;
    }

    constexpr bool contains(const T* const s) const noexcept
    {
        return find(s) != npos;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr bool contains(const S& other) const noexcept
    {
        return find(other) != npos;
    }

    //=========================================================================

    constexpr size_type find(const basic_static_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    constexpr size_type find(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_ch<traits_type>(
            m_data.ptr, m_data.size, off, c));
    }

    constexpr size_type find(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find<traits_type>(
            m_data.ptr, m_data.size, off, s, s_len));
    }

    constexpr size_type find(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find<traits_type>(
            m_data.ptr, m_data.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr size_type find(const S& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    //=========================================================================

    constexpr size_type rfind(const basic_static_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    constexpr size_type rfind(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind_ch<traits_type>(
            m_data.ptr, m_data.size, off, c));
    }

    constexpr size_type rfind(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_rfind<traits_type>(
            m_data.ptr, m_data.size, off, s, s_len));
    }

    constexpr size_type rfind(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind<traits_type>(
            m_data.ptr, m_data.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr size_type rfind(const S& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    //=========================================================================

    constexpr size_type find_first_of(const basic_static_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_of<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    constexpr size_type find_first_of(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_ch<traits_type>(
            m_data.ptr, m_data.size, off, c));
    }

    constexpr size_type find_first_of(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find_first_of<traits_type>(
            m_data.ptr, m_data.size, off, s, s_len));
    }

    constexpr size_type find_first_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_of<traits_type>(
            m_data.ptr, m_data.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr size_type find_first_of(const S& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_of<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    //=========================================================================

    constexpr size_type find_last_of(const basic_static_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_of<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    constexpr size_type find_last_of(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind_ch<traits_type>(
            m_data.ptr, m_data.size, off, c));
    }

    constexpr size_type find_last_of(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find_last_of<traits_type>(
            m_data.ptr, m_data.size, off, s, s_len));
    }

    constexpr size_type find_last_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_of<traits_type>(
            m_data.ptr, m_data.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr size_type find_last_of(const S& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_of<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    //=========================================================================

    constexpr size_type find_first_not_of(const basic_static_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_not_of<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    constexpr size_type find_first_not_of(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_not_ch<traits_type>(
            m_data.ptr, m_data.size, off, c));
    }

    constexpr size_type find_first_not_of(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find_first_not_of<traits_type>(
            m_data.ptr, m_data.size, off, s, s_len));
    }

    constexpr size_type find_first_not_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_not_of<traits_type>(
            m_data.ptr, m_data.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr size_type find_first_not_of(const S& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_first_not_of<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    //=========================================================================

    constexpr size_type find_last_not_of(const basic_static_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_not_of<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    constexpr size_type find_last_not_of(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_rfind_not_ch<traits_type>(
            m_data.ptr, m_data.size, off, c));
    }

    constexpr size_type find_last_not_of(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_char_traits_priv::traits_find_last_not_of<traits_type>(
            m_data.ptr, m_data.size, off, s, s_len));
    }

    constexpr size_type find_last_not_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_not_of<traits_type>(
            m_data.ptr, m_data.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr size_type find_last_not_of(const S& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_char_traits_priv::traits_find_last_not_of<traits_type>(
            m_data.ptr, m_data.size, off, other.data(), other.size()));
    }

    //=========================================================================
    // comparison
    //=========================================================================

    constexpr int compare(const basic_static_string& other) const noexcept
    {
        return _char_traits_priv::traits_compare<traits_type>(
            m_data.ptr, m_data.size,
            other.data(), other.size());
    }

    constexpr int compare(size_type off, size_type count, const basic_static_string& other) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return 1;
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data.ptr + off, count,
            other.data(), other.size());
    }

    constexpr int compare(size_type off1, size_type count1, const basic_static_string& other, size_type off2, size_type count2 = npos) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off1) || !_char_traits_priv::check_offset(other.size(), off2))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off1, count1));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off2, count2));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data.ptr + off1, count1,
            other.data() + off2, count2);
    }

    //=========================================================================

    constexpr int compare(const T* const s) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data.ptr, m_data.size,
            s, s_len);
    }

    constexpr int compare(size_type off, size_type count, const T* const s) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return 1;
        }
        count = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count));
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return compare(off, count, s, s_len);
    }

    constexpr int compare(size_type off, size_type count1, const T* const s, size_type count2) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off, count1));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data.ptr + off, count1,
            s, count2);
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr int compare(const S& other) const noexcept
    {
        return _char_traits_priv::traits_compare<traits_type>(
            m_data.ptr, m_data.size,
            other.data(), other.size());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    constexpr int compare(size_type off1, size_type count1, const S& other, size_type off2, size_type count2 = npos) const noexcept
    {
        if (!_char_traits_priv::check_offset(size(), off1) || !_char_traits_priv::check_offset(other.size(), off2))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(size(), off1, count1));
        count2 = static_cast<size_type>(_char_traits_priv::clamp_suffix_size(other.size(), off2, count2));
        return _char_traits_priv::traits_compare<traits_type>(
            m_data.ptr + off1, count1,
            other.data() + off2, count2);
    }
};

//=========================================================================
// binary + operators
//=========================================================================

template <size_t N, typename T>
constexpr basic_static_string<N, T> operator+(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs)
{
    basic_static_string<N, T> result(lhs);
    return result.append(rhs);
}

//=========================================================================

template <size_t N, typename T>
constexpr basic_static_string<N, T> operator+(const basic_static_string<N, T>& lhs, const T rhs)
{
    basic_static_string<N, T> result(lhs);
    return result.append(rhs);
}

template <size_t N, typename T>
constexpr basic_static_string<N, T> operator+(const T lhs, const basic_static_string<N, T>& rhs)
{
    basic_static_string<N, T> result(1, lhs);
    return result.append(rhs);
}

//=========================================================================

template <size_t N, typename T>
constexpr basic_static_string<N, T> operator+(const basic_static_string<N, T>& lhs, const T* const rhs)
{
    basic_static_string<N, T> result(lhs);
    return result.append(rhs);
}

template <size_t N, typename T>
constexpr basic_static_string<N, T> operator+(const T* const lhs, const basic_static_string<N, T>& rhs)
{
    basic_static_string<N, T> result(lhs);
    return result.append(rhs);
}

//=========================================================================
// comparison operators
//=========================================================================

template <size_t N, typename T>
constexpr bool operator==(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <size_t N, typename T>
constexpr bool operator==(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <size_t N, typename T>
constexpr bool operator==(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) == 0;
}

template <size_t N, typename T>
constexpr bool operator!=(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <size_t N, typename T>
constexpr bool operator!=(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <size_t N, typename T>
constexpr bool operator!=(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) != 0;
}

template <size_t N, typename T>
constexpr bool operator<(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <size_t N, typename T>
constexpr bool operator<(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <size_t N, typename T>
constexpr bool operator<(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) > 0;
}

template <size_t N, typename T>
constexpr bool operator>(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <size_t N, typename T>
constexpr bool operator>(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <size_t N, typename T>
constexpr bool operator>(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) < 0;
}

template <size_t N, typename T>
constexpr bool operator<=(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <size_t N, typename T>
constexpr bool operator<=(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <size_t N, typename T>
constexpr bool operator<=(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) >= 0;
}

template <size_t N, typename T>
constexpr bool operator>=(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <size_t N, typename T>
constexpr bool operator>=(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <size_t N, typename T>
constexpr bool operator>=(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) <= 0;
}

//=========================================================================
// stream operators
//=========================================================================

template <size_t N, typename T, typename Traits2>
std::basic_istream<T, Traits2>& operator>>(
    std::basic_istream<T, Traits2>& iss,
    basic_static_string<N, T>& s)
{
    std::string is;
    iss >> is;
    s = std::move(is);
    return iss;
}

template <size_t N, typename T, typename Traits2>
std::basic_ostream<T, Traits2>& operator<<(
    std::basic_ostream<T, Traits2>& oss,
    const basic_static_string<N, T>& s)
{
    std::string os(s.data(), s.size());
    oss << os;
    return oss;
}

} // namespace str

//=========================================================================

template <size_t N>
using static_string = str::basic_static_string<N, char>;

template <size_t N>
using static_wstring = str::basic_static_string<N, wchar_t>;

#if VX_HAVE_STD_CHAR8_T
template <size_t N>
using static_u8string = str::basic_static_string<N, char8_t>;
#endif // VX_HAVE_STD_CHAR8_T

template <size_t N>
using static_u16string = str::basic_static_string<N, char16_t>;

template <size_t N>
using static_u32string = str::basic_static_string<N, char32_t>;

} // namespace vx

//=========================================================================
// hashing
//=========================================================================

namespace vx {

template <typename T>
struct hash;

template <size_t N, typename T>
struct hash<str::basic_static_string<N, T>>
{
    size_t operator()(const vx::str::basic_static_string<N, T>& s) const noexcept
    {
        using traits = typename vx::str::basic_static_string<N, T>::traits_type;
        return traits::hash(s.data(), s.size());
    }
};

} // namespace vx

namespace std {

template <size_t N, typename T>
struct hash<vx::str::basic_static_string<N, T>>
{
    size_t operator()(const vx::str::basic_static_string<N, T>& s) const noexcept
    {
        return vx::hash<vx::str::basic_static_string<N, T>>{}(s);
    }
};

} // namespace std
