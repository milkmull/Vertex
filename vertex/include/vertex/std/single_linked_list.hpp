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

using X = std::unordered_set<int>;

namespace _single_linked_list_priv {

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

private:

    using node = _single_linked_list_priv::node_type<T>;
    using node_ptr = node*;

    using list_value = _single_linked_list_priv::value<T>;
    // rebind the value allocator into a node allocator
    using node_allocator = typename mem::rebind_allocator<allocator_type, node>::type;
    using intert_op = _single_linked_list_priv::insert_after_op<node_allocator>;

    // compressed storage: node_allocator + head pointer (or whatever "value"
    // you want compressed alongside it) in one base-optimized object
    mem::_mem_priv::allocator_storage<node_allocator, list_value> m_storage;

    node_allocator& allocator() noexcept
    {
        return m_storage.allocator();
    }
    const node_allocator& allocator() const noexcept
    {
        return m_storage.allocator();
    }

    list_value& get_value() noexcept
    {
        return m_storage.value;
    }
    const list_value& get_value() const noexcept
    {
        return m_storage.value;
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    single_linked_list() noexcept = default;

    explicit single_linked_list(const allocator_type& alloc)
        : m_storage(alloc)
    {}

    explicit single_linked_list(size_type count, const allocator_type& alloc = allocator_type())
        : m_storage(alloc)
    {
        intert_op op(allocator());
        op.append_n(count);
        op.attach_after(get_value().before_head());
    }

    single_linked_list(size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : m_storage(alloc)
    {
        intert_op op(allocator());
        op.append_n(count, value);
        op.attach_after(get_value().before_head());
    }

    //=========================================================================
    // destructor
    //=========================================================================

    ~single_linked_list()
    {
        clear();
    }

    //=========================================================================
    // memory
    //=========================================================================

    void clear()
    {
        auto& alloc = allocator();

        node_ptr node = mem::exchange(get_value().head, nullptr);
        node_ptr next;

        for (; node; node = next)
        {
            // delete an element
            next = node->next;
            node::template free(alloc, node);
        }
    }
};

//=========================================================================
// comparison
//=========================================================================

} // namespace vx
