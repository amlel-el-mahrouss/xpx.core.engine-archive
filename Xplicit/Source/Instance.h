/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Instance.h
 *			Purpose: Instance System
 *
 * =====================================================================
 */

#pragma once

#include "Foundation.h"

//
// Xplicit Instance System
// For C++/C# instances.
//

namespace Xplicit 
{
	class Instance;
	class InstanceManager;

	class XPLICIT_API InstanceManager final 
	{
	private:
		InstanceManager() {}

	public:
		~InstanceManager() {}

		InstanceManager& operator=(const InstanceManager&) = default;
		InstanceManager(const InstanceManager&) = default;

		template <typename T>
		std::vector<T*> find_all(const char* name);

		template <typename T, typename... Args>
		T* add(Args&&... args);

		template <typename T>
		bool remove(const char* name);

		template <typename T>
		T* find(const char* name);

		void update() noexcept;

	public:
		static InstanceManager* get_singleton_ptr();


	private:
		std::vector<Instance*> m_instances;

		friend class Instance;

	};

	class XPLICIT_API Instance 
	{
	public:
		Instance() {}
		virtual ~Instance() {}

		Instance& operator=(const Instance&) = default;
		Instance(const Instance&) = default;

		enum INSTANCE_TYPE : uint8_t 
		{
			MESH,
			ACTOR,
			LOGIC,
			CAMERA,
			SCRIPT,
			NETWORK,
			TERRAIN,
			INSTANCE,
			INSTANCE_TYPE_COUNT
		};

		virtual const char* name() noexcept;
		virtual INSTANCE_TYPE type() noexcept;
		virtual void update();

		virtual bool can_collide();
		virtual bool has_physics();

		virtual bool should_update();

		enum INSTANCE_PHYSICS : uint8_t
		{
			FAST,
			COMPLEX,
			INSTANCE_PHYSICS_COUNT,
		};

		virtual INSTANCE_PHYSICS physics() noexcept;

	};
}

#include "Instance.inl"