#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/_tools/invoke.hpp"
#include "vertex/std/algorithm.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/functional.hpp"
#include "vertex/std/iterator.hpp"
#include "vertex/std/memory.hpp"
#include "vertex/std/util.hpp"

namespace vx {

template <typename T, typename Allocator>
class doubly_linked_list;

namespace _doubly_linked_list_priv {

//=========================================================================
// node
//=========================================================================

// links only: everything here is independent of T, so there is one copy in the binary
struct node_base
{
    using link_ptr = node_base*;
    link_ptr next;
    link_ptr prev;

    // link one detached node before pos
    static void link_before(link_ptr pos, link_ptr n) noexcept
    {
        n->prev = pos->prev;
        n->next = pos;
        pos->prev->next = n;
        pos->prev = n;
    }

    // link the detached chain [first, last] (inclusive, already internally linked) before pos
    static void link_chain(link_ptr pos, link_ptr first, link_ptr last) noexcept
    {
        link_ptr before = pos->prev;
        before->next = first;
        first->prev = before;
        last->next = pos;
        pos->prev = last;
    }

    // remove n from its list (n's own links are left untouched)
    static void unlink(link_ptr n) noexcept
    {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

    // move [first, last) to just before `before`. `before` may be in another list and
    // the other list's sentinel is a valid `last`. Returns last.
    static link_ptr splice(link_ptr before, link_ptr first, link_ptr last) noexcept
    {
        VX_ASSERT(before != first && before != last && first != last);

        const auto first_prev = first->prev;
        first_prev->next = last;
        const auto last_prev = last->prev;
        last_prev->next = before;
        const auto before_prev = before->prev;
        before_prev->next = first;

        before->prev = last_prev;
        last->prev = first_prev;
        first->prev = before_prev;

        return last;
    }

    // move the single node n to just before `before`. Returns n->next (as it was).
    static link_ptr splice(link_ptr before, link_ptr n) noexcept
    {
        VX_ASSERT(before != n && n->next != before);

        const auto next = n->next;

        const auto n_prev = n->prev;
        n_prev->next = next;
        n->next = before;
        const auto before_prev = before->prev;
        before_prev->next = n;

        before->prev = n;
        next->prev = n_prev;
        n->prev = before_prev;

        return next;
    }

    // reverse the whole circle, sentinel included, by swapping every node's links
    static void reverse(link_ptr head) noexcept
    {
        link_ptr n = head;
        do
        {
            const auto next = n->next;
            n->next = n->prev;
            n->prev = next;
            n = next;

        } while (n != head);
    }
};

template <typename T>
struct node_type : node_base
{
    using node_ptr = node_type*;
    using value_type = T;

    value_type value;

    node_type(const node_type&) = delete;
    node_type& operator=(const node_type&) = delete;

    static node_ptr from_link(link_ptr p) noexcept
    {
        return static_cast<node_ptr>(p);
    }

    static T& val(link_ptr p) noexcept
    {
        return from_link(p)->value;
    }

    // destroys the value and frees the node; the links are the caller's business
    template <typename Allocator>
    static void free_node(Allocator& alloc, node_ptr n) noexcept
    {
        mem::destroy_in_place(std::addressof(n->value));
        alloc.deallocate(n, 1);
    }

    // free every node in the circle except the sentinel; the caller re-inits the sentinel
    template <typename Allocator>
    static void free_all(Allocator& alloc, link_ptr sentinel) noexcept
    {
        link_ptr cur = sentinel->next;
        while (cur != sentinel)
        {
            link_ptr next = cur->next;
            free_node(alloc, from_link(cur));
            cur = next;
        }
    }

    // merge the adjacent sorted ranges [first, mid) and [mid, last).
    // pred compares values. Returns the new first node of the merged range.
    template <typename Pred>
    static link_ptr inplace_merge(link_ptr first, link_ptr mid, const link_ptr last, Pred pred)
    {
        VX_ASSERT(first != mid && mid != last);

        link_ptr new_first;
        if (pred(val(mid), val(first)))
        {
            new_first = mid;
        }
        else
        {
            new_first = first;
            do
            {
                first = first->next;
                if (first == mid)
                {
                    return new_first;
                }

            } while (!pred(val(mid), val(first)));
        }

        for (;;)
        {
            const link_ptr run_start = mid;
            do
            {
                mid = mid->next;

            } while (mid != last && pred(val(mid), val(first)));

            splice(first, run_start, mid);
            if (mid == last)
            {
                return new_first;
            }

            do
            {
                first = first->next;
                if (first == mid)
                {
                    return new_first;
                }

            } while (!pred(val(mid), val(first)));
        }
    }

    // stable merge sort of the `size` nodes starting at `first`.
    // Returns the node after the range. `first` is updated to the new first node.
    template <typename Pred>
    static link_ptr sort_range(link_ptr& first, const size_t size, Pred pred)
    {
        if (size < 2)
        {
            return size ? first->next : first;
        }

        link_ptr mid = sort_range(first, size / 2, pred);
        // mid is updated to the second half's new first node
        const link_ptr last = sort_range(mid, size - size / 2, pred);
        first = inplace_merge(first, mid, last, pred);
        return last;
    }

    // sort the whole list. head is the sentinel, size is the element count
    template <typename Pred>
    static void sort(link_ptr head, const size_t size, Pred pred)
    {
        if (size < 2)
        {
            return;
        }

        link_ptr first = head->next;
        sort_range(first, size, pred);
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
// list value: just the embedded sentinel
//=========================================================================

struct list_value
{
    node_base head;

    list_value() noexcept
    {
        reset();
    }

    // non-copyable: the sentinel points at itself
    list_value(const list_value&) = delete;
    list_value& operator=(const list_value&) = delete;

    node_base* sentinel() const noexcept
    {
        return const_cast<node_base*>(&head);
    }

    bool empty() const noexcept
    {
        return head.next == &head;
    }

    // make the list empty WITHOUT freeing anything
    void reset() noexcept
    {
        head.next = head.prev = &head;
    }

    // steal o's nodes. *this must already be empty (cleared or freshly constructed)
    void take(list_value& o) noexcept
    {
        if (o.empty())
        {
            reset();
            return;
        }

        head.next = o.head.next;
        head.prev = o.head.prev;

        head.next->prev = &head;
        head.prev->next = &head;

        o.reset();
    }

    void swap(list_value& o) noexcept
    {
        list_value tmp;
        tmp.take(*this);
        take(o);
        o.take(tmp);
    }
};

//=========================================================================
// insert operation
//=========================================================================

// Builds a detached chain off to the side, then links it in with one operation.
// If anything fails (or the op is destroyed before attach) the chain is freed and
// the list is untouched, so inserts have the strong guarantee.
template <typename Allocator>
class insert_op
{
private:

    using allocator_type = Allocator;
    using node = typename allocator_type::value_type; // node_type<T>
    using node_ptr = node*;
    using link_ptr = node_base*;

    allocator_type& m_allocator;
    node_base m_anchor; // m_anchor.next is the first built node; the anchor never joins a list
    link_ptr m_tail;    // last built node, or &m_anchor while empty

    template <typename... Args>
    success push(Args&&... args)
    {
        node_ptr ptr = m_allocator.allocate(1);
        VX_RET_ERR_IF(!ptr, err::out_of_memory);

        mem::construct_in_place(std::addressof(ptr->value), std::forward<Args>(args)...);
        ptr->next = nullptr;
        ptr->prev = m_tail; // for the first node this is the anchor; link_chain overwrites it
        m_tail->next = ptr;
        m_tail = ptr;
        return success{};
    }

public:

    explicit insert_op(allocator_type& allocator) noexcept
        : m_allocator(allocator), m_anchor(), m_tail(&m_anchor)
    {
        m_anchor.next = nullptr;
        m_anchor.prev = nullptr;
    }

    insert_op(const insert_op&) = delete;
    insert_op& operator=(const insert_op&) = delete;

    ~insert_op()
    {
        discard();
    }

    // append count elements constructed from args (copied for every node)
    template <typename... Args>
    success append_n(size_t count, const Args&... args)
    {
        for (; count > 0; --count)
        {
            const auto ok = push(args...);
            VX_RET_ERR_IF(!ok, ok);
        }
        return success{};
    }

    // append the values in [first, last). A move_iterator moves the elements.
    template <typename IT1, typename IT2>
    success append_range(IT1 first, IT2 last)
    {
        for (; first != last; ++first)
        {
            const auto ok = push(*first);
            VX_RET_ERR_IF(!ok, ok);
        }
        return success{};
    }

    // link the built chain before pos. Returns the first inserted node, or pos if nothing
    // was built. Resets *this to empty.
    link_ptr attach(link_ptr pos) noexcept
    {
        if (!m_anchor.next)
        {
            return pos;
        }

        const link_ptr first = m_anchor.next;
        node_base::link_chain(pos, first, m_tail);
        m_anchor.next = nullptr;
        m_tail = &m_anchor;
        return first;
    }

    // free whatever chain is currently held
    void discard() noexcept
    {
        link_ptr cur = m_anchor.next;
        while (cur)
        {
            link_ptr next = cur->next;
            node::free_node(m_allocator, node::from_link(cur));
            cur = next;
        }

        m_anchor.next = nullptr;
        m_tail = &m_anchor;
    }
};

//=========================================================================
// iterators
//=========================================================================

template <typename T>
class const_iterator
{
    template <typename, typename>
    friend class ::vx::doubly_linked_list;

    using types = list_types<T>;
    using node = typename types::node;
    using link_ptr = node_base*;

    link_ptr m_ptr;

public:

    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = typename types::value_type;
    using difference_type = typename types::difference_type;
    using pointer = const value_type*;
    using reference = const value_type&;

public:

    const_iterator() noexcept
        : m_ptr()
    {}

    explicit const_iterator(link_ptr ptr) noexcept
        : m_ptr(ptr)
    {}

    reference operator*() const noexcept
    {
        return node::from_link(m_ptr)->value;
    }

    pointer operator->() const noexcept
    {
        return std::addressof(operator*());
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

    const_iterator& operator--() noexcept
    {
        m_ptr = m_ptr->prev;
        return *this;
    }

    const_iterator operator--(int) noexcept
    {
        const_iterator tmp = *this;
        m_ptr = m_ptr->prev;
        return tmp;
    }

    bool operator==(const const_iterator& other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const const_iterator& other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }
};

//=========================================================================

template <typename T>
class iterator : public const_iterator<T>
{
    using base = const_iterator<T>;
    using types = list_types<T>;

public:

    using iterator_category = std::bidirectional_iterator_tag;

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
        return std::addressof(operator*());
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

    iterator& operator--() noexcept
    {
        base::operator--();
        return *this;
    }

    iterator operator--(int) noexcept
    {
        iterator tmp = *this;
        base::operator--();
        return tmp;
    }
};

} // namespace _doubly_linked_list_priv

//=========================================================================
// doubly_linked_list
//=========================================================================

template <typename T, typename Allocator = mem::default_allocator<T>>
class doubly_linked_list
{
    using types = _doubly_linked_list_priv::list_types<T>;

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

    using iterator = _doubly_linked_list_priv::iterator<T>;
    using const_iterator = _doubly_linked_list_priv::const_iterator<T>;
    using reverse_iterator = vx::reverse_iterator<iterator>;
    using const_reverse_iterator = vx::reverse_iterator<const_iterator>;

private:

    using node = typename types::node;
    using node_ptr = typename types::node_ptr;
    using node_base = _doubly_linked_list_priv::node_base;
    using link_ptr = node_base*;

    using list_value = _doubly_linked_list_priv::list_value;

    // rebind the value allocator into a node allocator
    using node_allocator = typename mem::rebind_allocator<allocator_type, node>::type;
    using insert_op = _doubly_linked_list_priv::insert_op<node_allocator>;

    using alloc_traits = mem::allocator_traits<allocator_type>;

    // true if nodes can move between *this and other without crossing allocators
    bool allocators_compatible(const doubly_linked_list& other) const noexcept
    {
        return alloc_traits::is_always_equal::value || m_allocator() == other.m_allocator();
    }

    // compressed storage: node allocator + embedded sentinel
    _priv::compressed_pair<node_allocator, list_value> m_storage;

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

    // erase [first, last)
    void erase_range(link_ptr first, link_ptr last) noexcept
    {
        if (first == last)
        {
            return;
        }

        link_ptr before = first->prev;
        for (link_ptr cur = first; cur != last;)
        {
            link_ptr next = cur->next;
            node::free_node(m_allocator(), node::from_link(cur));
            cur = next;
        }

        before->next = last;
        last->prev = before;
    }

    // Allocate a node, construct its value from args, then link it before pos.
    // The value is constructed before anything is linked, so an argument that refers to an
    // element already in the list is safe. On success out is the new node; on failure the
    // list is unchanged and out_of_memory is returned.
    template <typename... Args>
    success emplace_impl(link_ptr pos, node_ptr& out, Args&&... args)
    {
        node_ptr n = m_allocator().allocate(1);
        VX_RET_ERR_IF(!n, err::out_of_memory);

        mem::construct_in_place(std::addressof(n->value), std::forward<Args>(args)...);
        node_base::link_before(pos, n);
        out = n;
        return success{};
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    doubly_linked_list() noexcept
        : m_storage(_priv::zero_then_variadic_args_tag{})
    {}

    explicit doubly_linked_list(const allocator_type& alloc) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {}

    //=========================================================================

    explicit doubly_linked_list(size_type count, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        const auto ok = op.append_n(count);
        VX_VERIFY(ok);
        op.attach(m_data().sentinel());
    }

    doubly_linked_list(size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        const auto ok = op.append_n(count, value);
        VX_VERIFY(ok);
        op.attach(m_data().sentinel());
    }

    //=========================================================================

    doubly_linked_list(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        const auto ok = op.append_range(init.begin(), init.end());
        VX_VERIFY(ok);
        op.attach(m_data().sentinel());
    }

    //=========================================================================

    doubly_linked_list(const doubly_linked_list& other)
        : m_storage(_priv::one_then_variadic_args_tag{}, other.m_allocator())
    {
        insert_op op(m_allocator());
        const auto ok = op.append_range(other.begin(), other.end());
        VX_VERIFY(ok);
        op.attach(m_data().sentinel());
    }

    doubly_linked_list(const doubly_linked_list& other, const allocator_type& alloc)
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        const auto ok = op.append_range(other.begin(), other.end());
        VX_VERIFY(ok);
        op.attach(m_data().sentinel());
    }

    //=========================================================================

    doubly_linked_list(doubly_linked_list&& other) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, std::move(other.m_allocator()))
    {
        m_data().take(other.m_data());
    }

    doubly_linked_list(doubly_linked_list&& other, const allocator_type& alloc) noexcept(alloc_traits::is_always_equal::value)
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        VX_PRIV_ASSERT_ALLOCATORS_EQUAL(other.m_allocator(), alloc);
        m_data().take(other.m_data());
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    doubly_linked_list(IT first, IT last, const allocator_type& alloc = allocator_type())
        : m_storage(_priv::one_then_variadic_args_tag{}, alloc)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        insert_op op(m_allocator());
        const auto ok = op.append_range(first, last);
        VX_VERIFY(ok);
        op.attach(m_data().sentinel());
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    // the allocator-only constructor never allocates, so create() can use it directly

    static expected<doubly_linked_list, error> create(const allocator_type& alloc = allocator_type())
    {
        return doubly_linked_list(alloc);
    }

    static expected<doubly_linked_list, error> create(size_type count, const allocator_type& alloc = allocator_type())
    {
        doubly_linked_list list(alloc);

        insert_op op(list.m_allocator());
        const auto ok = op.append_n(count);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        op.attach(list.m_data().sentinel());
        return list;
    }

    static expected<doubly_linked_list, error> create(size_type count, const T& value, const allocator_type& alloc = allocator_type())
    {
        doubly_linked_list list(alloc);

        insert_op op(list.m_allocator());
        const auto ok = op.append_n(count, value);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        op.attach(list.m_data().sentinel());
        return list;
    }

    static expected<doubly_linked_list, error> create(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
    {
        doubly_linked_list list(alloc);

        insert_op op(list.m_allocator());
        const auto ok = op.append_range(init.begin(), init.end());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        op.attach(list.m_data().sentinel());
        return list;
    }

    static expected<doubly_linked_list, error> create(const doubly_linked_list& other, const allocator_type& alloc = allocator_type())
    {
        doubly_linked_list list(alloc);

        insert_op op(list.m_allocator());
        const auto ok = op.append_range(other.begin(), other.end());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        op.attach(list.m_data().sentinel());
        return list;
    }

    static expected<doubly_linked_list, error> create(doubly_linked_list&& other)
    {
        doubly_linked_list list(std::move(other));
        return list;
    }

    static expected<doubly_linked_list, error> create(doubly_linked_list&& other, const allocator_type& alloc = allocator_type())
    {
        doubly_linked_list list(std::move(other), alloc);
        return list;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static expected<doubly_linked_list, error> create(IT first, IT last, const allocator_type& alloc = allocator_type())
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        doubly_linked_list list(alloc);

        insert_op op(list.m_allocator());
        const auto ok = op.append_range(first, last);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        op.attach(list.m_data().sentinel());
        return list;
    }

    //=========================================================================
    // destructor
    //=========================================================================

    ~doubly_linked_list()
    {
        node::free_all(m_allocator(), m_data().sentinel()); // the object is dying, no reset needed
    }

    //=========================================================================
    // allocator
    //=========================================================================

    allocator_type get_allocator() const noexcept
    {
        return static_cast<allocator_type>(m_allocator());
    }

    //=========================================================================
    // assignment
    //=========================================================================

private:

    // overwrite existing nodes first, then append the rest or erase the leftovers
    template <typename IT1, typename IT2>
    success assign_range(IT1 first, IT2 last)
    {
        auto& data = m_data();
        const link_ptr end_node = data.sentinel();
        link_ptr cur = data.head.next;

        for (; first != last && cur != end_node; ++first, cur = cur->next)
        {
            node::from_link(cur)->value = *first;
        }

        if (first != last)
        {
            // out of nodes: build the rest off to the side and attach at the end
            insert_op op(m_allocator());
            const auto ok = op.append_range(std::move(first), last);
            VX_RET_ERR_IF(!ok, ok);
            op.attach(end_node);
        }
        else
        {
            // out of input: erase the leftover tail
            erase_range(cur, end_node);
        }

        return success{};
    }

public:

    // Allocators never propagate: the container's allocator is fixed at construction.

    doubly_linked_list& operator=(const doubly_linked_list& other)
    {
        if (this == &other)
        {
            return *this;
        }

        const auto ok = assign_range(other.begin(), other.end());
        VX_VERIFY(ok);
        return *this;
    }

    doubly_linked_list& operator=(doubly_linked_list&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        clear();
        m_allocator() = std::move(other.m_allocator());
        m_data().take(other.m_data());
        return *this;
    }

    doubly_linked_list& operator=(std::initializer_list<T> init)
    {
        const auto ok = assign_range(init.begin(), init.end());
        VX_VERIFY(ok);
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    success assign(const doubly_linked_list& other)
    {
        if (this == &other)
        {
            return success{};
        }

        return assign_range(other.begin(), other.end());
    }

    success assign(doubly_linked_list&& other) noexcept(alloc_traits::is_always_equal::value)
    {
        operator=(std::move(other));
        return success{};
    }

    success assign(std::initializer_list<T> init)
    {
        return assign_range(init.begin(), init.end());
    }

    success assign(size_type count, const T& value)
    {
        // build first so a failure leaves *this unchanged and value may alias an element
        insert_op op(m_allocator());
        const auto ok = op.append_n(count, value);
        VX_RET_ERR_IF(!ok, ok);

        clear();
        op.attach(m_data().sentinel());
        return success{};
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    success assign(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        return assign_range(first, last);
    }

    //=========================================================================
    // element access
    //=========================================================================

    reference front() noexcept
    {
        VX_ASSERT(!empty());
        return node::val(m_data().head.next);
    }

    const_reference front() const noexcept
    {
        VX_ASSERT(!empty());
        return node::val(m_data().head.next);
    }

    reference back() noexcept
    {
        VX_ASSERT(!empty());
        return node::val(m_data().head.prev);
    }

    const_reference back() const noexcept
    {
        VX_ASSERT(!empty());
        return node::val(m_data().head.prev);
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_data().head.next);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_data().head.next);
    }

    iterator end() noexcept
    {
        return iterator(m_data().sentinel());
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_data().sentinel());
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    //=========================================================================

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    //=========================================================================
    // capacity
    //=========================================================================

    bool empty() const noexcept
    {
        return m_data().empty();
    }

    // O(n): the list does not store its size. Prefer empty() for emptiness tests.
    size_type count() const noexcept
    {
        size_type n = 0;
        const link_ptr end_node = m_data().sentinel();
        for (link_ptr p = m_data().head.next; p != end_node; p = p->next)
        {
            ++n;
        }
        return n;
    }

    size_type max_size() const noexcept
    {
        return static_cast<size_type>(mem::allocator_traits<node_allocator>::max_size(m_allocator()));
    }

    //=========================================================================
    // clear
    //=========================================================================

    void clear() noexcept
    {
        node::free_all(m_allocator(), m_data().sentinel());
        m_data().reset();
    }

    //=========================================================================
    // emplace (VX_VERIFY) / emplace_checked (expected)
    //=========================================================================

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        node_ptr n = nullptr;
        const auto ok = emplace_impl(pos.m_ptr, n, std::forward<Args>(args)...);
        VX_VERIFY(ok);
        return iterator(n);
    }

    template <typename... Args>
    expected<iterator, error> emplace_checked(const_iterator pos, Args&&... args)
    {
        node_ptr n = nullptr;
        const auto ok = emplace_impl(pos.m_ptr, n, std::forward<Args>(args)...);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return iterator(n);
    }

    //=========================================================================
    // insert (VX_VERIFY) / insert_checked (expected)
    //=========================================================================

    iterator insert(const_iterator pos, const T& value)
    {
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value)
    {
        return emplace(pos, std::move(value));
    }

    // returns an iterator to the first inserted element, or pos if nothing was inserted
    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        insert_op op(m_allocator());
        const auto ok = op.append_n(count, value);
        VX_VERIFY(ok);
        return iterator(op.attach(pos.m_ptr));
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        insert_op op(m_allocator());
        const auto ok = op.append_range(std::move(first), last);
        VX_VERIFY(ok);
        return iterator(op.attach(pos.m_ptr));
    }

    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        return insert(pos, init.begin(), init.end());
    }

    //=========================================================================

    expected<iterator, error> insert_checked(const_iterator pos, const T& value)
    {
        return emplace_checked(pos, value);
    }

    expected<iterator, error> insert_checked(const_iterator pos, T&& value)
    {
        return emplace_checked(pos, std::move(value));
    }

    expected<iterator, error> insert_checked(const_iterator pos, size_type count, const T& value)
    {
        insert_op op(m_allocator());
        const auto ok = op.append_n(count, value);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return iterator(op.attach(pos.m_ptr));
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    expected<iterator, error> insert_checked(const_iterator pos, IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        insert_op op(m_allocator());
        const auto ok = op.append_range(std::move(first), last);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return iterator(op.attach(pos.m_ptr));
    }

    expected<iterator, error> insert_checked(const_iterator pos, std::initializer_list<T> init)
    {
        return insert_checked(pos, init.begin(), init.end());
    }

    //=========================================================================
    // emplace / push at the ends (always fallible, like single_linked_list::push_front)
    //=========================================================================

    template <typename... Args>
    expected<pointer, error> emplace_back(Args&&... args)
    {
        node_ptr n = nullptr;
        const auto ok = emplace_impl(m_data().sentinel(), n, std::forward<Args>(args)...);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return std::addressof(n->value);
    }

    template <typename... Args>
    expected<pointer, error> emplace_front(Args&&... args)
    {
        node_ptr n = nullptr;
        const auto ok = emplace_impl(m_data().head.next, n, std::forward<Args>(args)...);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return std::addressof(n->value);
    }

    //=========================================================================

    success push_back(const T& value)
    {
        node_ptr n = nullptr;
        return emplace_impl(m_data().sentinel(), n, value);
    }

    success push_back(T&& value)
    {
        node_ptr n = nullptr;
        return emplace_impl(m_data().sentinel(), n, std::move(value));
    }

    success push_front(const T& value)
    {
        node_ptr n = nullptr;
        return emplace_impl(m_data().head.next, n, value);
    }

    success push_front(T&& value)
    {
        node_ptr n = nullptr;
        return emplace_impl(m_data().head.next, n, std::move(value));
    }

    //=========================================================================
    // erase / pop (cannot fail)
    //=========================================================================

    iterator erase(const_iterator pos) noexcept
    {
        VX_ASSERT(pos.m_ptr != m_data().sentinel());
        const link_ptr next = pos.m_ptr->next;
        node_base::unlink(pos.m_ptr);
        node::free_node(m_allocator(), node::from_link(pos.m_ptr));
        return iterator(next);
    }

    iterator erase(const_iterator first, const_iterator last) noexcept
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        erase_range(first.m_ptr, last.m_ptr);
        return iterator(last.m_ptr);
    }

    void pop_front() noexcept
    {
        VX_ASSERT(!empty());
        erase(begin());
    }

    void pop_back() noexcept
    {
        VX_ASSERT(!empty());
        erase(const_iterator(m_data().head.prev));
    }

    //=========================================================================
    // resize
    //=========================================================================

private:

    template <typename... Args>
    success resize_impl(size_type new_size, const Args&... args)
    {
        const link_ptr end_node = m_data().sentinel();
        link_ptr cur = m_data().head.next;

        // walk until we've passed new_size elements or hit the end
        for (; cur != end_node && new_size > 0; cur = cur->next)
        {
            --new_size;
        }

        if (cur == end_node)
        {
            // list too short (or exact): build the missing elements off to the side, then attach.
            // If new_size is 0 nothing is built and attach does nothing.
            insert_op op(m_allocator());
            const auto ok = op.append_n(new_size, args...);
            VX_RET_ERR_IF(!ok, ok);
            op.attach(end_node);
        }
        else
        {
            // list too long: cur is the first element to drop
            erase_range(cur, end_node);
        }

        return success{};
    }

public:

    success resize(const size_type count)
    {
        return resize_impl(count);
    }

    success resize(const size_type count, const T& value)
    {
        return resize_impl(count, value);
    }

    //=========================================================================
    // swap
    //=========================================================================

    void swap(doubly_linked_list& other) noexcept
    {
        vx::swap(m_storage, other.m_storage);
    }

    //=========================================================================
    // merge
    //=========================================================================

private:

    // merge in the elements of other; both lists must be ordered by pred
    template <typename Pred>
    void merge_impl(doubly_linked_list& other, Pred pred)
    {
        VX_ASSERT(alloc_traits::is_always_equal::value || m_allocator() == other.m_allocator());

        if (this == &other)
        {
            return;
        }

        auto& data = m_data();
        auto& other_data = other.m_data();

        if (other_data.empty())
        {
            return;
        }

        if (data.empty())
        {
            // *this is empty; take all elements of other with no comparisons
            data.take(other_data);
            return;
        }

        const link_ptr sentinel = data.sentinel();
        const link_ptr first = data.head.next;
        const link_ptr other_first = other_data.head.next;

        // if nothing in other belongs before our last element, appending is enough
        const bool already_ordered = !pred(node::val(other_first), node::val(data.head.prev));

        // move all of other to the end of *this; this leaves other's sentinel self-linked
        node_base::splice(sentinel, other_first, other_data.sentinel());

        if (!already_ordered)
        {
            node::inplace_merge(first, other_first, sentinel, pred);
        }
    }

public:

    // merge in elements from other, both ordered by operator<
    void merge(doubly_linked_list& other)
    {
        merge_impl(other, vx::less<>{});
    }

    // merge in elements from other, both ordered by operator<
    void merge(doubly_linked_list&& other)
    {
        merge_impl(other, vx::less<>{});
    }

    // merge in elements from other, both ordered by pred
    template <typename Pred>
    void merge(doubly_linked_list& other, Pred pred)
    {
        merge_impl(other, fn::pass_func(pred));
    }

    // merge in elements from other, both ordered by pred
    template <typename Pred>
    void merge(doubly_linked_list&& other, Pred pred)
    {
        merge_impl(other, fn::pass_func(pred));
    }

    //=========================================================================
    // splice (all O(1), including cross-list ranges)
    //=========================================================================

    // move all of other before pos
    void splice(const_iterator pos, doubly_linked_list& other) noexcept
    {
        VX_ASSERT(m_allocator() == other.m_allocator());
        VX_ASSERT(this != &other);

        if (other.empty())
        {
            return;
        }

        node_base::splice(pos.m_ptr, other.m_data().head.next, other.m_data().sentinel());
    }

    void splice(const_iterator pos, doubly_linked_list&& other) noexcept
    {
        splice(pos, other);
    }

    // move the element it (of other) before pos
    void splice(const_iterator pos, doubly_linked_list& other, const_iterator it) noexcept
    {
        VX_ASSERT(m_allocator() == other.m_allocator());
        VX_ASSERT(it.m_ptr != other.m_data().sentinel());

        // already in place
        if (pos.m_ptr == it.m_ptr || pos.m_ptr == it.m_ptr->next)
        {
            return;
        }

        node_base::splice(pos.m_ptr, it.m_ptr);
    }

    void splice(const_iterator pos, doubly_linked_list&& other, const_iterator it) noexcept
    {
        splice(pos, other, it);
    }

    // move [first, last) of other before pos. If other is *this, pos must not be in the range.
    void splice(const_iterator pos, doubly_linked_list& other, const_iterator first, const_iterator last) noexcept
    {
        VX_ASSERT(m_allocator() == other.m_allocator());

        if (first == last || pos == first || pos == last)
        {
            return;
        }

        node_base::splice(pos.m_ptr, first.m_ptr, last.m_ptr);
    }

    void splice(const_iterator pos, doubly_linked_list&& other, const_iterator first, const_iterator last) noexcept
    {
        splice(pos, other, first, last);
    }

    //=========================================================================
    // remove_if
    //=========================================================================

private:

    struct remove_op
    {
        // Tracks nodes pending removal, so that program-defined predicates (or the value
        // passed to remove) may reference those elements until the removal is complete.
        // Removed nodes are unlinked from the list but stay allocated until destruction.

        explicit remove_op(doubly_linked_list& list) noexcept
            : m_list(list), m_pending(nullptr)
        {}

        remove_op(const remove_op&) = delete;
        remove_op& operator=(const remove_op&) = delete;

        // unlink n from the list and park it; returns n's successor
        link_ptr transfer(link_ptr n) noexcept
        {
            const link_ptr next = n->next;
            node_base::unlink(n);
            n->next = m_pending; // the pending chain is singly linked through next
            m_pending = n;
            return next;
        }

        ~remove_op()
        {
            while (m_pending)
            {
                const link_ptr next = m_pending->next;
                node::free_node(m_list.m_allocator(), node::from_link(m_pending));
                m_pending = next;
            }
        }

        doubly_linked_list& m_list;
        link_ptr m_pending;
    };

public:

    // erase each element satisfying pred; returns the number removed
    template <typename Pred>
    size_type remove_if(Pred pred)
    {
        remove_op op(*this);
        size_type removed = 0;
        const link_ptr end_node = m_data().sentinel();

        for (link_ptr cur = m_data().head.next; cur != end_node;)
        {
            if (pred(node::val(cur)))
            {
                cur = op.transfer(cur);
                ++removed;
            }
            else
            {
                cur = cur->next;
            }
        }

        return removed;
    }

    //=========================================================================
    // remove
    //=========================================================================

    // erase each element matching value
    size_type remove(const T& value)
    {
        return remove_if([&](const T& other) -> bool
            { return other == value; });
    }

    //=========================================================================
    // unique
    //=========================================================================

    // erase each element matching previous
    size_type unique()
    {
        return unique(vx::equal_to<>{});
    }

    // erase each element satisfying pred with previous
    template <typename Pred>
    size_type unique(Pred pred)
    {
        remove_op op(*this);
        size_type removed = 0;
        const link_ptr end_node = m_data().sentinel();

        link_ptr first = m_data().head.next;
        if (first == end_node)
        {
            return 0;
        }

        link_ptr after = first->next;
        while (after != end_node)
        {
            if (pred(node::val(first), node::val(after)))
            {
                after = op.transfer(after);
                ++removed;
            }
            else
            {
                first = after;
                after = after->next;
            }
        }

        return removed;
    }

    //=========================================================================
    // sort
    //=========================================================================

    // order sequence (stable). Costs one O(n) count walk before sorting.
    void sort()
    {
        node::sort(m_data().sentinel(), count(), vx::less<>{});
    }

    // order sequence (stable)
    template <typename Pred>
    void sort(Pred pred)
    {
        node::sort(m_data().sentinel(), count(), fn::pass_func(pred));
    }

    //=========================================================================
    // reverse
    //=========================================================================

    // reverse sequence
    void reverse() noexcept
    {
        node_base::reverse(m_data().sentinel());
    }
};

//=========================================================================
// non-member functions
//=========================================================================

template <typename T, typename Allocator>
void swap(doubly_linked_list<T, Allocator>& a, doubly_linked_list<T, Allocator>& b) noexcept
{
    a.swap(b);
}

//=========================================================================
// comparison
//=========================================================================

template <typename T, typename Allocator>
bool operator==(const doubly_linked_list<T, Allocator>& lhs, const doubly_linked_list<T, Allocator>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Allocator>
bool operator!=(const doubly_linked_list<T, Allocator>& lhs, const doubly_linked_list<T, Allocator>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename Allocator>
bool operator<(const doubly_linked_list<T, Allocator>& lhs, const doubly_linked_list<T, Allocator>& rhs)
{
    return range::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Allocator>
bool operator>(const doubly_linked_list<T, Allocator>& lhs, const doubly_linked_list<T, Allocator>& rhs)
{
    return (rhs < lhs);
}

template <typename T, typename Allocator>
bool operator<=(const doubly_linked_list<T, Allocator>& lhs, const doubly_linked_list<T, Allocator>& rhs)
{
    return !(rhs < lhs);
}

template <typename T, typename Allocator>
bool operator>=(const doubly_linked_list<T, Allocator>& lhs, const doubly_linked_list<T, Allocator>& rhs)
{
    return !(lhs < rhs);
}

//=========================================================================

template <typename T, typename Allocator = mem::default_allocator<T>>
using dlist = doubly_linked_list<T, Allocator>;

} // namespace vx
