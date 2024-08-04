#pragma once

#include <unordered_map>

#include "vertex/app/video/video.h"
#include "vertex/image/image.h"

namespace vx {
namespace app {

class mouse
{
private: // Friends

    friend video;
    friend video::window;

private: // Implimentation

    class mouse_impl;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // cursor
    ///////////////////////////////////////////////////////////////////////////////

    using cursor_id = device_id;

    enum class cursor_shape : cursor_id
    {
        DEFAULT = 1,  /**< Default cursor. Usually an arrow. */
        TEXT,         /**< Text selection. Usually an I-beam. */
        WAIT,         /**< Wait. Usually an hourglass or watch or spinning ball. */
        CROSSHAIR,    /**< Crosshair. */
        PROGRESS,     /**< Program is busy but still interactive. Usually it's WAIT with an arrow. */
        NWSE_RESIZE,  /**< Double arrow pointing northwest and southeast. */
        NESW_RESIZE,  /**< Double arrow pointing northeast and southwest. */
        EW_RESIZE,    /**< Double arrow pointing west and east. */
        NS_RESIZE,    /**< Double arrow pointing north and south. */
        MOVE,         /**< Four pointed arrow pointing north, south, east, and west. */
        NOT_ALLOWED,  /**< Not permitted. Usually a slashed circle or crossbones. */
        POINTER,      /**< Pointer that indicates a link. Usually a pointing hand. */
        NW_RESIZE,    /**< Window resize top-left. This may be a single arrow or a double arrow like NWSE_RESIZE. */
        N_RESIZE,     /**< Window resize top. May be NS_RESIZE. */
        NE_RESIZE,    /**< Window resize top-right. May be NESW_RESIZE. */
        E_RESIZE,     /**< Window resize right. May be EW_RESIZE. */
        SE_RESIZE,    /**< Window resize bottom-right. May be NWSE_RESIZE. */
        S_RESIZE,     /**< Window resize bottom. May be NS_RESIZE. */
        SW_RESIZE,    /**< Window resize bottom-left. May be NESW_RESIZE. */
        W_RESIZE      /**< Window resize left. May be EW_RESIZE. */
    };

    class cursor
    {
    private:

        friend mouse;

    public:

        cursor();
        ~cursor();

    private:

        cursor(const cursor&);
        cursor(cursor&&) noexcept;

        cursor& operator=(const cursor&);
        cursor& operator=(cursor&&) noexcept;

        bool validate() const;

    private:

        cursor_id m_id;
        math::vec2i m_hotspot;

    private:

        class cursor_impl;
        std::shared_ptr<cursor_impl> m_impl;
    };

public:

    static const cursor* create_system_cursor(cursor_shape shape);
    static const cursor* create_custom_cursor(const img::image& image, const math::vec2i& hotspot);

    static void destroy_cursor(const cursor* c);

    static const cursor* get_cursor();
    static bool set_cursor(const cursor* c);

    static const cursor* get_default_cursor();
    static bool set_default_cursor(const cursor* c);

public:

    ///////////////////////////////////////////////////////////////////////////////
    // mouse enums
    ///////////////////////////////////////////////////////////////////////////////

    enum button : int
    {
        NONE = 0,

        BUTTON_1 = 1,
        BUTTON_2 = 2,
        BUTTON_3 = 3,
        BUTTON_4 = 4,
        BUTTON_5 = 5,
        BUTTON_6 = 6,
        BUTTON_7 = 7,
        BUTTON_8 = 8,
        BUTTON_9 = 9,

        BUTTON_LEFT = BUTTON_1,
        BUTTON_RIGHT = BUTTON_2,
        BUTTON_MIDDLE = BUTTON_3,

        BUTTON_WHEEL_UP = BUTTON_4,
        BUTTON_WHEEL_DOWN = BUTTON_5,
        BUTTON_WHEEL_LEFT = BUTTON_6,
        BUTTON_WHEEL_RIGHT = BUTTON_7,

        BUTTON_EXTRA_1 = BUTTON_8,
        BUTTON_EXTRA_2 = BUTTON_9
    };

    enum wheel : int
    {
        VERTICAL,
        HORIZONTAL
    };

public:

    ///////////////////////////////////////////////////////////////////////////////
    // mouse
    ///////////////////////////////////////////////////////////////////////////////

    mouse() = delete;
    ~mouse() = delete;

    mouse(const mouse&) = delete;
    mouse(mouse&&) noexcept = delete;

    mouse& operator=(const mouse&) = delete;
    mouse& operator=(mouse&&) noexcept = delete;

private:

    static bool init();
    static void quit();

private:

    struct mouse_data
    {
        const cursor* default_cursor = nullptr;
        const cursor* current_cursor = nullptr;
        const cursor* blank_cursor = nullptr;

        cursor_id custom_cursor_id = static_cast<cursor_id>(cursor_shape::W_RESIZE) + 1;
        std::unordered_map<cursor_id, std::unique_ptr<cursor>> cursors;

        bool has_focus = false;
        bool cursor_visible = false;

        device_id focussed_window_id = 0;
        math::vec2 position, last_position;
    };

    static mouse_data s_mouse_data;

};

}
}