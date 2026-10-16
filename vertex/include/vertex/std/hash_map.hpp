#pragma once

// vx::unordered_map (open addressing) and vx::chain_unordered_map (separate
// chaining, pointer-stable). Elements are map_slot<K, V>: key() is const,
// `value` is public, and structured bindings work: auto& [k, v] = *it;

#include <functional>

#include "vertex/std/memory.hpp" // mem::default_allocator, path is a guess

#include "hash_chain_table.hpp"
#include "hash_common.hpp"
#include "hash_front.hpp"
#include "hash_functions.hpp"
#include "hash_map_policy.hpp"
#include "hash_open_table.hpp" // the open_table file, rename to match
#include "hash_traits.hpp"

namespace vx {

// The allocator is typed on the slot, like the set's is typed on K.
template <typename K, typename V, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<_hash_priv::map_slot<K, V>>, typename Traits = _hash_priv::default_table_traits>
using unordered_map = _hash_priv::hash_table<_hash_priv::open_table<_hash_priv::map_policy<K, V>, Hash, Eq, Alloc, Traits>>;

template <typename K, typename V, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<_hash_priv::map_slot<K, V>>, typename Traits = _hash_priv::default_chain_traits>
using chain_unordered_map = _hash_priv::hash_table<_hash_priv::chain_table<_hash_priv::map_policy<K, V>, Hash, Eq, Alloc, Traits>>;

} // namespace vx
