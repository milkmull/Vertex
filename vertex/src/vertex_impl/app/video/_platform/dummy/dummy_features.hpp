#pragma once

// ------------------------------------------------------------
// Backend lifecycle
// ------------------------------------------------------------

#ifdef VX_VIDEO_BACKEND_HAVE_INIT
#   undef VX_VIDEO_BACKEND_HAVE_INIT
#endif
#define VX_VIDEO_BACKEND_HAVE_INIT 1

#ifdef VX_VIDEO_BACKEND_HAVE_QUIT
#   undef VX_VIDEO_BACKEND_HAVE_QUIT
#endif
#define VX_VIDEO_BACKEND_HAVE_QUIT 1

// ------------------------------------------------------------
// Window management
// ------------------------------------------------------------

#ifdef VX_VIDEO_BACKEND_HAVE_WINDOW_SET_POSITION
#   undef VX_VIDEO_BACKEND_HAVE_WINDOW_SET_POSITION
#endif
#define VX_VIDEO_BACKEND_HAVE_WINDOW_SET_POSITION 1

#ifdef VX_VIDEO_BACKEND_HAVE_WINDOW_SET_SIZE
#   undef VX_VIDEO_BACKEND_HAVE_WINDOW_SET_SIZE
#endif
#define VX_VIDEO_BACKEND_HAVE_WINDOW_SET_SIZE 1

// ------------------------------------------------------------
// Mouse
// ------------------------------------------------------------

#ifdef VX_VIDEO_BACKEND_HAVE_MOUSE_SET_RELATIVE_MOUSE_MODE
#   undef VX_VIDEO_BACKEND_HAVE_MOUSE_SET_RELATIVE_MOUSE_MODE
#endif
#define VX_VIDEO_BACKEND_HAVE_MOUSE_SET_RELATIVE_MOUSE_MODE 1