#pragma once

//=============================================================================
// Null while loop
//=============================================================================

#if defined(_MSC_VER)
    #define VX_NULL_WHILE_LOOP_CONDITION (0, 0)
#else
    #define VX_NULL_WHILE_LOOP_CONDITION (0)
#endif

//=============================================================================
// String and concatenation utilities
//=============================================================================

#define VX_PRIV_STRINGIFY(x) #x
#define VX_STRINGIFY(x)      VX_PRIV_STRINGIFY(x)

#define VX_PRIV_CONCAT(a, b) a##b
#define VX_CONCAT(a, b)      VX_PRIV_CONCAT(a, b)

//=============================================================================
// Bit and index utilities
//=============================================================================

#define VX_BIT(i) (1u << (i))

//=========================================================================
// Parameter Attributes
//=========================================================================

#define VX_UNUSED(x)         (static_cast<void>(x))
#define VX_EMPTY_STATEMENT() VX_UNUSED(0)
