/*-------------------------------------------------------------------------
 * drawElements Quality Program Tester Core
 * ----------------------------------------
 *
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *//*!
 * \file
 * \brief wayland utilities.
 *//*--------------------------------------------------------------------*/

#include "tcuWayland.hpp"
#include "gluRenderConfig.hpp"
#include "deMemory.h"

#include <stdio.h>

namespace tcu
{
namespace wayland
{

enum
{
	DEFAULT_WINDOW_WIDTH	= 400,
	DEFAULT_WINDOW_HEIGHT	= 300
};

EventState::EventState (void)
	: m_quit(false)
{
}

EventState::~EventState (void)
{
}

void EventState::setQuitFlag (bool quit)
{
	de::ScopedLock lock(m_mutex);
	m_quit = quit;
}

bool EventState::getQuitFlag (void)
{
	de::ScopedLock lock(m_mutex);
	return m_quit;
}
const struct wl_registry_listener Display::s_registryListener =
{
	Display::handleGlobal,
	Display::handleGlobalRemove
};

const struct wl_shell_surface_listener Window::s_shellSurfaceListener =
{
	Window::handlePing,
	Window::handleConfigure,
	Window::handlePopupDone,
};

void Display::handleGlobal(void *data,struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version UNUSED)
{
	Display *_this = static_cast<Display*>(data);

	if (!strcmp(interface, "wl_compositor"))
		_this->m_compositor = static_cast<struct wl_compositor*>(wl_registry_bind(registry, id, &wl_compositor_interface, 3));
	/* Todo: when the xdg_shell protocol has stablized, we should move wl_shell to xdg_shell. */
	if (!strcmp(interface, "wl_shell"))
		_this->m_shell = static_cast<struct wl_shell*>(wl_registry_bind(registry, id, &wl_shell_interface, 1));
}

void Display::handleGlobalRemove(void *data UNUSED, struct wl_registry *registry UNUSED, uint32_t name UNUSED)
{
}

Display::Display (EventState& eventState, const char* name)
	: m_eventState	(eventState)
	, m_display		(DE_NULL)
{
	try
	{
		m_display = wl_display_connect(name);
		if (!m_display)
			throw ResourceError("Failed to open display", name, __FILE__, __LINE__);

		m_registry = wl_display_get_registry(m_display);
		if (!m_registry)
			throw ResourceError("Failed to get registry", name, __FILE__, __LINE__);

		wl_registry_add_listener(m_registry, &s_registryListener, this);
		wl_display_roundtrip(m_display);
		if (!m_compositor)
			throw ResourceError("Failed to bind compositor", name, __FILE__, __LINE__);
		if (!m_shell)
			throw ResourceError("Failed to bind shell", name, __FILE__, __LINE__);
	}
	catch (...)
	{
		if (m_shell)
			wl_shell_destroy(m_shell);

		if (m_compositor)
			wl_compositor_destroy(m_compositor);

		if (m_registry)
			wl_registry_destroy(m_registry);

		if (m_display)
			wl_display_disconnect(m_display);

		throw;
	}
}

Display::~Display (void)
{
	if (m_shell)
		wl_shell_destroy(m_shell);

	if (m_compositor)
		wl_compositor_destroy(m_compositor);

	if (m_registry)
		wl_registry_destroy(m_registry);

	if (m_display)
		wl_display_disconnect(m_display);
}

void Display::processEvents (void)
{
}

Window::Window (Display& display, int width, int height)
	: m_display		(display)
{
	try
	{
		m_surface = wl_compositor_create_surface(display.getCompositor());
		if (!m_surface)
			throw ResourceError("Failed to create ", "surface", __FILE__, __LINE__);

		m_shellSurface = wl_shell_get_shell_surface(display.getShell(), m_surface);
		if (!m_shellSurface)
			throw ResourceError("Failed to create ", "shell_surface", __FILE__, __LINE__);

		wl_shell_surface_add_listener(m_shellSurface, &s_shellSurfaceListener, this);
		wl_shell_surface_set_title(m_shellSurface, "CTS for OpenGL (ES)");
		wl_shell_surface_set_toplevel(m_shellSurface);

		if (width == glu::RenderConfig::DONT_CARE)
			width = DEFAULT_WINDOW_WIDTH;
		if (height == glu::RenderConfig::DONT_CARE)
			height = DEFAULT_WINDOW_HEIGHT;

		m_window = wl_egl_window_create(m_surface, width, height);
		if (!m_window)
			throw ResourceError("Failed to create ", "window", __FILE__, __LINE__);
	}
	catch (...)
	{
		throw;
	}
	TCU_CHECK(m_window);
}

void Window::setVisibility (bool visible)
{
	m_visible = visible;
}

void Window::getDimensions (int* width, int* height) const
{
	wl_egl_window_get_attached_size(m_window, width, height);
}

void Window::setDimensions (int width, int height)
{
	wl_egl_window_resize(m_window, width, height, 0, 0);
}

void Window::processEvents (void)
{
}

void Window::handlePing(void *data UNUSED, struct wl_shell_surface *shellSurface, uint32_t serial)
{
	wl_shell_surface_pong(shellSurface, serial);
}

void Window::handleConfigure(void *data UNUSED, struct wl_shell_surface *shellSurface UNUSED, uint32_t edges UNUSED, int32_t width UNUSED, int32_t height UNUSED)
{
}

void Window::handlePopupDone(void *data UNUSED, struct wl_shell_surface *shellSurface UNUSED)
{
}

Window::~Window (void)
{
	if (m_window)
		wl_egl_window_destroy(m_window);
	if (m_shellSurface)
		wl_shell_surface_destroy(m_shellSurface);
	if (m_surface)
		wl_surface_destroy(m_surface);
}

} // wayland
} // tcu