#pragma once

#include "vertex/core/assert.hpp"

// Check if two iterators are within a valid range (first <= last)
#define __VX_ITERATOR_VALID_RANGE(first, last) \
    VX_ASSERT(last >= first)

// Validate that a pointer is non-null
#define __VX_VALIDATE_NON_NULL(ptr) \
    VX_ASSERT((ptr) != nullptr)

// Validate index range for accessing containers
#define __VX_VALIDATE_INDEX(container, index) \
    VX_ASSERT((index) >= 0 && (index) < static_cast<int>((container).size()))