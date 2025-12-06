#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/flags.hpp"

namespace vx {
namespace app {

/**
 * @brief Flags controlling which application subsystems are initialized.
 *
 * These flags are used with init_subsystem(), quit_subsystem(), and
 * is_subsystem_init(). They define optional components of the runtime such as
 * event handling, video, audio, etc..
 *
 * Multiple flags may be OR-combined. The @ref everything and
 * @ref requires_events convenience masks are provided for common cases.
 */
VX_FLAGS_DECLARE_BEGIN(init_flags)
{
    none                = 0,          // no subsystems

    events              = VX_BIT(0),  // Core event system. Required for event dispatch.
    video               = VX_BIT(1),  // Video / windowing subsystem.
    audio               = VX_BIT(2),  // Audio playback subsystem.
    camera              = VX_BIT(3),  // Camera / capture subsystem.

    everything          = events | video | audio | camera,  // All subsystems.
    requires_events     = events | video | audio | camera   // Alias for all subsystems that depend on events.
}
VX_FLAGS_DECLARE_END(init_flags)

/**
 * @brief Initializes the base application runtime.
 *
 * This must be called exactly once before using any other vx::app functionality.
 * It performs global setup required by the framework and may implicitly
 * initialize mandatory low-level services.
 *
 * This function is automatically called by the framework before user
 * init callbacks are invoked in the generic runner. Users normally do not need
 * to call it manually unless bypassing the standard runner.
 *
 * @return true on success, false if initialization fails.
 *
 * @note Initialization is idempotent; repeated calls return true once already initialized.
 */
VX_API bool init();

/**
 * @brief Returns whether the application runtime has been initialized.
 */
VX_API bool is_init();

/**
 * @brief Shuts down the base application runtime.
 *
 * This must be called exactly once after the application has finished running.
 * All subsystems are shut down and all resources owned by the base runtime are
 * released.
 *
 * In the generic runner, this function is called automatically after the quit
 * callback; user code should not call it directly.
 */
VX_API void quit();

/**
 * @brief Initializes one or more optional subsystems.
 *
 * Subsystems are **reference counted**:
 * - Calling init_subsystem() on a subsystem increments its internal reference count.
 * - A subsystem is considered initialized once its reference count is > 0.
 * - Initialization logic runs only on the transition from count 0 -> 1.
 *
 * This may be called multiple times, and subsystems already initialized remain
 * active. The call always succeeds for already-initialized subsystems.
 *
 * In callback-driven applications (using the generic runner), the event
 * subsystem may already have been initialized by the framework before user
 * code runs. User initialization will increase the reference count further.
 *
 * @param flags Bitmask of subsystems to initialize.
 * @return The set of subsystems that were successfully initialized during this
 *         call (i.e., those whose reference count transitioned from 0 -> 1).
 */
VX_API init_flags init_subsystem(init_flags flags);

/**
 * @brief Checks whether the specified subsystems are initialized.
 *
 * A subsystem is considered initialized when its internal reference count is > 0.
 *
 * @param flags Bitmask of subsystems to test.
 * @return true if every subsystem in @p flags is currently initialized.
 */
VX_API bool is_subsystem_init(init_flags flags);

/**
 * @brief Decrements the reference count for one or more subsystems.
 *
 * Subsystems are **reference counted**:
 * - Each quit_subsystem() call decrements the reference count for the specified
 *   subsystems.
 * - The subsystem is actually shut down only when the reference count reaches 0.
 *
 * This means a subsystem may remain active even after quit_subsystem() is
 * called, if another part of the application or the framework has previously
 * incremented its reference count.
 *
 * Example:
 * - The generic runner initializes the event subsystem.
 * - User code also calls init_subsystem(events).
 * - When user code calls quit_subsystem(events), the subsystem does **not**
 *   fully shut down because the runner still holds one reference.
 *
 * @param flags Bitmask of subsystems whose reference count should be decremented.
 */
VX_API void quit_subsystem(init_flags flags);

/**
 * @brief Metadata describing the application.
 *
 * Applications may populate this structure to provide descriptive information
 * about the program. Metadata is optional but useful for platform integration,
 * diagnostics, and OS-level introspection.
 *
 * None of the fields are required; any may be nullptr.
 *
 * Typical fields:
 * - @ref name: Human-readable application name.
 * - @ref version: Version string.
 * - @ref id: Package or bundle identifier.
 * - @ref creator: Company or developer name.
 * - @ref url: Website or support URL.
 */
struct app_metadata
{
    const char* name = nullptr;
    const char* version = nullptr;
    const char* id = nullptr;
    const char* creator = nullptr;
    const char* url = nullptr;
};

/**
 * @brief Retrieves the current application metadata.
 *
 * @param metadata Output structure populated with the current metadata.
 * @return true if metadata is available, false otherwise.
 *
 * All fields may be nullptr if metadata has not been set.
 */
VX_API bool get_metadata(app_metadata& metadata);

/**
 * @brief Sets the application metadata.
 *
 * User code may call this at startup to populate human-readable application
 * information used by various subsystems and platform layers.
 *
 * @param metadata Structure containing the new metadata to apply.
 */
VX_API void set_metadata(const app_metadata& metadata);

} // namespace app
} // namespace vx
