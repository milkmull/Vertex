#include "vertex/config/assert.hpp"
#include "vertex/os/time.hpp"
#include "vertex/system/error.hpp"
#include "vertex_impl/app/input/_platform/windows/windows_raw_input.hpp"
#include "vertex_impl/app/input/usb_ids.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace input {

///////////////////////////////////////////////////////////////////////////////
// iitialization
///////////////////////////////////////////////////////////////////////////////

bool raw_input_manager::init(video::video_instance* owner)
{
    VX_ASSERT(!video);
    VX_ASSERT(owner);
    video = owner;

    data.raw_mouse_enabled = false;
    data.raw_keyboard_enabled = false;

    return true;
}

void raw_input_manager::quit()
{
    reset_thread();

    video = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// thread
///////////////////////////////////////////////////////////////////////////////

void raw_input_manager::reset_thread()
{
    data.thread_data.flags = raw_input_flags::NONE;

    if (data.thread_data.thread.is_valid())
    {
        data.thread_data.done = true;
        ::SetEvent(data.thread_data.done_event.get());
        data.thread_data.thread.join();
    }

    data.thread_data.ready_event.close();
    data.thread_data.done_event.close();
}

////////////////////////////////////////

static void thread_entry(raw_input_manager* this_)
{
    this_->thread();
}

////////////////////////////////////////

bool raw_input_manager::start_thread(typename raw_input_flags::type flags)
{
    reset_thread();

    bool success = false;

    if (flags)
    {
        data.thread_data.flags = flags;

        data.thread_data.ready_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!data.thread_data.ready_event.is_valid())
        {
            os::windows::error_message("CreateEvent()");
            goto done;
        }

        data.thread_data.done = false;
        data.thread_data.done_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!data.thread_data.done_event.is_valid())
        {
            os::windows::error_message("CreateEvent()");
            goto done;
        }

        data.thread_data.thread.start(thread_entry, this);
        if (!data.thread_data.thread.is_valid())
        {
            os::windows::error_message("CreateThread()");
            goto done;
        }

        // Wait for the thread to signal ready or exit
        if (::WaitForSingleObject(data.thread_data.ready_event.get(), INFINITE) != WAIT_OBJECT_0)
        {
            err::set(err::SYSTEM_ERROR, "Filed to set up raw input handling");
            goto done;
        }

        success = true;
    }
    else
    {
        success = true;
    }

    done:
    {
        if (!success)
        {
            reset_thread();
        }
    }

    return success;
}

////////////////////////////////////////

void raw_input_manager::thread()
{
    // Create a message-only window (no visible surface, just a message target)
    // HWND_MESSAGE means it doesn’t appear on screen and doesn’t participate in Z-ordering.
    // This window will receive WM_INPUT messages from the raw input system.
    HWND window = ::CreateWindowEx(0, TEXT("Message"), NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
    if (!window)
    {
        return;
    }

    RAWINPUTDEVICE devices[2]{};
    UINT count = 0;

    if (data.thread_data.flags & raw_input_flags::ENABLE_RAW_MOUSE_INPUT)
    {
        devices[count].usUsagePage = usb::USAGEPAGE_GENERIC_DESKTOP;
        devices[count].usUsage = usb::USAGE_GENERIC_MOUSE;
        devices[count].dwFlags = 0;// RIDEV_INPUTSINK;  // <-- must sink input
        devices[count].hwndTarget = window;
        ++count;
    }

    if (data.thread_data.flags & raw_input_flags::ENABLE_RAW_KEYBOARD_INPUT)
    {
        devices[count].usUsagePage = usb::USAGEPAGE_GENERIC_DESKTOP;
        devices[count].usUsage = usb::USAGE_GENERIC_KEYBOARD;
        devices[count].dwFlags = 0;// RIDEV_INPUTSINK;  // <-- must sink input
        devices[count].hwndTarget = window;
        ++count;
    }

    // Register the raw input devices with the OS so the thread can receive events.
    if (!::RegisterRawInputDevices(devices, count, sizeof(devices[0])))
    {
        ::DestroyWindow(window);
        return;
    }

    // Set the thread priority very high — raw input needs to be processed quickly
    // to minimize input latency (time-critical input handling)
    ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    // Tell the parent we're ready to go!
    ::SetEvent(data.thread_data.ready_event.get());

    while (!data.thread_data.done)
    {
        const time::time_point idle_begin = os::get_ticks();

        // Wait for either:
        //  - done_event (thread termination)
        //  - or new raw input (QS_RAWINPUT message)
        const DWORD res = ::MsgWaitForMultipleObjects(1, &data.thread_data.done_event.get(), FALSE, INFINITE, QS_RAWINPUT);

        const time::time_point idle_end = os::get_ticks();

        // If the result is not indicating WM_INPUT messages, we’re probably done
        if (res != (WAIT_OBJECT_0 + 1))
        {
            break;
        }

        // Clear the queue status for QS_RAWINPUT
         // This ensures MsgWaitForMultipleObjects() will block again next iteration.
        (void)::GetQueueStatus(QS_RAWINPUT);

        const time::time_point idle_time = idle_end - idle_begin;
        const time::time_point usb_8khz_interval = time::microseconds(125);

        // If we’ve been idle less than one USB frame, reuse the last poll time.
        // Otherwise, update with the new timestamp.
        const time::time_point poll_start = idle_time < usb_8khz_interval ? data.last_raw_input_poll : idle_end;

        // Process the pending raw input messages
        poll(poll_start);
    }

    devices[0].dwFlags |= RIDEV_REMOVE;
    devices[0].hwndTarget = NULL;
    devices[1].dwFlags |= RIDEV_REMOVE;
    devices[1].hwndTarget = NULL;

    ::RegisterRawInputDevices(devices, count, sizeof(devices[0]));
    ::DestroyWindow(window);

    return;
}

///////////////////////////////////////////////////////////////////////////////
// configure
///////////////////////////////////////////////////////////////////////////////

bool raw_input_manager::set_raw_input_enabled(typename raw_input_flags::type flags)
{
    return start_thread(flags);
}

////////////////////////////////////////

bool raw_input_manager::update_raw_input_enabled()
{
    typename raw_input_flags::type flags = raw_input_flags::NONE;

    if (data.raw_mouse_enabled)
    {
        flags |= raw_input_flags::ENABLE_RAW_MOUSE_INPUT;
    }
    if (data.raw_keyboard_enabled)
    {
        flags |= raw_input_flags::ENABLE_RAW_KEYBOARD_INPUT;
    }

    if (flags != data.flags)
    {
        if (!set_raw_input_enabled(flags))
        {
            return false;
        }

        data.flags = flags;
    }

    return true;
}

////////////////////////////////////////

bool raw_input_manager::set_raw_mouse_enabled(bool enabled)
{
    data.raw_mouse_enabled = enabled;

    if (!update_raw_input_enabled())
    {
        data.raw_mouse_enabled = !enabled;
        return false;
    }

    return true;
}

////////////////////////////////////////

bool raw_input_manager::set_raw_keyboard_enabled(bool enabled)
{
    data.raw_keyboard_enabled = enabled;

    if (!update_raw_input_enabled())
    {
        data.raw_keyboard_enabled = !enabled;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// processing
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsevents.c#L545

void raw_input_manager::handle_raw_mouse_input(time::time_point t, os::handle device_handle, RAWMOUSE* raw)
{
    struct raw_button
    {
        USHORT usButtonFlags;
        mouse::buttons button;
        bool down;
    };

    raw_button raw_buttons[] = {

        { RI_MOUSE_LEFT_BUTTON_DOWN, mouse::BUTTON_LEFT, true },
        { RI_MOUSE_LEFT_BUTTON_UP, mouse::BUTTON_LEFT, false },

        { RI_MOUSE_RIGHT_BUTTON_DOWN, mouse::BUTTON_RIGHT, true },
        { RI_MOUSE_RIGHT_BUTTON_UP, mouse::BUTTON_RIGHT, false },

        { RI_MOUSE_MIDDLE_BUTTON_DOWN, mouse::BUTTON_MIDDLE, true },
        { RI_MOUSE_MIDDLE_BUTTON_UP, mouse::BUTTON_MIDDLE, false },

        { RI_MOUSE_BUTTON_4_DOWN, mouse::BUTTON_EXTRA_1, true },
        { RI_MOUSE_BUTTON_4_UP, mouse::BUTTON_EXTRA_1, false },

        { RI_MOUSE_BUTTON_5_DOWN, mouse::BUTTON_EXTRA_2, true },
        { RI_MOUSE_BUTTON_5_UP, mouse::BUTTON_EXTRA_2, false }

    };

    const int dx = static_cast<int>(raw->lLastX);
    const int dy = static_cast<int>(raw->lLastY);
    const bool have_motion = (dx || dy);
    const bool have_button = (raw->usButtonFlags != 0);
    const bool is_absolute = ((raw->usFlags & MOUSE_MOVE_ABSOLUTE) != 0);
    const mouse::mouse_id id = (mouse::mouse_id)(uintptr_t)device_handle.get();

    if (!data.raw_mouse_enabled)
    {
        return;
    }

    // Relative mouse motion is delivered to the window with keyboard focus
    const video::window_id w = video->data.keyboard_ptr->get_focus();
    if (!is_valid_id(w))
    {
        return;
    }

    //if (GetMouseMess)

}

void raw_input_manager::handle_raw_keyboard_input(time::time_point t, os::handle device_handle, RAWKEYBOARD* raw)
{

}

void raw_input_manager::poll(time::time_point start_time)
{
    if (data.raw_input_offset == 0)
    {
        data.raw_input_offset = sizeof(RAWINPUTHEADER);

        BOOL Wow64Process;
        if (::IsWow64Process(::GetCurrentProcess(), &Wow64Process) && Wow64Process)
        {
            // We're going to get 64-bit data, so use the 64-bit RAWINPUTHEADER size
            data.raw_input_offset += 8;
        }
    }

    time::time_point end_time;

    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getrawinputbuffer

    UINT total = 0;
    size_t buffer_offset = 0;
    UINT remaining_size = static_cast<UINT>(data.raw_input.size());
    RAWINPUT* input = NULL;

    // Get all available events
    while (true)
    {
        input = reinterpret_cast<RAWINPUT*>(data.raw_input.data() + buffer_offset);

        UINT required_size = remaining_size;
        UINT count = ::GetRawInputBuffer(input, &required_size, sizeof(RAWINPUTHEADER));

        // mark end time quickly for high accuracy time stamps
        end_time = os::get_ticks();

        if (count == 0 || count == (UINT)-1)
        {
            if (!input || (count == (UINT)-1 && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER))
            {
                if (data.raw_input.size() > 1048576) // 1MB safety cap
                {
                    data.raw_input.clear();
                    break;
                }

                // Windows returns required buffer size in bytes
                const size_t new_size = buffer_offset + required_size;
                data.raw_input.resize(new_size);
                continue;
            }
            else
            {
                break;
            }
        }
        else if (count == 0)
        {
            // No new events available
            break;
        }
        else
        {
            total += count;
            buffer_offset += required_size;
        }
    }

    if (total > 0)
    {
        const time::time_point total_time = end_time - start_time;

        struct event_spacer
        {
            size_t event_total = 0;
            size_t event_index = 0;

            time::time_point next_stamp(time::time_point total, time::time_point end) noexcept
            {
                ++event_index;
                return end - (total * (event_total - event_index)) / event_total;
            }
        };

        // helpers to evenly space raw input events out over the polling interval
        event_spacer mouse_spacer;
        event_spacer keyboard_spacer;

        input = reinterpret_cast<RAWINPUT*>(data.raw_input.data());

        for (UINT i = 0; i < total; ++i)
        {
            switch (input->header.dwType)
            {
                case RIM_TYPEMOUSE:
                {
                    ++mouse_spacer.event_total;
                    break;
                }
                case RIM_TYPEKEYBOARD:
                {
                    ++keyboard_spacer.event_total;
                    break;
                }
                default:
                {
                    break;
                }
            }

            input = NEXTRAWINPUTBLOCK(input);
        }

        input = reinterpret_cast<RAWINPUT*>(data.raw_input.data());

        for (UINT i = 0; i < total; ++i)
        {
            switch (input->header.dwType)
            {
                case RIM_TYPEMOUSE:
                {
                    RAWMOUSE* raw = reinterpret_cast<RAWMOUSE*>(reinterpret_cast<BYTE*>(input) + data.raw_input_offset);
                    const time::time_point t = mouse_spacer.next_stamp(total_time, end_time);
                    handle_raw_mouse_input(t, input->header.hDevice, raw);
                    break;
                }
                case RIM_TYPEKEYBOARD:
                {
                    RAWKEYBOARD* raw = reinterpret_cast<RAWKEYBOARD*>(reinterpret_cast<BYTE*>(input) + data.raw_input_offset);
                    const time::time_point t = keyboard_spacer.next_stamp(total_time, end_time);
                    handle_raw_keyboard_input(t, input->header.hDevice, raw);
                    break;
                }
                default:
                {
                    break;
                }
            }

            input = NEXTRAWINPUTBLOCK(input);
        }
    }

    data.last_raw_input_poll = end_time;
}

} // namespace input
} // namespace app
} // namespace vx