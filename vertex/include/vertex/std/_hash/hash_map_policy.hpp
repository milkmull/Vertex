#pragma once

// Slot policy for maps. The slot keeps the key private (so user code cannot
// mutate it through an iterator) and the mapped value public.

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/memory.hpp" // path is a guess

namespace vx {
namespace _hash_priv {

template <typename K, typename V>
class map_slot
{
public:

    // piecewise construction: this is how the front builds a slot from (key, args...)
    template <typename... KA, typename... VA>
    map_slot(std::piecewise_construct_t, std::tuple<KA...> k, std::tuple<VA...> v)
        : map_slot(k, v, std::index_sequence_for<KA...>{}, std::index_sequence_for<VA...>{})
    {}

    map_slot(const map_slot&) = default;
    map_slot(map_slot&&) = default;

    const K& key() const noexcept
    {
        return m_key;
    }

    V value;

    // structured bindings: auto& [k, v] = *it;
    template <size_t I>
    decltype(auto) get() noexcept
    {
        return get_impl(std::integral_constant<size_t, I>{});
    }

    template <size_t I>
    decltype(auto) get() const noexcept
    {
        return get_impl(std::integral_constant<size_t, I>{});
    }

private:

    template <typename KT, typename VT, size_t... KI, size_t... VI>
    map_slot(KT& k, VT& v, std::index_sequence<KI...>, std::index_sequence<VI...>)
        : m_key(std::get<KI>(std::move(k))...)
        , value(std::get<VI>(std::move(v))...)
    {}

    const K& get_impl(std::integral_constant<size_t, 0>) const noexcept
    {
        return m_key;
    }

    V& get_impl(std::integral_constant<size_t, 1>) noexcept
    {
        return value;
    }

    const V& get_impl(std::integral_constant<size_t, 1>) const noexcept
    {
        return value;
    }

    K m_key;
};

template <typename K, typename V>
struct map_policy
{
    using key_type = K;
    using mapped_type = V;
    using slot_type = map_slot<K, V>;

    static constexpr bool mutable_slots = true;

    // true => rehash may memcpy slots and skip destroy
    static constexpr bool trivially_relocatable = std::is_trivially_copyable<K>::value && std::is_trivially_copyable<V>::value;

    static const K& key(const slot_type& s) noexcept
    {
        return s.key();
    }

    template <typename... Args>
    static void construct(slot_type* p, Args&&... args)
    {
        mem::construct_in_place(p, std::forward<Args>(args)...);
    }

    static void destroy(slot_type* p) noexcept
    {
        mem::destroy_in_place(p);
    }

    static void relocate(slot_type* dst, slot_type* src)
    {
        construct(dst, std::move(*src));
        destroy(src);
    }
};

} // namespace _hash_priv
} // namespace vx

namespace std {

template <typename K, typename V>
struct tuple_size<vx::_hash_priv::map_slot<K, V>> : integral_constant<size_t, 2>
{};

template <size_t I, typename K, typename V>
struct tuple_element<I, vx::_hash_priv::map_slot<K, V>>
{
    using type = typename conditional<I == 0, const K, V>::type;
};

} // namespace std
