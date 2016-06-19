message("*** Using Wayland target")
set(DEQP_TARGET_NAME 	"WAYLAND")
set(DEQP_SUPPORT_GLES2	ON)
set(DEQP_SUPPORT_GLES3	ON)
set(DEQP_SUPPORT_EGL	ON)

# Use Wayland target
set(DEQP_USE_WAYLAND	ON)

find_package(Wayland)
if (NOT WAYLAND_FOUND)
	message(FATAL_ERROR "Wayland development package not found")
endif ()

set(DEQP_PLATFORM_LIBRARIES ${WAYLAND_LIBRARIES})
include_directories(${WAYLAND_INCLUDE_DIR})
