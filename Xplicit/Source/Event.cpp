/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Event.cpp
 *			Purpose: Xplicit Event System
 *
 * =====================================================================
 */

#include "Event.h"

namespace Xplicit
{
	void EventDispatcher::update() noexcept
	{
		for (size_t i = 0; i < m_events.size(); i++)
		{
			(*m_events[i])();
		}
	}

	EventDispatcher* EventDispatcher::get_singleton_ptr()
	{
		static EventDispatcher* ptr;

		if (!ptr)
			ptr = new EventDispatcher();

		return ptr;
	}


	void Event::operator()() {}

	const char* Event::name() noexcept { return ("Event"); }
}