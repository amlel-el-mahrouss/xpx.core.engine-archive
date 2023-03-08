/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Event.h
 *			Purpose: Xplicit Event System
 *
 * =====================================================================
 */

#pragma once

#include "Foundation.h"
#include "Instance.h"

namespace Xplicit {
	class Event;
	class EventDispatcher;

	class XPLICIT_API EventDispatcher final 
	{
	private:
		EventDispatcher() {}

	public:
		~EventDispatcher() {}

		EventDispatcher& operator=(const EventDispatcher&) = default;
		EventDispatcher(const EventDispatcher&) = default;

		void update() noexcept;

		template <typename T, typename... Args>
		T* add(Args&&... args);

		template <typename T>
		T* find(const char* name);

		template <typename T>
		bool remove(const char* name);
		
	public:
		static EventDispatcher* get_singleton_ptr();

	private:
		std::vector<Event*> m_events;

		friend class Event;

	};

	class XPLICIT_API Event 
	{
	public:
		Event() {}
		virtual ~Event() {}

		Event& operator=(const Event&) = default;
		Event(const Event&) = default;

		virtual void operator()();
		virtual const char* name() noexcept;

	};

}

#include "Event.inl"