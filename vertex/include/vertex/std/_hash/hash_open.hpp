#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/std/bit.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/memory.hpp"

#include "vertex/std/_hash/hash_common.hpp"
#include "vertex/std/_hash/hash_group.hpp"
#include "vertex/std/_hash/hash_storage.hpp"

namespace vx {
namespace _hash_priv {

//=========================================================================
// layout
//=========================================================================

// One allocation: [ ctrl bytes | padding | slots ]
struct layout
{
    size_t slots_offset;
    size_t alloc_size;
    size_t alloc_align;
};

// Known layout should not overflow
inline void compute_safe_layout(size_t cap, size_t slot_size, size_t slot_align, layout& out) noexcept
{
    VX_ASSERT(cap != 0);
    VX_ASSERT(bit::has_single_bit(slot_align));

    out.slots_offset = (cap + group_width + slot_align - 1) & ~(slot_align - 1);
    out.alloc_align = slot_align > group_width ? slot_align : group_width;
    out.alloc_size = out.slots_offset + cap * slot_size;
    // multiple of alloc_align, so the block allocator's unit count is exact
    out.alloc_size = (out.alloc_size + out.alloc_align - 1) & ~(out.alloc_align - 1);
}

template <size_t Align>
struct alignas(Align) block_unit
{
    unsigned char bytes[Align];
};

//=========================================================================
// probing
//=========================================================================

template <typename Growth>
class probe_seq
{
public:

    probe_seq(size_t h1, size_t cap) noexcept
        : m_cap(cap)
        , m_offset(Growth::wrap(h1, cap))
        , m_index(0)
    {}

    // Start of the current group, in [0, cap]. Note it can equal cap, where
    // the group starts on the sentinel. That is fine: lanes are mapped to
    // slots by slot_at() below and the sentinel never matches anything.
    size_t offset() const noexcept
    {
        return m_offset;
    }

    // Slot index for lane `lane` of the current group.
    size_t slot_at(size_t lane) const noexcept
    {
        VX_ASSERT(m_offset + lane < 2 * (m_cap + 1));
        return Growth::wrap_lane(m_offset + lane, m_cap);
    }

    // Number of ctrl bytes walked past so far (a multiple of W).
    size_t index() const noexcept
    {
        return m_index;
    }

    void next() noexcept
    {
        // Stepping a second time on a table smaller than one group, or past
        // cap+1 bytes on a larger one, means a group with no EMPTY byte was
        // found, which the load factor rules out. It is a table bug.
        VX_ASSERT(m_index <= m_cap);

        m_index += group_width;
        m_offset = Growth::advance(m_offset, m_index, m_cap);
    }

private:

    size_t m_cap;
    size_t m_offset;
    size_t m_index;
};

// First EMPTY or DELETED slot on the probe path. Precondition: the table has
// room (growth_left > 0, or a tombstone exists), so one is guaranteed.
// Free function so rehash can run it on a block that is not installed yet.
template <typename Growth>
size_t find_first_non_full(const ctrl_t* ctrl, size_t cap, size_t hash) noexcept
{
    probe_seq<Growth> seq(h1_of(hash), cap);

    while (true)
    {
        const group g(ctrl + seq.offset());
        const auto mask = g.match_empty_or_deleted();
        if (mask)
        {
            return seq.slot_at(mask.lowest());
        }
        seq.next();
    }
}

// start of the group that ends at slot i - 1 (erase's "was it ever surrounded" check)
template <typename Growth>
size_t group_before(size_t i, size_t cap) noexcept
{
    return Growth::wrap_lane(i + (cap + 1) - group_width, cap);
}

//=========================================================================
// iterators
//=========================================================================

template <typename Slot>
class open_iterator
{
    template <typename>
    friend class open_iterator;

public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::remove_const<Slot>::type;
    using difference_type = ptrdiff_t;
    using pointer = Slot*;
    using reference = Slot&;

    open_iterator() noexcept = default;

    open_iterator(ctrl_t* ctrl, Slot* slot) noexcept
        : m_ctrl(ctrl)
        , m_slot(slot)
    {}

    // iterator -> const_iterator
    template <typename S2, VX_REQUIRES(std::is_same<const S2, Slot>::value && !std::is_same<S2, Slot>::value)>
    open_iterator(const open_iterator<S2>& o) noexcept
        : m_ctrl(o.m_ctrl)
        , m_slot(o.m_slot)
    {}

    reference operator*() const noexcept
    {
        VX_ASSERT(slot_is_full(*m_ctrl));
        return *m_slot;
    }

    pointer operator->() const noexcept
    {
        return m_slot;
    }

    open_iterator& operator++() noexcept
    {
        VX_ASSERT(*m_ctrl != ctrl_sentinel);

        ++m_ctrl;
        ++m_slot;
        skip_empty_or_deleted();
        return *this;
    }

    open_iterator operator++(int) noexcept
    {
        open_iterator it = *this;
        ++*this;
        return it;
    }

    // Moves forward until the current byte is FULL or the SENTINEL. Each step
    // jumps over a whole run of EMPTY/DELETED lanes. Used by begin().
    void skip_empty_or_deleted() noexcept
    {
        while (slot_is_empty_or_deleted(*m_ctrl))
        {
            // all lanes empty/deleted gives trailing_zeros == width, so we skip a
            // whole group, and the sentinel can never be jumped over
            const uint32_t n = group(m_ctrl).match_full_or_sentinel().trailing_zeros();
            m_ctrl += n;
            m_slot += n;
        }
    }

    ctrl_t* ctrl() const noexcept
    {
        return m_ctrl;
    }

    friend bool operator==(const open_iterator& a, const open_iterator& b) noexcept
    {
        return a.m_ctrl == b.m_ctrl;
    }

    friend bool operator!=(const open_iterator& a, const open_iterator& b) noexcept
    {
        return a.m_ctrl != b.m_ctrl;
    }

private:

    ctrl_t* m_ctrl = nullptr;
    Slot* m_slot = nullptr;
};

//=========================================================================
// table data
//=========================================================================

template <typename SlotType>
struct open_data
{
    using slot_type = SlotType;

    ctrl_t* ctrl = empty_group(); // never null
    slot_type* slots = nullptr;   // valid only when capacity != 0

    size_t capacity = 0;
    size_t size = 0;
    size_t growth_left = 0; // load::max_size(capacity) - size - tombstones

    void reset() noexcept
    {
        ctrl = empty_group();
        slots = nullptr;
        capacity = 0;
        size = 0;
        growth_left = 0;
    }

    void acquire(open_data& other) noexcept
    {
        *this = other;
        other.reset();
    }
};

//=========================================================================
// open_table (open addressing backend)
//=========================================================================

template <
    typename SlotTraits,
    typename Hasher,
    typename Eq,
    typename Allocator,
    typename Growth = _hash_priv::pow2_growth>
class open_table
{
public:

    using slot_traits = SlotTraits;

private:

    //=========================================================================
    // internal types and policies
    //=========================================================================

    using slot_traits = SlotTraits;
    using slot_type = typename slot_traits::slot_type;
    using table_data = open_data<slot_type>;
    using growth = Growth;

    //=========================================================================
    // implementation constants
    //=========================================================================

    // We are storing 2 types in the allocation: group control bytes and slots.
    // The alignment of the allocation must be at least the max of the two.
    static constexpr size_t block_align =
        alignof(slot_type) > group_width
        ? alignof(slot_type)
        : group_width;

    using block_type = block_unit<block_align>;
    using block_allocator = typename mem::rebind_allocator<Allocator, block_type>::type;

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

    using iterator = open_iterator<
        typename std::conditional<
            slot_traits::mutable_slots,
            slot_type,
            const slot_type>::type>;

    using const_iterator = open_iterator<const slot_type>;

    static constexpr size_t npos = static_cast<size_t>(-1);

    struct insert_result
    {
        iterator it;
        bool inserted;
    };

    //=========================================================================
    // public iterator utilities
    //=========================================================================

    iterator unconst(const_iterator it) noexcept
    {
        return iterator_at(index_of(it));
    }

    // Mutable access to a full slot, whatever the iterator's constness. For relocating an
    // element out of the table right before erasing it (merge).
    slot_type& value_at(const_iterator it) noexcept
    {
        const size_t i = index_of(it);
        VX_ASSERT(i < m_data().capacity && slot_is_full(m_data().ctrl[i]));
        return m_data().slots[i];
    }

private:

    //=========================================================================
    // storage
    //=========================================================================

    table_storage<block_allocator, Hasher, Eq, table_data> m_storage;

    //=========================================================================
    // accessors
    //=========================================================================

    block_allocator& m_allocator() noexcept
    {
        return m_storage.alloc();
    }

    const block_allocator& m_allocator() const noexcept
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

    unsigned char* allocate_block(size_t bytes) noexcept
    {
        VX_ASSERT(bytes % block_align == 0);
        block_type* p = m_allocator().allocate(bytes / block_align);
        return reinterpret_cast<unsigned char*>(p);
    }

    void release_block(size_t cap) noexcept
    {
        layout l;
        compute_safe_layout(cap, sizeof(slot_type), alignof(slot_type), l);
        m_allocator().deallocate(reinterpret_cast<block_type*>(m_data().ctrl), l.alloc_size / block_align);
    }

    //=========================================================================
    // copy helpers
    //=========================================================================

    struct data_pointer_pair
    {
        ctrl_t* ctrl;
        slot_type* slots;

        bool ok() const noexcept
        {
            return ctrl != nullptr;
        }
    };

    // allocate new table and copy over each full slot
    data_pointer_pair allocate_and_copy_from(const table_data& src)
    {
        layout l;
        compute_safe_layout(src.capacity, sizeof(slot_type), alignof(slot_type), l);

        unsigned char* block = allocate_block(l.alloc_size);
        if (!block)
        {
            return { nullptr, nullptr };
        }

        ctrl_t* ctrl = reinterpret_cast<ctrl_t*>(block);
        slot_type* slots = reinterpret_cast<slot_type*>(block + l.slots_offset);

        // ctrl section is trivially copyable
        mem::copy(ctrl, src.ctrl, src.capacity + group_width);

        // copy the values over
        for (size_t i = 0; i < src.capacity; ++i)
        {
            if (slot_is_full(ctrl[i]))
            {
                slot_traits::construct(slots + i, src.slots[i]);
            }
        }

        return { ctrl, slots };
    }

    success copy_construct_from(const table_data& src)
    {
        auto& d = m_data();

        // should only be called after being cleared or during cinstruction
        VX_ASSERT(d.capacity == 0);

        if (src.capacity == 0)
        {
            return success{};
        }

        const auto ptrs = allocate_and_copy_from(src);
        VX_RET_ERR_IF(!ptrs.ok(), err::out_of_memory);

        d.ctrl = ptrs.ctrl;
        d.slots = ptrs.slots;

        d.capacity = src.capacity;
        d.size = src.size;
        d.growth_left = src.growth_left;

        return success{};
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    open_table() noexcept(noexcept(allocator_type()))
    {}

    explicit open_table(const Hasher& h, const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : m_storage(a, h, e)
    {}

    explicit open_table(size_t n, const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : open_table(h, e, a)
    {
        const auto ok = reserve(n);
        VX_VERIFY(ok);
    }

    open_table(const open_table& other)
        : m_storage(other.m_allocator(), other.m_hasher(), other.m_eq())
    {
        const auto ok = copy_construct_from(other.m_data());
        VX_VERIFY(ok);
    }

    open_table(open_table&& other) noexcept
        : m_storage(std::move(other.m_allocator()), std::move(other.m_hasher()), std::move(other.m_eq()))
    {
        m_data().acquire(other.m_data());
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<open_table, error> create(const open_table& other)
    {
        open_table t(other.m_hasher(), other.m_eq(), other.m_allocator());
        const auto ok = t.copy_construct_from(other.m_data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return t;
    }

    static expected<open_table, error> create(open_table&& other) noexcept
    {
        open_table t(std::move(other.m_hasher()), std::move(other.m_eq()), std::move(other.m_allocator()));
        t.m_data().acquire(other.m_data());
        return t;
    }

private:

    //=========================================================================
    // destructor helpers
    //=========================================================================

    void destroy_slots() noexcept
    {
        auto& d = m_data();

        if (d.capacity == 0)
        {
            return;
        }

        for (size_t i = 0; i < d.capacity; ++i)
        {
            if (slot_is_full(d.ctrl[i]))
            {
                slot_traits::destroy(d.slots + i);
            }
        }
    }

    void destroy_and_release() noexcept
    {
        destroy_slots();

        if (m_data().capacity != 0)
        {
            release_block(m_data().capacity);
        }
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~open_table()
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
    // assignment helpers
    //=========================================================================

private:

    success assign_from(const table_data& src) noexcept
    {
        auto& d = m_data();

        if (src.capacity == 0)
        {
            clear_and_deallocate();
            return success{};
        }

        const auto ptrs = allocate_and_copy_from(src);
        VX_RET_ERR_IF(!ptrs.ok(), err::out_of_memory);

        destroy_and_release();

        d.ctrl = ptrs.ctrl;
        d.slots = ptrs.slots;

        d.capacity = src.capacity;
        d.size = src.size;
        d.growth_left = src.growth_left;

        return success{};
    }

public:

    //=========================================================================
    // assignment operators
    //=========================================================================

    open_table& operator=(const open_table& other)
    {
        if (this == &other)
        {
            return *this;
        }

        const auto ok = assign(other);
        VX_VERIFY(ok);
        return *this;
    }

    open_table& operator=(open_table&& other) noexcept
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

    success assign(const open_table& other)
    {
        VX_ASSERT(this != &other);
        return assign_from(other.m_data());
    }

    success assign(open_table&& other) noexcept
    {
        operator=(std::move(other));
        return success{};
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator iterator_at(size_t i) noexcept
    {
        return iterator(m_data().ctrl + i, m_data().slots + i);
    }

    const_iterator iterator_at(size_t i) const noexcept
    {
        return const_iterator(m_data().ctrl + i, m_data().slots + i);
    }

    size_t index_of(const_iterator it) const noexcept
    {
        return static_cast<size_t>(it.ctrl() - m_data().ctrl);
    }

    iterator begin() noexcept
    {
        iterator it = iterator_at(0);
        it.skip_empty_or_deleted();
        return it;
    }

    const_iterator begin() const noexcept
    {
        const_iterator it = iterator_at(0);
        it.skip_empty_or_deleted();
        return it;
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator_at(m_data().capacity);
    }

    const_iterator end() const noexcept
    {
        return iterator_at(m_data().capacity);
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

private:

    // Allocator limit in bytes. block_allocator counts block_type units, so convert,
    // clamping first so the multiply can't overflow, then cap at PTRDIFF_MAX.
    constexpr size_t max_bytes() const noexcept
    {
        const size_t units = mem::allocator_traits<block_allocator>::max_size(m_allocator());
        constexpr size_t limit = static_cast<size_t>(PTRDIFF_MAX) / block_align;
        return (units < limit ? units : limit) * block_align;
    }

public:

    // Loose upper bound on slots (not elements: the element limit is lower by the
    // load factor). Allocation can still fail below this, with size_error or out_of_memory.
    constexpr size_t max_size() const noexcept
    {
        // worst case: group_width clone bytes, padding before the slots, final round-up
        constexpr size_t overhead = group_width + 2 * block_align;
        const size_t bytes = max_bytes();
        return bytes > overhead ? (bytes - overhead) / (sizeof(slot_type) + 1) : 0;
    }

    //=========================================================================
    // capacity
    //=========================================================================

    // number of elements the table can hold before it must grow
    size_t capacity() const noexcept
    {
        const auto& d = m_data();
        return growth::max_size(d.capacity);
    }

    // raw ring size (slots), 0 or a capacity produced by the growth slot_traits
    size_t slot_count() const noexcept
    {
        const auto& d = m_data();
        return d.capacity;
    }

    size_t tombstone_count() const noexcept
    {
        const auto& d = m_data();
        return growth::max_size(d.capacity) - d.size - d.growth_left;
    }

    bool needs_cleanup() const noexcept
    {
        const auto& d = m_data();
        return d.capacity != 0 && tombstone_count() != 0 && d.size <= cleanup_limit_of<Growth>(d.capacity);
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

    // Returns the slot index of the key, or npos.
    template <typename Q>
    size_t find_index_with_hash(const Q& key, size_t hash) const noexcept
    {
        const auto& d = m_data();

        const h2_t h2 = h2_of(hash);
        probe_seq<growth> seq(h1_of(hash), d.capacity);

        while (true)
        {
            const group g(d.ctrl + seq.offset());

            // candidates: lanes whose ctrl byte equals H2
            for (uint32_t lane : g.match(h2))
            {
                const size_t i = seq.slot_at(lane);
                if (m_eq()(key, slot_traits::key(d.slots[i])))
                {
                    return i;
                }
            }

            // an EMPTY lane means no probe path continues past this group
            if (g.match_empty())
            {
                return npos;
            }

            seq.next();
        }
    }

public:

    template <typename Q>
    size_t find_index(const Q& key) const noexcept
    {
        return find_index_with_hash(key, hash_of(key));
    }

    template <typename Q>
    iterator find_with_hash(const Q& key, size_t hash) noexcept
    {
        const size_t i = find_index_with_hash(key, hash);
        return i == npos ? end() : iterator_at(i);
    }

    template <typename Q>
    const_iterator find_with_hash(const Q& key, size_t hash) const noexcept
    {
        const size_t i = find_index_with_hash(key, hash);
        return i == npos ? end() : iterator_at(i);
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
        return find_index(key) != npos;
    }

private:

    //=========================================================================
    // rehash
    //=========================================================================

    // Capacity to grow into so that `needed` elements fit. Returns 0 if no valid capacity
    // satisfies it under the allocator ceiling (report size_error). After allocating:
    //     m_data().growth_left = growth::max_size(new_cap) - m_data().size;
    size_t grow_capacity(size_t needed) const noexcept
    {
        const size_t cap = growth::next_capacity(m_data().capacity, needed, max_size());
        VX_ASSERT(cap == 0 || (cap > m_data().capacity && growth::max_size(cap) >= needed && cap <= max_size()));
        return cap;
    }

    // Smallest valid slot count that holds n elements. 0 if n == 0 or nothing fits under max_size().
    size_t slots_for(size_t n) const noexcept
    {
        return n == 0 ? 0 : growth::next_capacity(0, n, max_size());
    }

    // Allocates a block of new_cap slots, relocates every element into it by
    // hash, then frees the old block. The old block is untouched on failure.
    // Serves growth, reserve and shrink_to_fit.
    success rehash_to(size_t new_cap)
    {
        layout l;
        compute_safe_layout(new_cap, sizeof(slot_type), alignof(slot_type), l);

        unsigned char* block = allocate_block(l.alloc_size);
        VX_RET_ERR_IF(!block, err::out_of_memory);

        ctrl_t* new_ctrl = reinterpret_cast<ctrl_t*>(block);
        slot_type* new_slots = reinterpret_cast<slot_type*>(block + l.slots_offset);
        reset_ctrl(new_ctrl, new_cap);

        auto& d = m_data();

        if (d.capacity != 0)
        {
            for (size_t i = 0; i < d.capacity; ++i)
            {
                if (!slot_is_full(d.ctrl[i]))
                {
                    continue;
                }

                // keys are unique: first free slot, no match(h2), no Eq
                const size_t hash = hash_of(slot_traits::key(d.slots[i]));
                const size_t t = find_first_non_full<growth>(new_ctrl, new_cap, hash);
                set_ctrl(new_ctrl, new_cap, t, static_cast<ctrl_t>(h2_of(hash)));
                slot_traits::relocate(new_slots + t, d.slots + i);
            }

            release_block(d.capacity);
        }

        d.ctrl = new_ctrl;
        d.slots = new_slots;
        d.capacity = new_cap;
        d.growth_left = growth::max_size(new_cap) - d.size;

        return success{};
    }

    success cleanup_tombstones()
    {
        return rehash_to(m_data().capacity);
    }

    // Called when an insert finds growth_left == 0: clean in place if tombstones
    // dominate, otherwise grow.
    success grow_for_insert()
    {
        if (needs_cleanup())
        {
            return cleanup_tombstones();
        }

        const size_t new_cap = grow_capacity(m_data().size + 1);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);
        return rehash_to(new_cap);
    }

public:

    //=========================================================================
    // memory
    //=========================================================================

    // destroys elements, keeps the allocation
    void clear() noexcept
    {
        auto& d = m_data();
        if (d.capacity == 0)
        {
            return;
        }

        destroy_slots();
        reset_ctrl(d.ctrl, d.capacity);
        d.size = 0;
        d.growth_left = growth::max_size(d.capacity);
    }

    void clear_and_deallocate() noexcept
    {
        destroy_and_release();
        m_data().reset();
    }

    // Ensures n elements can be inserted with no allocation (and no cleanup).
    success reserve(size_t n)
    {
        auto& d = m_data();

        // size + growth_left is the budget that is actually spendable. It is lower
        // than max_size(capacity) when tombstones are using some of it.
        if (n <= d.size + d.growth_left)
        {
            return success{};
        }

        if (d.capacity != 0 && n <= growth::max_size(d.capacity))
        {
            return cleanup_tombstones();
        }

        const size_t new_cap = grow_capacity(n);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);

        return rehash_to(new_cap);
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

        // Only rebuild if it actually shrinks. Equal capacity would just be a cleanup.
        return target < d.capacity ? rehash_to(target) : success{};
    }

    success compact()
    {
        return tombstone_count() == 0 ? success{} : cleanup_tombstones();
    }

    void swap(open_table& other) noexcept
    {
        m_storage.swap(other.m_storage);
    }

private:

    //=========================================================================
    // insert helpers
    //=========================================================================

    // Insert: prepare (find slot, grow if needed), construct, commit.
    expected<size_t, error> prepare_insert(size_t hash)
    {
        auto& d = m_data();
        size_t i = find_first_non_full<growth>(d.ctrl, d.capacity, hash);

        // != DELETED, not == EMPTY: on the singleton and on a full tiny table the
        // probe returns a position that is not free, and that must also grow.
        if (d.growth_left == 0 && d.ctrl[i] != ctrl_deleted)
        {
            const auto ok = grow_for_insert();
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
            i = find_first_non_full<growth>(d.ctrl, d.capacity, hash);
        }

        return i;
    }

    size_t prepare_insert_assume_capacity(size_t hash) noexcept
    {
        auto& d = m_data();
        const size_t i = find_first_non_full<growth>(d.ctrl, d.capacity, hash);
        VX_ASSERT(d.growth_left > 0 || d.ctrl[i] == ctrl_deleted);
        return i;
    }

    // Commits slot i. The element must already be constructed there.
    void insert_index(size_t i, size_t hash) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(i < d.capacity && !slot_is_full(d.ctrl[i]));
        d.growth_left -= (d.ctrl[i] == ctrl_empty); // only EMPTY spends budget
        set_ctrl(d.ctrl, d.capacity, i, static_cast<ctrl_t>(h2_of(hash)));
        ++d.size;
    }

public:

    //=========================================================================
    // insert
    //=========================================================================

    // Key known absent (the front end did the find). Args must not alias an
    // element of this table: growing invalidates them before construction.
    template <typename... Args>
    expected<iterator, error> emplace_new(size_t hash, Args&&... args)
    {
        const auto r = prepare_insert(hash);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());

        const size_t i = r.value();
        slot_traits::construct(m_data().slots + i, std::forward<Args>(args)...);
        insert_index(i, hash);
        return iterator_at(i);
    }

    // requires growth_left > 0 (or a tombstone on the probe path)
    template <typename... Args>
    iterator emplace_new_assume_capacity(size_t hash, Args&&... args)
    {
        const size_t i = prepare_insert_assume_capacity(hash);
        slot_traits::construct(m_data().slots + i, std::forward<Args>(args)...);
        insert_index(i, hash);
        return iterator_at(i);
    }

private:

    //=========================================================================
    // erase helpers
    //=========================================================================

    void erase_index(size_t i) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(i < d.capacity && slot_is_full(d.ctrl[i]));
        slot_traits::destroy(d.slots + i);

        // Could any probe window of W bytes have passed over slot i while it was
        // full? Measure the run of non-EMPTY bytes around i. If it is shorter than
        // W, no window fit inside it, so EMPTY is safe. Otherwise leave a tombstone.
        const auto after = group(d.ctrl + i).match_empty();
        const auto before = group(d.ctrl + group_before<growth>(i, d.capacity)).match_empty();
        const bool never_full = before && after && (after.trailing_zeros() + before.leading_zeros()) < group_width;

        set_ctrl(d.ctrl, d.capacity, i, never_full ? ctrl_empty : ctrl_deleted);
        d.growth_left += never_full;
        --d.size;
    }

public:

    //=========================================================================
    // erase
    //=========================================================================

    // Leaves the iterator's position non-full, so a loop can ++it afterwards. No other
    // element moves, so erase_if can be: if (pred(*it)) erase_at(it); ++it;
    void erase_at(const_iterator it) noexcept
    {
        erase_index(index_of(it));
    }
};

//=========================================================================
// swap
//=========================================================================

template <typename P, typename H, typename E, typename A, typename T>
void swap(open_table<P, H, E, A, T>& a, open_table<P, H, E, A, T>& b) noexcept
{
    a.swap(b);
}

} // namespace _hash_priv
} // namespace vx
