#pragma once

namespace vx {
namespace cpu {

//=============================================================================
// memory_order
//=============================================================================

enum class memory_order : int
{
    relaxed = 0,
    consume = 1, // treated as acquire, like every compiler does
    acquire = 2,
    release = 3,
    acq_rel = 4,
    seq_cst = 5
};

//=============================================================================
// properties
//=============================================================================

// consume counts as acquire

constexpr bool has_acquire(memory_order order) noexcept
{
    return order == memory_order::consume || order == memory_order::acquire ||
        order == memory_order::acq_rel || order == memory_order::seq_cst;
}

constexpr bool has_release(memory_order order) noexcept
{
    return order == memory_order::release || order == memory_order::acq_rel ||
        order == memory_order::seq_cst;
}

constexpr bool is_seq_cst(memory_order order) noexcept
{
    return order == memory_order::seq_cst;
}

//=============================================================================
// validity
//=============================================================================

// Loads can't release, stores can't acquire. Read-modify-write operations
// accept every order.

constexpr bool is_valid_load_order(memory_order order) noexcept
{
    return order == memory_order::relaxed || order == memory_order::consume ||
        order == memory_order::acquire || order == memory_order::seq_cst;
}

constexpr bool is_valid_store_order(memory_order order) noexcept
{
    return order == memory_order::relaxed || order == memory_order::release ||
        order == memory_order::seq_cst;
}

constexpr bool is_valid_rmw_order(memory_order order) noexcept
{
    return order == memory_order::relaxed || order == memory_order::consume ||
        order == memory_order::acquire || order == memory_order::release ||
        order == memory_order::acq_rel || order == memory_order::seq_cst;
}

// A failed compare-exchange only loads, so its order must be a valid load order.
// Since C++17 (P0418R2) it may be stronger than the success order.

constexpr bool is_valid_failure_order(memory_order order) noexcept
{
    return is_valid_load_order(order);
}

//=============================================================================
// transformations
//=============================================================================

// consume -> acquire. Backends use this so they never have to handle consume.

constexpr memory_order promote_consume(memory_order order) noexcept
{
    return order == memory_order::consume ? memory_order::acquire : order;
}

// Failure order implied by a single-order compare_exchange (the std rule):
// acq_rel -> acquire, release -> relaxed, anything else unchanged.

constexpr memory_order default_failure_order(memory_order success) noexcept
{
    return success == memory_order::acq_rel ? memory_order::acquire
        : success == memory_order::release  ? memory_order::relaxed
                                            : success;
}

// Smallest single order that provides at least the guarantees of both.
// For backends that take one order per operation (MSVC intrinsics, the locked
// backend), a compare_exchange with separate success/failure orders uses
// combine_order(success, failure).
//
// seq_cst on either side gives seq_cst. Otherwise acquire and release are
// merged, with consume promoted to acquire.

constexpr memory_order combine_order(memory_order a, memory_order b) noexcept
{
    return (a == memory_order::seq_cst || b == memory_order::seq_cst)
        ? memory_order::seq_cst
        : (has_acquire(a) || has_acquire(b))
        ? ((has_release(a) || has_release(b)) ? memory_order::acq_rel
                                              : memory_order::acquire)
        : ((has_release(a) || has_release(b)) ? memory_order::release
                                              : memory_order::relaxed);
}

} // namespace cpu
} // namespace vx
