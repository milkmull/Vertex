#pragma once

#include <initializer_list>
#include <ratio>

#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"

namespace vx {
namespace _priv {

#if 0

constexpr bool _Is_pow_2(const size_t _Value) noexcept
{
    return _Value != 0 && (_Value & (_Value - 1)) == 0;
}

#if defined(_M_IX86) || defined(_M_X64)
_INLINE_VAR constexpr size_t _Big_allocation_threshold = 4096;
_INLINE_VAR constexpr size_t _Big_allocation_alignment = 32;

// Big allocation alignment should at least match vector register alignment
_STL_INTERNAL_STATIC_ASSERT(2 * sizeof(void*) <= _Big_allocation_alignment);

// Big allocation alignment must be a power of two
_STL_INTERNAL_STATIC_ASSERT(_Is_pow_2(_Big_allocation_alignment));

    #ifdef _DEBUG
_INLINE_VAR constexpr size_t _Non_user_size = 2 * sizeof(void*) + _Big_allocation_alignment - 1;
    #else  // ^^^ defined(_DEBUG) / !defined(_DEBUG) vvv
_INLINE_VAR constexpr size_t _Non_user_size = sizeof(void*) + _Big_allocation_alignment - 1;
    #endif // ^^^ !defined(_DEBUG) ^^^

    #ifdef _WIN64
_INLINE_VAR constexpr size_t _Big_allocation_sentinel = 0xFAFAFAFAFAFAFAFAULL;
    #else  // ^^^ defined(_WIN64) / !defined(_WIN64) vvv
_INLINE_VAR constexpr size_t _Big_allocation_sentinel = 0xFAFAFAFAUL;
    #endif // ^^^ !defined(_WIN64) ^^^

template <class _Traits>
__declspec(allocator) void* _Allocate_manually_vector_aligned(const size_t _Bytes)
{
    // allocate _Bytes manually aligned to at least _Big_allocation_alignment
    const size_t _Block_size = _Non_user_size + _Bytes;
    if (_Block_size <= _Bytes)
    {
        _Throw_bad_array_new_length(); // add overflow
    }

    const uintptr_t _Ptr_container = reinterpret_cast<uintptr_t>(_Traits::_Allocate(_Block_size));
    _STL_VERIFY(_Ptr_container != 0, "invalid argument"); // validate even in release since we're doing p[-1]
    void* const _Ptr = reinterpret_cast<void*>((_Ptr_container + _Non_user_size) & ~(_Big_allocation_alignment - 1));
    static_cast<uintptr_t*>(_Ptr)[-1] = _Ptr_container;

#ifdef _DEBUG
    static_cast<uintptr_t*>(_Ptr)[-2] = _Big_allocation_sentinel;
#endif // defined(_DEBUG)
    return _Ptr;
}

inline void _Adjust_manually_vector_aligned(void*& _Ptr, size_t& _Bytes)
{
    // adjust parameters from _Allocate_manually_vector_aligned to pass to operator delete
    _Bytes += _Non_user_size;

    const uintptr_t* const _Ptr_user = static_cast<uintptr_t*>(_Ptr);
    const uintptr_t _Ptr_container = _Ptr_user[-1];

    // If the following asserts, it likely means that we are performing
    // an aligned delete on memory coming from an unaligned allocation.
    _STL_ASSERT(_Ptr_user[-2] == _Big_allocation_sentinel, "invalid argument");

    // Extra paranoia on aligned allocation/deallocation; ensure _Ptr_container is
    // in range [_Min_back_shift, _Non_user_size]
#ifdef _DEBUG
    constexpr uintptr_t _Min_back_shift = 2 * sizeof(void*);
#else  // ^^^ defined(_DEBUG) / !defined(_DEBUG) vvv
    constexpr uintptr_t _Min_back_shift = sizeof(void*);
#endif // ^^^ !defined(_DEBUG) ^^^
    const uintptr_t _Back_shift = reinterpret_cast<uintptr_t>(_Ptr) - _Ptr_container;
    _STL_VERIFY(_Back_shift >= _Min_back_shift && _Back_shift <= _Non_user_size, "invalid argument");
    _Ptr = reinterpret_cast<void*>(_Ptr_container);
}
#endif // defined(_M_IX86) || defined(_M_X64)

template <size_t _Align, class _Traits = _Default_allocate_traits>
__declspec(allocator) _CONSTEXPR20 void* _Allocate(const size_t _Bytes)
{
    // allocate _Bytes
    if (_Bytes == 0)
    {
        return nullptr;
    }

#if _HAS_CXX20 // TRANSITION, GH-1532
    if (_STD is_constant_evaluated())
    {
        return _Traits::_Allocate(_Bytes);
    }
#endif // _HAS_CXX20

#ifdef __cpp_aligned_new
    if constexpr (_Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
    {
        size_t _Passed_align = _Align;
    #if defined(_M_IX86) || defined(_M_X64)
        if (_Bytes >= _Big_allocation_threshold)
        {
            // boost the alignment of big allocations to help autovectorization
            _Passed_align = (_STD max)(_Align, _Big_allocation_alignment);
        }
    #endif // defined(_M_IX86) || defined(_M_X64)
        return _Traits::_Allocate_aligned(_Bytes, _Passed_align);
    }
    else
#endif // defined(__cpp_aligned_new)
    {
#if defined(_M_IX86) || defined(_M_X64)
        if (_Bytes >= _Big_allocation_threshold)
        {
            // boost the alignment of big allocations to help autovectorization
            return _Allocate_manually_vector_aligned<_Traits>(_Bytes);
        }
#endif // defined(_M_IX86) || defined(_M_X64)
        return _Traits::_Allocate(_Bytes);
    }
}

template <size_t _Align>
_CONSTEXPR20 void _Deallocate(void* _Ptr, size_t _Bytes) noexcept
{
    // deallocate storage allocated by _Allocate
#if _HAS_CXX20 // TRANSITION, GH-1532
    if (_STD is_constant_evaluated())
    {
        ::operator delete(_Ptr);
        return;
    }
#endif // _HAS_CXX20

#ifdef __cpp_aligned_new
    if constexpr (_Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
    {
        size_t _Passed_align = _Align;
    #if defined(_M_IX86) || defined(_M_X64)
        if (_Bytes >= _Big_allocation_threshold)
        {
            // boost the alignment of big allocations to help autovectorization
            _Passed_align = (_STD max)(_Align, _Big_allocation_alignment);
        }
    #endif // defined(_M_IX86) || defined(_M_X64)
        ::operator delete(_Ptr, _Bytes, align_val_t{ _Passed_align });
    }
    else
#endif // defined(__cpp_aligned_new)
    {
#if defined(_M_IX86) || defined(_M_X64)
        if (_Bytes >= _Big_allocation_threshold)
        {
            // boost the alignment of big allocations to help autovectorization
            _Adjust_manually_vector_aligned(_Ptr, _Bytes);
        }
#endif // defined(_M_IX86) || defined(_M_X64)
        ::operator delete(_Ptr, _Bytes);
    }
}

#endif

template <typename T, typename Allocator = default_allocator, typename Growth = std::ratio<3, 2>>
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
        mem::construct_array_in_place(m_buffer.data, count, std::forward<Args>(args)...);
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

            VX_IF_CONSTEXPR(std::is_trivially_copyable<T>::value && std::is_trivially_destructible<T>::value)
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
            m_buffer.capacity = 0;
            m_buffer.size = 0;
            m_buffer.data = nullptr;
        }
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

        VX_IF_CONSTEXPR(std::is_trivially_copyable<T>::value && std::is_trivially_destructible<T>::value)
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
