/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: MonoInterop.cpp
 *			Purpose: C# Classes in C++
 *
 * =====================================================================
 */

#include "MonoInterop.h"

namespace Xplicit
{
	MonoClassInstance::MonoClassInstance(const char* namespase, const char* klass)
		: Instance(), m_klass(nullptr), m_object(nullptr)
		, m_script(InstanceManager::get_singleton_ptr()->find<MonoScriptInstance>("MonoScriptInstance"))
	{
		assert(m_script);

		if (m_script)
			m_klass = m_script->get()->make(m_script, namespase, klass);
		
		if (m_klass)
		{
			// search and call the constructor.
			// surronded with assertions.
			m_object = mono_object_new(m_script->get()->domain(), m_klass);
			mono_runtime_object_init(m_object);
		}
	}

	MonoClassInstance::operator bool() { return m_script; }

	MonoClassInstance::~MonoClassInstance()
	{
		// Free anything that you allocated here.
		// use smart pointers if you can.
	}

	MonoClassInstance::INSTANCE_TYPE MonoClassInstance::type() noexcept
	{
		return SCRIPT;
	}

	const char* MonoClassInstance::name() noexcept
	{
		return ("MonoClassInstance");
	}

	void MonoClassInstance::update() 
	{
		// Nothing to do for now.
	}

	static MonoObject* xplicit_mono_call(const char* method_name, MonoClass* klass)
	{
		if (!method_name) return nullptr;
		if (!klass) return nullptr;

		MonoMethodDesc* update_desc = mono_method_desc_new(method_name, false);

		if (!update_desc)
			return nullptr;

		MonoMethod* method = mono_method_desc_search_in_class(update_desc, klass);

		if (!method)
			return nullptr;

		return mono_runtime_invoke(method, klass, nullptr, nullptr);
	}

	// this method actually updates the C# methods.
	void MonoClassInstance::script_update() noexcept
	{
		if (this->should_update())
			return;

		xplicit_mono_call(":OnUpdate()", m_klass);
	}

	bool MonoClassInstance::can_collide()
	{
		MonoObject* obj = xplicit_mono_call(":CanCollide()", m_klass);
		if (obj) return *((bool*)mono_object_unbox(obj));

		return false;
	}

	bool MonoClassInstance::has_physics()
	{
		MonoObject* obj = xplicit_mono_call(":HasPhysics()", m_klass);
		if (obj) return *((bool*)mono_object_unbox(obj));

		return false;
	}

	bool MonoClassInstance::should_update()
	{
		MonoObject* obj = xplicit_mono_call(":ShouldUpdate()", m_klass);
		if (obj) return *((bool*)mono_object_unbox(obj));

		return false;
	}

	//
	// Mono events
	//

	void MonoUpdateEvent::operator()()
	{
		auto scripts = InstanceManager::get_singleton_ptr()->find_all<MonoClassInstance>("MonoClassInstance");

		for (size_t i = 0; i < scripts.size(); i++)
		{
			if (scripts[i])
				scripts[i]->script_update();
		}
	}

	const char* MonoUpdateEvent::name() noexcept
	{
		return ("MonoUpdateEvent");
	}

}
