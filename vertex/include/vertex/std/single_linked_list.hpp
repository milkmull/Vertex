#pragma once

#include <cstdint>
#include <forward_list>
#include <initializer_list>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"

//#define VX_SINGLE_LINKED_LIST_DISABLE_MAX_SIZE_CHECK 1

namespace vx {

using X = std::forward_list<int>;

template <typename T, typename Allocator>
class single_linked_list;

namespace _single_linked_list_priv {

//=========================================================================
// node
//=========================================================================

template <typename T>
struct node_type
{
    using node_ptr = node_type*;
    using value_type = T;

    node_ptr next;
    value_type value;

    node_type() = default;
    node_type(const node_type&) = delete;
    node_type& operator=(const node_type&) = delete;

    template <typename Allocator>
    static void free(Allocator& allocator, node_ptr node) noexcept
    {
        mem::destroy_in_place(node->next);
        mem::destroy_in_place(std::addressof(node->next));
        allocator.deallocate(node, 1);
    }
};

//=========================================================================
// list value
//=========================================================================

template <typename T>
struct value
{
    using node = node_type<T>;
    using node_ptr = node*;

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
        return &reinterpret_cast<node&>(const_cast<node_ptr&>(head));
    }

    node_ptr head;

public:

    // Merge two already-sorted ranges:
    //
    //   (b_first1, b_mid]    and    (b_mid, b_last)
    //
    // into one sorted range, in-place.
    //
    // The function returns the node immediately before the new logical end
    // of the merged range (which is normally b_mid).
    template <typename Pred>
    static node_ptr inplace_merge(
        node_ptr b_first1,     // the node immediately BEFORE the first range
        const node_ptr b_mid,  // the last node of the first range
        const node_ptr b_last, // the last node of the second range
        Pred pred)
    {
        // 'first2' points to the first element of the SECOND sorted range.
        //
        // Initially the list looks roughly like:
        //
        //   b_first1 -> [first range ...] -> b_mid
        //                                      |
        //                                      v
        //                                  first2 -> [second range ...] -> b_last
        auto first2 = b_mid->next;

        // Repeatedly find a run of elements from the second range that belongs
        // somewhere inside the first range, then splice that run into place.
        for (;;)
        {
            node_ptr first1;

            // ---------------------------------------------------------------
            // Find the first element in the first range that is "too large"
            // to remain before first2.
            // ---------------------------------------------------------------
            //
            // Because the first range is already sorted, everything before
            // 'first1' is already in the correct position.
            for (;;)
            {
                if (b_first1 == b_mid)
                {
                    // We've reached the end of the first range.
                    //
                    // There is nothing left to compare against, so the
                    // remaining elements from the second range are already
                    // in the correct position.
                    break;
                }

                // first1 is the actual node after b_first1.
                //
                // b_first1 acts as the "node before first1", which is useful
                // because we'll eventually insert something between them.
                first1 = b_first1->next;

                // If first2 should come before first1, then first2 is out of
                // order relative to the first range and needs to be moved.
                if (pred(first2->value, first1->value))
                {
                    // We found the insertion point.
                    //
                    // Everything before first1 is already correctly placed.
                    break;
                }

                // first1 is <= first2 according to the comparator, so first1
                // is already in the correct position.
                //
                // Move forward through the first range.
                b_first1 = first1;
            }

            // ---------------------------------------------------------------
            // Find a consecutive "run" at the beginning of the second range
            // that belongs BEFORE first1.
            // ---------------------------------------------------------------
            //
            // Suppose we have:
            //
            //   first range:   1  4  7  10
            //   second range:  2  3  8  9
            //
            // If first1 points to 4, then [2, 3] belongs before 4.
            //
            // We want to move the whole run [2, 3] at once rather than moving
            // each node individually.
            auto b_run_end = first2;
            node_ptr run_end;

            for (;;)
            {
                // run_end is the node immediately AFTER b_run_end.
                run_end = b_run_end->next;

                if (b_run_end == b_last)
                {
                    // We've reached the end of the second range.
                    //
                    // The entire remaining second range belongs before
                    // first1.
                    break;
                }

                // If run_end should NOT come before first1, we've found the
                // end of the run.
                if (!pred(run_end->value, first1->value))
                {
                    // In other words:
                    //
                    //   [first2 ... b_run_end] = nodes that belong before first1
                    //   [run_end ...]          = nodes that belong after first1
                    //
                    // We will splice the first group into the first range.
                    break;
                }

                // run_end also belongs before first1, so extend the run.
                b_run_end = run_end;
            }

            // ---------------------------------------------------------------
            // Splice the run [first2 ... b_run_end] out of the second range
            // and insert it immediately before first1.
            // ---------------------------------------------------------------
            //
            // Before:
            //
            //   b_first1 -> first1 -> ... -> b_mid
            //                              |
            //                              v
            //                           first2 -> ... -> b_run_end -> run_end
            //
            // After:
            //
            //   b_first1 -> first2 -> ... -> b_run_end -> first1 -> ... -> b_mid
            //                              |
            //                              v
            //                            run_end
            //
            // The three pointer updates below perform that splice.

            // Remove the run from the second range.
            //
            // b_mid used to point to first2. Now it skips over the run.
            b_mid->next = run_end;

            // Make the node before first1 point to the beginning of the run.
            b_first1->next = first2;

            // Make the end of the run point to first1.
            //
            // This inserts the run immediately before first1.
            b_run_end->next = first1;

            // If the run reached the end of the second range, we're done.
            //
            // b_mid->next now points past the entire moved run, so b_mid
            // remains the boundary immediately before the remaining range.
            if (b_run_end == b_last)
            {
                return b_mid;
            }

            // ---------------------------------------------------------------
            // Continue merging.
            // ---------------------------------------------------------------
            //
            // 'first1' was just moved after the run, so it is now in its
            // final position. We don't need to compare it again.
            //
            // Therefore, the next search in the first range starts with
            // first1 as the node BEFORE the next candidate.
            b_first1 = first1;

            // The next unmerged element from the second range is run_end.
            first2 = run_end;
        }
    }
};

//=========================================================================
// insert operation
//=========================================================================

template <typename Allocator>
class insert_after_op
{
private:

    using allocator_type = Allocator;
    using value_type = typename allocator_type::value_type;
    using pointer = value_type*;

    allocator_type& m_allocator;
    pointer m_tail; // Points to the most recently constructed node. If pointer{}, the value of _Head is indeterminate.
                    // _Tail->_Next is not constructed.
    pointer m_head; // Points at the first constructed node.

public:

    explicit insert_after_op(allocator_type& allocator)
        : m_allocator(allocator), m_tail(), m_head()
    {}

    insert_after_op(const insert_after_op&) = delete;
    insert_after_op& operator=(const insert_after_op&) = delete;

    // Build a chain of `count` copies of `val`. Returns false on allocation
    // failure; whatever was built so far is retained in *this and will be
    // freed by the destructor unless commit() moved it out first, or the
    // caller explicitly wants to keep the partial chain (see note below).
    template <typename... Args>
    bool append_n(size_t count, const Args&... args)
    {
        for (; count > 0; --count)
        {
            pointer n = m_allocator.allocate(1);
            if (!n)
            {
                return false; // partial chain stays in m_head/m_tail
            }

            mem::construct_in_place(n->value, args...);
            n->next = nullptr;

            if (!m_tail)
            {
                m_head = m_tail = n;
            }
            else
            {
                m_tail->next = n;
                m_tail = n;
            }
        }

        return true;
    }

    template <typename It, typename Sentinel>
    bool append_range(It first, Sentinel last)
    {
        for (; first != last; ++first)
        {
            pointer n = m_allocator.allocate(1);
            if (!n)
            {
                return false;
            }

            mem::construct_in_place(std::addressof(n->value), *first);
            n->next = nullptr;

            if (!m_tail)
            {
                m_head = m_tail = n;
            }
            else
            {
                m_tail->next = n;
                m_tail = n;
            }
        }

        return true;
    }

    // Splice the built chain in after `after`, and reset *this to empty.
    // Only call this once append_n/append_range returned true.
    pointer attach_after(pointer after) noexcept
    {
        if (!m_tail)
        {
            return after;
        }

        m_tail->next = after->next;
        after->next = m_head;

        pointer result = m_tail;
        m_head = m_tail = nullptr;
        return result;
    }

    // Frees whatever chain is currently held, without attaching it.
    // Called automatically by the destructor; exposed so callers that want
    // to bail out early (e.g. on failure) can do it explicitly and check
    // for it clearly at the call site if they prefer not to rely on scope exit.
    void discard() noexcept
    {
        pointer n = m_head;
        while (n)
        {
            pointer next = n->next;
            value_type::template free<allocator_type>(m_allocator, n);
            n = next;
        }
        m_head = m_tail = nullptr;
    }

    ~insert_after_op()
    {
        discard();
    }
};

//=========================================================================
// types helper
//=========================================================================

template <typename T>
struct list_types
{
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using node = node_type<T>;
    using node_ptr = node*;
};

//=========================================================================
// iterators
//=========================================================================

template <typename T>
class const_iterator
{
    template <typename, typename>
    friend class single_linked_list;

    using types = list_types<T>;
    using node_ptr = typename types::node_ptr;
    node_ptr m_ptr;

public:

    using iterator_category = std::forward_iterator_tag;

    using value_type = typename types::value_type;
    using difference_type = typename types::difference_type;
    using pointer = const value_type*;
    using reference = const value_type&;

public:

    const_iterator() noexcept
        : m_ptr()
    {}

    const_iterator(node_ptr node) noexcept
        : m_ptr(node)
    {}

    reference operator*() const noexcept
    {
        return m_ptr->value;
    }

    pointer operator->() const noexcept
    {
        return &operator*();
    }

    const_iterator& operator++() noexcept
    {
        m_ptr = m_ptr->next;
        return *this;
    }

    const_iterator operator++(int) noexcept
    {
        const_iterator tmp = *this;
        m_ptr = m_ptr->next;
        return tmp;
    }

    bool operator==(const const_iterator& other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const const_iterator& other) const noexcept
    {
        return !(*this == other);
    }
};

//=========================================================================

template <typename T>
class iterator : public const_iterator<T>
{
    template <typename, typename>
    friend class single_linked_list;

    using base = const_iterator<T>;
    using types = list_types<T>;

public:

    using iterator_category = std::forward_iterator_tag;

    using value_type = typename types::value_type;
    using difference_type = typename types::difference_type;
    using pointer = value_type*;
    using reference = value_type&;

    using base::base;

    reference operator*() const noexcept
    {
        return const_cast<reference>(base::operator*());
    }

    pointer operator->() const noexcept
    {
        return &operator*();
    }

    iterator& operator++() noexcept
    {
        base::operator++();
        return *this;
    }

    iterator operator++(int) noexcept
    {
        iterator tmp = *this;
        base::operator++();
        return tmp;
    }
};

} // namespace _single_linked_list_priv

template <typename T, typename Allocator = mem::default_allocator<T>>
class single_linked_list
{
    using types = _single_linked_list_priv::list_types<T>;

public:

    //=========================================================================
    // member types
    //=========================================================================

    using allocator_type = Allocator;
    using value_type = typename types::value_type;
    using pointer = typename types::pointer;
    using const_pointer = typename types::const_pointer;
    using reference = typename types::reference;
    using const_reference = typename types::const_reference;
    using size_type = typename types::size_type;
    using difference_type = typename types::difference_type;

    using iterator = _single_linked_list_priv::iterator<T>;
    using const_iterator = _single_linked_list_priv::const_iterator<T>;

private:

    using node = typename types::node;
    using node_ptr = typename types::node_ptr;

    using list_value = _single_linked_list_priv::value<T>;

    // rebind the value allocator into a node allocator
    using node_allocator = typename mem::rebind_allocator<allocator_type, node>::type;
    using insert_op = _single_linked_list_priv::insert_after_op<node_allocator>;

    // compressed storage: node_allocator + head pointer (or whatever "value"
    // you want compressed alongside it) in one base-optimized object
    _compressed_pair_priv::compressed_pair<node_allocator, list_value> m_storage;

    node_allocator& m_allocator() noexcept
    {
        return m_storage.first();
    }
    const node_allocator& m_allocator() const noexcept
    {
        return m_storage.first();
    }

    list_value& m_data() noexcept
    {
        return m_storage.second;
    }
    const list_value& m_data() const noexcept
    {
        return m_storage.second;
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    single_linked_list() noexcept
        : m_storage(_compressed_pair_priv::zero_then_variadic_args_tag{})
    {}

    explicit single_linked_list(const allocator_type& alloc)
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {}

    //=========================================================================

    explicit single_linked_list(size_type count, const allocator_type& alloc = allocator_type())
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        op.append_n(count);
        op.attach_after(m_data().before_head());
    }

    single_linked_list(size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        op.append_n(count, value);
        op.attach_after(m_data().before_head());
    }

    //=========================================================================

    single_linked_list(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
    }

    //=========================================================================

    single_linked_list(const single_linked_list& other)
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, other.m_allocator())
    {
        //insert_op op(m_allocator());
        //op.append_range(count, value);
        //op.attach_after(m_data().before_head());
    }

    // copy with an explicitly supplied allocator
    single_linked_list(const single_linked_list& other, const allocator_type& alloc)
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
    }

    single_linked_list(single_linked_list&& other) noexcept
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, std::move(other.m_allocator()))
    {
    }

    single_linked_list(single_linked_list&& other, const allocator_type& alloc) noexcept
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    single_linked_list(IT first, IT last, const allocator_type& alloc = allocator_type()) noexcept
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
    }

    //=========================================================================

    bool is_valid() const noexcept
    {
        return true;
    }

    //=========================================================================
    // destructor
    //=========================================================================

    ~single_linked_list()
    {
        clear();
    }

    //=========================================================================
    // allocator
    //=========================================================================

    allocator_type get_allocator() const noexcept
    {
        return static_cast<allocator_type>(m_allocator());
    }

    //=========================================================================
    // assignment operators
    //=========================================================================

    //=========================================================================
    // assign
    //=========================================================================

    //=========================================================================
    // element access
    //=========================================================================

    reference front() noexcept
    {
        return m_data().head->value;
    }

    const reference front() const noexcept
    {
        return m_data().head->value;
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator before_begin() noexcept
    {
        return iterator(m_data().before_head());
    }

    const_iterator before_begin() const noexcept
    {
        return const_iterator(m_data().before_head());
    }

    const_iterator cbefore_begin() const noexcept
    {
        return before_begin();
    }

    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_data().head);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_data().head);
    }

    //=========================================================================

    iterator end() noexcept
    {
        return iterator(nullptr);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(nullptr);
    }

    //=========================================================================

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    //=========================================================================
    // capacity
    //=========================================================================

    const bool empty() const noexcept
    {
        return m_data().head == nullptr;
    }

    static constexpr size_type max_size() noexcept
    {
        // need to finalize this, maybe allocator traits?
        return std::numeric_limits<size_type>::max();
    }

    //=========================================================================
    // clear
    //=========================================================================

    void clear()
    {
        auto& alloc = m_allocator();

        node_ptr node = mem::exchange(m_data().head, nullptr);
        node_ptr next;

        for (; node; node = next)
        {
            // delete an element
            next = node->next;
            node::template free(alloc, node);
        }
    }

    //=========================================================================
    // insert after
    //=========================================================================

private:

    template <typename... Args>
    node_ptr insert_after_impl(node_ptr ptr, Args&&... args)
    {
        node_ptr new_ptr = m_allocator().allocate(1);
        if (!new_ptr)
        {
            return nullptr;
        }

        mem::construct_in_place(new_ptr, std::forward<Args>(args)...);
        ptr->next = new_ptr;
        return new_ptr;
    }

public:

    iterator insert_after(const_iterator pos, const T& value)
    {
        const auto ptr = insert_after_impl(pos.m_ptr, value);
        return iterator(ptr);
    }

    iterator insert_after(const_iterator pos, T&& value)
    {
        return emplace_after(pos, std::move(value));
    }

    iterator insert_after(const_iterator pos, const size_t count, const T& value)
    {
        if (count != 0)
        {
            insert_op op(m_allocator());
            op.append_n(count, value);
            pos.m_ptr = op.attach_after(pos.m_ptr);
        }

        return iterator(pos.m_ptr);
    }

    iterator insert_after(const_iterator pos, std::initializer_list<T> init)
    {
        return insert_after(pos, init.begin(), init.end());
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert_after(const_iterator pos, IT first, IT last)
    {
        VX_ASSERT(first <= last);

        if (first == last)
        {
            return iterator(pos.m_ptr);
        }

        insert_op op(m_allocator());
        op.append_range(std::move(first), last);
        return iterator(op.attach_after(pos.m_ptr));
    }

    //=========================================================================
    // emplace after
    //=========================================================================

    template <typename... Args>
    iterator emplace_after(Args&&... args)
    {
        return iterator(insert_after_impl(
            m_data().before_head(),
            std::forward<Args>(args)...));
    }

    //=========================================================================
    // erase after
    //=========================================================================

    //=========================================================================
    // push front
    //=========================================================================

    //=========================================================================
    // emplace front
    //=========================================================================

    //=========================================================================
    // pop front
    //=========================================================================

    //=========================================================================
    // resize
    //=========================================================================

    //=========================================================================
    // swap
    //=========================================================================

    //=========================================================================
    // merge
    //=========================================================================

    //=========================================================================
    // splice_after
    //=========================================================================

    //=========================================================================
    // remove
    //=========================================================================

    //=========================================================================
    // reverse
    //=========================================================================

    //=========================================================================
    // unique
    //=========================================================================

    //=========================================================================
    // sort
    //=========================================================================
};

//=========================================================================
// comparison
//=========================================================================

} // namespace vx
