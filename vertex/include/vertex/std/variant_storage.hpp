#pragma once

#include "vertex/std/aligned_storage.hpp"

namespace vx {

namespace _variant_storage_priv {

//=====================================
// max size/alignment over a type pack
//=====================================

template <typename...>
struct max_sizeof;

template <typename T>
struct max_sizeof<T> : std::integral_constant<size_t, sizeof(T)>
{};

template <typename T, typename... Rest>
struct max_sizeof<T, Rest...> : std::integral_constant<size_t, (sizeof(T) > max_sizeof<Rest...>::value) ? sizeof(T) : max_sizeof<Rest...>::value>
{};

template <typename...>
struct max_alignof;

template <typename T>
struct max_alignof<T> : std::integral_constant<size_t, alignof(T)>
{};

template <typename T, typename... Rest>
struct max_alignof<T, Rest...> : std::integral_constant<size_t, (alignof(T) > max_alignof<Rest...>::value) ? alignof(T) : max_alignof<Rest...>::value>
{};

//=====================================
// pack membership check
//=====================================

template <typename T, typename... Types>
struct pack_contains : std::integral_constant<bool, false>
{};

template <typename T, typename First, typename... Rest>
struct pack_contains<T, First, Rest...> : std::integral_constant<bool, std::is_same<T, First>::value || pack_contains<T, Rest...>::value>
{};

} // namespace _variant_storage_priv

//=====================================
// variant_storage: an aligned_storage sized to fit any of Types...,
// restricted so only members of that pack can be constructed/read.
//=====================================

template <typename... Types>
class variant_storage : public aligned_storage<_variant_storage_priv::max_sizeof<Types...>::value, _variant_storage_priv::max_alignof<Types...>::value>
{
    VX_STATIC_ASSERT_MSG(sizeof...(Types) > 0, "variant_storage requires at least one type");
    using base = aligned_storage<_variant_storage_priv::max_sizeof<Types...>::value, _variant_storage_priv::max_alignof<Types...>::value>;

public:

    variant_storage() noexcept = default;
    ~variant_storage() noexcept = default;

    variant_storage(const variant_storage&) = delete;
    variant_storage(variant_storage&&) noexcept = delete;
    variant_storage& operator=(const variant_storage&) = delete;
    variant_storage& operator=(variant_storage&&) noexcept = delete;

    template <typename T, typename... Args>
    void construct(Args&&... args)
    {
        VX_STATIC_ASSERT_MSG((_variant_storage_priv::pack_contains<T, Types...>::value), "T is not one of variant_storage's Types...");
        base::template construct<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    void destroy() noexcept
    {
        VX_STATIC_ASSERT_MSG((_variant_storage_priv::pack_contains<T, Types...>::value), "T is not one of variant_storage's Types...");
        base::template destroy<T>();
    }

    template <typename T>
    T* ptr() noexcept
    {
        VX_STATIC_ASSERT_MSG((_variant_storage_priv::pack_contains<T, Types...>::value), "T is not one of variant_storage's Types...");
        return base::template ptr<T>();
    }

    template <typename T>
    const T* ptr() const noexcept
    {
        VX_STATIC_ASSERT_MSG((_variant_storage_priv::pack_contains<T, Types...>::value), "T is not one of variant_storage's Types...");
        return base::template ptr<T>();
    }

    template <typename T>
    T& get() noexcept
    {
        VX_STATIC_ASSERT_MSG((_variant_storage_priv::pack_contains<T, Types...>::value), "T is not one of variant_storage's Types...");
        return base::template get<T>();
    }

    template <typename T>
    const T& get() const noexcept
    {
        VX_STATIC_ASSERT_MSG((_variant_storage_priv::pack_contains<T, Types...>::value), "T is not one of variant_storage's Types...");
        return base::template get<T>();
    }
};

} // namespace vx
