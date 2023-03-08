/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Event.inl
 *			Purpose: Xplicit Event System
 *
 * =====================================================================
 */

template <typename T, typename... Args>
T* Xplicit::EventDispatcher::add(Args&&... args)
{
	T* ptr = new T{ args... };
	m_events.push_back(ptr);

	return ptr;
}

template <typename T>
T* Xplicit::EventDispatcher::find(const char* name)
{
	for (size_t i = 0; i < m_events.size(); i++)
	{
		if (!m_events[i])
			continue;

		if (strcmp(name, m_events[i]->name()) == 0)
			return static_cast<T*>(m_events[i]);
	}

	return nullptr;
}

template <typename T>
bool Xplicit::EventDispatcher::remove(const char* id)
{
	if (!id)
		return false;

	if (*id == 0)
		return false;

	for (auto it = 0; it < m_events.size(); ++it)
	{
		T* obj = static_cast<T*>(m_events[it]);

		if (obj && (strcmp(obj->name(), id) == 0))
		{
			//
			auto where_is = std::find(m_events.cbegin(), m_events.cend(), m_events[it]);

			if (where_is != m_events.cend())
				m_events.erase(where_is);

			// finally remove the object
			delete obj;

#ifndef _NDEBUG
			XPLICIT_INFO("Dropped Instance from InstanceManager");
#endif

			return true;
		}
	}

	return false;
}