#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/platform.hpp"

// Provides a unified mechanism for defining application entry points across
// different platforms (Windows, iOS, Android, etc.).  
// This allows user code to define a normal `main(int,char**)` function while
// Vertex supplies platform-specific entry wrappers when necessary.
// 
// Users targeting platforms with special entry semantics may include this header
// to ensure their app is launched correctly regardless of the platform’s native
// entry-point requirements.

//=========================================================================
// Platform Entry-Point Detection
//=========================================================================

/**
 * @def VX_APP_MAIN_HANDLED
 * @brief Define to indicate the user provides their own platform entry point.
 *
 * When not defined, Vertex may supply platform-specific entry wrappers such as
 * WinMain on Windows or custom entry logic on mobile platforms.
 */

/**
 * @def VX_APP_MAIN_AVAILABLE
 * @brief Indicates Vertex provides a compatible entry point on this platform.
 */

/**
 * @def VX_APP_MAIN_NEEDED
 * @brief Indicates this platform requires Vertex to supply a nonstandard main().
 */

/**
 * @def VX_APP_MAIN_EXPORTED
 * @brief Indicates the engine must export `vx_main()` for use by an external launcher.
 */

#if !defined(VX_APP_MAIN_HANDLED)

    //=============================================================================
    // windows
    //=============================================================================

    #if defined(VX_OS_WINDOWS)

        // Windows builds using the GUI subsystem do not call `main()`.
        // Vertex supplies a WinMain implementation which forwards control
        // to the user-defined `main(int,char**)`.
        // 
        // Users may provide their own WinMain and suppress this by defining
        // VX_APP_MAIN_HANDLED.
        #define VX_APP_MAIN_AVAILABLE

    //=============================================================================
    // iOS
    //=============================================================================

    #elif defined(VX_OS_IOS)

        // iOS requires a custom `main()` that constructs the application
        // delegate and enters the Cocoa Touch run loop. Vertex supplies this
        // entry point automatically.
        #define VX_APP_MAIN_NEEDED

    //=============================================================================
    // Android
    //=============================================================================

    #elif defined(VX_PLATFORM_ANDROID)

        // Android applications are launched from Java, not from a native `main()`.
        // Vertex therefore supplies a native entry function (`vx_main`) which
        // is invoked from Java.
        #define VX_APP_MAIN_NEEDED

        // Ensures the engine exports a `vx_main()` native entry point instead of
        // a traditional `main(int,char**)`.
        #define VX_APP_MAIN_EXPORTED

    //=============================================================================

    #endif

#endif // VX_APP_MAIN_HANDLED

//=============================================================================
// Export Macro
//=============================================================================

// Only defined when a platform (e.g., Android) requires the entry functions to
// be exported for external callers.

#if defined(VX_APP_MAIN_EXPORTED)
    #define VX_APP_MAIN_API VX_API
#else
    #define VX_APP_MAIN_API
#endif // VX_APP_MAIN_EXPORTED

//=============================================================================
// main override
//=============================================================================

#if defined(VX_APP_MAIN_NEEDED) || defined(VX_APP_MAIN_AVAILABLE) || defined(VX_APP_USE_CALLBACKS)
    #define main vx::app::_priv::main_
#endif

//=============================================================================

namespace vx {
namespace app {

//=========================================================================
// Result Enum
//=========================================================================

/**
 * @enum app_result
 * @brief Signals whether the application should continue running or terminate.
 *
 * Used as the return type for all app callbacks to control application flow.
 */
enum class app_result
{
    continue_,         // Continue running the application loop.
    terminate_success, // Exit the application with success.
    terminate_failure  // Exit the application with failure.
};

//=============================================================================
// init
//=============================================================================

/**
 * @typedef init_callback_t
 * @brief Signature for application initialization callbacks.
 *
 * This callback is invoked once after Vertex performs its own internal
 * initialization (`app::init()` and minimal subsystem setup). The user may
 * allocate and initialize application-specific state here.
 *
 * Engine behavior before and after this callback:
 * - The engine itself is already initialized before this runs.
 * - The event subsystem will be automatically initialized by Vertex if the
 *   user has not done so.
 * - If initialization succeeds (returns `continue_`), Vertex will attach an
 *   internal event watcher responsible for immediate dispatch of certain
 *   high-priority events (e.g., app-terminating, low-memory, backgrounding).
 *
 * Recommended usage:
 * - Allocate and store any application state in `app_state`. This pointer is
 *   passed to later callbacks.
 * - Perform lightweight setup only. Heavy initialization should be moved to the
 *   first iteration callback to avoid startup stalls.
 * - Avoid processing events here; the event system may not yet be fully active.
 * - Avoid starting long-running tasks, threads, or blocking I/O before the event
 *   system is guaranteed to be ready.
 *
 * Incorrect usage:
 * - Do not assume rendering, audio, or other subsystems are active unless
 *   explicitly initialized by the user.
 * - Do not rely on events being delivered during this phase.
 * - Do not hold references to transient initialization data outside this
 *   callback unless explicitly stored in `app_state`.
 *
 * @param[out] app_state Location where the user may store their application data.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return app_result `continue_` on success; any terminating value aborts startup.
 */
using init_callback_t = app_result (*)(void** app_state, int argc, char* argv[]);

/**
 * @brief Default initialization callback symbol exported when needed.
 *
 * The engine calls this during startup if the user registers callbacks rather
 * than defining a traditional `main`.
 *
 * @copydetails init_callback_t
 */
VX_APP_MAIN_API app_result init_callback(void** app_state, int argc, char* argv[]);

//=============================================================================
// iter
//=============================================================================

/**
 * @typedef iterate_callback_t
 * @brief Signature for per-frame iteration callbacks.
 *
 * Called once per main-loop iteration *after* the engine has pumped events and
 * dispatched all queued events (including those dispatched immediately through
 * the internal event watcher).
 *
 * Engine behavior before calling this function each iteration:
 * - Vertex optionally pumps OS/engine events.
 * - All queued events are delivered to the user via `event_callback_t` before
 *   this function runs.
 * - Special events (e.g., app_terminating, low-memory, background transitions)
 *   may be dispatched immediately, even between iterations.
 * - If a terminating result has been set by an event callback, this function
 *   will not be invoked for that iteration.
 *
 * Recommended usage:
 * - Implement core per-frame logic such as simulation, input interpretation,
 *   resource updates, timers, scene management, etc.
 * - Keep this fast and predictable to maintain stable framerate.
 * - Handle the consequences of any events processed earlier in the same frame.
 * - Iterate application-specific subsystems using the state stored in `app_state`.
 *
 * Incorrect usage:
 * - Do not block for long periods (disk I/O, sleeps, network waits).
 * - Do not assume this will always run before shutdown; terminating events may
 *   preemptively skip iteration.
 * - Do not rely on events being deferred until after this callback; some events
 *   are dispatched immediately before iteration begins.
 *
 * @param app_state User-managed application state pointer from init callback.
 * @return app_result `continue_` to keep running; termination results exit the loop.
 */
using iterate_callback_t = app_result (*)(void* app_state);

/**
 * @brief Default iteration callback symbol exported when needed.
 *
 * @copydetails iterate_callback_t
 */
VX_APP_MAIN_API app_result iterate_callback(void* app_state);

//=============================================================================
// event
//=============================================================================

namespace event {
struct event;
}

/**
 * @typedef event_callback_t
 * @brief Signature for event-handling callbacks.
 *
 * Called whenever an event is dispatched by the engine. There are two event
 * delivery paths:
 *
 * 1. **Immediate dispatch**: Certain critical events are dispatched *before*
 *    control returns to the main loop. These include:
 *    - app_terminating
 *    - app_low_memory
 *    - app_will_enter_background
 *    - app_did_enter_background
 *    - app_will_enter_foreground
 *    - app_did_enter_foreground
 *
 *    These events preempt normal event processing and may be delivered in the
 *    middle of iteration or initialization. If an event indicates termination,
 *    the engine sets the termination result immediately.
 *
 * 2. **Queued dispatch**: Regular events are stored and then delivered in batches
 *    at the start of each iteration, before `iterate_callback_t` is invoked.
 *
 * Recommended usage:
 * - Respond to high-level system transitions (backgrounding, low memory, etc.).
 * - Update internal state immediately upon receiving high-priority events.
 * - Handle all normal input/window/application events with minimal overhead.
 * - Ensure event handling is idempotent; some events may occur in bursts.
 *
 * Incorrect usage:
 * - Do not block, especially inside immediate events (risk of stalling the app).
 * - Do not attempt to delay handling of immediate events; they require rapid
 *   response before the loop continues.
 * - Do not modify the event object in unsupported ways.
 * - Do not assume this callback is always followed by an iteration callback.
 *   Immediate events may directly trigger shutdown.
 *
 * @param app_state User-managed application state.
 * @param event The event currently being processed.
 * @return app_result `continue_` to continue; a terminating result stops further dispatch.
 */
using event_callback_t = app_result (*)(void* app_state, event::event& event);

/**
 * @brief Default event callback symbol exported when needed.
 *
 * @copydetails event_callback_t
 */
VX_APP_MAIN_API app_result event_callback(void* app_state, event::event& event);

//=============================================================================
// quit
//=============================================================================

/**
 * @typedef quit_callback_t
 * @brief Signature for shutdown callbacks.
 *
 * Called when the application is terminating for any reason, after the engine
 * has stopped dispatching events. Vertex guarantees:
 * - No further callbacks will be invoked after this returns.
 * - All special terminating events (including app_terminating) have already been
 *   delivered before entering this function.
 * - The internal event watcher has been removed.
 *
 * Recommended usage:
 * - Release all memory and resources associated with `app_state`.
 * - Shut down any external systems you created (threads, GPU resources, files).
 * - Optionally log shutdown information or analytics.
 *
 * Incorrect usage:
 * - Do not attempt to reinitialize systems or re-enter the main loop.
 * - Do not dispatch events or rely on the event system.
 * - Do not assume asynchronous tasks can complete; cancel and free them instead.
 * - Do not access `app_state` after this returns, as the engine will null it.
 *
 * @param app_state User-managed application state created during init.
 * @param result Termination reason (success or failure).
 */
using quit_callback_t = void (*)(void* app_state, app_result result);

/**
 * @brief Default quit callback symbol exported when needed.
 *
 * @copydetails quit_callback_t
 */
VX_APP_MAIN_API void quit_callback(void* app_state, app_result result);

//=============================================================================
// internal functions
//=============================================================================

namespace _priv {

using main_t = int (*)(int argc, char* argv[]);
VX_APP_MAIN_API int main_(int argc, char* argv[]);

VX_API int run(int argc, char* argv[], main_t main_fn);

VX_API int enter_callbacks(
    int argc,
    char* argv[],
    init_callback_t init_fn,
    iterate_callback_t iterate_fn,
    event_callback_t event_fn,
    quit_callback_t quit_fn);

} // namespace _priv

} // namespace app
} // namespace vx

//=============================================================================

#if !defined(VX_APP_MAIN_HANDLED) && !defined(VX_APP_MAIN_NO_IMPL)
    #if defined(VX_APP_USE_CALLBACKS) || defined(VX_APP_MAIN_NEEDED) || defined(VX_APP_MAIN_AVAILABLE)
        #include "vertex/app/_priv/main_impl.hpp"
    #endif
#endif
