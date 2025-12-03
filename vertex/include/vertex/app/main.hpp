#pragma once

#include "vertex/config/platform.hpp"
#include "vertex/config/language_config.hpp"

// This header provides platform-specific entry point wrappers,
// forwarding Windows WinMain (GUI subsystem) to main(argc, argv).
// Allows user code to implement 'main' normally while supporting GUI subsystem builds.

#if !defined(VX_MAIN_HANDLED)

//=============================================================================
// windows
//=============================================================================

#if defined(VX_OS_WINDOWS)

    // Windows:
    // Vertex supplies a WinMain() implementation that parses the command
    // line and forwards the arguments to your main() function.
    // 
    // If you provide your own WinMain(), define VX_MAIN_HANDLED.
#   define VX_MAIN_AVAILABLE

//=============================================================================
// iOS
//=============================================================================

#elif defined(VX_OS_IOS)

    // iOS:
    // Vertex supplies a main() entry point that creates the application
    // delegate and starts the iOS run loop.
    // 
    // Include the Vertex main header in the source file containing main().
#   define VX_MAIN_NEEDED

//=============================================================================
// Android
//=============================================================================

#elif defined(VX_PLATFORM_ANDROID)

    // Android:
    // Vertex provides a Java activity class that serves as the primary
    // application entry point.
#   define VX_MAIN_NEEDED

    // Because the application is launched from Java, the native binary does
    // not provide a standard main().This define ensures that the engine
    // exports a vx_main() entry point for Java to call, without defining a
    // native main(int, char**).
#   define VX_MAIN_EXPORTED

//=============================================================================

#endif

#endif // VX_MAIN_HANDLED

//=============================================================================
// Export Macro
//=============================================================================

// We need to export vx::app::_priv::main_ so it can be launched from external code

#if defined(VX_MAIN_EXPORTED)
#   define VX_MAIN_API VX_API
#else
#   define VX_MAIN_API
#endif // VX_MAIN_EXPORTED

//=============================================================================
// main override
//=============================================================================

#if defined(VX_MAIN_NEEDED) || defined(VX_MAIN_AVAILABLE) || defined(VX_MAIN_USE_CALLBACKS)
#   define main vx::app::_priv::main_
#endif

//=============================================================================

namespace vx {
namespace app {

enum class app_result
{
    continue_,          // app should continue running
    terminate_success,  // app should terminate with success
    terminate_failure   // app should terminate with failure
};

//=============================================================================
// init
//=============================================================================

using init_callback_t = app_result(*)(void** app_state, int argc, char* argv[]);

/**
 * @brief Callback type for application initialization.
 *
 * @param[out] app_state Pointer to a pointer where the application state can be stored.
 *                       The callback can allocate and assign its internal state here.
 * @param argc Command line argument count.
 * @param argv Command line arguments.
 * @return callback_result Indicates whether to continue or terminate initialization.
 *
 * @note The app_state should be used to store any state needed across app callbacks.
 * @note Avoid performing heavy work in this callback; initialize only necessary components.
 * @note Returning terminate_failure or terminate_success will abort app startup.
 */
VX_MAIN_API app_result init_callback(void** app_state, int argc, char* argv[]);

//=============================================================================
// quit
//=============================================================================

using quit_callback_t = void(*)(void* app_state, app_result result);

/**
 * @brief Callback type for application quit/cleanup.
 *
 * @param app_state Pointer to the application state previously set in init callback.
 * @param result The result code indicating the reason for quitting.
 *
 * @note This callback should clean up all resources associated with app_state.
 * @note Do not perform any further state changes after this function returns.
 */
VX_MAIN_API void quit_callback(void* app_state, app_result result);

//=============================================================================
// iter
//=============================================================================

using iterate_callback_t = app_result(*)(void* app_state);

/**
 * @brief Callback type for main application loop iteration.
 *
 * @param app_state Pointer to the application state.
 * @return callback_result Indicates whether the app should continue or terminate.
 *
 * @note This function is called repeatedly during the main loop.
 * @note Avoid blocking operations here; keep iteration fast and responsive.
 * @note Return terminate_success or terminate_failure to exit the main loop.
 */
VX_MAIN_API app_result iterate_callback(void* app_state);

//=============================================================================
// event
//=============================================================================

namespace event { struct event; }
using event_callback_t = app_result(*)(void* app_state, event::event& event);

/**
 * @brief Callback type for handling application events.
 *
 * @param app_state Pointer to the application state.
 * @param event Reference to the event to be processed.
 * @return callback_result Indicates whether the app should continue or terminate.
 *
 * @note This function should process or dispatch events as needed.
 * @note Do not modify or invalidate the event reference.
 * @note Avoid long blocking operations inside this callback.
 */
VX_MAIN_API app_result event_callback(void* app_state, event::event& event);

//=============================================================================
// internal functions
//=============================================================================

namespace _priv {

using main_t = int(*)(int argc, char* argv[]);
VX_MAIN_API int main_(int argc, char* argv[]);
VX_API int run(int argc, char* argv[], main_t main_fn);
VX_API int enter_callbacks(int argc, char* argv[], init_callback_t init_fn, quit_callback_t quit_fn, iterate_callback_t iterate_fn, event_callback_t event_fn);

} // namespace _priv

} // namespace app
} // namespace vx

//=============================================================================

#if !defined(VX_MAIN_HANDLED) && !defined(VX_MAIN_NO_IMPL)
#if defined(VX_MAIN_USE_CALLBACKS) || defined(VXL_MAIN_NEEDED) || defined(VX_MAIN_AVAILABLE)
#   include "vertex/app/_priv/main_impl.hpp"
#endif
#endif