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

#pragma once

#include "SDK.h"

namespace Xplicit
{
	class XPLICIT_API PlayerJoinLeaveEvent : public Event
	{
	public:
		PlayerJoinLeaveEvent() : m_id_counter(0) {}
		virtual ~PlayerJoinLeaveEvent() {}

		PlayerJoinLeaveEvent& operator=(const PlayerJoinLeaveEvent&) = default;
		PlayerJoinLeaveEvent(const PlayerJoinLeaveEvent&) = default;

		virtual void operator()() override
		{
			this->handle_player_leave();
			this->handle_new_player();
		}

	private:
		bool handle_join_success(NetworkServerInstance* server, GenericPacket& packet, ActorInstance* actor) noexcept;
		bool handle_new_player() noexcept;
		bool handle_player_leave() noexcept;

		virtual const char* name() noexcept override;

	private:
		int64_t m_id_counter;

	};
}