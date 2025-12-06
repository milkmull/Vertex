#pragma once

#include "vertex/app/input/clipboard.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }

namespace clipboard {

//=============================================================================
// forward declares
//=============================================================================

class clipboard_instance_impl;

//=============================================================================
// clipboard data
//=============================================================================

struct clipboard_manager_data
{
    // When the clipboard data is set, no data is actually coppied. Instead this
    // callback provides a way to retrieve  pointer to the data only when it is
    // needed.
    data_callback callback = nullptr;
    cleanup_callback cleanup = nullptr;
    void* user_data = nullptr;

    uint32_t sequence = 0;
    std::vector<std::string> mime_types;
    std::string primary_selection_text;
};

//=============================================================================
// helpers
//=============================================================================

inline bool is_text_mime_type(const char* type)
{
    return std::strncmp(type, "text", 4) == 0;
}

//=============================================================================
// clipboard_instance
//=============================================================================

class clipboard_manager
{
public:

    clipboard_manager();
    ~clipboard_manager();

    clipboard_manager(const clipboard_manager&) = delete;
    clipboard_manager& operator=(const clipboard_manager&) = delete;

    clipboard_manager(clipboard_manager&&) noexcept = delete;
    clipboard_manager& operator=(clipboard_manager&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(video::video_instance* owner);
    void quit();

    //=============================================================================
    // mime types
    //=============================================================================

    const std::vector<std::string>& get_mime_types() const;
    const char** get_text_mime_types(size_t& count) const;
    bool update_mime_types(const std::vector<std::string>& mime_types);

    //=============================================================================
    // data
    //=============================================================================

    bool has_internal_data(const char* mime_type) const;
    std::vector<uint8_t> get_internal_data(const char* mime_type) const;

    void cancel_data(uint32_t sequence);
    bool clear_data();

    bool has_data(const char* mime_type) const;
    std::vector<uint8_t> get_data(const char* mime_type) const;
    bool set_data(data_callback callback, cleanup_callback cleanup, void* user_data, const std::vector<std::string>& mime_types);

    //=============================================================================
    // text
    //=============================================================================

    bool has_text() const;
    std::string get_text() const;
    bool set_text(const std::string& text);

    //=============================================================================
    // primary selection
    //=============================================================================

    bool has_primary_selection_text() const;
    const std::string& get_primary_selection_text() const;
    bool set_primary_selection_text(const std::string& text);

    //=============================================================================
    // events
    //=============================================================================

    void send_update(bool owner, const std::vector<std::string>& mime_types);

    //=============================================================================
    // Data
    //=============================================================================

    video::video_instance* video = nullptr;
    clipboard_manager_data data;
    std::unique_ptr<clipboard_instance_impl> impl_ptr;
};

} // namespace clipboard
} // namespace app
} // namespace vx
