/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Instance.cpp
 *			Purpose: Instance System
 *
 * =====================================================================
 */

#include "Instance.h"

namespace Xplicit {
	void InstanceManager::update() noexcept
	{
		// invalid instance list.
		std::vector<size_t> m_invalid_instances;

		for (size_t i = 0; i < m_instances.size(); i++)
		{
			// either delete or invalidate the instance.
			if (m_instances[i])
			{
				if (m_instances[i]->should_update())
					m_instances[i]->update();
			}
			else
			{
				m_invalid_instances.push_back(i);
			}
		}

		// clean out some mess.
		for (size_t i = 0; i < m_invalid_instances.size(); ++i)
		{
			// am i even deleting some that is valid?
			if (!m_instances.at(m_invalid_instances[i]))
			{
				auto element = std::find(m_instances.cbegin(), m_instances.cend(), m_instances[m_invalid_instances[i]]);

				if (element != m_instances.cend())
				{
					m_instances.erase(element);

#ifndef _NDEBUG
					XPLICIT_INFO("Removed unused Instance..");
#endif
				}
			}
		}
	}

	InstanceManager* InstanceManager::get_singleton_ptr()
	{
		static InstanceManager* ptr;
		if (!ptr)
			ptr = new InstanceManager();

		return ptr;
	}

	const char* Instance::name() noexcept { return ("Instance"); }
	Instance::INSTANCE_TYPE Instance::type() noexcept { return INSTANCE; }
	void Instance::update() {}

	bool Instance::can_collide() { return false; }
	bool Instance::has_physics() { return false; }

	bool Instance::should_update() { return true; }

	Instance::INSTANCE_PHYSICS Instance::physics() noexcept { return FAST; }
}
