#pragma once

#include <cstdint>
#include <initializer_list>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/_tools/invoke.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"

// #define VX_DOUBLY_LINKED_LIST_

namespace vx {

template <typename T, typename Allocator>
class doubly_linked_list;

namespace _doubly_linked_list_priv {

//=========================================================================
// node
//=========================================================================

template <typename T>
struct node_type
{
    using node_ptr = node_type*;
    using value_type = T;

    node_ptr next;
    node_ptr prev;
    value_type value;

    node_type() = default;
    node_type(const node_type&) = delete;
    node_type& operator=(const node_type&) = delete;

    template <typename Allocator>
    static node_ptr buy_head_node(Allocator& alloc)
    {
        const auto node = alloc.allocate(1);
        if (node)
        {
            mem::construct_in_place(node->next, node);
            mem::construct_in_place(node->prev, node);
        }

        return node;
    }

    template <typename Allocator>
    static void free_node_internal(Allocator& alloc, node_ptr node) noexcept
    {
        mem::destroy_in_place(node->next);
        mem::destroy_in_place(node->prev);
        alloc.deallocate(node, 1);
    }

    template <typename Allocator>
    static void free_node(Allocator& alloc, node_ptr node) noexcept
    {
        mem::destroy_in_place(std::addressof(node->value));
        free_node_internal(alloc, node);
    }

    template <typename Allocator>
    static void free_non_head_node(Allocator& alloc, node_ptr head_node) noexcept
    {
        head_node->prev->next = nullptr;

        auto current_node = head_node->next;
        for (node_ptr next_node; current_node; current_node = next_node)
        {
            next_node = current_node->next;
            free_node(alloc, current_node);
        }
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
struct list_ops
{
    using types = list_types<T>;
    using node = typename types::node;
    using node_ptr = typename types::node_ptr;
    using size_type = typename types::size_type;

    // splice [first, last) before before_node; returns last
    static node_ptr splice(const node_ptr before_node, const node_ptr first, const node_ptr last) noexcept
    {
        VX_ASSERT(before_node != first && before_node != last && first != last);

        const auto first_prev = first->prev;
        first_prev->next = last;
        const auto last_prev = last->prev;
        last_prev->next = before_node;
        const auto before_node_prev = before_node->prev;
        before_node_prev->next = first;

        before_node->prev = last_prev;
        last->prev = first_prev;
        first->prev = before_node_prev;

        return last;
    }

    // splice [first, first->next) before before_node; returns first->next
    static node_ptr splice(const node_ptr before_node, const node_ptr first) noexcept
    {
        VX_ASSERT(before_node != first && first->next != before_node);

        const auto last = first->next;

        const auto first_prev = first->prev;
        first_prev->next = last;
        first->next = before_node;
        const auto before_node_prev = before_node->prev;
        before_node_prev->next = first;

        before_node->prev = first;
        last->prev = first_prev;
        first->prev = before_node_prev;

        return last;
    }

    template <typename Pred>
    static node_ptr merge_same(node_ptr first, node_ptr mid, const node_ptr last, Pred pred)
    {
        VX_ASSERT(first != mid && mid != last);

        node_ptr newfirst;
        if (pred(mid->value, first->value))
        {
            newfirst = mid;
        }
        else
        {
            newfirst = first;
            do
            {
                first = first->next;
                if (first == mid)
                {
                    return newfirst;
                }

            } while (!pred(mid->value, first->value));
        }

        for (;;)
        {
            auto run_start = mid;

            do
            {
                mid = mid->next;

            } while (mid != last && pred(mid->value, first->value));

            splice(first, run_start, mid);
            if (mid == last)
            {
                return newfirst;
            }

            do
            {
                first = first->next;
                if (first == mid)
                {
                    return newfirst;
                }

            } while (!pred(mid->value, first->value));
        }
    }

    template <typename Pred>
    static node_ptr sort(node_ptr& first, const size_type size, Pred pred)
    {
        switch (size)
        {
            case 0: return first;
            case 1: return first->next;
            default: break;
        }

        auto mid = sort(first, size / 2, pred);
        const auto last = sort(mid, size - size / 2, pred);
        first = merge_same(first, mid, last, pred);
        return last;
    }
};

template <typename T, bool Sized = true>
struct list_data : public list_ops<T>
{
    using types = list_types<T>;
    using node_ptr = typename types::node_ptr;
    using size_type = typename types::size_type;

    list_data() noexcept
        : head(), size(0)
    {}

    node_ptr head;
    size_type size;

    size_type get_size() const noexcept
    {
        return size;
    }

    void increment_size(size_type n = 1) noexcept
    {
        size += n;
    }
};

template <typename T>
struct list_data<T, false> : public list_ops<T>
{
    using types = list_types<T>;
    using node_ptr = typename types::node_ptr;
    using size_type = typename types::size_type;

    list_data() noexcept
        : head()
    {}

    node_ptr head;

    size_type get_size() const noexcept
    {
        size_type count = 0;
        for (node_ptr n = head; n != head->prev; n = n->next)
        {
            ++count;
        }
        return count;
    }

    void increment_size(size_type) noexcept
    {
    }
};

template <typename T, bool Sized>
struct value : public list_data<T, Sized>
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

    static constexpr bool sized = Sized;
    using base = list_data<T, Sized>;

    using base::base; // inherit constructor
    using base::decrement_size;
    using base::get_size;
    using base::head;
    using base::increment_size;
    using base::set_size;

    // unlink node at pos from the list
    node_ptr unlink_node(node_ptr node) noexcept
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        decrement_size();
        return node;
    }
};

//=========================================================================
// insert operation
//=========================================================================

template <typename Allocator, bool Sized = true>
class insert_op
{
private:

    using allocator_type = Allocator;
    using value_type = typename allocator_type::value_type;
    using pointer = value_type*;

    using types = list_types<value_type>;
    using size_type = typename types::size_type;

    static constexpr bool sized = Sized;

    Allocator& m_allocator;
    size_type m_added; // if 0, the values of m_head and m_tail are indeterminate
    pointer m_tail;    // points to the most recently appended element; it doesn't have next constructed
    pointer m_head;    // points to the first appended element; it doesn't have prev constructed
    bool m_ok = true;  // Success of the operation

public:

    explicit insert_op(allocator_type& allocator)
        : m_allocator(allocator), m_added(0), m_tail(nullptr), m_head(nullptr), m_ok(true)
    {}

    insert_op(const insert_op&) = delete;
    insert_op& operator=(const insert_op&) = delete;

    // Append count elements constructed from args
    template <typename... Args>
    void append_n(size_type count, const Args&... args)
    {
        if (count <= 0)
        {
            return;
        }

        if (m_added == 0)
        {
            auto ptr = m_allocator.allocate(1);
            if (!ptr)
            {
                m_ok = false;
                return;
            }

            mem::construct_in_place(std::addressof(ptr->value), args...);
            m_head = ptr;
            m_tail = ptr;
            ++m_added;
            --count;
        }

        for (; 0 < count; --count)
        {
            auto ptr = m_allocator.allocate(1);
            if (!ptr)
            {
                m_ok = false;
                return;
            }

            mem::construct_in_place(std::addressof(ptr->value), args...);
            mem::construct_in_place(m_tail->next, ptr);
            mem::construct_in_place(ptr->prev, m_tail);
            m_tail = ptr;
            ++m_added;
        }
    }

    // Append the values in [first, last)
    template <typename IT1, typename IT2>
    void append_range(IT1 first, const IT2 last)
    {
        if (first == last)
        {
            return;
        }

        if (m_added == 0)
        {
            _Newnode._Allocate();
            Allocator_traits::construct(_Al, _STD addressof(ptr->_Myval), *first);
            const auto _Newhead = _STD exchange(ptr, pointer{});
            m_head = _Newhead;
            m_tail = _Newhead;
            ++m_added;
            ++first;
        }

        while (first != last)
        {
            _Newnode._Allocate();
            Allocator_traits::construct(_Al, _STD addressof(ptr->_Myval), *first);
            _Construct_in_place(m_tail->_Next, ptr);
            _Construct_in_place(ptr->_Prev, m_tail);
            m_tail = _STD exchange(ptr, pointer{});
            ++m_added;
            ++first;
        }
    }

    template <class _Val_types>
    pointer _Attach_before(_List_val<_Val_types>& _List_data, const pointer _Insert_before) noexcept
    {
        // Attach the elements in *this before _Insert_before.
        // If *this is empty, returns _Insert_before; otherwise returns a pointer to the first inserted list node.
        // Resets *this to the default-initialized state.

        const auto _Local_added = m_added;
        if (_Local_added == 0)
        {
            return _Insert_before;
        }

        const auto _Local_head = m_head;
        const auto _Local_tail = m_tail;
        const auto _Insert_after = _Insert_before->_Prev;

        _Construct_in_place(_Local_head->_Prev, _Insert_after);
        _Insert_after->_Next = _Local_head;
        _Construct_in_place(_Local_tail->_Next, _Insert_before);
        _Insert_before->_Prev = _Local_tail;

        _List_data._Mysize += _Local_added;
        m_added = 0;
        return _Local_head;
    }

    template <class _Val_types>
    void _Attach_at_end(_List_val<_Val_types>& _List_data) noexcept
    {
        _Attach_before(_List_data, _List_data._Myhead);
    }

    template <class _Val_types>
    void _Attach_head(_List_val<_Val_types>& _List_data)
    {
        _Alloc_construct_ptr<Allocator> _Newnode(_Al);
        _Newnode._Allocate();
        const auto _Local_added = _STD exchange(m_added, size_type{ 0 });
        if (_Local_added == 0)
        {
            _Construct_in_place(ptr->_Next, ptr);
            _Construct_in_place(ptr->_Prev, ptr);
        }
        else
        {
            const auto _Local_head = m_head;
            const auto _Local_tail = m_tail;
            _Construct_in_place(ptr->_Next, _Local_head);
            _Construct_in_place(ptr->_Prev, _Local_tail);
            _Construct_in_place(_Local_head->_Prev, ptr);
            _Construct_in_place(_Local_tail->_Next, ptr);
        }

        _List_data._Mysize = _Local_added;
        _List_data._Myhead = _Newnode._Release();
    }

    ~_List_node_insert_op2()
    {
        if (m_added == 0)
        {
            return;
        }

        _Construct_in_place(m_head->_Prev, pointer{});
        _Construct_in_place(m_tail->_Next, pointer{});
        pointer _Subject = m_head;
        while (_Subject)
        {
            value_type::_Freenode(_Al, _STD exchange(_Subject, _Subject->_Next));
        }
    }
};

//=========================================================================
// iterators
//=========================================================================

template <typename T>
class const_iterator
{
    template <typename, typename>
    friend class doubly_linked_list;

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
    friend class doubly_linked_list;

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

} // namespace _doubly_linked_list_priv

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

private:

    using node = typename types::node;
    using node_ptr = typename types::node_ptr;

    using list_value = _doubly_linked_list_priv::value<T>;

    // rebind the value allocator into a node allocator
    using node_allocator = typename mem::rebind_allocator<allocator_type, node>::type;
    using insert_op = _doubly_linked_list_priv::insert_op<node_allocator>;

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

    void take_head(doubly_linked_list& other) noexcept
    {
        m_data().head = mem::exchange(other.m_data().head, nullptr);
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    doubly_linked_list() noexcept
        : m_storage(_compressed_pair_priv::zero_then_variadic_args_tag{})
    {}

    explicit doubly_linked_list(const allocator_type& alloc)
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {}

    //=========================================================================

    explicit doubly_linked_list(size_type count, const allocator_type& alloc = allocator_type())
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        op.append_n(count);
        op.attach_after(m_data().before_head());
    }

    doubly_linked_list(size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        op.append_n(count, value);
        op.attach_after(m_data().before_head());
    }

    //=========================================================================

    doubly_linked_list(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_after(before_begin(), init.begin(), init.end());
    }

    //=========================================================================

    doubly_linked_list(const doubly_linked_list& other)
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, other.m_allocator())
    {
        insert_op op(m_allocator());
        op.append_range(other.begin(), other.end());
        op.attach_after(m_data().before_head());
    }

    doubly_linked_list(const doubly_linked_list& other, const allocator_type& alloc)
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        insert_op op(m_allocator());
        op.append_range(other.begin(), other.end());
        op.attach_after(m_data().before_head());
    }

    doubly_linked_list(doubly_linked_list&& other) noexcept
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, std::move(other.m_allocator()))
    {
        take_head(other);
    }

    doubly_linked_list(doubly_linked_list&& other, const allocator_type& alloc) noexcept
        : m_storage(_compressed_pair_priv::one_then_variadic_args_tag{}, alloc)
    {
        take_head(other);
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    doubly_linked_list(IT first, IT last, const allocator_type& alloc = allocator_type()) noexcept
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

    ~doubly_linked_list()
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
            node::free(m_allocator(), del_node);
            del_node = next_node;
        }

        return true;
    }

public:

    doubly_linked_list& operator=(const doubly_linked_list& other)
    {
        if (this == &other)
        {
            return *this;
        }

        assign_range(other.begin(), other.end());
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
        take_head(other);
        return *this;
    }

    doubly_linked_list& operator=(std::initializer_list<T> init) noexcept
    {
        assign_range(init.begin(), init.end());
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    bool assign(const doubly_linked_list& other)
    {
        if (this == &other)
        {
            return true;
        }

        return assign_range(other.begin(), other.end());
    }

    bool assign(doubly_linked_list&& other) noexcept
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
            node::free(alloc, node);
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
        node::free(m_allocator(), erase_node);

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
            node::free(m_allocator(), erase_node);
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
                    node::free(alloc, next_node);
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

    void swap(doubly_linked_list& other) noexcept
    {
        mem::swap(m_storage, other.m_storage);
    }

    //=========================================================================
    // merge
    //=========================================================================

private:

    // merge in elements from other, both ordered by pred
    template <typename Pred>
    void merge_impl(doubly_linked_list& other, Pred pred)
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

        auto beforefirst_node = data.before_head();
        auto otherfirst_node = other_data.head;

        for (;;)
        {
            // process 1 splice
            node_ptr first_node;

            for (;;)
            {
                // advance beforefirst_node over elements already in position
                first_node = beforefirst_node->next;
                if (!first_node)
                {
                    // all elements in other are greater than elements in *this, splice them all
                    beforefirst_node->next = otherfirst_node;
                    other_data.head = nullptr;
                    return;
                }

                if (pred(otherfirst_node->value, first_node->value))
                {
                    // otherfirst_node->value is out of order
                    break;
                }

                // first_node->value is already in position; advance
                beforefirst_node = first_node;
            }

            // find the end of the "run" of elements less than first_node->value in other
            auto other_run_end = otherfirst_node;
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

            other_data.head = run_end_node;           // snip out the run from its old position
            beforefirst_node->next = otherfirst_node; // insert into new position
            other_run_end->next = first_node;
            if (!run_end_node)
            {
                return;
            }

            beforefirst_node = first_node;
            otherfirst_node = run_end_node;
        }
    }

public:

    // merge in elements from other, both ordered by operator<
    void merge(doubly_linked_list& other)
    {
        merge_impl(other, std::less<>{});
    }

    // merge in elements from other, both ordered by operator<
    void merge(doubly_linked_list&& other)
    {
        merge_impl(other, std::less<>{});
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
    // splice_after
    //=========================================================================

private:

    // splice other (prev, prev + 2) after pos
    void splice_after_impl(node_ptr pos, doubly_linked_list& other, node_ptr prev) noexcept
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
    template <class IT2>
    void splice_after_impl(const_iterator pos, doubly_linked_list& other, const_iterator first, IT2 last) noexcept
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

        const_iterator lastlast = first;
        do
        {
            lastlast = after;
            ++after;

        } while (after != last);

        const auto extracted_head = first.m_ptr->next;
        first.m_ptr->next = after.m_ptr;
        lastlast.m_ptr->next = pos.m_ptr->next;
        pos.m_ptr->next = extracted_head;
    }

public:

    // splice all of other after pos
    void splice_after(const_iterator pos, doubly_linked_list& other) noexcept
    {
        if (this != &other && !other.empty())
        {
            // worth splicing, do it
            splice_after_impl(pos, other, other.before_begin(), other.end());
        }
    }

    // splice all of other after pos
    void splice_after(const_iterator pos, doubly_linked_list&& other) noexcept
    {
        splice_after(pos, other);
    }

    // splice other (first, first + 2) after pos
    void splice_after(const_iterator pos, doubly_linked_list& other, const_iterator first) noexcept
    {
        splice_after_impl(pos.m_ptr, other, first.m_ptr);
    }

    // splice other (first, first + 2) after pos
    void splice_after(const_iterator pos, doubly_linked_list&& other, const_iterator first) noexcept
    {
        splice_after(pos, other, first);
    }

    // splice other (first, last) after pos
    void splice_after(const_iterator pos, doubly_linked_list& other, const_iterator first, const_iterator last) noexcept
    {
        splice_after_impl(pos, other, first, last);
    }

    // splice other [first, last) after pos
    void splice_after(const_iterator pos, doubly_linked_list&& other, const_iterator first, const_iterator last) noexcept
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

        explicit remove_op(doubly_linked_list& list) noexcept
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

        doubly_linked_list& m_list;
        node_ptr m_head;
        node_ptr* m_tail;
    };

public:

    // erase each element satisfying Pred
    template <typename Pred>
    size_type remove_if(Pred pred)
    {
        remove_op op(*this);

        auto beforefirst = before_begin();
        size_type count = 0;

        for (auto first = begin(); first.m_ptr;)
        {
            if (pred(*first))
            {
                first.m_ptr = op.transfer_back(beforefirst.m_ptr);
                ++count;
            }
            else
            {
                beforefirst = first;
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
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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

} // namespace vx
