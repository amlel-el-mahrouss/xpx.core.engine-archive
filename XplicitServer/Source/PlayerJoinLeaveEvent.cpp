/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: PlayerJoinLeaveEvent.h
 *			Purpose: Player Join and Leave
 *
 * =====================================================================
 */

#include "PlayerJoinLeaveEvent.h"

namespace Xplicit
{
	bool PlayerJoinLeaveEvent::handle_join_success(NetworkServerInstance* server, GenericPacket& packet, ActorInstance* actor) noexcept
	{
		if (!actor)
			return false;

		if (!server)
			return false;

		packet.CMD = Xplicit::NETWORK_CMD_ACCEPT;
		packet.ID = actor->id();

		server->queue(packet);
		server->target(actor->sockaddr());

#ifndef _NDEBUG
		XPLICIT_INFO("[ACTOR] Sending NETWORK_CMD_ACCEPT..");
#endif

		return true;
	}

	bool PlayerJoinLeaveEvent::handle_new_player() noexcept
	{
		if (!InstanceManager::get_singleton_ptr())
			return false;

		auto serv_event = EventDispatcher::get_singleton_ptr()->find<NetworkServerEvent>("ServerEvent");

		if (!serv_event)
			return false;

		if (auto server = InstanceManager::get_singleton_ptr()->find<NetworkServerInstance>("NetworkServerInstance"))
		{
			auto packet = serv_event->get();

			if (packet.CMD == Xplicit::NETWORK_CMD_BEGIN)
			{
				auto vec_actor = InstanceManager::get_singleton_ptr()->find_all<Xplicit::ActorInstance>("ActorInstance");

				for (size_t i = 0; i < vec_actor.size(); i++)
				{
					auto actor = vec_actor[i];

					// fail if client already exists in the server.
					if (actor->sockaddr().sin_addr.S_un.S_addr == server->get().sin_addr.S_un.S_addr)
						return false;

					if (actor->sockaddr().sin_addr.S_un.S_addr == 0)
					{
						actor->set(server->get());
						actor->set(m_id_counter);

						++m_id_counter;

#ifndef _NDEBUG
						XPLICIT_INFO("[ACTOR] Reused inactive actor..");
#endif // !_NDEBUG

						this->handle_join_success(server, packet, actor);

						return true;
					}
				}

				auto actor = InstanceManager::get_singleton_ptr()->add<Xplicit::ActorInstance>(true);

				if (actor)
				{
					actor->set(server->get());
					actor->set(m_id_counter);

					++m_id_counter;

					this->handle_join_success(server, packet, actor);

					return true;
				}
			}
		}

		return false;
	}

	bool PlayerJoinLeaveEvent::handle_player_leave() noexcept
	{
		auto serv_event = EventDispatcher::get_singleton_ptr()->find<NetworkServerEvent>("ServerEvent");

		if (!serv_event)
			return false;

		auto server = InstanceManager::get_singleton_ptr()->find<NetworkServerInstance>("NetworkServerInstance");

		if (!server)
			return false;

		auto packet = serv_event->get();

		if (packet.CMD == Xplicit::NETWORK_CMD_STOP)
		{
			auto current_addr = server->get();
			auto vec_actor = InstanceManager::get_singleton_ptr()->find_all<Xplicit::ActorInstance>("ActorInstance");

			for (size_t i = 0; i < vec_actor.size(); i++)
			{
				auto actor = vec_actor[i];

				if (actor->sockaddr().sin_addr.S_un.S_addr == current_addr.sin_addr.S_un.S_addr)
				{
					actor->reset();
					--m_id_counter;

					return true;
				}
			}
		}

		return false;
	}

	const char* PlayerJoinLeaveEvent::name() noexcept { return ("PlayerJoinLeaveEvent"); }
}