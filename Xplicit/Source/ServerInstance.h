/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: ServerInstance.h
 *			Purpose: UDP Networking API
 *
 * =====================================================================
 */

#pragma once

#include "NetworkInstance.h"
#include "Foundation.h"
#include "Event.h"

namespace Xplicit
{
	class NetworkServerEvent;

	constexpr const int MAX_CONNECTIONS = 30;

	// the main instance behind the networking.
	class XPLICIT_API NetworkServerInstance final : public Instance
	{
	public:
		explicit NetworkServerInstance(const char* ip);
		virtual ~NetworkServerInstance();

		NetworkServerInstance& operator=(const NetworkServerInstance&) = default;
		NetworkServerInstance(const NetworkServerInstance&) = default;

		virtual const char* name() noexcept;
		virtual INSTANCE_TYPE type() noexcept;
		virtual void update() override;

		// queue a packet.
		void queue(GenericPacket& packet) noexcept;
		void target(struct sockaddr_in& in);

		// mark packets as 'to send'
		void send() noexcept;

		struct sockaddr_in get();
		const char* ip() noexcept;

	private:
		std::vector<GenericPacket> m_packet;
		bool m_send;

	private:
		std::string m_ip;
		SOCKET m_socket;

	private:
		struct sockaddr_in m_server{ };
		struct sockaddr_in m_client{ };

		friend class NetworkServerTraits;

	};

	class XPLICIT_API NetworkServerTraits
	{
	public:
		static void recv(NetworkServerEvent* env, NetworkServerInstance* instance);

	};

	class XPLICIT_API NetworkServerEvent : public Event
	{
	public:
		NetworkServerEvent(NetworkServerInstance* instance);
		virtual ~NetworkServerEvent();

		NetworkServerEvent& operator=(const NetworkServerEvent&) = default;
		NetworkServerEvent(const NetworkServerEvent&) = default;

		virtual void operator()() override;

		GenericPacket& get();
		virtual const char* name() noexcept;

	private:
		NetworkServerInstance* m_instance;
		GenericPacket m_tmp_packet;

		friend NetworkServerTraits;

	};
}