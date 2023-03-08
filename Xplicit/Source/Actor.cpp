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

#include "Actor.h"
#include "Event.h"

// for the server api
#include "ServerInstance.h"

// for C# scripts.
#include "MonoInstance.h"

namespace Xplicit
{
	constexpr const int32_t XPLICIT_ACTOR_COOLDOWN = 1000;

	/*
		Actor Instance methods.
	*/

	ActorInstance::ActorInstance(const bool human)
		: m_health(0), m_network_cmd(NETWORK_CMD_INVALID), m_pos(0, 0, 0), m_sockaddr(), m_respawn_delay(0), m_id(-1)
	{
#ifndef _NDEBUG
		XPLICIT_INFO("ActorInstance::ActorInstance");
#endif
	}

	ActorInstance::~ActorInstance()
	{
#ifndef _NDEBUG
		XPLICIT_INFO("ActorInstance::~ActorInstance");
#endif
	}

	struct sockaddr_in& ActorInstance::sockaddr() noexcept { return m_sockaddr; }
	ActorInstance::INSTANCE_PHYSICS ActorInstance::physics() noexcept { return FAST; }

	bool ActorInstance::is_colliding() noexcept { return false; }
	bool ActorInstance::has_physics() { return false; }

	int64_t ActorInstance::id() noexcept { return m_id; }

	void ActorInstance::update() 
	{
		auto server = Xplicit::InstanceManager::get_singleton_ptr()->find<Xplicit::NetworkServerInstance>("NetworkServerInstance");

		if (!server)
			return;

		if (this->get_cmd() == NETWORK_CMD_DEAD)
		{
			if (this->m_respawn_delay > 0)
			{
				--this->m_respawn_delay;
			}
			else if (this->m_respawn_delay < 1)
			{
				this->set(NETWORK_CMD_SPAWN);
				this->m_respawn_delay = 0;

				GenericPacket packet;

				packet.CMD = NETWORK_CMD_SPAWN;
				packet.ID = this->m_id;

				server->queue(packet);
			}
		}

		// prepare a packet 

		if (this->health() < 1)
		{
			this->set(NETWORK_CMD_DEAD);
			this->m_respawn_delay = XPLICIT_ACTOR_COOLDOWN;

			GenericPacket packet;

			packet.CMD = NETWORK_CMD_DEAD;
			packet.ID = this->m_id;
			
			server->queue(packet);
		}
		else if (this->get_cmd() == NETWORK_CMD_POS)
		{
			// just do something as a simple as that
			// rely on the physics engine for collision detection.

			if (this->is_colliding())
				return;

			GenericPacket packet;

			packet.CMD = NETWORK_CMD_POS;
			packet.ID = this->m_id;

			packet.X = m_pos.X;
			packet.Y = m_pos.Y;
			packet.Z = m_pos.Y;

			server->queue(packet);
		}
	}

	int16_t& ActorInstance::health() noexcept { return m_health; }
	bool ActorInstance::can_collide() { return false; }

	ActorInstance::ActorPos& ActorInstance::get_pos() { return m_pos; }

	void ActorInstance::set(int64_t id) noexcept
	{
		if (id < 0) return;
		m_id = id;
	}

	const char* ActorInstance::name() noexcept { return "ActorInstance"; }
	ActorInstance::INSTANCE_TYPE ActorInstance::type() noexcept { return ACTOR; }

	// Actor's Getters

	NETWORK_CMD ActorInstance::get_cmd() const { return m_network_cmd; }

	// Actor's Setters

	void ActorInstance::set(NETWORK_CMD cmd) noexcept { m_network_cmd = cmd; }

	void ActorInstance::set(struct sockaddr_in sockaddr) noexcept { m_sockaddr = sockaddr; }

	void ActorInstance::set(float x, float y, float z)  noexcept
	{
		m_pos.X = x;
		m_pos.X = y;
		m_pos.X = z;
	}

	void ActorInstance::reset() noexcept
	{
		m_health = 0;
		m_id = -1;

		this->set(0.f, 0.f, 0.f);

		memset(&m_sockaddr, 0, sizeof(sockaddr_in));

#ifndef _NDEBUG
		XPLICIT_INFO("[ACTOR] Actor has been reset!");
#endif
	}

	/*
		Actor Events
	*/

	void NetworkActorEvent::add_listener(const std::function<void(ActorInstance*)>& fn)
	{
		if (fn)
			m_functions.push_back(fn);
	}

	void PlayerActorEvent::add_listener(const std::function<void(ActorInstance*)>& fn)
	{
		if (fn)
			m_functions.push_back(fn);
	}

	void PhysicsActorEvent::add_listener(const std::function<void(ActorInstance*)>& fn)
	{
		if (fn)
			m_functions.push_back(fn);
	}

	/*
		Update function goes there.
	*/

	// process and sends packet cmds.
	void NetworkActorEvent::operator()()
	{
		auto vec_actor = InstanceManager::get_singleton_ptr()->find_all<ActorInstance>("ActorInstance");

		if (vec_actor.empty())
			return;

		for (size_t i = 0; i < vec_actor.size(); i++)
		{
			for (size_t y = 0; y < m_functions.size(); y++)
			{
				if (m_functions[y])
					m_functions[y](vec_actor[i]);
			}
		}
	}

	void PlayerActorEvent::operator()()
	{
		auto vec_actor = InstanceManager::get_singleton_ptr()->find_all<ActorInstance>("ActorInstance");
		if (vec_actor.empty()) return;

		//
		// iterate over events, that we attached with this instance.
		//

		for (size_t i = 0; i < vec_actor.size(); i++)
		{
			for (size_t y = 0; y < m_functions.size(); y++)
			{
				if (m_functions[y])
					m_functions[y](vec_actor[i]);
			}
		}
	}

	void PhysicsActorEvent::operator()()
	{
		auto vec_actor = InstanceManager::get_singleton_ptr()->find_all<ActorInstance>("ActorInstance");
		
		if (vec_actor.empty()) 
			return;

		for (size_t i = 0; i < vec_actor.size(); i++)
		{
			for (size_t y = 0; y < m_functions.size(); y++)
			{
				if (m_functions[y])
					m_functions[y](vec_actor[i]);
			}
		}
	}
}