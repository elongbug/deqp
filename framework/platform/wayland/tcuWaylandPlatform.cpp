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
 * \brief wayland Platform.
 *//*--------------------------------------------------------------------*/

#include "tcuWaylandPlatform.hpp"
#include "tcuWaylandEglPlatform.hpp"

#include "deUniquePtr.hpp"
#include "gluPlatform.hpp"
#include "tcuWayland.hpp"

namespace tcu
{
namespace wayland
{

class WaylandGLPlatform : public glu::Platform
{
public:
	void		registerFactory	(de::MovePtr<glu::ContextFactory> factory)
	{
		m_contextFactoryRegistry.registerFactory(factory.release());
	}
};

class WaylandPlatform : public tcu::Platform
{
public:
							WaylandPlatform	(void);
	bool					processEvents	(void) { return !m_eventState.getQuitFlag(); }
	const glu::Platform&	getGLPlatform	(void) const { return m_glPlatform; }

	const eglu::Platform&	getEGLPlatform	(void) const { return m_eglPlatform; }

private:
	EventState				m_eventState;
	wayland::egl::Platform	m_eglPlatform;
	WaylandGLPlatform		m_glPlatform;
};

WaylandPlatform::WaylandPlatform (void)
	: m_eglPlatform	(m_eventState)
{
	m_glPlatform.registerFactory(m_eglPlatform.createContextFactory());
}

} // wayland
} // tcu

tcu::Platform* createPlatform (void)
{
	return new tcu::wayland::WaylandPlatform();
}