/*
 * =====================================================================
 * 
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 * 
 *			File: MonoInterop.h
 *			Purpose: C# Classes in C++
 * 
 * =====================================================================
 */

#pragma once

// foundation headers

#include "Event.h"
#include "Instance.h"
#include "Foundation.h"

// for C#
#include "MonoInstance.h"

namespace Xplicit
{
	class XPLICIT_API MonoClassInstance : public Instance
	{
	public:
		MonoClassInstance() = delete;
		MonoClassInstance(const char* namespase, const char* ctor);
		virtual ~MonoClassInstance();

		MonoClassInstance& operator=(const MonoClassInstance&) = default;
		MonoClassInstance(const MonoClassInstance&) = default;

		virtual INSTANCE_TYPE type() noexcept override;
		virtual const char* name() noexcept override;

		virtual void update() override;
		void script_update() noexcept;

		virtual bool can_collide() override;
		virtual bool has_physics() override;

		virtual bool should_update() override;

		operator bool();

	private:
		Ref<MonoScriptInstance*> m_script; // Image Handle, for the specified Mono app.
		MonoObject* m_object;
		MonoClass* m_klass;

	};

	class XPLICIT_API MonoUpdateEvent : public Event
	{
	public:
		MonoUpdateEvent() {}
		virtual ~MonoUpdateEvent() {}

		MonoUpdateEvent& operator=(const MonoUpdateEvent&) = default;
		MonoUpdateEvent(const MonoUpdateEvent&) = default;

		virtual void operator()() override;

		virtual const char* name() noexcept override;

	};
}
