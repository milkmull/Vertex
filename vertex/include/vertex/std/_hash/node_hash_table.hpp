#pragma once

// Closed-addressing backend: bucket array of head pointers, one allocation per
// node. Element addresses are stable across rehash, insert and erase of other
// elements. Exposes the same backend interface as flat_table, so it can sit
// behind hash_table_base unchanged.
//
// Layout:
//   buckets: [ head_0 | head_1 | ... | head_cap | END ]      (cap + 2 pointers)
//   node:    [ next | slot ]

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/memory.hpp"

#include "vertex/std/_hash/hash_common.hpp"
#include "vertex/std/_hash/hash_storage.hpp"

namespace vx {
namespace _node_hash_priv {

using hash_size_t = _hash_priv::hash_size_t;

//=========================================================================
// Node sizing
//=========================================================================

using default_sizing = _hash_priv::pow2_sizing_t<_hash_priv::load_1_0>;

//=========================================================================
// nodes
//=========================================================================

struct node_base
{
    node_base* next;
};

inline node_base* node_end_node() noexcept
{
    static node_base end_node = { nullptr };
    return &end_node;
}

inline node_base** node_empty_buckets() noexcept
{
    static node_base* buckets[2] = { nullptr, node_end_node() };
    return buckets;
}

template <typename Slot>
struct node : node_base
{
    using slot_type = Slot;

    // union: no automatic construction, but the debugger sees a real Slot
    union
    {
        Slot m_slot;
    };

    node() noexcept
    {}

    ~node()
    {}

    Slot* slot() noexcept
    {
        return &m_slot;
    }

    const Slot* slot() const noexcept
    {
        return &m_slot;
    }
};

//=========================================================================
// bucketing
//=========================================================================

// Bucket index of a hash. Capacities are 2^k - 1, so the mask is the ring wrap.
inline hash_size_t bucket_of(hash_t hash, hash_size_t cap) noexcept
{
    return static_cast<hash_size_t>(hash) & cap;
}

//=========================================================================
// iterators
//=========================================================================

// (node, bucket) pair. The bucket pointer makes erase_at O(node length)
// without rehashing the key. Slot is slot_type or const slot_type.
template <typename Node, typename Slot>
class node_iterator
{
    template <typename, typename>
    friend class node_iterator;

public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::remove_const<Slot>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = Slot*;
    using reference = Slot&;

    node_iterator() noexcept = default;

    node_iterator(node_base* node, node_base** bucket) noexcept
        : m_node(node)
        , m_bucket(bucket)
    {}

    // iterator -> const_iterator
    template <typename S2, VX_REQUIRES(std::is_same<const S2, Slot>::value && !std::is_same<S2, Slot>::value)>
    node_iterator(const node_iterator<Node, S2>& o) noexcept
        : m_node(o.m_node)
        , m_bucket(o.m_bucket)
    {}

    reference operator*() const noexcept
    {
        VX_ASSERT(m_node != nullptr);
        return *slot_ptr();
    }

    pointer operator->() const noexcept
    {
        VX_ASSERT(m_node != nullptr);
        return slot_ptr();
    }

    node_iterator& operator++() noexcept
    {
        VX_ASSERT(m_node != nullptr);

        m_node = m_node->next;
        if (m_node == nullptr)
        {
            // the last bucket holds END, so this stops
            do
            {
                ++m_bucket;

            } while (*m_bucket == nullptr);

            m_node = *m_bucket;
        }
        return *this;
    }

    node_iterator operator++(int) noexcept
    {
        node_iterator it = *this;
        ++*this;
        return it;
    }

    node_base* node() const noexcept
    {
        return m_node;
    }

    node_base** bucket() const noexcept
    {
        return m_bucket;
    }

    friend bool operator==(const node_iterator& a, const node_iterator& b) noexcept
    {
        return a.m_node == b.m_node;
    }

    friend bool operator!=(const node_iterator& a, const node_iterator& b) noexcept
    {
        return a.m_node != b.m_node;
    }

private:

    Slot* slot_ptr() const noexcept
    {
        return static_cast<Node*>(m_node)->slot();
    }

    node_base* m_node = nullptr;
    node_base** m_bucket = nullptr;
};

//=========================================================================
// table data
//=========================================================================

template <typename Node>
struct node_data
{
    node_base** buckets = node_empty_buckets(); // never null

    hash_size_t cap = 0; // ring size - 1, 0 when unallocated
    hash_size_t size = 0;

    void reset() noexcept
    {
        buckets = node_empty_buckets();
        cap = 0;
        size = 0;
    }

    void acquire(node_data& other) noexcept
    {
        *this = other;
        other.reset();
    }
};

//=========================================================================
// node_table
//=========================================================================

template <
    typename SlotTraits,
    typename Hasher,
    typename Eq,
    typename Allocator,
    typename Sizing = default_sizing>
class node_table
{
    VX_STATIC_ASSERT_MSG(_hash_priv::valid_sizing<Sizing>(),
        "invalid sizing: needs max_load(0) == 0, 2^k - 1 capacities, "
        "min_capacity <= max_capacity, and no overflow of cap + 1");

    VX_STATIC_ASSERT_MSG(Sizing::pow2_capacity,
        "bucket_of masks the hash, so node_table requires power-of-two capacities");

public:

    //=========================================================================
    // public types
    //=========================================================================

    using slot_traits = SlotTraits;
    using allocator_type = Allocator;
    using hasher_type = Hasher;
    using eq_type = Eq;

    using key_type = typename slot_traits::key_type;
    using value_type = typename slot_traits::slot_type;
    using size_type = hash_size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = node_iterator<
        node<value_type>,
        typename std::conditional<
            slot_traits::mutable_slots,
            value_type,
            const value_type>::type>;

    using const_iterator = node_iterator<node<value_type>, const value_type>;

    // Result of an insert through the front end: the element's position, and
    // whether it was newly inserted (false means an equal key already existed).
    struct insert_result
    {
        iterator it;
        bool inserted;
    };

private:

    //=========================================================================
    // internal types
    //=========================================================================

    using slot_type = value_type;
    using node_type = node<slot_type>;
    using table_data = node_data<node_type>;

    using sizing = Sizing;
    using bucket_ptr = node_base*;

    using node_allocator = typename mem::rebind_allocator<Allocator, node_type>::type;
    using bucket_allocator = typename mem::rebind_allocator<Allocator, bucket_ptr>::type;

    //=========================================================================
    // storage
    //=========================================================================

    _hash_priv::table_storage<node_allocator, Hasher, Eq, table_data> m_storage;

    //=========================================================================
    // accessors
    //=========================================================================

    node_allocator& m_allocator() noexcept
    {
        return m_storage.alloc();
    }

    const node_allocator& m_allocator() const noexcept
    {
        return m_storage.alloc();
    }

    //=========================================================================

    Hasher& m_hasher() noexcept
    {
        return m_storage.hasher();
    }

    const Hasher& m_hasher() const noexcept
    {
        return m_storage.hasher();
    }

    //=========================================================================

    Eq& m_eq() noexcept
    {
        return m_storage.eq();
    }

    const Eq& m_eq() const noexcept
    {
        return m_storage.eq();
    }

    //=========================================================================

    table_data& m_data() noexcept
    {
        return m_storage.data();
    }

    const table_data& m_data() const noexcept
    {
        return m_storage.data();
    }

    //=========================================================================
    // memory block helpers
    //=========================================================================

    bucket_allocator bucket_alloc() const noexcept
    {
        return bucket_allocator(m_allocator());
    }

    // Largest cap whose bucket array (cap + 2 pointers) the allocator can hand out,
    // clamped to PTRDIFF_MAX bytes. 0 if not even the minimum fits.
    size_type max_buckets() const noexcept
    {
        const size_t units = mem::allocator_traits<bucket_allocator>::max_size(bucket_alloc());
        constexpr size_t limit = static_cast<size_t>(PTRDIFF_MAX) / sizeof(bucket_ptr);
        const size_t n = units < limit ? units : limit;
        return static_cast<size_type>(n > 2 ? n - 2 : 0);
    }

    // [0, cap] = nullptr, [cap + 1] = END. nullptr on allocation failure.
    bucket_ptr* allocate_buckets(size_type cap) noexcept
    {
        VX_ASSERT(cap != 0 && cap <= max_buckets());

        bucket_ptr* b = bucket_alloc().allocate(cap + 2);
        if (!b)
        {
            return nullptr;
        }

        for (size_type i = 0; i <= cap; ++i)
        {
            b[i] = nullptr;
        }
        b[cap + 1] = node_end_node();
        return b;
    }

    void release_buckets(bucket_ptr* b, size_type cap) noexcept
    {
        bucket_alloc().deallocate(b, cap + 2);
    }

    node_type* allocate_node() noexcept
    {
        node_type* n = m_allocator().allocate(1);
        if (n)
        {
            n->next = nullptr;
        }
        return n;
    }

    void free_node(node_type* n) noexcept
    {
        m_allocator().deallocate(n, 1);
    }

    //=========================================================================
    // copy helpers
    //=========================================================================

    // Rebuilds every node in the same order. No hashing, no probing.
    // Only valid on an empty table (during construction, or after clear_and_deallocate).
    success copy_construct_from(const table_data& src)
    {
        auto& d = m_data();
        VX_ASSERT(d.cap == 0);

        if (src.cap == 0)
        {
            return success{};
        }

        VX_RET_ERR_IF(src.cap > max_buckets(), err::size_error);

        bucket_ptr* nb = allocate_buckets(src.cap);
        VX_RET_ERR_IF(!nb, err::out_of_memory);

        for (size_type i = 0; i <= src.cap; ++i)
        {
            node_base** tail = &nb[i];
            for (node_base* p = src.buckets[i]; p; p = p->next)
            {
                const node_type* s = static_cast<const node_type*>(p);
                node_type* n = allocate_node();
                if (!n)
                {
                    destroy_nodes(nb, src.cap); // what was linked so far
                    release_buckets(nb, src.cap);
                    return err::out_of_memory;
                }

                slot_traits::construct(n->slot(), *s->slot());
                *tail = n;
                tail = &n->next;
            }
        }

        d.buckets = nb;
        d.cap = src.cap;
        d.size = src.size;
        return success{};
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    node_table() = default;

    explicit node_table(const Hasher& h, const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : m_storage(node_allocator(a), h, e)
    {}

    explicit node_table(size_type n, const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : node_table(h, e, a)
    {
        const auto ok = reserve_assume_empty(n);
        VX_VERIFY(ok);
    }

    node_table(const node_table& other)
        : m_storage(other.m_allocator(), other.m_hasher(), other.m_eq())
    {
        const auto ok = copy_construct_from(other.m_data());
        VX_VERIFY(ok);
    }

    node_table(node_table&& other) noexcept
        : m_storage(std::move(other.m_allocator()), std::move(other.m_hasher()), std::move(other.m_eq()))
    {
        m_data().acquire(other.m_data());
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<node_table, error> create(const node_table& other)
    {
        node_table t(other.m_hasher(), other.m_eq(), other.get_allocator());
        const auto ok = t.copy_construct_from(other.m_data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return t;
    }

    static expected<node_table, error> create(node_table&& other) noexcept
    {
        return node_table(std::move(other));
    }

private:

    //=========================================================================
    // destructor helpers
    //=========================================================================

    // destroys and frees every node in buckets [0, cap]
    void destroy_nodes(bucket_ptr* b, size_type cap) noexcept
    {
        for (size_type i = 0; i <= cap; ++i)
        {
            node_base* p = b[i];
            while (p)
            {
                node_base* next = p->next;
                node_type* n = static_cast<node_type*>(p);
                slot_traits::destroy(n->slot());
                free_node(n);
                p = next;
            }
        }
    }

    // Leaves m_data() dangling. Callers must overwrite or reset it.
    void destroy_and_release() noexcept
    {
        const auto& d = m_data();
        if (d.cap != 0)
        {
            destroy_nodes(d.buckets, d.cap);
            release_buckets(d.buckets, d.cap);
        }
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~node_table()
    {
        destroy_and_release();
    }

    //=========================================================================
    // observers
    //=========================================================================

    allocator_type get_allocator() const noexcept
    {
        return static_cast<allocator_type>(m_allocator());
    }

    Hasher hash_function() const
    {
        return m_hasher();
    }

    Eq key_eq() const
    {
        return m_eq();
    }

    //=========================================================================
    // assignment
    //=========================================================================

    node_table& operator=(const node_table& other)
    {
        if (this == &other)
        {
            return *this;
        }

        const auto ok = assign(other);
        VX_VERIFY(ok);
        return *this;
    }

    node_table& operator=(node_table&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        // old buckets and nodes are released with the old allocator, before it is replaced
        destroy_and_release();
        m_allocator() = std::move(other.m_allocator());
        m_hasher() = std::move(other.m_hasher());
        m_eq() = std::move(other.m_eq());
        m_data().acquire(other.m_data());

        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    // Strong guarantee: build the copy first, then swap. Keeps this table's allocator.
    success assign(const node_table& other)
    {
        VX_ASSERT(this != &other);

        node_table tmp(other.m_hasher(), other.m_eq(), get_allocator());
        const auto ok = tmp.copy_construct_from(other.m_data());
        VX_RET_ERR_IF(!ok, ok);

        swap(tmp);
        return success{};
    }

    success assign(node_table&& other) noexcept
    {
        operator=(std::move(other));
        return success{};
    }

private:

    //=========================================================================
    // iterator helpers
    //=========================================================================

    template <typename It>
    It begin_impl() const noexcept
    {
        node_base** b = m_data().buckets;
        while (*b == nullptr) // stops on END at the latest
        {
            ++b;
        }
        return It(*b, b);
    }

    template <typename It>
    It end_impl() const noexcept
    {
        node_base** b = m_data().buckets + m_data().cap + 1;
        return It(*b, b);
    }

public:

    //=========================================================================
    // iterators
    //=========================================================================

    // const_iterator -> iterator (for maps these differ)
    iterator unconst(const_iterator it) const noexcept
    {
        return iterator(it.node(), it.bucket());
    }

    // Mutable access to a full slot, whatever the iterator's constness. For relocating an
    // element out of the table right before erasing it (merge). Touches no table state, so
    // it works with an iterator of any node_table of the same slot type.
    slot_type& value_at(const_iterator it) noexcept
    {
        VX_ASSERT(it.node() != nullptr);
        return *static_cast<node_type*>(it.node())->slot();
    }

    iterator begin() noexcept
    {
        return begin_impl<iterator>();
    }

    const_iterator begin() const noexcept
    {
        return begin_impl<const_iterator>();
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return end_impl<iterator>();
    }

    const_iterator end() const noexcept
    {
        return end_impl<const_iterator>();
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_data().size == 0;
    }

    size_type size() const noexcept
    {
        return m_data().size;
    }

    // Loose upper bound on elements, set by the node allocator. Allocation can
    // still fail below this, with size_error or out_of_memory.
    size_type max_size() const noexcept
    {
        return static_cast<size_type>(mem::allocator_traits<node_allocator>::max_size(m_allocator()));
    }

    //=========================================================================
    // capacity
    //=========================================================================

    // number of elements the table can hold before it must grow
    size_type capacity() const noexcept
    {
        return sizing::max_load(m_data().cap);
    }

    // number of buckets, 0 when unallocated
    size_type bucket_count() const noexcept
    {
        return m_data().cap == 0 ? 0 : m_data().cap + 1;
    }

    //=========================================================================
    // hashing
    //=========================================================================

private:

    // the hash of a stored node: always recomputed from the key
    hash_t node_hash(const node_type* n) const noexcept
    {
        return hash_of(slot_traits::key(*n->slot()));
    }

public:

    template <typename Q>
    hash_t hash_of(const Q& key) const noexcept
    {
        return m_storage.hash_of(key);
    }

    //=========================================================================
    // lookup
    //=========================================================================

private:

    template <typename It, typename Q>
    It find_impl(const Q& key, hash_t hash) const noexcept
    {
        const auto& d = m_data();

        node_base** b = d.buckets + bucket_of(hash, d.cap);
        for (node_base* p = *b; p != nullptr; p = p->next)
        {
            const node_type* n = static_cast<const node_type*>(p);
            if (m_eq()(key, slot_traits::key(*n->slot())))
            {
                return It(p, b);
            }
        }
        return end_impl<It>();
    }

public:

    template <typename Q>
    iterator find_with_hash(const Q& key, hash_t hash) noexcept
    {
        return find_impl<iterator>(key, hash);
    }

    template <typename Q>
    const_iterator find_with_hash(const Q& key, hash_t hash) const noexcept
    {
        return find_impl<const_iterator>(key, hash);
    }

    template <typename Q>
    iterator find(const Q& key) noexcept
    {
        return find_with_hash(key, hash_of(key));
    }

    template <typename Q>
    const_iterator find(const Q& key) const noexcept
    {
        return find_with_hash(key, hash_of(key));
    }

    template <typename Q>
    bool contains(const Q& key) const noexcept
    {
        return find(key) != end();
    }

private:

    //=========================================================================
    // rehash
    //=========================================================================

    // Capacity to move to so that `needed` elements fit.
    // Pass current = 0 for the tightest fit (shrink_to_fit).
    // Returns 0 if the sizing can't represent it or it exceeds the allocator limit
    size_type next_slots(size_type current, size_type needed) const noexcept
    {
        VX_ASSERT(needed > 0);
        const size_type cap = sizing::next_capacity(current, needed);
        VX_ASSERT(cap == 0 || (cap > current && sizing::max_load(cap) >= needed));
        return cap <= max_buckets() ? cap : 0;
    }

    // Allocates a new bucket array and relinks every node into it. No element is
    // constructed, moved or destroyed. Hashes are recomputed.
    // The old state is untouched on failure. Serves growth, reserve and shrink_to_fit.
    success relink_to(size_type new_cap)
    {
        auto& d = m_data();
        VX_ASSERT(new_cap != 0 && new_cap <= max_buckets() && sizing::max_load(new_cap) >= d.size);

        bucket_ptr* nb = allocate_buckets(new_cap);
        VX_RET_ERR_IF(!nb, err::out_of_memory);

        if (d.cap != 0)
        {
            for (size_type i = 0; i <= d.cap; ++i)
            {
                node_base* p = d.buckets[i];
                while (p)
                {
                    node_base* next = p->next;
                    const hash_t h = node_hash(static_cast<const node_type*>(p));
                    node_base*& head = nb[bucket_of(h, new_cap)];
                    p->next = head;
                    head = p;
                    p = next;
                }
            }
            release_buckets(d.buckets, d.cap);
        }

        d.buckets = nb;
        d.cap = new_cap;
        return success{};
    }

public:

    //=========================================================================
    // memory
    //=========================================================================

    // destroys elements, keeps the bucket array
    void clear() noexcept
    {
        auto& d = m_data();
        if (d.cap == 0)
        {
            return;
        }

        destroy_nodes(d.buckets, d.cap);
        for (size_type i = 0; i <= d.cap; ++i)
        {
            d.buckets[i] = nullptr;
        }
        d.size = 0;
    }

    void clear_and_deallocate() noexcept
    {
        destroy_and_release();
        m_data().reset();
    }

    // Ensures n elements can be inserted with no bucket reallocation.
    success reserve(size_type n)
    {
        auto& d = m_data();

        if (n <= sizing::max_load(d.cap))
        {
            return success{};
        }

        VX_RET_ERR_IF(n > max_size(), err::size_error);

        const size_type new_cap = next_slots(d.cap, n);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);
        return relink_to(new_cap);
    }

    // For a freshly constructed table (no allocation yet, nothing ever inserted).
    // Allocates once and installs the bucket array. Nothing to relink or free.
    success reserve_assume_empty(size_type n) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(d.cap == 0 && d.size == 0);

        if (n == 0)
        {
            return success{};
        }

        const size_type new_cap = next_slots(0, n);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);

        bucket_ptr* nb = allocate_buckets(new_cap);
        VX_RET_ERR_IF(!nb, err::out_of_memory);

        d.buckets = nb;
        d.cap = new_cap;

        return success{};
    }

    success shrink_to_fit()
    {
        auto& d = m_data();

        if (d.size == 0)
        {
            clear_and_deallocate();
            return success{};
        }

        // current = 0 gives the tightest fit
        const size_type target = next_slots(0, d.size);

        // Only rebuild if it actually shrinks.
        return (target != 0 && target < d.cap) ? relink_to(target) : success{};
    }

    void swap(node_table& other) noexcept
    {
        m_storage.swap(other.m_storage);
    }

private:

    //=========================================================================
    // insert helpers
    //=========================================================================

    // Called when an insert finds no budget left: grow the bucket array.
    success grow_for_insert()
    {
        const size_type new_cap = next_slots(m_data().cap, m_data().size + 1);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);
        return relink_to(new_cap);
    }

    // links a node whose slot is already constructed
    iterator link_node(node_type* n, hash_t hash) noexcept
    {
        auto& d = m_data();
        node_base** b = d.buckets + bucket_of(hash, d.cap);
        n->next = *b;
        *b = n;
        ++d.size;
        return iterator(n, b);
    }

public:

    //=========================================================================
    // insert
    //=========================================================================

    // Key known absent (the front end did the find). Unlike flat_table, args
    // MAY alias an element: nodes do not move when the table grows.
    template <typename... Args>
    expected<iterator, error> emplace_new(hash_t hash, Args&&... args)
    {
        auto& d = m_data();

        if (d.size >= sizing::max_load(d.cap))
        {
            const auto ok = grow_for_insert();
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        }

        node_type* n = allocate_node();
        VX_RET_UNEXPECTED_ERR_IF(!n, err::out_of_memory);

        slot_traits::construct(n->slot(), std::forward<Args>(args)...);
        return link_node(n, hash);
    }

    // Requires size < capacity(): the bucket array will not grow. Each element
    // still needs a node, so a failed node allocation is a VX_VERIFY here.
    template <typename... Args>
    iterator emplace_new_assume_capacity(hash_t hash, Args&&... args)
    {
        VX_ASSERT(m_data().size < sizing::max_load(m_data().cap));

        node_type* n = allocate_node();
        VX_VERIFY(n);

        slot_traits::construct(n->slot(), std::forward<Args>(args)...);
        return link_node(n, hash);
    }

    //=========================================================================
    // erase
    //=========================================================================

    // Frees the node. Only `it` is invalidated: advance a copy first if the
    // position is still needed (the front end does).
    void erase_at(const_iterator it) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(d.cap != 0 && it.node() != nullptr && it.node() != d.buckets[d.cap + 1]);

        node_base** link = it.bucket();
        node_base* target = it.node();
        while (*link != target)
        {
            link = &(*link)->next;
        }
        *link = target->next;

        node_type* n = static_cast<node_type*>(target);
        slot_traits::destroy(n->slot());
        free_node(n);
        --d.size;
    }
};

} // namespace _node_hash_priv
} // namespace vx
