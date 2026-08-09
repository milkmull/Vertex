#pragma once

#include <cstdint>
#include <forward_list>
#include <initializer_list>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include "vertex/config/language_config.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"

//#define VX_SINGLE_LINKED_LIST_DISABLE_MAX_SIZE_CHECK 1

namespace vx {

using X = std::forward_list<int>;

namespace _single_linked_list_priv {

template <typename T>
struct node_type
{
    using node_ptr = node_type*;
    using value_type = T;

    node_ptr next;
    value_type value;

    nnodnode_typee_typeode() = default;
    node_type(const node_type&) = delete;
    node_type& operator=(const node_type&) = delete;

    template <typename Allocator>
    static void free(node_ptr node) noexcept
    {
        mem::destroy_in_place(node->next);
        mem::destroy_in_place(std::addressof(node->value));
        Allocator::deallocate(node, 1);
    }
};

template <typename T>
struct value
{
    using node = node_type<T>;
    using node_ptr = node_type*;

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    value() noexcept : head()
    {}

    // return pointer to the "before begin" pseudo node
    node_ptr before_head() const noexcept
    {
        return &head;
    }

    node_ptr head;

public:

    // Merge the sorted ranges (_BFirst1, _BMid] and (_BMid, _BLast)
    // Returns one before the new logical end of the range.
    template <typename Pred>
    static node_ptr inplace_merge(node_ptr _BFirst1, const node_ptr _BMid, const node_ptr _BLast, Pred _Pred)
};

} // namespace _single_linked_list_priv

template <typename T, typename Allocator = mem::default_allocator<T>>
class single_linked_list
{
public:

    //=========================================================================
    // member types
    //=========================================================================

    using value_type = T;
    using allocator_type = Allocator;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    //using iterator = _priv::pointer_iterator<vector, T>;
    //using const_iterator = _priv::pointer_iterator<vector, const T>;
    //using reverse_iterator = _priv::reverse_pointer_iterator<iterator>;
    //using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

private:
};

//=========================================================================
// comparison
//=========================================================================

} // namespace vx
