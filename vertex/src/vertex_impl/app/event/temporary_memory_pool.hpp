#pragma once

#include <list>

#include "vertex/app/event/event.hpp"
#include "vertex/os/mutex.hpp"
#include "vertex_impl/app/video/_platform/platform_features.hpp"

namespace vx {
namespace app {
namespace event {

struct temp_memory_entry
{
    void* ptr;
    temp_memory_entry* prev;
    temp_memory_entry* next;
};

class temporary_memory_pool
{
public:

    temporary_memory_pool() : m_head(nullptr), m_tail(nullptr) {}
    ~temporary_memory_pool() { clear(); }

private:

    // Allocate raw memory
    void* malloc_impl(size_t size)
    {
        return std::malloc(size);
    }

    // Free raw memory
    void free_impl(void* ptr)
    {
        if (ptr)
        {
            std::free(ptr);
        }
    }

    // Insert entry at end of list
    void link(temp_memory_entry* entry)
    {
        entry->prev = m_tail;
        entry->next = nullptr;

        if (m_tail)
        {
            m_tail->next = entry;
        }
        else
        {
            m_head = entry;
        }

        m_tail = entry;
    }

    // Remove entry from list, optionally free entry and/or its memory
    void unlink(temp_memory_entry* entry, bool free_entry, bool free_memory)
    {
        if (m_head == entry)
        {
            m_head = entry->next;
        }
        if (m_tail == entry)
        {
            m_tail = entry->prev;
        }

        if (entry->prev)
        {
            entry->prev->next = entry->next;
        }
        if (entry->next)
        {
            entry->next->prev = entry->prev;
        }

        if (free_memory)
        {
            free_impl(entry->ptr);
        }

        if (free_entry)
        {
            delete entry;
        }
        else
        {
            entry->prev = nullptr;
            entry->next = nullptr;
        }
    }

    // Find entry by its memory pointer
    temp_memory_entry* find(const void* ptr)
    {
        if (!ptr)
        {
            return nullptr;
        }

        for (auto it = m_tail; it; it = it->prev)
        {
            if (it->ptr == ptr)
            {
                return it;
            }
        }

        return nullptr;
    }

public:

    bool give_memory(void* ptr)
    {
        if (!ptr)
        {
            return false;
        }

        temp_memory_entry* entry = find(ptr);
        if (entry)
        {
            return true;
        }

        entry = new temp_memory_entry;
        if (!entry)
        {
            return false;
        }

        entry->ptr = ptr;
        link(entry);

        return true;
    }

    // Allocate memory + entry, take ownership
    void* allocate_memory(size_t size)
    {
        if (size == 0)
        {
            return nullptr;
        }

        temp_memory_entry* entry = new temp_memory_entry;
        if (!entry)
        {
            return nullptr;
        }

        void* ptr = malloc_impl(size);
        if (!ptr)
        {
            delete entry;
            return nullptr;
        }

        entry->ptr = ptr;
        link(entry);

        return ptr;
    }

    // Free memory and remove entry entirely
    void free_memory(void* ptr)
    {
        temp_memory_entry* entry = find(ptr);
        if (!entry)
        {
            return;
        }

        unlink(entry, true, true);
    }

    // Give memory ownership back to caller; free entry only
    const void* release_memory(const void* ptr)
    {
        temp_memory_entry* entry = find(ptr);
        if (!entry)
        {
            return nullptr;
        }

        unlink(entry, true, false);
        return ptr;
    }

    // Reinsert entry or chain of entries back into pool
    void give_entry(temp_memory_entry* entry)
    {
        if (!entry || !entry->ptr)
        {
            return;
        }

        while (entry)
        {
            temp_memory_entry* next = entry->next;
            entry->next = nullptr;
            link(entry);
            entry = next;
        }
    }

    // Remove entry from pool and return it to caller
    temp_memory_entry* release_entry(const void* ptr)
    {
        temp_memory_entry* entry = find(ptr);
        if (!entry)
        {
            return nullptr;
        }

        unlink(entry, false, false);
        return entry;
    }

    // Free all memory and destroy all entries
    void clear()
    {
        while (m_head)
        {
            unlink(m_head, true, true);
        }

        m_head = nullptr;
        m_tail = nullptr;
    }

private:

    temp_memory_entry* m_head;
    temp_memory_entry* m_tail;
};

} // namespace event
} // namespace app
} // namespace vx