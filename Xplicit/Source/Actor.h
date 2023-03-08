/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Actor.h
 *			Purpose: Actors, or players
 *
 * =====================================================================
 */

#pragma once

#include "ServerInstance.h"
#include "Foundation.h"
#include "Instance.h"
#include "Event.h"
#include "Irr.h"

//
//	File: Actor.h
//	Purpose: This file handles actor logic.
//	Last Edit 3/3/23
//

namespace Xplicit
{
	// Server-Side Actor Class
	class XPLICIT_API ActorInstance final : public Instance
	{
	public:
		struct ActorPos final
		{
			ActorPos(float x, float y, float z)
				: X(x), Y(y), Z(z)
			{}

			~ActorPos() {}

			ActorPos& operator=(const ActorPos&) = default;
			ActorPos(const ActorPos&) = default;

			float X;
			float Y;
			float Z;

		};

	public:
		explicit ActorInstance(const bool human = true);
		virtual ~ActorInstance();

		ActorInstance& operator=(const ActorInstance&) = default;
		ActorInstance(const ActorInstance&) = default;

		int16_t& health() noexcept;
		void reset() noexcept;
		int64_t id() noexcept;
		
		ActorPos& get_pos();
		NETWORK_CMD get_cmd() const;

		virtual const char* name() noexcept override;
		virtual INSTANCE_TYPE type() noexcept override;

		void set(int64_t id) noexcept;
		void set(NETWORK_CMD cmd) noexcept;
		void set(struct sockaddr_in sockaddr) noexcept;
		void set(float x, float y, float z) noexcept;

		virtual void update() override;

	public:
		virtual INSTANCE_PHYSICS physics() noexcept override;
		struct sockaddr_in& sockaddr() noexcept;

		virtual bool can_collide() override;
		virtual bool has_physics() override;
		bool is_colliding() noexcept;

	private:
		struct sockaddr_in m_sockaddr;
		NETWORK_CMD m_network_cmd;
		ActorPos m_pos;

	private:
		int32_t m_respawn_delay;
		int16_t m_health;
		int64_t m_id;

		friend class PlayerActorEvent;

	};

	class XPLICIT_API NetworkActorEvent final : public Event
	{
	public:
		NetworkActorEvent() {}
		virtual ~NetworkActorEvent() {}

		NetworkActorEvent& operator=(const NetworkActorEvent&) = default;
		NetworkActorEvent(const NetworkActorEvent&) = default;

		virtual const char* name() noexcept { return ("NetworkActorEvent"); }

		void add_listener(const std::function<void(ActorInstance*)>& fn);

		virtual void operator()();

	private:
		std::vector<std::function<void(ActorInstance*)>> m_functions;

	};

	class XPLICIT_API PlayerActorEvent final : public Event
	{
	public:
		PlayerActorEvent() {}
		virtual ~PlayerActorEvent() {}

		PlayerActorEvent& operator=(const PlayerActorEvent&) = default;
		PlayerActorEvent(const PlayerActorEvent&) = default;

		virtual const char* name() noexcept { return ("PlayerActorEvent"); }

		void add_listener(const std::function<void(ActorInstance*)>& fn);

		virtual void operator()();

	private:
		std::vector<std::function<void(ActorInstance*)>> m_functions;

	};

	class XPLICIT_API PhysicsActorEvent final : public Event
	{
	public:
		PhysicsActorEvent() {}
		virtual ~PhysicsActorEvent() {}

		PhysicsActorEvent& operator=(const PhysicsActorEvent&) = default;
		PhysicsActorEvent(const PhysicsActorEvent&) = default;

		virtual const char* name() noexcept { return ("ActorPhysicsEvent"); }

		void add_listener(const std::function<void(ActorInstance*)>& fn);

		virtual void operator()();

	private:
		std::vector<std::function<void(ActorInstance*)>> m_functions;

	};
}