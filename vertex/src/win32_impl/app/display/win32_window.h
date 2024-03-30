#pragma once

#include "vertex/app/display/window.h"
#include "win32_header.h"
#include "vertex_impl/app/init_internal.h"

namespace vx {
namespace app {

class win32_window : public window
{
public:

    // =============== win32 class stuff ===============

    static constexpr const wchar_t* s_class_name = L"Vertex_Window";
    static size_t s_window_count;

    static void register_window_class();

    // =============== constructors ===============

    win32_window(const std::string& title, const math::vec2i& size, const math::vec2i& position);
    ~win32_window() override;

    win32_window(const win32_window&) = delete;
    win32_window(win32_window&&) = delete;

    win32_window& operator=(const win32_window&) = delete;
    win32_window& operator=(win32_window&&) = delete;

private:

    void on_destroy();

public:

    // =============== open ===============

    bool is_open() const override;

private:

    // =============== events ===============

    static LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
    void process_event(UINT Msg, WPARAM wParam, LPARAM lParam);
    void process_events() override;

public:

    // =============== title ===============

    std::string get_title() const override;
    void set_title(const std::string& title) override;

    // =============== position ===============

    math::vec2i get_position() const override;
    void set_position(const math::vec2i& position) override;

    // =============== size ===============

    math::vec2i get_size() const override;
    void set_size(const math::vec2i& size) override;

private:

    HWND m_handle = nullptr;

    bool m_resizing_or_moving = false;
    math::vec2i m_last_size;
    math::vec2i m_last_position;

};

}
}