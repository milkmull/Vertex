#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_hash/hash_common.hpp"
#include "vertex/std/hash.hpp"
#include "vertex/std/_hash/flat_hash_table.hpp"
#include "vertex/std/_hash/node_hash_table.hpp"
#include "vertex/std/_hash/hash_table.hpp"
#include "vertex/std/expected.hpp"

namespace vx {
namespace _hash_priv {

template <typename Backend>
class basic_hash_set : public hash_table_base<basic_hash_set<Backend>, Backend>
{
    using base = hash_table_base<basic_hash_set, Backend>;

public:

    using typename base::key_type;
    using typename base::size_type;

    // constructors (including range / initializer_list) come from the base
    using base::base;
    using base::operator=; // operator=(initializer_list)
    using base::swap;

    basic_hash_set() = default;
    basic_hash_set(const basic_hash_set&) = default;
    basic_hash_set(basic_hash_set&&) noexcept = default;
    basic_hash_set& operator=(const basic_hash_set&) = default;
    basic_hash_set& operator=(basic_hash_set&&) noexcept = default;
    ~basic_hash_set() = default;

    friend void swap(basic_hash_set& a, basic_hash_set& b) noexcept
    {
        a.swap(b);
    }
};

} // namespace _hash_priv
} // namespace vx

namespace vx {

template <typename K, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<K>, typename Sizing = _flat_hash_priv::default_sizing>
using flat_set = _hash_priv::basic_hash_set<_flat_hash_priv::flat_table<_hash_priv::set_policy<K>, Hash, Eq, Alloc, Sizing>>;

template <typename K, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<K>, typename Sizing = _node_hash_priv::default_sizing>
using node_set = _hash_priv::basic_hash_set<_node_hash_priv::node_table<_hash_priv::set_policy<K>, Hash, Eq, Alloc, Sizing>>;

template <typename K, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<K>, typename Sizing = _node_hash_priv::default_sizing>
using hash_set = node_set<K, Hash, Eq, Alloc, Sizing>;

} // namespace vx
