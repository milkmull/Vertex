#pragma once

// Open-addressing (Swiss-style) backend: ctrl bytes, probing, layout, iterator
// and open_table. Shared policies live in hash_common.hpp, allocator/hasher/eq
// storage in hash_storage.hpp.

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/aligned_storage.hpp"
#include "vertex/std/bit.hpp" // path is a guess
#include "vertex/std/expected.hpp"
#include "vertex/std/memory.hpp" // path is a guess

#include "hash_common.hpp"
#include "hash_group.hpp"
#include "hash_storage.hpp"

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

// Returns false if the total size overflows size_t.
inline bool compute_layout(size_t cap, size_t slot_size, size_t slot_align, layout& out) noexcept
{
    VX_ASSERT(cap != 0);
    VX_ASSERT(bit::has_single_bit(slot_align));

    // ctrl bytes = cap slots + 1 sentinel + (width - 1) clones = cap + group_width.
    // The round-up below adds up to slot_align - 1 more, so check for both
    // before doing either addition.
    if (cap > SIZE_MAX - group_width - (slot_align - 1))
    {
        return false;
    }

    const size_t ctrl_bytes = cap + group_width;
    const size_t offset = (ctrl_bytes + slot_align - 1) & ~(slot_align - 1);

    // offset + cap * slot_size must fit
    if (slot_size != 0 && cap > (SIZE_MAX - offset) / slot_size)
    {
        return false;
    }

    out.slots_offset = offset;
    out.alloc_size = offset + cap * slot_size;
    out.alloc_align = slot_align > group_width ? slot_align : group_width;

    if (out.alloc_size > SIZE_MAX - (out.alloc_align - 1))
    {
        return false;
    }

    // multiple of alloc_align, so the block allocator's unit count is exact
    out.alloc_size = (out.alloc_size + out.alloc_align - 1) & ~(out.alloc_align - 1);

    return true;
}

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

//=========================================================================
// ctrl array
//=========================================================================

// What an unallocated table points at. capacity == 0, so ctrl[capacity] is
// ctrl[0], which is the sentinel. The other width-1 bytes are EMPTY, so:
//   - a find loads this group, matches nothing, sees an EMPTY, and reports a miss
//   - begin() lands on the sentinel, so begin() == end() with no null check
// It is never written: an unallocated table has growth_left == 0, so any insert
// takes the grow path first. Anything that writes ctrl (clear, erase, rehash)
// must check capacity != 0. Never compare a pointer against it (not DLL-safe),
// test capacity == 0 instead.
// Growth policies must therefore accept cap == 0 (wrap(x, 0) == 0).
inline ctrl_t* empty_group() noexcept
{
    // The initializer below is written for width 8. Any other width would
    // zero-fill the tail, and 0x00 is a FULL byte with h2 == 0, so a lookup on
    // an empty table would call eq() on a null slot array.
    VX_STATIC_ASSERT_MSG(group_width == 8, "empty_group() initializer assumes group_width == 8");

    // clang-format off
    alignas(group_width) static const ctrl_t group_bytes[group_width] = {
        ctrl_sentinel,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty
    };
    // clang-format on
    return const_cast<ctrl_t*>(group_bytes);
}

// ctrl layout: [ cap slots | sentinel | min(cap, W-1) clones | phantoms ]
// For cap >= W - 1 there are no phantoms. For smaller caps the remaining bytes
// stay EMPTY forever (see has_phantom_bytes). Nothing may write them.
inline void reset_ctrl(ctrl_t* ctrl, size_t cap) noexcept
{
    VX_ASSERT(cap != 0);
    mem::set(ctrl, static_cast<int>(ctrl_empty), cap + group_width);
    ctrl[cap] = ctrl_sentinel;
}

// Re-establish the sentinel and clones after bulk-rewriting ctrl[0..cap).
inline void restore_ctrl_clones(ctrl_t* ctrl, size_t cap) noexcept
{
    VX_ASSERT(cap != 0);
    constexpr size_t max_clones = group_width - 1;
    const size_t n = cap < max_clones ? cap : max_clones;

    ctrl[cap] = ctrl_sentinel;
    for (size_t i = 0; i < n; ++i)
    {
        ctrl[cap + 1 + i] = ctrl[i];
    }
}

// Writes ctrl[i] and its clone, if it has one. Every ctrl write after reset_ctrl
// goes through this (insert writes H2, erase writes EMPTY or DELETED).
// Works for any capacity: the clone of slot i (i < width - 1) lives at cap + 1 + i.
inline void set_ctrl(ctrl_t* ctrl, size_t cap, size_t i, ctrl_t value) noexcept
{
    VX_ASSERT(i < cap);

    constexpr size_t cloned = group_width - 1;
    // For i >= cloned the "mirror" is just i again (harmless, branch-free cmov).
    const size_t mirror = i < cloned ? cap + 1 + i : i;

    ctrl[i] = value;
    ctrl[mirror] = value;
}


//=========================================================================
// probing
//=========================================================================

template <typename Growth>
class probe_traits
{
    static size_t wrap(size_t x, size_t cap) noexcept
    {
        return x % (cap + 1);
    }

    static size_t wrap_lane(size_t x, size_t cap) noexcept
    {
        const size_t r = cap + 1;
        return (x >= r) ? x - r : x;
    }

    static size_t advance(size_t offset, size_t, size_t cap) noexcept
    {
        return wrap_lane(offset + group_width, cap);
    }
};

template <>
class probe_traits<pow2_growth>
{
    static size_t wrap(size_t x, size_t cap) noexcept
    {
        return x & cap;
    }
    static size_t wrap_lane(size_t x, size_t cap) noexcept
    {
        return x & cap;
    }
    static size_t advance(size_t offset, size_t index, size_t cap) noexcept
    {
        return (offset + index) & cap;
    }
};

template <typename Growth>
class probe_seq : public probe_traits<Growth>
{
public:

    using base = probe_traits<Growth>;

    probe_seq(size_t h1, size_t cap) noexcept
        : m_cap(cap)
        , m_offset(base::wrap(h1, cap))
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
        return base::wrap_lane(m_offset + lane, m_cap);
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
        m_offset = base::advance(m_offset, m_index, m_cap);
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

// Backend iterator: a (ctrl*, slot*) pair that advances in lockstep.
// Slot is slot_type or const slot_type.
template <typename Slot>
class open_iterator
{
    template <typename>
    friend class open_iterator;

public:

    using iterator_category = std::forward_iterator_tag; // swap for your own tag type
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
        VX_ASSERT(is_full(*m_ctrl));
        return *m_slot;
    }

    pointer operator->() const noexcept
    {
        return m_slot;
    }

    open_iterator& operator++() noexcept
    {
        VX_ASSERT(*m_ctrl != ctrl_sentinel); // cannot advance end()
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
        while (is_empty_or_deleted(*m_ctrl))
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

    // same role as vector's data_type::acquire: take other's block, leave it empty
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
    typename Growth = pow2_growth>
class open_table
{
private:

    //=========================================================================
    // internal types and policies
    //=========================================================================

    using slot_traits = SlotTraits;
    using slot_type = typename slot_traits::slot_type;
    using table_data = open_data<slot_type>;
    using growth = Growth;

    using block_allocator = typename Allocator::template rebind<unsigned char>::other;

    //=========================================================================
    // implementation constants
    //=========================================================================

    // We are storing 2 types in the allocation: group control bytes and slots.
    // The alignment of the allocation must be at least the max of the two.
    static constexpr size_t block_align =
        alignof(slot_type) > group_width
        ? alignof(slot_type)
        : group_width;

    // Above this size, avoid using a temporary slot on the stack.
    static constexpr size_t max_in_place_slot_size = 256;

    //=========================================================================
    // compile-time validation
    //=========================================================================

    VX_STATIC_ASSERT_MSG(
        (growth::is_valid_capacity(0)),
        "growth must accept capacity 0");

    VX_STATIC_ASSERT_MSG(
        (growth::is_valid_capacity(growth::min_capacity)),
        "min_capacity must be valid");

    VX_STATIC_ASSERT_MSG(
        (growth::next_capacity(0) == growth::min_capacity),
        "next_capacity(0) must be min_capacity");

    VX_STATIC_ASSERT_MSG(
        (growth::next_capacity(growth::min_capacity) > growth::min_capacity),
        "next_capacity must increase");

    VX_STATIC_ASSERT_MSG(
        (load::max_size(0) == 0),
        "max_size(0) must be 0");

    VX_STATIC_ASSERT_MSG(
        (load::max_size(growth::min_capacity) >= 1),
        "load slot_traits leaves no room in min_capacity");

    VX_STATIC_ASSERT_MSG(
        (validate_traits<Traits>::value),
        "invalid table traits");

public:

    //=========================================================================
    // public types
    //=========================================================================

    using allocator_type = block_allocator;
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

private:

    //=========================================================================
    // storage
    //=========================================================================

    table_storage<allocator_type, Hasher, Eq, table_data> m_storage;

    //=========================================================================
    // accessors
    //=========================================================================

    allocator_type& m_allocator() noexcept
    {
        return m_storage.alloc();
    }

    const allocator_type& m_allocator() const noexcept
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
        return m_allocator().allocate(bytes);
    }

    void release_block(size_t cap) noexcept
    {
        layout l;
        const bool ok = compute_safe_layout(cap, sizeof(slot_type), alignof(slot_type), l);
        VX_ASSERT(ok);
        m_allocator().deallocate(m_data().ctrl, l.alloc_size);
    }

    //=========================================================================
    // copy helpers
    //=========================================================================

    struct data_pointer_pair
    {
        ctrl_t* ctrl;
        slot_type* slots;

        explicit operator bool() const noexcept
        {
            return ctrl != nullptr;
        }
    };

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
            if (is_full(ctrl[i]))
            {
                slot_traits::construct(slots + i, src.slots[i]);
            }
        }

        return { ctrl, slots };
    }

    // Copies ctrl bytes verbatim (sentinel, clones, phantoms, and tombstones), then
    // copy-constructs each full slot at the same index. No hashing, no probing.
    // Requires *this to be unallocated.
    success copy_construct_from(const table_data& src)
    {
        auto& d = m_data();

        // should only be called when empty
        VX_ASSERT(d.capacity == 0);

        if (src.capacity == 0)
        {
            return success{};
        }

        const auto ptrs = allocate_and_copy_from(src);
        VX_RET_ERR_IF(!ctrl, err::out_of_memory);

        d.ctrl = ptrs.ctrl;
        d.slots = ptrs.slots;

        d.capacity = src.m_data().capacity;
        d.size = src.m_data().size;
        d.growth_left = src.m_data().growth_left;

        return success{};
    }

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
        : m_storage(other.alloc(), other.hasher(), other.eq())
    {
        const auto ok = copy_construct_from(other.m_data());
        VX_VERIFY(ok);
    }

    // like vector: the allocator moves along with the block that it produced
    open_table(open_table&& other) noexcept
        : m_storage(std::move(other.alloc()), std::move(other.hasher()), std::move(other.eq()))
    {
        m_data().acquire(other.m_data());
    }

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<open_table, error> create(const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
    {
        return open_table(h, e, a);
    }

    static expected<open_table, error> create(size_t n, const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
    {
        open_table t(h, e, a);
        const auto ok = t.reserve(n);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return t;
    }

    static expected<open_table, error> create(const open_table& other)
    {
        open_table t(other.hasher(), other.eq(), other.alloc());
        const auto ok = t.copy_construct_from(other.m_data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return t;
    }

    static expected<open_table, error> create(open_table&& other) noexcept
    {
        open_table t(std::move(other.alloc()), std::move(other.hasher()), std::move(other.eq()));
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
            if (is_full(d.ctrl[i]))
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
        return m_allocator();
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

    success assign_from(const open_table& src) noexcept
    {
        auto& d = m_data();

        if (src.capacity == 0)
        {
            clear_and_deallocate();
            return success{};
        }

        const auto ptrs = allocate_and_copy_from(src);
        VX_RET_ERR_IF(!ptrs, err::out_of_memory);

        d.ctrl = ptrs.ctrl;
        d.slots = ptrs.slots;

        d.capacity = src.m_data().capacity;
        d.size = src.m_data().size;
        d.growth_left = src.m_data().growth_left;

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
        return assign_from(other);
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

    // Conservative: rehash_to still reports size_error on layout overflow.
    static constexpr size_t max_size() noexcept
    {
        return (SIZE_MAX / 2) / (sizeof(slot_type) + 1);
    }

    //=========================================================================
    // capacity
    //=========================================================================

    // number of elements the table can hold before it must grow
    size_t capacity() const noexcept
    {
        return growth::max_size(m_data().capacity);
    }

    // raw ring size (slots), 0 or a capacity produced by the growth slot_traits
    size_t slot_count() const noexcept
    {
        return m_data().capacity;
    }

    // Capacity needed to hold n elements without growing (0 on n == 0 or overflow).
    static constexpr size_t capacity_for_size(size_t n) noexcept
    {
        return _hash_priv::capacity_for_size<growth, load>(n);
    }

    // Tombstones are the budget that erase left behind. Derived, so no extra field.
    size_t tombstone_count() const noexcept
    {
        const auto& d = m_data();
        return growth::max_size(d.capacity) - d.size - d.growth_left;
    }

    bool needs_cleanup() const noexcept
    {
        const auto& d = m_data();
        // if there are some slots that can be reclaimed and the table is at it's limit,
        // then the next insert will trigger a cleanup instead of a grow
        return d.capacity != 0 && tombstone_count() != 0 && d.size <= cleanup_limit_of < ;
    }

    //=========================================================================
    // hashing
    //=========================================================================

    // Hash plus finalizer (see table_storage::hash_of).
    template <typename Q>
    size_t hash_of(const Q& key) const noexcept
    {
        return m_storage.hash_of(key);
    }

    //=========================================================================
    // lookup
    //=========================================================================

    // Returns the slot index of the key, or npos.
    template <typename Q>
    size_t find_index_with_hash(const Q& key, size_t hash) const noexcept
    {
        const h2_t h2 = h2_of(hash);
        probe_seq<growth> seq(h1_of(hash), m_data().capacity);

        while (true)
        {
            const group g(m_data().ctrl + seq.offset());

            // candidates: lanes whose ctrl byte equals H2
            for (uint32_t lane : g.match(h2))
            {
                const size_t i = seq.slot_at(lane);
                if (eq()(key, slot_traits::key(m_data().slots[i])))
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

    template <typename Q>
    size_t find_index(const Q& key) const noexcept
    {
        return find_index_with_hash(key, hash_of(key));
    }

    template <typename Q>
    iterator find_with_hash(const Q& key, size_t hash) noexcept
    {
        VX_ASSERT(hash == hash_of(key)); // debug: a precomputed hash must agree with the hasher
        const size_t i = find_index_with_hash(key, hash);
        return i == npos ? end() : iterator_at(i);
    }

    template <typename Q>
    const_iterator find_with_hash(const Q& key, size_t hash) const noexcept
    {
        VX_ASSERT(hash == hash_of(key));
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

    template <typename Q>
    size_t count(const Q& key) const noexcept
    {
        return contains(key) ? 1 : 0;
    }

private:

    //=========================================================================
    // rehash
    //=========================================================================

    // Capacity to grow into so that `needed` elements fit. The growth slot_traits picks
    // the sequence, so this can step more than once (reserve, tiny load factors).
    // Always steps at least once. Returns 0 on overflow. After allocating:
    //     m_data().growth_left = load::max_size(new_cap) - m_data().size;
    size_t grown_capacity(size_t needed) const noexcept
    {
        size_t cap = growth::next_capacity(m_data().capacity);
        while (cap != 0 && load::max_size(cap) < needed)
        {
            cap = growth::next_capacity(cap);
        }
        return cap;
    }

    // Allocates a block of new_cap slots, relocates every element into it by
    // hash, then frees the old block. The old block is untouched on failure.
    // Serves growth, reserve and shrink_to_fit.
    success rehash_to(size_t new_cap)
    {
        layout l;
        VX_RET_ERR_IF(!compute_layout(new_cap, sizeof(slot_type), alignof(slot_type), l), err::size_error);

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
                if (!is_full(d.ctrl[i]))
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
        d.growth_left = load::max_size(new_cap) - d.size;
        return success{};
    }

    // Re-places every element at the first free slot on its probe path without
    // allocating. Elements in their first probe window stay put. Others move into
    // EMPTY slots, or swap with another pending element (one temp slot).
    void rehash_in_place() noexcept
    {
        auto& d = m_data();
        const size_t cap = d.capacity;
        ctrl_t* ctrl = d.ctrl;
        slot_type* slots = d.slots;
        VX_ASSERT(cap != 0 && !fits_in_one_group(cap));

        // 1. FULL -> DELETED ("pending"), everything else -> EMPTY. Groups may
        //    write past the ring into the clone bytes, which the next call rewrites.
        for (size_t pos = 0; pos < cap + 1; pos += group_width)
        {
            group(ctrl + pos).convert_special_to_empty_and_full_to_deleted(ctrl + pos);
        }
        restore_ctrl_clones(ctrl, cap); // sentinel + clones, before any probing

        // 2. Re-place each pending element.
        aligned_storage<sizeof(slot_type), alignof(slot_type)> tmp;

        for (size_t i = 0; i < cap; ++i)
        {
            while (ctrl[i] == ctrl_deleted)
            {
                const size_t hash = hash_of(slot_traits::key(slots[i]));
                const h2_t h2 = h2_of(hash);
                const size_t offset = growth::wrap(h1_of(hash), cap);

                // In the first probe window: every lookup examines it, so the
                // element is already findable. Just mark it full.
                if (growth::wrap_lane(i + (cap + 1) - offset, cap) < group_width)
                {
                    set_ctrl(ctrl, cap, i, static_cast<ctrl_t>(h2));
                    break;
                }

                const size_t t = find_first_non_full<growth>(ctrl, cap, hash);

                if (t == i)
                {
                    set_ctrl(ctrl, cap, i, static_cast<ctrl_t>(h2));
                    break;
                }

                const bool was_empty = (ctrl[t] == ctrl_empty);
                set_ctrl(ctrl, cap, t, static_cast<ctrl_t>(h2));

                if (was_empty)
                {
                    slot_traits::relocate(slots + t, slots + i);
                    set_ctrl(ctrl, cap, i, ctrl_empty);
                    break;
                }

                // t holds another pending element: swap through tmp, then retry
                // slot i, which now holds that displaced element.
                slot_type* s = tmp.template ptr<slot_type>();
                slot_traits::relocate(s, slots + i);
                slot_traits::relocate(slots + i, slots + t);
                slot_traits::relocate(slots + t, s);
            }
        }

        d.growth_left = load::max_size(cap) - d.size;
    }

    success cleanup_tombstones_impl(std::true_type) noexcept
    {
        rehash_in_place();
        return success{};
    }

    success cleanup_tombstones_impl(std::false_type)
    {
        return rehash_to(m_data().capacity);
    }

    // tag dispatch, so the untaken overload is never instantiated
    success cleanup_tombstones()
    {
        return cleanup_tombstones_impl(std::integral_constant<bool, (sizeof(slot_type) <= max_in_place_slot_size)>{});
    }

    // Called when an insert finds growth_left == 0: clean in place if tombstones
    // dominate, otherwise grow.
    success grow_for_insert()
    {
        auto& d = m_data();

        if (d.capacity != 0 && d.size <= cleanup_limit_of<load>(load::max_size(d.capacity)))
        {
            return cleanup_tombstones();
        }

        const size_t new_cap = grown_capacity(d.size + 1);
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
        d.growth_left = load::max_size(d.capacity);
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

        VX_RET_ERR_IF(n > max_size(), err::size_error);

        // the capacity is big enough, tombstones are the problem: reclaim them
        if (d.capacity != 0 && n <= load::max_size(d.capacity))
        {
            return cleanup_tombstones();
        }

        const size_t new_cap = grown_capacity(n);
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

        const size_t target = capacity_for_size(d.size);
        VX_RET_ERR_IF(target == 0, err::size_error);

        return target < d.capacity ? rehash_to(target) : success{};
    }

    // Explicit tombstone cleanup (decision 8's compact()).
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

    size_t prepare_insert_capacity(size_t hash) noexcept
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
        VX_ASSERT(i < d.capacity && !is_full(d.ctrl[i]));
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
    iterator emplace_new_capacity(size_t hash, Args&&... args)
    {
        const size_t i = prepare_insert_capacity(hash);
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
        VX_ASSERT(i < d.capacity && is_full(d.ctrl[i]));
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
