/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Instance.inl
 *			Purpose: Instance System
 *
 * =====================================================================
 */


template <typename T, typename... Args>
T* Xplicit::InstanceManager::add(Args&&... args)
{
	T* ptr = new T{ args... };
	m_instances.push_back(ptr);

	return ptr;
}

template <typename T>
T* Xplicit::InstanceManager::find(const char* name)
{
	for (size_t i = 0; i < m_instances.size(); ++i)
	{
		if (!m_instances[i])
			continue;

		if (strcmp(name, m_instances[i]->name()) == 0)
			return static_cast<T*>(m_instances[i]);
	}

	return nullptr;
}

template <typename T>
std::vector<T*> Xplicit::InstanceManager::find_all(const char* name)
{
	if (!name)
		return {};

	if (*name == 0)
		return {};

	std::vector<T*> list;

	for (size_t i = 0; i < m_instances.size(); ++i)
	{
		if (!m_instances[i])
			continue;

		if (strcmp(name, m_instances[i]->name()) == 0)
			list.push_back(static_cast<T*>(m_instances[i]));
	}

	return list;
}

template <typename T>
bool Xplicit::InstanceManager::remove(const char* name)
{
	if (!name)
		return false;

	if (*name == 0)
		return false;

	for (auto it = 0; it < m_instances.size(); ++it)
	{
		if (m_instances[it] && strcmp(name, m_instances[it]->name()) == 0)
		{
			auto obj = m_instances[it];

			auto element = std::find(m_instances.cbegin(), m_instances.cend(), m_instances[it]);

			if (element != m_instances.cend())
				m_instances.erase(element);

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
