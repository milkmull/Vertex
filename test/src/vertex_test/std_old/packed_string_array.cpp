#include "vertex_test/test.hpp"
#include "vertex/std/packed_string_array.hpp"

//==============================================================================

using vx::str::packed_string_array;

//==============================================================================

VX_TEST_CASE(test_default_constructed)
{
    packed_string_array<char> psa;

    VX_CHECK(psa.empty());
    VX_CHECK(psa.size() == 0);
    VX_CHECK(psa.data() == nullptr);
    VX_CHECK(!static_cast<bool>(psa));
}

//==============================================================================

VX_TEST_CASE(test_create_basic)
{
    const char* src[] = { "hello", "world", "foo" };
    const size_t count = sizeof(src) / sizeof(src[0]);

    auto psa = packed_string_array<char>::create(src, count);

    VX_CHECK(static_cast<bool>(psa));
    VX_CHECK(!psa.empty());
    VX_CHECK(psa.size() == count);

    VX_CHECK(std::strcmp(psa[0], "hello") == 0);
    VX_CHECK(std::strcmp(psa[1], "world") == 0);
    VX_CHECK(std::strcmp(psa[2], "foo") == 0);
}

//==============================================================================

VX_TEST_CASE(test_create_single_string)
{
    const char* src[] = { "single" };

    auto psa = packed_string_array<char>::create(src, 1);

    VX_CHECK(static_cast<bool>(psa));
    VX_CHECK(psa.size() == 1);
    VX_CHECK(std::strcmp(psa[0], "single") == 0);
}

//==============================================================================

VX_TEST_CASE(test_create_contains_empty_strings)
{
    const char* src[] = { "", "non_empty", "" };
    const size_t count = sizeof(src) / sizeof(src[0]);

    auto psa = packed_string_array<char>::create(src, count);

    VX_CHECK(static_cast<bool>(psa));
    VX_CHECK(psa.size() == count);

    VX_CHECK(std::strcmp(psa[0], "") == 0);
    VX_CHECK(std::strcmp(psa[1], "non_empty") == 0);
    VX_CHECK(std::strcmp(psa[2], "") == 0);
}

//==============================================================================

VX_TEST_CASE(test_pointer_table_null_terminated)
{
    const char* src[] = { "a", "b", "c", "d" };
    const size_t count = sizeof(src) / sizeof(src[0]);

    auto psa = packed_string_array<char>::create(src, count);

    VX_CHECK(psa.data()[count] == nullptr);
}

//==============================================================================

VX_TEST_CASE(test_pointers_are_independent_copies)
{
    // The strings stored in the packed array should not alias the
    // original source pointers; they must be independent copies.
    char buf0[] = "mutable0";
    char buf1[] = "mutable1";
    const char* src[] = { buf0, buf1 };

    auto psa = packed_string_array<char>::create(src, 2);

    VX_CHECK(psa[0] != buf0);
    VX_CHECK(psa[1] != buf1);

    VX_CHECK(std::strcmp(psa[0], "mutable0") == 0);
    VX_CHECK(std::strcmp(psa[1], "mutable1") == 0);

    // Mutating the source buffer should not affect the packed copy
    buf0[0] = 'X';
    VX_CHECK(std::strcmp(psa[0], "mutable0") == 0);
}

//==============================================================================

VX_TEST_CASE(test_strings_stored_contiguously)
{
    // Each string (including its null terminator) should be laid out
    // back-to-back in the packed storage region, so consecutive string
    // pointers should differ by exactly (length + 1).
    const char* src[] = { "ab", "cde" };

    auto psa = packed_string_array<char>::create(src, 2);

    const ptrdiff_t diff = psa[1] - psa[0];
    VX_CHECK(diff == static_cast<ptrdiff_t>(std::strlen("ab") + 1));
}

//==============================================================================

VX_TEST_CASE(test_move_constructor)
{
    const char* src[] = { "move", "me" };

    auto psa = packed_string_array<char>::create(src, 2);
    auto moved = std::move(psa);

    // Moved-to object owns the data
    VX_CHECK(static_cast<bool>(moved));
    VX_CHECK(moved.size() == 2);
    VX_CHECK(std::strcmp(moved[0], "move") == 0);
    VX_CHECK(std::strcmp(moved[1], "me") == 0);

    // Moved-from object is left empty
    VX_CHECK(!static_cast<bool>(psa));
    VX_CHECK(psa.empty());
    VX_CHECK(psa.size() == 0);
    VX_CHECK(psa.data() == nullptr);
}

//==============================================================================

VX_TEST_CASE(test_move_assignment)
{
    const char* src_a[] = { "aaa" };
    const char* src_b[] = { "bbb", "ccc" };

    auto a = packed_string_array<char>::create(src_a, 1);
    auto b = packed_string_array<char>::create(src_b, 2);

    a = std::move(b);

    VX_CHECK(a.size() == 2);
    VX_CHECK(std::strcmp(a[0], "bbb") == 0);
    VX_CHECK(std::strcmp(a[1], "ccc") == 0);

    VX_CHECK(!static_cast<bool>(b));
    VX_CHECK(b.size() == 0);
    VX_CHECK(b.data() == nullptr);
}

//==============================================================================

VX_TEST_CASE(test_move_assignment_self)
{
    const char* src[] = { "self", "assign" };
    auto psa = packed_string_array<char>::create(src, 2);

    auto* self = &psa;
    psa = std::move(*self);

    // Should remain valid and unchanged after self-move-assignment
    VX_CHECK(static_cast<bool>(psa));
    VX_CHECK(psa.size() == 2);
    VX_CHECK(std::strcmp(psa[0], "self") == 0);
    VX_CHECK(std::strcmp(psa[1], "assign") == 0);
}

//==============================================================================

VX_TEST_CASE(test_reset)
{
    const char* src[] = { "x", "y", "z" };
    auto psa = packed_string_array<char>::create(src, 3);

    VX_CHECK(static_cast<bool>(psa));

    psa.reset();

    VX_CHECK(!static_cast<bool>(psa));
    VX_CHECK(psa.empty());
    VX_CHECK(psa.size() == 0);
    VX_CHECK(psa.data() == nullptr);
}

//==============================================================================

VX_TEST_CASE(test_reset_idempotent)
{
    packed_string_array<char> psa;

    // Calling reset() on an already-empty array should be a safe no-op
    psa.reset();
    psa.reset();

    VX_CHECK(!static_cast<bool>(psa));
    VX_CHECK(psa.size() == 0);
}

//==============================================================================

VX_TEST_CASE(test_destructor_frees_memory)
{
    // Not directly observable without instrumentation, but this at least
    // exercises construction + destruction of a non-trivial array under
    // sanitizers / leak checkers.
    {
        const char* src[] = { "one", "two", "three", "four", "five" };
        auto psa = packed_string_array<char>::create(src, 5);
        VX_CHECK(psa.size() == 5);
    }

    VX_CHECK(true);
}

//==============================================================================

VX_TEST_CASE(test_data_matches_operator_index)
{
    const char* src[] = { "match0", "match1", "match2" };
    const size_t count = sizeof(src) / sizeof(src[0]);

    auto psa = packed_string_array<char>::create(src, count);

    for (size_t i = 0; i < count; ++i)
    {
        VX_CHECK(psa.data()[i] == psa[i]);
    }
}

//==============================================================================

VX_TEST_CASE(test_wide_char_strings)
{
    const wchar_t* src[] = { L"wide", L"chars", L"here" };
    const size_t count = sizeof(src) / sizeof(src[0]);

    auto psa = packed_string_array<wchar_t>::create(src, count);

    VX_CHECK(static_cast<bool>(psa));
    VX_CHECK(psa.size() == count);

    VX_CHECK(std::wcscmp(psa[0], L"wide") == 0);
    VX_CHECK(std::wcscmp(psa[1], L"chars") == 0);
    VX_CHECK(std::wcscmp(psa[2], L"here") == 0);

    VX_CHECK(psa.data()[count] == nullptr);
}

//==============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
