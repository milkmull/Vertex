#pragma once

#include <cstdint>
#include <initializer_list>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/_tools/invoke.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

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
    static void free_node(Allocator& alloc, node_ptr node) noexcept
    {
        mem::destroy_in_place(node->next);
        mem::destroy_in_place(std::addressof(node->value));
        alloc.deallocate(node, 1);
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
// list value
//=========================================================================

template <typename T>
struct value
{
    using types = list_types<T>;

    using node = typename types::node;
    using node_ptr = typename types::node_ptr;

    using value_type = typename types::value_type;
    using pointer = typename types::pointer;
    using const_pointer = typename types::const_pointer;
    using reference = typename types::reference;
    using const_reference = typename types::const_reference;
    using size_type = typename types::size_type;
    using difference_type = typename types::difference_type;

    value() noexcept
        : head()
    {}

    // return pointer to the "before begin" pseudo node
    node_ptr before_head() const noexcept
    {
        return &reinterpret_cast<node&>(const_cast<node_ptr&>(head));
    }

    node_ptr head;

public:

    template <typename Pred>
    static node_ptr inplace_merge(
        node_ptr before_first_node,      // the node immediately BEFORE the first range
        const node_ptr before_mid_node,  // the last node of the first range
        const node_ptr before_last_node, // the last node of the second range
        Pred pred)
    {
        auto mid_node = before_mid_node->next;

        for (;;)
        {
            node_ptr first_node;

            for (;;)
            {
                if (before_first_node == before_mid_node)
                {
                    return before_last_node;
                }

                first_node = before_first_node->next;
                if (pred(mid_node->value, first_node->value))
                {
                    break;
                }

                before_first_node = first_node;
            }

            auto before_run_end_node = mid_node;
            node_ptr run_end_node;

            for (;;)
            {
                run_end_node = before_run_end_node->next;

                if (before_run_end_node == before_last_node)
                {
                    break;
                }

                if (!pred(run_end_node->value, first_node->value))
                {
                    break;
                }

                before_run_end_node = run_end_node;
            }

            before_mid_node->next = run_end_node;
            before_first_node->next = mid_node;
            before_run_end_node->next = first_node;

            if (before_run_end_node == before_last_node)
            {
                return before_mid_node;
            }

            before_first_node = first_node;
            mid_node = run_end_node;
        }
    }

    // Sort (before_first_node, before_first_node + 2]
    template <class Pred>
    static node_ptr sort2(const node_ptr before_first_node, Pred pred)
    {
        const auto first_node = before_first_node->next;
        if (!first_node)
        {
            return before_first_node;
        }

        auto second_node = first_node->next;
        if (!second_node || !pred(second_node->value, first_node->value))
        {
            return first_node;
        }

        // swap second_node and first_node
        first_node->next = second_node->next;  // snip out *second_node
        before_first_node->next = second_node; // insert *second_node before *first_node
        second_node->next = first_node;
        return second_node;
    }

    // Sort(before_first_node, before_first_node + bound)
    template <class Pred>
    static node_ptr sort(const node_ptr before_first_node, size_type bound, Pred pred)
    {
        if (bound <= 2)
        {
            return sort2(before_first_node, pred);
        }

        const auto half_bound = bound / 2;
        const auto before_mid_node = sort(before_first_node, half_bound, pred);
        if (!before_mid_node->next)
        {
            return before_mid_node;
        }

        const auto before_last_node = sort(before_mid_node, half_bound, pred);
        return inplace_merge(before_first_node, before_mid_node, before_last_node, pred);
    }

    template <class Pred>
    static void sort(node_ptr before_first_node, Pred pred)
    {
        auto before_mid_node = sort2(before_first_node, pred);
        size_type bound = 2;

        do
        {
            if (!before_mid_node->next)
            {
                return;
            }

            const auto before_last_node = sort(before_mid_node, bound, pred);
            before_mid_node = inplace_merge(before_first_node, before_mid_node, before_last_node, pred);
            bound <<= 1; // divide by 2

        } while (bound != 0);
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
    pointer m_tail; // Points to the most recently constructed node. If pointer{}, the value of head is indeterminate.
                    // m_tail->next is not constructed.
    pointer m_head; // Points at the first constructed node.
    bool m_ok;      // Success of the operation

public:

    explicit insert_after_op(allocator_type& allocator)
        : m_allocator(allocator), m_tail(), m_head(), m_ok(true)
    {}

    insert_after_op(const insert_after_op&) = delete;
    insert_after_op& operator=(const insert_after_op&) = delete;

    template <typename... Args>
    void append_n(size_t count, const Args&... args)
    {
        VX_ASSERT(m_ok);

        if (m_tail == nullptr)
        {
            pointer ptr = m_allocator.allocate(1);
            if (!ptr)
            {
                m_ok = false;
                return;
            }

            mem::construct_in_place(std::addressof(ptr->value), args...);
            m_head = ptr;
            m_tail = ptr;
        }

        for (; count > 0; --count)
        {
            pointer ptr = m_allocator.allocate(1);
            if (!ptr)
            {
                m_ok = false;
                return;
            }

            mem::construct_in_place(std::addressof(ptr->value), args...);
            mem::construct_in_place(m_tail->next, ptr);
            m_tail = ptr;
        }
    }

    template <typename IT1, typename IT2>
    void append_range(IT1 first, IT2 last)
    {
        VX_ASSERT(m_ok);

        for (; first != last; ++first)
        {
            pointer n = m_allocator.allocate(1);
            if (!n)
            {
                m_ok = false;
                return;
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
    }

    bool success() const noexcept
    {
        return m_ok;
    }

    pointer attach_after(pointer after) noexcept
    {
        if (!m_ok)
        {
            // Failed operation
            return nullptr;
        }

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

    // Frees whatever chain is currently held
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

    void take_head(single_linked_list& other) noexcept
    {
        m_data().head = mem::exchange(other.m_data().head, nullptr);
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
        insert_after(before_begin(), init.begin(), init.end());
    }

    //=========================================================================

    single_linked_list(const single_linked_list& other)
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, other.m_allocator())
    {
        insert_op op(m_allocator());
        op.append_range(other.begin(), other.end());
        op.attach_after(m_data().before_head());
    }

    single_linked_list(const single_linked_list& other, const allocator_type& alloc)
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        op.append_range(other.begin(), other.end());
        op.attach_after(m_data().before_head());
    }

    single_linked_list(single_linked_list&& other) noexcept
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, std::move(other.m_allocator()))
    {
        take_head(other);
    }

    single_linked_list(single_linked_list&& other, const allocator_type& alloc) noexcept
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        take_head(other);
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    single_linked_list(IT first, IT last, const allocator_type& alloc = allocator_type()) noexcept
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        op.append_range(first, last);
        op.attach_after(m_data().before_head());
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

private:

    template <typename IT1, typename IT2>
    bool assign_range(IT1 first, IT2 last)
    {
        auto head_node = m_data().before_head();

        for (; first != last; ++first)
        {
            const auto next_node = head_node->next;
            if (!next_node)
            {
                insert_op op(m_allocator());
                op.append_range(std::move(first), last);
                op.attach_after(head_node);
                return op.success();
            }

            next_node->value = *first;
            head_node = next_node;
        }

        for (auto del_node = mem::exchange(head_node->next, nullptr); del_node;)
        {
            const auto next_node = del_node->next;
            node::free_node(m_allocator(), del_node);
            del_node = next_node;
        }

        return true;
    }

public:

    single_linked_list& operator=(const single_linked_list& other)
    {
        if (this == &other)
        {
            return *this;
        }

        assign_range(other.begin(), other.end());
        return *this;
    }

    single_linked_list& operator=(single_linked_list&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        clear();
        m_allocator() = std::move(other.m_allocator());
        take_head(other);
        return *this;
    }

    single_linked_list& operator=(std::initializer_list<T> init) noexcept
    {
        assign_range(init.begin(), init.end());
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    bool assign(const single_linked_list& other)
    {
        if (this == &other)
        {
            return true;
        }

        return assign_range(other.begin(), other.end());
    }

    bool assign(single_linked_list&& other) noexcept
    {
        operator=(std::move(other));
        return true;
    }

    bool assign(std::initializer_list<T> init)
    {
        return assign_range(init.begin(), init.end());
    }

    bool assign(size_type count, const T& value)
    {
        clear();
        return insert_after(before_begin(), count, value) != end();
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    bool assign(IT first, IT last)
    {
        return assign_range(first, last);
    }

    //=========================================================================
    // element access
    //=========================================================================

    reference front() noexcept
    {
        return m_data().head->value;
    }

    const_reference front() const noexcept
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

    constexpr size_type max_size() const noexcept
    {
        return static_cast<size_type>(std::allocator_traits<node_allocator>::max_size(m_allocator()));
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
            node::free_node(alloc, node);
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

        mem::construct_in_place(std::addressof(new_ptr->value), std::forward<Args>(args)...);
        new_ptr->next = ptr->next;
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
    iterator emplace_after(const_iterator pos, Args&&... args)
    {
        auto ptr = insert_after_impl(
            pos.m_ptr,
            std::forward<Args>(args)...);

        return iterator(ptr);
    }

    //=========================================================================
    // erase after
    //=========================================================================

    iterator erase_after(const_iterator pos)
    {
        node_ptr keep_node = pos.m_ptr;
        node_ptr erase_node = keep_node->next;
        VX_ASSERT(erase_node != nullptr);

        keep_node->next = erase_node->next;
        node::free_node(m_allocator(), erase_node);

        return iterator(keep_node->next);
    }

    iterator erase_after(const_iterator first, const_iterator last)
    {
        node_ptr keep_node = first.m_ptr;

        for (;;)
        {
            const node_ptr erase_node = keep_node->next;
            if (erase_node == last.m_ptr)
            {
                break;
            }

            keep_node->next = erase_node->next;
            node::free_node(m_allocator(), erase_node);
        }

        return iterator(last.m_ptr);
    }

    //=========================================================================
    // push front
    //=========================================================================

    bool push_front(const T& value)
    {
        return insert_after_impl(m_data().before_head(), value) != nullptr;
    }

    bool push_front(T&& value)
    {
        return insert_after_impl(m_data().before_head(), std::move(value)) != nullptr;
    }

    //=========================================================================
    // emplace front
    //=========================================================================

    template <typename... Args>
    pointer emplace_front(Args&&... args)
    {
        node_ptr ptr = insert_after_impl(m_data().before_head(), std::forward<Args>(args)...);
        return ptr ? std::addressof(ptr->value) : nullptr;
    }

    //=========================================================================
    // pop front
    //=========================================================================

    void pop_front()
    {
        erase_after(m_data().before_head());
    }

    //=========================================================================
    // resize
    //=========================================================================

private:

    template <typename... Args>
    bool resize_impl(size_type new_size, const Args&... args)
    {
        auto head_node = m_data().before_head();
        auto& alloc = m_allocator();

        for (;;)
        {
            auto next_node = head_node->next;
            if (!next_node)
            {
                // list too short, insert remaining new_size objects initialized from args...
                insert_op op(alloc);
                op.append_n(new_size, args...);
                op.attach_after(head_node);
                return op.success();
            }

            if (new_size == 0)
            {
                // list is too long, erase the next and after
                head_node->next = nullptr;

                do
                {
                    const auto nextnext_node = next_node->next;
                    node::free_node(alloc, next_node);
                    next_node = nextnext_node;

                } while (next_node);

                break;
            }

            head_node = next_node;
            --new_size;
        }

        return true;
    }

public:

    bool resize(const size_type count)
    {
        return resize_impl(count);
    }

    bool resize(const size_type count, const T& value)
    {
        return resize_impl(count, value);
    }

    //=========================================================================
    // swap
    //=========================================================================

    void swap(single_linked_list& other) noexcept
    {
        mem::swap(m_storage, other.m_storage);
    }

    //=========================================================================
    // merge
    //=========================================================================

private:

    // merge in elements from other, both ordered by pred
    template <typename Pred>
    void merge_impl(single_linked_list& other, Pred pred)
    {
        if (this == &other)
        {
            return;
        }

        auto& data = m_data();
        auto& other_data = other.m_data();

        if (!data.head)
        {
            // *this is empty; take all elements of other with no comparisons
            data.head = other_data.head;
            other_data.head = nullptr;
            return;
        }

        if (!other_data.head)
        {
            return;
        }

        auto before_first_node = data.before_head();
        auto other_first_node = other_data.head;

        for (;;)
        {
            // process 1 splice
            node_ptr first_node;

            for (;;)
            {
                // advance before_first_node over elements already in position
                first_node = before_first_node->next;
                if (!first_node)
                {
                    // all elements in other are greater than elements in *this, splice them all
                    before_first_node->next = other_first_node;
                    other_data.head = nullptr;
                    return;
                }

                if (pred(other_first_node->value, first_node->value))
                {
                    // other_first_node->value is out of order
                    break;
                }

                // first_node->value is already in position; advance
                before_first_node = first_node;
            }

            // find the end of the "run" of elements less than first_node->value in other
            auto other_run_end = other_first_node;
            node_ptr run_end_node;

            for (;;)
            {
                run_end_node = other_run_end->next;
                if (!run_end_node)
                {
                    break;
                }

                if (!pred(run_end_node->value, first_node->value))
                {
                    // run_end_node is the first element in other that shouldn't precede first_node->value.
                    // After the splice first_node->value will be in position and must not be compared again.
                    break;
                }

                other_run_end = run_end_node;
            }

            other_data.head = run_end_node;             // snip out the run from its old position
            before_first_node->next = other_first_node; // insert into new position
            other_run_end->next = first_node;
            if (!run_end_node)
            {
                return;
            }

            before_first_node = first_node;
            other_first_node = run_end_node;
        }
    }

public:

    // merge in elements from other, both ordered by operator<
    void merge(single_linked_list& other)
    {
        merge_impl(other, std::less<>{});
    }

    // merge in elements from other, both ordered by operator<
    void merge(single_linked_list&& other)
    {
        merge_impl(other, std::less<>{});
    }

    // merge in elements from other, both ordered by pred
    template <typename Pred>
    void merge(single_linked_list& other, Pred pred)
    {
        merge_impl(other, fn::pass_func(pred));
    }

    // merge in elements from other, both ordered by pred
    template <typename Pred>
    void merge(single_linked_list&& other, Pred pred)
    {
        merge_impl(other, fn::pass_func(pred));
    }

    //=========================================================================
    // splice_after
    //=========================================================================

private:

    // splice other (prev, prev + 2) after pos
    void splice_after_impl(node_ptr pos, single_linked_list& other, node_ptr prev) noexcept
    {
        if (pos != prev)
        {
            const auto first_node = prev->next;

            if (pos != first_node)
            {
                prev->next = first_node->next;
                first_node->next = pos->next;
                pos->next = first_node;
            }
        }
    }

    // splice other (first, last) just after pos
    template <class _Sentinel>
    void splice_after_impl(const_iterator pos, single_linked_list& other, const_iterator first, _Sentinel last) noexcept
    {
        if (first == last)
        {
            return;
        }

        // find prev(last)
        const_iterator after = first;
        ++after;
        if (after == last)
        {
            return;
        }

        const_iterator last_last = first;
        do
        {
            last_last = after;
            ++after;

        } while (after != last);

        const auto extracted_head = first.m_ptr->next;
        first.m_ptr->next = after.m_ptr;
        last_last.m_ptr->next = pos.m_ptr->next;
        pos.m_ptr->next = extracted_head;
    }

public:

    // splice all of other after pos
    void splice_after(const_iterator pos, single_linked_list& other) noexcept
    {
        if (this != &other && !other.empty())
        {
            // worth splicing, do it
            splice_after_impl(pos, other, other.before_begin(), other.end());
        }
    }

    // splice all of other after pos
    void splice_after(const_iterator pos, single_linked_list&& other) noexcept
    {
        splice_after(pos, other);
    }

    // splice other (first, first + 2) after pos
    void splice_after(const_iterator pos, single_linked_list& other, const_iterator first) noexcept
    {
        splice_after_impl(pos.m_ptr, other, first.m_ptr);
    }

    // splice other (first, first + 2) after pos
    void splice_after(const_iterator pos, single_linked_list&& other, const_iterator first) noexcept
    {
        splice_after(pos, other, first);
    }

    // splice other (first, last) after pos
    void splice_after(const_iterator pos, single_linked_list& other, const_iterator first, const_iterator last) noexcept
    {
        splice_after_impl(pos, other, first, last);
    }

    // splice other [first, last) after pos
    void splice_after(const_iterator pos, single_linked_list&& other, const_iterator first, const_iterator last) noexcept
    {
        splice_after(pos, other, first, last);
    }

    //=========================================================================
    // remove_if
    //=========================================================================

private:

    struct remove_op
    {
        // tracks nodes pending removal in a remove operation, so that
        // program-defined predicates may reference those elements until
        // the removal is complete.

        explicit remove_op(single_linked_list& list) noexcept
            : m_list(list), m_head(), m_tail(&m_head)
        {}

        remove_op(const remove_op&) = delete;
        remove_op& operator=(const remove_op&) = delete;

        node_ptr transfer_back(const node_ptr prev_node) noexcept
        {
            // extract prev_node->next from the container, and add it to
            // the singly-linked m_list of nodes to destroy returns the
            // successor of the removed node

            // snip the node out
            const auto removed_node = prev_node->next;
            const auto next_node = removed_node->next;
            removed_node->next = nullptr;
            prev_node->next = next_node;

            *m_tail = removed_node;
            m_tail = &removed_node->next;

            return next_node;
        }

        ~remove_op()
        {
            auto target_node = m_head;
            while (target_node)
            {
                auto next_node = target_node->next;
                mem::destroy_in_place(target_node->next);
                mem::destroy_in_place(std::addressof(target_node->value));
                m_list.m_allocator().deallocate(target_node, 1);
                target_node = next_node;
            }
        }

        single_linked_list& m_list;
        node_ptr m_head;
        node_ptr* m_tail;
    };

public:

    // erase each element satisfying Pred
    template <typename Pred>
    size_type remove_if(Pred pred)
    {
        remove_op op(*this);

        auto before_first = before_begin();
        size_type count = 0;

        for (auto first = begin(); first.m_ptr;)
        {
            if (pred(*first))
            {
                first.m_ptr = op.transfer_back(before_first.m_ptr);
                ++count;
            }
            else
            {
                before_first = first;
                ++first;
            }
        }

        return count;
    }

    //=========================================================================
    // remove
    //=========================================================================

    // erase each element matching _Val
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
        return unique(std::equal_to<>{});
    }

    // erase each element satisfying pred with previous
    template <typename Pred>
    size_type unique(Pred pred)
    {
        remove_op op(*this);
        auto first = begin();
        size_type count = 0;

        if (first.m_ptr)
        {
            auto after = first;
            ++after;

            while (after.m_ptr)
            {
                if (pred(*first, *after))
                {
                    after.m_ptr = op.transfer_back(first.m_ptr);
                    ++count;
                }
                else
                {
                    first = after;
                    ++after;
                }
            }
        }

        return count;
    }

    //=========================================================================
    // sort
    //=========================================================================

    // order sequence
    void sort()
    {
        list_value::sort(m_data().before_head(), std::less<>{});
    }

    // order sequence
    template <class Pred>
    void sort(Pred pred)
    {
        list_value::sort(m_data().before_head(), fn::pass_func(pred));
    }

    //=========================================================================
    // reverse
    //=========================================================================

    // reverse sequence
    void reverse() noexcept
    {
        auto current_node = m_data().head;
        if (!current_node)
        {
            // empty
            return;
        }

        node_ptr prev_node{};
        for (;;)
        {
            const auto next_node = current_node->next;
            current_node->next = prev_node;
            if (!next_node)
            {
                m_data().head = current_node;
                return;
            }

            prev_node = current_node;
            current_node = next_node;
        }
    }
};

//=========================================================================
// comparison
//=========================================================================

template <typename T, typename Allocator>
bool operator==(const single_linked_list<T, Allocator>& lhs, const single_linked_list<T, Allocator>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Allocator>
bool operator!=(const single_linked_list<T, Allocator>& lhs, const single_linked_list<T, Allocator>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename Allocator>
bool operator<(const single_linked_list<T, Allocator>& lhs, const single_linked_list<T, Allocator>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Allocator>
bool operator>(const single_linked_list<T, Allocator>& lhs, const single_linked_list<T, Allocator>& rhs)
{
    return (rhs < lhs);
}

template <typename T, typename Allocator>
bool operator<=(const single_linked_list<T, Allocator>& lhs, const single_linked_list<T, Allocator>& rhs)
{
    return !(rhs < lhs);
}

template <typename T, typename Allocator>
bool operator>=(const single_linked_list<T, Allocator>& lhs, const single_linked_list<T, Allocator>& rhs)
{
    return !(lhs < rhs);
}

//=========================================================================

template <typename T, typename Allocator = mem::default_allocator<T>>
using slist = single_linked_list<T, Allocator>;

} // namespace vx
