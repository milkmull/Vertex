#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/bit.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/memory.hpp"

#include "vertex/std/_hash/hash_common.hpp"
#include "vertex/std/_hash/hash_group.hpp"
#include "vertex/std/_hash/hash_storage.hpp"

// Open-addressing (Swiss-table style) backend.
//
// One allocation: [ ctrl bytes | padding | slots ]
//   ctrl:  cap + group_width bytes. One per slot, the sentinel at ctrl[cap],
//          then clones of the first group_width - 1 bytes so a group load at
//          any offset in [0, cap] reads valid memory.
//   slots: cap slots.
//
// Built for exceptions disabled: failures are reported through expected/success,
// and nothing here is exception-safe.
//   - The allocator's allocate() must return nullptr on failure.
//   - The hasher must mix its output well. H2 is the low 7 bits and H1 starts
//     above them, so an identity hash on small integers clusters badly.

namespace vx {
namespace _flat_hash_priv {

using hash_size_t = _hash_priv::hash_size_t;

//=========================================================================
// probe policies
//=========================================================================

// Probe contract:
//   requires_pow2              true if capacities must be 2^k - 1
//   start(h1, cap)             first group offset, in [0, cap]
//   advance(offset, index, cap) next group offset; `index` is the byte distance
//                              already stepped, including the step being taken
//   wrap_lane(x, cap)          map a ring position (possibly in the clone area)
//                              back to [0, cap]
struct triangular_probe
{
    static constexpr bool requires_pow2 = true;

    static constexpr hash_size_t start(hash_size_t h1, hash_size_t cap) noexcept
    {
        return h1 & cap;
    }

    static constexpr hash_size_t advance(hash_size_t offset, hash_size_t index, hash_size_t cap) noexcept
    {
        return (offset + index) & cap;
    }

    static constexpr hash_size_t wrap_lane(hash_size_t x, hash_size_t cap) noexcept
    {
        return x & cap;
    }
};

using default_probe = triangular_probe;

//=========================================================================
// tombstone cleanup policy
//=========================================================================

// Cleanup contract: cleanup_limit(max_load) < max_load, otherwise cleanup would
// never trigger. When an insert finds no budget left and size <= the limit,
// the table is rebuilt at the same capacity instead of growing.
struct default_cleanup
{
    // == max_load * 25 / 32, split so it can't overflow
    static constexpr hash_size_t cleanup_limit(hash_size_t max_load) noexcept
    {
        return (max_load / 32) * 25 + ((max_load % 32) * 25) / 32;
    }
};

template <typename Sizing, typename Cleanup>
hash_size_t checked_cleanup_limit(hash_size_t cap) noexcept
{
    const hash_size_t max_load = Sizing::max_load(cap);
    VX_ASSERT(max_load > 0);

    const hash_size_t limit = Cleanup::cleanup_limit(max_load);
    VX_ASSERT(limit < max_load);
    return limit;
}

//=========================================================================
// layout
//=========================================================================

// One allocation: [ ctrl bytes | padding | slots ]
struct layout
{
    hash_size_t slots_offset;
    hash_size_t alloc_size;
};

// Cannot overflow for any cap <= max_slots() (see flat_table::max_slots).
inline layout compute_layout(hash_size_t cap, hash_size_t slot_size, hash_size_t slot_align, hash_size_t block_align) noexcept
{
    VX_ASSERT(cap != 0);
    VX_ASSERT(bit::has_single_bit(slot_align));
    VX_ASSERT(bit::has_single_bit(block_align) && slot_align <= block_align);

    layout out;
    out.slots_offset = (cap + group_width + slot_align - 1) & ~(slot_align - 1);
    // multiple of block_align, so the block allocator's unit count is exact
    out.alloc_size = (out.slots_offset + cap * slot_size + block_align - 1) & ~(block_align - 1);
    return out;
}

template <hash_size_t Align>
struct alignas(Align) block_unit
{
    unsigned char bytes[Align];
};

//=========================================================================
// hashing
//=========================================================================

inline hash_t h1_of(hash_t hash) noexcept
{
    return hash >> 7;
}

inline h2_t h2_of(hash_t hash) noexcept
{
    return static_cast<h2_t>(hash & 0x7F);
}

//=========================================================================
// probing
//=========================================================================

template <typename Probe>
class probe_seq
{
public:

    probe_seq(hash_size_t h1, hash_size_t cap) noexcept
        : m_cap(cap)
        , m_offset(Probe::start(h1, cap))
        , m_index(0)
    {}

    // Start of the current group, in [0, cap]. Note it can equal cap, where
    // the group starts on the sentinel. That is fine: lanes are mapped to
    // slots by slot_at() below and the sentinel never matches anything.
    hash_size_t offset() const noexcept
    {
        return m_offset;
    }

    // Slot index for lane `lane` of the current group.
    hash_size_t slot_at(hash_size_t lane) const noexcept
    {
        return Probe::wrap_lane(m_offset + lane, m_cap);
    }

    void next() noexcept
    {
        // Stepping a second time on a table smaller than one group, or past
        // cap + 1 bytes on a larger one, means a group with no EMPTY byte was
        // found, which the load factor rules out. It is a table bug.
        VX_ASSERT(m_index <= m_cap);

        m_index += group_width;
        m_offset = Probe::advance(m_offset, m_index, m_cap);
    }

private:

    hash_size_t m_cap;
    hash_size_t m_offset;
    hash_size_t m_index;
};

// First EMPTY or DELETED slot on the probe path. Precondition: the table has
// room (growth_left > 0, or a tombstone exists), so one is guaranteed.
// Free function so rehash can run it on a block that is not installed yet.
template <typename Probe>
hash_size_t find_first_non_full(const ctrl_t* ctrl, hash_size_t cap, hash_t hash) noexcept
{
    probe_seq<Probe> seq(h1_of(hash), cap);

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
template <typename Probe>
hash_size_t group_before(hash_size_t i, hash_size_t cap) noexcept
{
    return Probe::wrap_lane(i + (cap + 1) - group_width, cap);
}

//=========================================================================
// iterators
//=========================================================================

template <typename Slot>
class flat_iterator
{
    template <typename>
    friend class flat_iterator;

public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::remove_const<Slot>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = Slot*;
    using reference = Slot&;

    flat_iterator() noexcept = default;

    flat_iterator(ctrl_t* ctrl, Slot* slot) noexcept
        : m_ctrl(ctrl)
        , m_slot(slot)
    {}

    // iterator -> const_iterator
    template <typename S2, VX_REQUIRES(std::is_same<const S2, Slot>::value && !std::is_same<S2, Slot>::value)>
    flat_iterator(const flat_iterator<S2>& o) noexcept
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
        VX_ASSERT(slot_is_full(*m_ctrl));
        return m_slot;
    }

    flat_iterator& operator++() noexcept
    {
        VX_ASSERT(*m_ctrl != ctrl_sentinel);

        ++m_ctrl;
        ++m_slot;
        skip_empty_or_deleted();
        return *this;
    }

    flat_iterator operator++(int) noexcept
    {
        flat_iterator it = *this;
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
            const std::uint32_t n = group(m_ctrl).match_full_or_sentinel().trailing_zeros();
            m_ctrl += n;
            m_slot += n;
        }
    }

    ctrl_t* ctrl() const noexcept
    {
        return m_ctrl;
    }

    friend bool operator==(const flat_iterator& a, const flat_iterator& b) noexcept
    {
        return a.m_ctrl == b.m_ctrl;
    }

    friend bool operator!=(const flat_iterator& a, const flat_iterator& b) noexcept
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
struct flat_data
{
    using slot_type = SlotType;

    ctrl_t* ctrl = empty_group(); // never null
    slot_type* slots = nullptr;   // valid only when cap != 0

    hash_size_t cap = 0; // 0, or a Sizing capacity (2^k - 1); also the slot count
    hash_size_t size = 0;
    hash_size_t growth_left = 0; // Sizing::max_load(cap) - size - tombstones

    void reset() noexcept
    {
        ctrl = empty_group();
        slots = nullptr;
        cap = 0;
        size = 0;
        growth_left = 0;
    }

    void acquire(flat_data& other) noexcept
    {
        *this = other;
        other.reset();
    }
};

//=========================================================================
// flat_table
//=========================================================================

template <
    typename SlotTraits,
    typename Hasher,
    typename Eq,
    typename Allocator,
    typename Sizing = _hash_priv::default_sizing,
    typename Probe = default_probe,
    typename Cleanup = default_cleanup>
class flat_table
{
    VX_STATIC_ASSERT_MSG(_hash_priv::valid_flat_sizing<Sizing>(),
        "invalid flat sizing policy (needs max_load(cap) < cap so an EMPTY slot always exists)");

    VX_STATIC_ASSERT_MSG(!Probe::requires_pow2 || Sizing::pow2_capacity,
        "this probe policy requires power-of-two capacities");

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

    using iterator = flat_iterator<
        typename std::conditional<
            slot_traits::mutable_slots,
            value_type,
            const value_type>::type>;

    using const_iterator = flat_iterator<const value_type>;

    // Result of an insert through the front end: the element's position, and
    // whether it was newly inserted (false means an equal key already existed).
    struct insert_result
    {
        iterator it;
        bool inserted;
    };

    static constexpr size_type npos = static_cast<size_type>(-1);

private:

    //=========================================================================
    // internal types
    //=========================================================================

    using slot_type = value_type;
    using table_data = flat_data<slot_type>;

    using sizing = Sizing;
    using probe = Probe;

    // The allocation holds two kinds of data: control bytes and slots.
    // Its alignment must be at least the larger of the two.
    static constexpr size_type block_align = vx::max<size_type>(alignof(slot_type), group_width);

    using block_type = block_unit<block_align>;
    using block_allocator = typename mem::rebind_allocator<Allocator, block_type>::type;

    //=========================================================================
    // storage
    //=========================================================================

    _hash_priv::table_storage<block_allocator, Hasher, Eq, table_data> m_storage;

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

    struct allocation
    {
        ctrl_t* ctrl;
        slot_type* slots;

        explicit operator bool() const noexcept
        {
            return ctrl != nullptr;
        }
    };

    static layout layout_of(size_type cap) noexcept
    {
        return compute_layout(cap, sizeof(slot_type), alignof(slot_type), block_align);
    }

    // The allocator must report failure with nullptr, not by throwing.
    allocation allocate_table(size_type cap) noexcept
    {
        const layout l = layout_of(cap);
        block_type* p = m_allocator().allocate(l.alloc_size / block_align);

        if (!p)
        {
            return { nullptr, nullptr };
        }

        unsigned char* bytes = reinterpret_cast<unsigned char*>(p);
        return { reinterpret_cast<ctrl_t*>(bytes), reinterpret_cast<slot_type*>(bytes + l.slots_offset) };
    }

    void release_table(ctrl_t* ctrl, size_type cap) noexcept
    {
        m_allocator().deallocate(reinterpret_cast<block_type*>(ctrl), layout_of(cap).alloc_size / block_align);
    }

    //=========================================================================
    // copy helpers
    //=========================================================================

    // Deep copy of a non-empty table into `out`. `out` is only written on success.
    bool clone_data(const table_data& src, table_data& out)
    {
        VX_ASSERT(src.cap != 0);

        const allocation a = allocate_table(src.cap);
        if (!a)
        {
            return false;
        }

        // ctrl bytes (including sentinel, clones and padding) are trivially copyable
        mem::copy(a.ctrl, src.ctrl, src.cap + group_width);

        for (size_type i = 0; i < src.cap; ++i)
        {
            if (slot_is_full(a.ctrl[i]))
            {
                slot_traits::construct(a.slots + i, src.slots[i]);
            }
        }

        out.ctrl = a.ctrl;
        out.slots = a.slots;
        out.cap = src.cap;
        out.size = src.size;
        out.growth_left = src.growth_left;

        return true;
    }

    // Only valid on an empty table (during construction, or after clear_and_deallocate).
    success copy_construct_from(const table_data& src)
    {
        VX_ASSERT(m_data().cap == 0);

        if (src.cap != 0)
        {
            const bool ok = clone_data(src, m_data());
            VX_RET_ERR_IF(!ok, err::out_of_memory);
        }

        return success{};
    }

    success assign_from(const flat_table& other)
    {
        if (other.m_data().cap == 0)
        {
            clear_and_deallocate();
        }
        else
        {
            table_data copy;
            const bool ok = clone_data(other.m_data(), copy);
            VX_RET_ERR_IF(!ok, err::out_of_memory);

            // only after the copy succeeded, so failure leaves *this intact
            destroy_and_release();
            m_data() = copy;
        }

        // Copied last. The cloned layout is only valid for the source's hasher,
        // and nothing hashes between the install above and this point.
        m_hasher() = other.m_hasher();
        m_eq() = other.m_eq();

        return success{};
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    flat_table() = default;

    explicit flat_table(const Hasher& h, const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : m_storage(block_allocator(a), h, e)
    {}

    explicit flat_table(size_type n, const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : flat_table(h, e, a)
    {
        const auto ok = reserve(n);
        VX_VERIFY(ok);
    }

    flat_table(const flat_table& other)
        : m_storage(other.m_allocator(), other.m_hasher(), other.m_eq())
    {
        const auto ok = copy_construct_from(other.m_data());
        VX_VERIFY(ok);
    }

    flat_table(flat_table&& other) noexcept
        : m_storage(std::move(other.m_allocator()), std::move(other.m_hasher()), std::move(other.m_eq()))
    {
        m_data().acquire(other.m_data());
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<flat_table, error> create(const flat_table& other)
    {
        flat_table t(other.m_hasher(), other.m_eq(), other.get_allocator());
        const auto ok = t.copy_construct_from(other.m_data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return t;
    }

    static expected<flat_table, error> create(flat_table&& other) noexcept
    {
        return flat_table(std::move(other));
    }

private:

    //=========================================================================
    // destructor helpers
    //=========================================================================

    void destroy_slots() noexcept
    {
        const auto& d = m_data();

        // cap == 0 means no iterations, so no early-out is needed
        for (size_type i = 0; i < d.cap; ++i)
        {
            if (slot_is_full(d.ctrl[i]))
            {
                slot_traits::destroy(d.slots + i);
            }
        }
    }

    // Leaves m_data() dangling. Callers must overwrite or reset it.
    void destroy_and_release() noexcept
    {
        destroy_slots();

        const auto& d = m_data();
        if (d.cap != 0)
        {
            release_table(d.ctrl, d.cap);
        }
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~flat_table()
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

    flat_table& operator=(const flat_table& other)
    {
        if (this == &other)
        {
            return *this;
        }

        const auto ok = assign_from(other);
        VX_VERIFY(ok);
        return *this;
    }

    flat_table& operator=(flat_table&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        // old block is released with the old allocator, before it is replaced
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

    success assign(const flat_table& other)
    {
        VX_ASSERT(this != &other);
        return assign_from(other);
    }

    success assign(flat_table&& other) noexcept
    {
        operator=(std::move(other));
        return success{};
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator unconst(const_iterator it) noexcept
    {
        return iterator_at(index_of(it));
    }

    // Mutable access to a full slot, whatever the iterator's constness. For relocating an
    // element out of the table right before erasing it (merge).
    slot_type& value_at(const_iterator it) noexcept
    {
        const size_type i = index_of(it);
        VX_ASSERT(i < m_data().cap && slot_is_full(m_data().ctrl[i]));
        return m_data().slots[i];
    }

    iterator iterator_at(size_type i) noexcept
    {
        return iterator(m_data().ctrl + i, m_data().slots + i);
    }

    const_iterator iterator_at(size_type i) const noexcept
    {
        return const_iterator(m_data().ctrl + i, m_data().slots + i);
    }

    size_type index_of(const_iterator it) const noexcept
    {
        return static_cast<size_type>(it.ctrl() - m_data().ctrl);
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
        return iterator_at(m_data().cap);
    }

    const_iterator end() const noexcept
    {
        return iterator_at(m_data().cap);
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

private:

    // Allocator limit in bytes. block_allocator counts block_type units, so convert,
    // clamping first so the multiply can't overflow, then cap at PTRDIFF_MAX.
    size_type max_bytes() const noexcept
    {
        const size_type units = mem::allocator_traits<block_allocator>::max_size(m_allocator());
        constexpr size_type limit = static_cast<size_type>(PTRDIFF_MAX) / block_align;
        return vx::min(units, limit) * block_align;
    }

    // Upper bound on the slot count (ctrl bytes + slots must fit in max_bytes()).
    // Capacities from the sizing are compared against this.
    size_type max_slots() const noexcept
    {
        // worst case: group_width clone bytes, padding before the slots, final round-up
        constexpr size_type overhead = group_width + 2 * block_align;
        const size_type bytes = max_bytes();
        return bytes > overhead ? (bytes - overhead) / (sizeof(slot_type) + 1) : 0;
    }

public:

    // Loose upper bound on elements. Allocation can still fail below this.
    size_type max_size() const noexcept
    {
        return sizing::max_load(max_slots());
    }

    //=========================================================================
    // capacity
    //=========================================================================

    // number of elements the table can hold before it must grow
    size_type capacity() const noexcept
    {
        return sizing::max_load(m_data().cap);
    }

    // number of slots: 0, or a capacity produced by the sizing
    size_type slot_count() const noexcept
    {
        return m_data().cap;
    }

    size_type tombstone_count() const noexcept
    {
        const auto& d = m_data();
        return sizing::max_load(d.cap) - d.size - d.growth_left;
    }

private:

    // Tombstones dominate: rebuilding at the same capacity is better than growing.
    bool needs_cleanup() const noexcept
    {
        const auto& d = m_data();
        return d.cap != 0 && tombstone_count() != 0 && d.size <= checked_cleanup_limit<Sizing, Cleanup>(d.cap);
    }

    //=========================================================================
    // hashing
    //=========================================================================

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

    // Returns the slot index of the key, or npos.
    template <typename Q>
    size_type find_index_with_hash(const Q& key, hash_t hash) const noexcept
    {
        const auto& d = m_data();

        const h2_t h2 = h2_of(hash);
        probe_seq<probe> seq(h1_of(hash), d.cap);

        while (true)
        {
            const group g(d.ctrl + seq.offset());

            // candidates: lanes whose ctrl byte equals H2
            for (std::uint32_t lane : g.match(h2))
            {
                const size_type i = seq.slot_at(lane);
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
    size_type find_index(const Q& key) const noexcept
    {
        return find_index_with_hash(key, hash_of(key));
    }

    template <typename Q>
    iterator find_with_hash(const Q& key, hash_t hash) noexcept
    {
        const size_type i = find_index_with_hash(key, hash);
        return i == npos ? end() : iterator_at(i);
    }

    template <typename Q>
    const_iterator find_with_hash(const Q& key, hash_t hash) const noexcept
    {
        const size_type i = find_index_with_hash(key, hash);
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

    // Slot count to move to so that `needed` elements fit, growing from `current`.
    // Pass current = 0 for the tightest fit (shrink_to_fit).
    // Returns 0 if the sizing can't represent it or it exceeds the allocator limit
    // (the caller reports size_error).
    size_type next_slots(size_type current, size_type needed) const noexcept
    {
        VX_ASSERT(needed > 0);

        const size_type cap = sizing::next_capacity(current, needed);
        VX_ASSERT(cap == 0 || (cap > current && sizing::max_load(cap) >= needed));

        return cap <= max_slots() ? cap : 0;
    }

    // Allocates a block of new_cap slots, relocates every element into it by
    // hash, then frees the old block. The old block is untouched on failure
    // (the only failure is the allocation, before anything moves).
    // Serves growth, reserve, cleanup and shrink_to_fit.
    success rehash_to(size_type new_cap) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(new_cap != 0 && sizing::max_load(new_cap) >= d.size);

        const allocation a = allocate_table(new_cap);
        VX_RET_ERR_IF(!a, err::out_of_memory);
        reset_ctrl(a.ctrl, new_cap);

        for (size_type i = 0; i < d.cap; ++i)
        {
            if (!slot_is_full(d.ctrl[i]))
            {
                continue;
            }

            // keys are unique: first free slot, no match(h2), no Eq
            const hash_t hash = hash_of(slot_traits::key(d.slots[i]));
            const size_type t = find_first_non_full<probe>(a.ctrl, new_cap, hash);
            set_ctrl(a.ctrl, new_cap, t, static_cast<ctrl_t>(h2_of(hash)));
            slot_traits::relocate(a.slots + t, d.slots + i);
        }

        if (d.cap != 0)
        {
            release_table(d.ctrl, d.cap);
        }

        d.ctrl = a.ctrl;
        d.slots = a.slots;
        d.cap = new_cap;
        // all tombstones are gone
        d.growth_left = sizing::max_load(new_cap) - d.size;

        return success{};
    }

    // Rebuilds into a fresh block of the same capacity. Not truly in place, so
    // it can fail with out_of_memory.
    success cleanup_tombstones() noexcept
    {
        return rehash_to(m_data().cap);
    }

    // Called when an insert finds growth_left == 0: rebuild at the same capacity
    // if tombstones dominate, otherwise grow.
    success grow_for_insert() noexcept
    {
        if (needs_cleanup())
        {
            return cleanup_tombstones();
        }

        const size_type new_cap = next_slots(m_data().cap, m_data().size + 1);
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
        if (d.cap == 0)
        {
            return;
        }

        destroy_slots();
        reset_ctrl(d.ctrl, d.cap);
        d.size = 0;
        d.growth_left = sizing::max_load(d.cap);
    }

    void clear_and_deallocate() noexcept
    {
        destroy_and_release();
        m_data().reset();
    }

    // Ensures n elements can be inserted with no allocation (and no cleanup).
    success reserve(size_type n) noexcept
    {
        auto& d = m_data();

        // size + growth_left is the budget that is actually spendable. It is lower
        // than max_load(cap) when tombstones are using some of it.
        if (n <= d.size + d.growth_left)
        {
            return success{};
        }

        // fits at this capacity once tombstones are cleared
        if (n <= sizing::max_load(d.cap))
        {
            return cleanup_tombstones();
        }

        const size_type new_cap = next_slots(d.cap, n);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);
        return rehash_to(new_cap);
    }

    success shrink_to_fit() noexcept
    {
        auto& d = m_data();

        if (d.size == 0)
        {
            clear_and_deallocate();
            return success{};
        }

        // current = 0 gives the tightest fit
        const size_type target = next_slots(0, d.size);

        // Only rebuild if it actually shrinks. Equal capacity would just be a cleanup.
        return (target != 0 && target < d.cap) ? rehash_to(target) : success{};
    }

    success compact() noexcept
    {
        return tombstone_count() == 0 ? success{} : cleanup_tombstones();
    }

    void swap(flat_table& other) noexcept
    {
        m_storage.swap(other.m_storage);
    }

    friend void swap(flat_table& a, flat_table& b) noexcept
    {
        a.swap(b);
    }

private:

    //=========================================================================
    // insert helpers
    //=========================================================================

    // Finds the slot for a new key, growing or cleaning first if there is no budget.
    expected<size_type, error> prepare_insert(hash_t hash)
    {
        auto& d = m_data();
        size_type i = find_first_non_full<probe>(d.ctrl, d.cap, hash);

        // No budget and the slot found isn't a tombstone (reusing one is free).
        // `!= DELETED` rather than `== EMPTY` matters for the empty singleton:
        // the probe lands on its sentinel byte, which is neither, and must grow.
        if (d.growth_left == 0 && d.ctrl[i] != ctrl_deleted)
        {
            const auto ok = grow_for_insert();
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok);

            // both cleanup and growth leave at least one slot of budget
            VX_ASSERT(d.growth_left > 0);
            i = find_first_non_full<probe>(d.ctrl, d.cap, hash);
        }

        return i;
    }

    size_type prepare_insert_assume_capacity(hash_t hash) noexcept
    {
        const auto& d = m_data();
        const size_type i = find_first_non_full<probe>(d.ctrl, d.cap, hash);
        VX_ASSERT(d.growth_left > 0 || d.ctrl[i] == ctrl_deleted);
        return i;
    }

    // Commits slot i. The element must already be constructed there.
    void insert_index(size_type i, hash_t hash) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(i < d.cap && !slot_is_full(d.ctrl[i]));
        // only EMPTY spends budget; a tombstone is reused
        d.growth_left -= (d.ctrl[i] == ctrl_empty);
        set_ctrl(d.ctrl, d.cap, i, static_cast<ctrl_t>(h2_of(hash)));
        ++d.size;
    }

public:

    //=========================================================================
    // insert
    //=========================================================================

    // Key known absent (the front end did the find). Args must not alias an
    // element of this table: growing invalidates them before construction.
    template <typename... Args>
    expected<iterator, error> emplace_new(hash_t hash, Args&&... args)
    {
        const auto r = prepare_insert(hash);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());

        const size_type i = r.value();
        slot_traits::construct(m_data().slots + i, std::forward<Args>(args)...);
        insert_index(i, hash);
        return iterator_at(i);
    }

    // requires growth_left > 0 (or a tombstone on the probe path)
    template <typename... Args>
    iterator emplace_new_assume_capacity(hash_t hash, Args&&... args)
    {
        const size_type i = prepare_insert_assume_capacity(hash);
        slot_traits::construct(m_data().slots + i, std::forward<Args>(args)...);
        insert_index(i, hash);
        return iterator_at(i);
    }

private:

    //=========================================================================
    // erase helpers
    //=========================================================================

    // Could a probe window of W bytes have passed over slot i while it was full?
    // Measure the run of non-EMPTY bytes around i (this must be called while ctrl[i]
    // is still FULL). If the run is shorter than W, no window fit inside it, so the
    // slot can become EMPTY. Otherwise it must become a tombstone.
    bool was_never_full(size_type i) const noexcept
    {
        const auto& d = m_data();

        // Tables smaller than one group read through cloned bytes. Always leaving a
        // tombstone is correct there, and cleanup reclaims it.
        if (d.cap < group_width)
        {
            return false;
        }

        const auto after = group(d.ctrl + i).match_empty();
        const auto before = group(d.ctrl + group_before<probe>(i, d.cap)).match_empty();

        return before && after && (after.trailing_zeros() + before.leading_zeros()) < group_width;
    }

    void erase_index(size_type i) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(i < d.cap && slot_is_full(d.ctrl[i]));

        const bool never_full = was_never_full(i); // before ctrl[i] changes

        slot_traits::destroy(d.slots + i);
        set_ctrl(d.ctrl, d.cap, i, never_full ? ctrl_empty : ctrl_deleted);
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

} // namespace _flat_hash_priv
} // namespace vx
