#pragma once

// Closed-addressing backend: bucket array of head pointers, one allocation per
// node. Element addresses are stable across rehash, insert and erase of other
// elements. Exposes the same backend interface as open_table, so it can sit
// behind hash_table_base unchanged.
//
// Layout:
//   buckets: [ head_0 | head_1 | ... | head_cap | END ]      (cap + 2 pointers)
//   node:    [ next | (hash, if CacheHash) | slot ]
//
// The bucket count is cap + 1, the ring size of the growth policy, so
// growth::wrap(hash, cap) is the bucket index and every policy works as is.
// The load limit is growth::max_size(cap), exactly as in open_table.
//
// The extra last bucket holds an END node. Iterators advance by scanning for
// the next non-null bucket, and that scan always stops on END, so no bucket
// count has to be carried in the iterator. end() is read from the array
// itself (never compared against a global), which keeps it DLL-safe.
//
// An unallocated table points at a static two-element array { nullptr, END }:
//   find   -> bucket 0 is empty, reports a miss (no null check)
//   begin  == end
// It is never written: capacity == 0 means growth::max_size(0) == 0, so insert
// takes the grow path first. Test capacity == 0 before writing, never compare
// pointers against the static array.
//
// Iterator invalidation: rehash (growth, reserve, shrink_to_fit) invalidates
// iterators but NOT references or pointers to elements. Erase invalidates only
// the erased element. Advancing end() is undefined.

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/aligned_storage.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/memory.hpp"

#include "vertex/std/_hash/hash_common.hpp"
#include "vertex/std/_hash/hash_storage.hpp"

namespace vx {
namespace _hash_priv {

//=========================================================================
// nodes
//=========================================================================

struct chain_node_base
{
    chain_node_base* next;
};

inline chain_node_base* chain_end_node() noexcept
{
    static chain_node_base end_node = { nullptr };
    return &end_node;
}

inline chain_node_base** chain_empty_buckets() noexcept
{
    static chain_node_base* buckets[2] = { nullptr, chain_end_node() };
    return buckets;
}

template <typename Slot>
struct chain_node : chain_node_base
{
    using slot_type = Slot;

    // union: no automatic construction, but the debugger sees a real Slot
    union
    {
        Slot m_slot;
    };

    chain_node() noexcept
    {}

    ~chain_node()
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
// iterator
//=========================================================================

// (node, bucket) pair. The bucket pointer makes erase_at O(chain length)
// without rehashing the key. Slot is slot_type or const slot_type.
template <typename Node, typename Slot>
class chain_iterator
{
    template <typename, typename>
    friend class chain_iterator;

public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::remove_const<Slot>::type;
    using difference_type = ptrdiff_t;
    using pointer = Slot*;
    using reference = Slot&;

    chain_iterator() noexcept = default;

    chain_iterator(chain_node_base* node, chain_node_base** bucket) noexcept
        : m_node(node)
        , m_bucket(bucket)
    {}

    // iterator -> const_iterator
    template <typename S2, VX_REQUIRES(std::is_same<const S2, Slot>::value && !std::is_same<S2, Slot>::value)>
    chain_iterator(const chain_iterator<Node, S2>& o) noexcept
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
        return slot_ptr();
    }

    chain_iterator& operator++() noexcept
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

    chain_iterator operator++(int) noexcept
    {
        chain_iterator it = *this;
        ++*this;
        return it;
    }

    chain_node_base* node() const noexcept
    {
        return m_node;
    }

    chain_node_base** bucket() const noexcept
    {
        return m_bucket;
    }

    friend bool operator==(const chain_iterator& a, const chain_iterator& b) noexcept
    {
        return a.m_node == b.m_node;
    }

    friend bool operator!=(const chain_iterator& a, const chain_iterator& b) noexcept
    {
        return a.m_node != b.m_node;
    }

private:

    Slot* slot_ptr() const noexcept
    {
        return static_cast<Node*>(m_node)->slot();
    }

    chain_node_base* m_node = nullptr;
    chain_node_base** m_bucket = nullptr;
};

//=========================================================================
// table data
//=========================================================================

template <typename Node>
struct chain_data
{
    chain_node_base** buckets = chain_empty_buckets(); // never null
    size_t capacity = 0;                               // ring size - 1, 0 when unallocated
    size_t size = 0;

    void reset() noexcept
    {
        buckets = chain_empty_buckets();
        capacity = 0;
        size = 0;
    }

    void acquire(chain_data& other) noexcept
    {
        *this = other;
        other.reset();
    }
};

//=========================================================================
// chain_table (closed addressing backend)
//=========================================================================

template <
    typename SlotTraits,
    typename Hasher,
    typename Eq,
    typename Allocator,
    typename Growth = pow2_growth>
class chain_table
{
public:

    using slot_traits = SlotTraits;

private:

    //=========================================================================
    // internal types and policies
    //=========================================================================

    using slot_type = typename slot_traits::slot_type;
    using growth = Growth;
    using node_base = chain_node_base;
    using bucket_ptr = chain_node_base*;

    using node_type = chain_node<slot_type>;
    using node_allocator = typename mem::rebind_allocator<Allocator, node_type>::type;
    using bucket_allocator = typename mem::rebind_allocator<Allocator, bucket_ptr>::type;
    using chain_data_type = chain_data<node_type>;

public:

    //=========================================================================
    // public types
    //=========================================================================

    using allocator_type = Allocator;
    using hasher_type = Hasher;
    using eq_type = Eq;

    using key_type = typename slot_traits::key_type;
    using value_type = slot_type;
    using size_type = size_t;

    using iterator = chain_iterator<
        node_type,
        typename std::conditional<
            slot_traits::mutable_slots,
            slot_type,
            const slot_type>::type>;

    using const_iterator = chain_iterator<node_type, const slot_type>;

    struct insert_result
    {
        iterator it;
        bool inserted;
    };

    //=========================================================================
    // public iterator utilities
    //=========================================================================

    // const_iterator -> iterator (for maps these differ)
    iterator unconst(const_iterator it) const noexcept
    {
        return iterator(it.node(), it.bucket());
    }

    // Mutable access to a full slot, whatever the iterator's constness. For relocating an
    // element out of the table right before erasing it (merge). Touches no table state, so
    // it works with an iterator of any chain_table of the same slot type.
    slot_type& value_at(const_iterator it) noexcept
    {
        VX_ASSERT(it.node() != nullptr);
        return *static_cast<node_type*>(it.node())->slot();
    }

private:

    //=========================================================================
    // storage
    //=========================================================================

    table_storage<node_allocator, Hasher, Eq, chain_data_type> m_storage;

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

    chain_data_type& m_data() noexcept
    {
        return m_storage.data();
    }

    const chain_data_type& m_data() const noexcept
    {
        return m_storage.data();
    }

    //=========================================================================
    // memory helpers
    //=========================================================================

    bucket_allocator bucket_alloc() const noexcept
    {
        return bucket_allocator(m_allocator());
    }

    // Largest cap whose bucket array (cap + 2 pointers) the allocator can hand out,
    // clamped to PTRDIFF_MAX bytes. 0 if not even the minimum fits.
    constexpr size_t max_buckets() const noexcept
    {
        const size_t units = mem::allocator_traits<bucket_allocator>::max_size(bucket_allocator(m_allocator()));
        constexpr size_t limit = static_cast<size_t>(PTRDIFF_MAX) / sizeof(bucket_ptr);
        const size_t n = units < limit ? units : limit;
        return n > 2 ? n - 2 : 0;
    }

    // [0, cap] = nullptr, [cap + 1] = END. nullptr on allocation failure.
    bucket_ptr* allocate_buckets(size_t cap) noexcept
    {
        VX_ASSERT(cap != 0 && cap <= max_buckets());
        bucket_ptr* b = bucket_alloc().allocate(cap + 2);
        if (!b)
        {
            return nullptr;
        }

        for (size_t i = 0; i <= cap; ++i)
        {
            b[i] = nullptr;
        }
        b[cap + 1] = chain_end_node();
        return b;
    }

    void release_buckets(bucket_ptr* b, size_t cap) noexcept
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
    // hashing helpers
    //=========================================================================

    size_t bucket_index(size_t hash) const noexcept
    {
        return growth::wrap(hash, m_data().capacity);
    }

    // the hash of a stored node: read from the node, or recomputed
    size_t node_hash(const node_type* n) const noexcept
    {
        return hash_of(slot_traits::key(*n->slot()));
    }

    //=========================================================================
    // copy helpers
    //=========================================================================

    // Rebuilds every chain in the same order. No hashing, no probing.
    // Requires *this to be unallocated (cleared, or during construction).
    success copy_construct_from(const chain_data_type& src)
    {
        auto& d = m_data();
        VX_ASSERT(d.capacity == 0);

        if (src.capacity == 0)
        {
            return success{};
        }

        VX_RET_ERR_IF(src.capacity > max_buckets(), err::size_error);

        bucket_ptr* nb = allocate_buckets(src.capacity);
        VX_RET_ERR_IF(!nb, err::out_of_memory);

        for (size_t i = 0; i <= src.capacity; ++i)
        {
            node_base** tail = &nb[i];
            for (node_base* p = src.buckets[i]; p; p = p->next)
            {
                const node_type* s = static_cast<const node_type*>(p);
                node_type* n = allocate_node();
                if (!n)
                {
                    destroy_nodes(nb, src.capacity); // what was linked so far
                    release_buckets(nb, src.capacity);
                }
                VX_RET_ERR_IF(!n, err::out_of_memory);

                slot_traits::construct(n->slot(), *s->slot());
                *tail = n;
                tail = &n->next;
            }
        }

        d.buckets = nb;
        d.capacity = src.capacity;
        d.size = src.size;
        return success{};
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    chain_table() noexcept(noexcept(allocator_type()))
    {}

    explicit chain_table(const Hasher& h, const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : m_storage(a, h, e)
    {}

    explicit chain_table(size_t n, const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : chain_table(h, e, a)
    {
        const auto ok = reserve(n);
        VX_VERIFY(ok);
    }

    chain_table(const chain_table& other)
        : m_storage(other.m_allocator(), other.m_hasher(), other.m_eq())
    {
        const auto ok = copy_construct_from(other.m_data());
        VX_VERIFY(ok);
    }

    chain_table(chain_table&& other) noexcept
        : m_storage(std::move(other.m_allocator()), std::move(other.m_hasher()), std::move(other.m_eq()))
    {
        m_data().acquire(other.m_data());
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<chain_table, error> create(const chain_table& other)
    {
        chain_table t(other.m_hasher(), other.m_eq(), other.m_allocator());
        const auto ok = t.copy_construct_from(other.m_data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return t;
    }

    static expected<chain_table, error> create(chain_table&& other) noexcept
    {
        chain_table t(std::move(other.m_hasher()), std::move(other.m_eq()), std::move(other.m_allocator()));
        t.m_data().acquire(other.m_data());
        return t;
    }

private:

    //=========================================================================
    // destructor helpers
    //=========================================================================

    // destroys and frees every node in buckets [0, cap]
    void destroy_nodes(bucket_ptr* b, size_t cap) noexcept
    {
        for (size_t i = 0; i <= cap; ++i)
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

    void destroy_and_release() noexcept
    {
        auto& d = m_data();
        if (d.capacity != 0)
        {
            destroy_nodes(d.buckets, d.capacity);
            release_buckets(d.buckets, d.capacity);
        }
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~chain_table()
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
    // assignment operators
    //=========================================================================

    chain_table& operator=(const chain_table& other)
    {
        if (this == &other)
        {
            return *this;
        }

        const auto ok = assign(other);
        VX_VERIFY(ok);
        return *this;
    }

    chain_table& operator=(chain_table&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

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
    success assign(const chain_table& other)
    {
        VX_ASSERT(this != &other);

        chain_table tmp(other.m_hasher(), other.m_eq(), get_allocator());
        const auto ok = tmp.copy_construct_from(other.m_data());
        VX_RET_ERR_IF(!ok, ok);

        swap(tmp);
        return success{};
    }

    success assign(chain_table&& other) noexcept
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
        node_base** b = m_data().buckets + m_data().capacity + 1;
        return It(*b, b);
    }

public:

    //=========================================================================
    // iterators
    //=========================================================================

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

    size_t size() const noexcept
    {
        return m_data().size;
    }

    // Loose upper bound on elements, set by the node allocator. Allocation can
    // still fail below this, with size_error or out_of_memory.
    constexpr size_t max_size() const noexcept
    {
        return mem::allocator_traits<node_allocator>::max_size(m_allocator());
    }

    //=========================================================================
    // capacity
    //=========================================================================

    // number of elements the table can hold before it must grow
    size_t capacity() const noexcept
    {
        return growth::max_size(m_data().capacity);
    }

    // number of buckets, 0 when unallocated
    size_t bucket_count() const noexcept
    {
        return m_data().capacity == 0 ? 0 : m_data().capacity + 1;
    }

    //=========================================================================
    // hashing
    //=========================================================================

    template <typename Q>
    size_t hash_of(const Q& key) const noexcept
    {
        return m_storage.hash_of(key);
    }

    //=========================================================================
    // lookup
    //=========================================================================

private:

    template <typename It, typename Q>
    It find_impl(const Q& key, size_t hash) const noexcept
    {
        node_base** b = m_data().buckets + bucket_index(hash);
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
    iterator find_with_hash(const Q& key, size_t hash) noexcept
    {
        return find_impl<iterator>(key, hash);
    }

    template <typename Q>
    const_iterator find_with_hash(const Q& key, size_t hash) const noexcept
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

    // Capacity to grow into so that `needed` elements fit. Returns 0 if no valid capacity
    // satisfies it under the allocator ceiling (report size_error).
    size_t grow_capacity(size_t needed) const noexcept
    {
        const size_t cap = growth::next_capacity(m_data().capacity, needed, max_buckets());
        VX_ASSERT(cap == 0 || (cap > m_data().capacity && growth::max_size(cap) >= needed && cap <= max_buckets()));
        return cap;
    }

    // Smallest valid capacity that holds n elements. 0 if n == 0 or nothing fits.
    size_t slots_for(size_t n) const noexcept
    {
        return n == 0 ? 0 : growth::next_capacity(0, n, max_buckets());
    }

    // Allocates a new bucket array and relinks every node into it. No element is
    // constructed, moved or destroyed. Hashes are recomputed unless cached.
    // The old state is untouched on failure. Serves growth, reserve and shrink_to_fit.
    success relink_to(size_t new_cap)
    {
        VX_RET_ERR_IF(new_cap > max_buckets(), err::size_error);

        bucket_ptr* nb = allocate_buckets(new_cap);
        VX_RET_ERR_IF(!nb, err::out_of_memory);

        auto& d = m_data();

        if (d.capacity != 0)
        {
            for (size_t i = 0; i <= d.capacity; ++i)
            {
                node_base* p = d.buckets[i];
                while (p)
                {
                    node_base* next = p->next;
                    const size_t h = node_hash(static_cast<const node_type*>(p));
                    node_base*& head = nb[growth::wrap(h, new_cap)];
                    p->next = head;
                    head = p;
                    p = next;
                }
            }
            release_buckets(d.buckets, d.capacity);
        }

        d.buckets = nb;
        d.capacity = new_cap;
        return success{};
    }

    // links a node whose slot is already constructed
    iterator link_node(node_type* n, size_t hash) noexcept
    {
        auto& d = m_data();
        node_base** b = d.buckets + bucket_index(hash);
        n->next = *b;
        *b = n;
        ++d.size;
        return iterator(n, b);
    }

public:

    //=========================================================================
    // memory
    //=========================================================================

    // destroys elements, keeps the bucket array
    void clear() noexcept
    {
        auto& d = m_data();
        if (d.capacity == 0)
        {
            return;
        }

        destroy_nodes(d.buckets, d.capacity);
        for (size_t i = 0; i <= d.capacity; ++i)
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
    success reserve(size_t n)
    {
        auto& d = m_data();

        if (n <= growth::max_size(d.capacity))
        {
            return success{};
        }

        VX_RET_ERR_IF(n > max_size(), err::size_error);

        const size_t new_cap = grow_capacity(n);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);

        return relink_to(new_cap);
    }

    success shrink_to_fit()
    {
        auto& d = m_data();

        if (d.size == 0)
        {
            clear_and_deallocate();
            return success{};
        }

        const size_t target = slots_for(d.size);
        VX_ASSERT(target != 0);

        // Only rebuild if it actually shrinks.
        return target < d.capacity ? relink_to(target) : success{};
    }

    void swap(chain_table& other) noexcept
    {
        m_storage.swap(other.m_storage);
    }

    //=========================================================================
    // insert
    //=========================================================================

    // Key known absent (the front end did the find). Unlike the open backend, args
    // MAY alias an element: nodes do not move when the table grows.
    template <typename... Args>
    expected<iterator, error> emplace_new(size_t hash, Args&&... args)
    {
        auto& d = m_data();

        if (d.size >= growth::max_size(d.capacity))
        {
            const size_t new_cap = grow_capacity(d.size + 1);
            VX_RET_UNEXPECTED_ERR_IF(new_cap == 0, err::size_error);

            const auto ok = relink_to(new_cap);
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
    iterator emplace_new_assume_capacity(size_t hash, Args&&... args)
    {
        VX_ASSERT(m_data().size < growth::max_size(m_data().capacity));

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
        VX_ASSERT(d.capacity != 0 && it.node() != nullptr && it.node() != d.buckets[d.capacity + 1]);

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

//=========================================================================
// swap
//=========================================================================

template <typename P, typename H, typename E, typename A, typename G>
void swap(chain_table<P, H, E, A, G>& a, chain_table<P, H, E, A, G>& b) noexcept
{
    a.swap(b);
}

} // namespace _hash_priv
} // namespace vx
