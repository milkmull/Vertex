#pragma once

#include "vertex/config/os.hpp"

// clang-format off

#if !defined(VX_APP_MAIN_HANDLED) && !defined(VX_APP_MAIN_NO_IMPL)

    // The implementations below must be able to call the real main() without any
    // macro-based renaming. The user's main() will be renamed to vx::app_main so
    // that this file can invoke it explicitly.
    #if defined(main)
        #undef main
    #endif

    //=============================================================================
    // callbacks
    //=============================================================================

    #if defined(VX_APP_USE_CALLBACKS)

        // If a platform requires a special entry point for callback - driven execution,
        // implement it in the #if 0 section below.None currently do.
        //
        // Otherwise, we use the standard callback entry, and the engine supplies
        // vx::app::_priv::main_() as the app entry point.Applications must * not*
        // define their own vx::app_main() in this mode.

        #if 0

            // Placeholder for platform-specific callback-based entry points.

        #else

            // we are using callbacks but the user has not
            // defined an entry point so we define one here
            #define VX_APP_CALLBACK_STANDARD

            int vx::app::_priv::main_(int argc, char** argv)
            {
                return enter_callbacks(argc, argv, init_callback, iterate_callback, event_callback, quit_callback);
            }

        #endif // callback platform branch

    #endif // VX_APP_USE_CALLBACKS

    // Set up usual main stuff if we are not using callbacks or if we are but need
    // the normal entry point, unless the real entry point needs to be somewhere
    // else, like Android where it is in Java code

    #if (!defined(VX_APP_USE_CALLBACKS) || defined(VX_APP_CALLBACK_STANDARD)) && !defined(VX_APP_MAIN_EXPORTED)

        //=============================================================================
        // Windows entry points
        //=============================================================================

        #if defined(VX_OS_WINDOWS)

            // normal main required for CRT setup
            extern int main(int argc, char* argv[]);

            // Basic Win32 declarations needed to define WinMain variations.
            #if !defined(WINAPI)
                #define WINAPI __stdcall
            #endif

            typedef struct HINSTANCE__* HINSTANCE;
            typedef char*               LPSTR;
            typedef wchar_t*            PWSTR;

            //=============================================================================
            // C runtime entry points for Visual C++
            //=============================================================================

            #if defined(_MSC_VER)

                #if defined(UNICODE) && UNICODE

                    int wmain(int, wchar_t*[], wchar_t*)
                    {
                        return vx::app::_priv::run(0, NULL, vx::app::_priv::main_);
                    }

                #else /* ANSI */

                    int main(int, char*[])
                    {
                        return vx::app::_priv::run(0, NULL, vx::app::_priv::main_);
                    }

                #endif

            #endif // _MSC_VER

            //=============================================================================
            // GUI entry (WinMain / wWinMain)
            //=============================================================================

            #if defined(__cplusplus)
            extern "C" {
            #endif

            #if defined(UNICODE) && UNICODE
            int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
            #else /* ANSI */
            int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
            #endif
            {
                return vx::app::_priv::run(0, NULL, vx::app::_priv::main_);
            }

            #if defined(__cplusplus)
            } // extern C
            #endif

        #else // platforms that use a standard main() and just call run(), like iOS

            //=============================================================================
            // Standard main() path
            //=============================================================================

            int main(int argc, char* argv[])
            {
                return vx::app::_priv::run(argc, argv, vx::app::_priv::main_);
            }

            //=============================================================================

        #endif // entry-selection branch

    #endif // (!defined(VX_APP_USE_CALLBACKS) || defined(VX_APP_CALLBACK_STANDARD)) && !defined(VX_APP_MAIN_EXPORTED)

    /**
     * @def main
     * @brief Redirects the user's `main` symbol to Vertex's internal main function.
     *
     * When platform logic requires use of the internal `vx::app::_priv::main_`,
     * this macro redefines `main` so that user code still declares a normal
     * `int main(int,char**)`.
     */
    #define main vx::app::_priv::main_

#endif // !VX_APP_MAIN_HANDLED && !VX_APP_MAIN_NO_IMPL

// clang-format on
