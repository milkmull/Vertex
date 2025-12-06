#include "vertex/util/memory/memory.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/input/_platform/platform_clipboard.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace clipboard {

//=============================================================================
// helper macros
//=============================================================================

#define hints_ptr video->app->hints_ptr
#define events_ptr video->app->events_ptr

//=============================================================================
// initialization
//=============================================================================

clipboard_manager::clipboard_manager() = default;
clipboard_manager::~clipboard_manager() { quit(); }

//=============================================================================

bool clipboard_manager::init(video::video_instance* owner)
{
    if (video)
    {
        quit();
    }
    video = owner;
    return true;
}

void clipboard_manager::quit()
{
    cancel_data(0);
    data.primary_selection_text.clear();
    data.sequence = 0;
    video = nullptr;
}

//=============================================================================
// mime types
//=============================================================================

const std::vector<std::string>& clipboard_manager::get_mime_types() const
{
    return data.mime_types;
}

const char** clipboard_manager::get_text_mime_types(size_t& count) const
{
#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_GET_TEXT_MIME_TYPES

    return impl_ptr->get_text_mime_types(count);

#else

    static const char* text_mime_types[] = { "text/plain;charset=utf-8" };
    count = mem::array_size(text_mime_types);
    return text_mime_types;

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_GET_TEXT_MIME_TYPES
}

//=============================================================================

bool clipboard_manager::update_mime_types(const std::vector<std::string>& mime_types)
{
    data.mime_types = mime_types;
    return true;
}

//=============================================================================
// data
//=============================================================================

void clipboard_manager::cancel_data(uint32_t sequence)
{
    if (sequence && sequence != data.sequence)
    {
        // already canceled
        return;
    }

    if (data.cleanup)
    {
        data.cleanup(data.user_data);
    }

    data.mime_types.clear();

    data.callback = nullptr;
    data.cleanup = nullptr;
    data.user_data = nullptr;
}

//=============================================================================

bool clipboard_manager::clear_data()
{
    return set_data(nullptr, nullptr, nullptr, {});
}

//=============================================================================

bool clipboard_manager::has_data(const char* mime_type) const
{
    if (!mime_type)
    {
        return false;
    }

#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_HAS_DATA

    return impl_ptr->has_data(mime_type);

#else

#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_HAS_TEXT

    if (is_text_mime_type(mime_type))
    {
        return impl_ptr->has_text();
    }

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_HAS_TEXT

    return has_internal_data(mime_type);

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_HAS_DATA
}

//=============================================================================

bool clipboard_manager::has_internal_data(const char* mime_type) const
{
    for (const std::string& type : data.mime_types)
    {
        if (type.compare(mime_type) == 0)
        {
            return true;
        }
    }

    return false;
}

//=============================================================================

std::vector<uint8_t> clipboard_manager::get_internal_data(const char* mime_type) const
{
    std::vector<uint8_t> internal_data;

    if (has_internal_data(mime_type) && data.callback)
    {
        // obtain a pointer to the raw data
        size_t size = 0;
        const void* provided_data = data.callback(mime_type, size, data.user_data);

        if (provided_data && size)
        {
            internal_data.resize(size);
            std::memcpy(const_cast<void*>(provided_data), internal_data.data(), size);
        }
    }

    return internal_data;
}

//=============================================================================

std::vector<uint8_t> clipboard_manager::get_data(const char* mime_type) const
{
#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_GET_DATA

    return impl_ptr->get_data();

#else

#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_GET_TEXT

    if (is_text_mime_type(mime_type))
    {
        return impl_ptr->get_text();
    }

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_GET_TEXT

    return get_internal_data(mime_type);

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_GET_DATA
}

//=============================================================================

bool clipboard_manager::set_data(
    data_callback callback, cleanup_callback cleanup, void* user_data,
    const std::vector<std::string>& mime_types
)
{
    // this should go in the front end function
    if (!((callback && !mime_types.empty()) ||
        (!callback && mime_types.empty())))
    {
        err::set(err::invalid_argument, "invalid arguments");
        return false;
    }

    cancel_data(0);

    ++data.sequence;
    if (!data.sequence)
    {
        data.sequence = 1;
    }

    data.callback = callback;
    data.cleanup = cleanup;
    data.user_data = user_data;

    if (!update_mime_types(mime_types))
    {
        clear_data();
        return false;
    }

#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_SET_DATA

    if (!impl_ptr->set_data())
    {
        return false;
    }

#else

#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_SET_TEXT

    bool match = false;

    for (const std::string& mime_type : data.mime_types)
    {
        if (is_text_mime_type(mime_type.c_str()))
        {
            // get a pointer to the data from the callback
            size_t size;
            const void* new_data = data.callback(mime_type.c_str(), size, data.user_data);

            if (new_data && size)
            {
                const std::string text(static_cast<const char*>(new_data), size);
                if (!impl_ptr->set_text(text))
                {
                    return false;
                }

                match = true;
                break;
            }
        }
    }

    if (!match && !impl_ptr->set_text(""))
    {
        return false;
    }

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_SET_TEXT

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_SET_DATA

    send_update(true, mime_types);
    return true;
}

//=============================================================================
// text
//=============================================================================

bool clipboard_manager::has_text() const
{
    for (const std::string& type : data.mime_types)
    {
        if (has_data(type.c_str()))
        {
            return true;
        }
    }

    return false;
}

//=============================================================================

std::string clipboard_manager::get_text() const
{
    std::vector<uint8_t> raw_data;
    std::string text;

    for (const std::string& type : data.mime_types)
    {
        raw_data = get_data(type.c_str());
        if (!raw_data.empty())
        {
            text.assign(raw_data.begin(), raw_data.end());
            break;
        }
    }

    return text;
}

//=============================================================================

static const void* text_callback(const char*, size_t& size, void* user_data)
{
    const char* text = static_cast<const char*>(user_data);
    size = text ? std::strlen(text) : 0;
    return text;
}

bool clipboard_manager::set_text(const std::string& text)
{
    if (!text.empty())
    {
        const size_t size = text.size() + 1;
        char* text_copy = static_cast<char*>(std::malloc(sizeof(char) * size));
        if (!text_copy)
        {
            return false;
        }

        // copy the string and store as a raw string
        // that will be freed with std::free on cleanup
        std::strcpy(text_copy, text.data());
        return set_data(text_callback, std::free, text_copy, data.mime_types);
    }

    return clear_data();
}

//=============================================================================
// primary selection
//=============================================================================

bool clipboard_manager::has_primary_selection_text() const
{
#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_HAS_PRIMARY_SELECTION_TEXT

    return impl_ptr->has_primary_selection_text();

#else

    return !data.primary_selection_text.empty();

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_HAS_PRIMARY_SELECTION_TEXT
}

//=============================================================================

const std::string& clipboard_manager::get_primary_selection_text() const
{
#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_GET_PRIMARY_SELECTION_TEXT

    return impl_ptr->get_primary_selection_text();

#else

    return data.primary_selection_text;

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_GET_PRIMARY_SELECTION_TEXT
}

//=============================================================================

bool clipboard_manager::set_primary_selection_text(const std::string& text)
{
#if VX_VIDEO_BACKEND_HAVE_CLIPBOARD_SET_PRIMARY_SELECTION_TEXT

    if (!impl_ptr->set_primary_selection_text(text))
    {
        return false;
    }

#else

    data.primary_selection_text = text;

#endif // VX_VIDEO_BACKEND_HAVE_CLIPBOARD_SET_PRIMARY_SELECTION_TEXT

    send_update(true, data.mime_types);
    return true;
}

//=============================================================================
// events
//=============================================================================

void clipboard_manager::send_update(bool owner, const std::vector<std::string>& mime_types)
{
    if (!owner)
    {
        cancel_data(0);
        update_mime_types(mime_types);
    }

    std::vector<const char*> mime_type_ptrs(mime_types.size());
    for (size_t i = 0; i < mime_types.size(); ++i)
    {
        mime_type_ptrs[i] = mime_types[i].c_str();
    }

    const char** event_mime_types = event::create_temporary_string_array(mime_type_ptrs.data(), mime_type_ptrs.size());
    if (!event_mime_types)
    {
        return;
    }

    event::event e{};
    e.type = event::clipboard_updated;
    e.clipboard_event.clipboard_updated.owner = owner;
    e.clipboard_event.clipboard_updated.mime_types = event_mime_types;
    e.clipboard_event.clipboard_updated.mime_type_count = mime_types.size();
    events_ptr->push_event(e);
}

} // namespace clipboard
} // namespace app
} // namespace vx
