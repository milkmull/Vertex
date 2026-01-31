#pragma once

// These algorithms are part of MSVC library code:
// https://github.com/microsoft/STL/blob/main/stl/src/vector_algorithms.cpp

namespace vx {
namespace _priv {

extern "C" {

const void* __stdcall __std_search_1(
    const void* const _First1,
    const void* const _Last1,
    const void* const _First2,
    const size_t _Count2) noexcept
{
    return _Find_seq::_Search_impl<_Finding::_Find_traits_1, void, void, uint8_t>(_First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_search_2(
    const void* const _First1,
    const void* const _Last1,
    const void* const _First2,
    const size_t _Count2) noexcept
{
    return _Find_seq::_Search_impl<_Finding::_Find_traits_2, _Find_seq::_Find_seq_traits_avx_2, void, uint16_t>(
        _First1,
        _Last1,
        _First2,
        _Count2);
}

const void* __stdcall __std_search_4(
    const void* const _First1,
    const void* const _Last1,
    const void* const _First2,
    const size_t _Count2) noexcept
{
    return _Find_seq::_Search_impl<_Finding::_Find_traits_4, _Find_seq::_Find_seq_traits_avx_4, _Find_seq::_Find_seq_traits_sse_4, uint32_t>(_First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_search_8(
    const void* const _First1,
    const void* const _Last1,
    const void* const _First2,
    const size_t _Count2) noexcept
{
    return _Find_seq::_Search_impl<_Finding::_Find_traits_8, _Find_seq::_Find_seq_traits_avx_8, _Find_seq::_Find_seq_traits_sse_8, uint64_t>(_First1, _Last1, _First2, _Count2);
}


const void* __stdcall __std_find_end_1(
    const void* const _First1,
    const void* const _Last1,
    const void* const _First2,
    const size_t _Count2) noexcept
{
    return _Find_seq::_Find_end_impl<_Finding::_Find_traits_1, _Find_seq::_Find_seq_traits_avx_1, void, uint8_t>(
        _First1,
        _Last1,
        _First2,
        _Count2);
}

const void* __stdcall __std_find_end_2(
    const void* const _First1,
    const void* const _Last1,
    const void* const _First2,
    const size_t _Count2) noexcept
{
    return _Find_seq::_Find_end_impl<_Finding::_Find_traits_2, _Find_seq::_Find_seq_traits_avx_2, void, uint16_t>(
        _First1,
        _Last1,
        _First2,
        _Count2);
}

const void* __stdcall __std_find_end_4(
    const void* const _First1,
    const void* const _Last1,
    const void* const _First2,
    const size_t _Count2) noexcept
{
    return _Find_seq::_Find_end_impl<_Finding::_Find_traits_4, _Find_seq::_Find_seq_traits_avx_4, _Find_seq::_Find_seq_traits_sse_4, uint32_t>(_First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_find_end_8(
    const void* const _First1,
    const void* const _Last1,
    const void* const _First2,
    const size_t _Count2) noexcept
{
    return _Find_seq::_Find_end_impl<_Finding::_Find_traits_8, _Find_seq::_Find_seq_traits_avx_8, _Find_seq::_Find_seq_traits_sse_8, uint64_t>(_First1, _Last1, _First2, _Count2);
}

} // extern "C"

} // namespace _priv
} // namespace vx
