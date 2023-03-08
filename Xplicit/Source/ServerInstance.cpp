/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: ServerInstance.cpp
 *			Purpose: UDP Networking API
 *
 * =====================================================================
 */

#include "ServerInstance.h"
#include "Actor.h"

namespace Xplicit
{
	NetworkServerInstance::NetworkServerInstance(const char* ip)
		: m_socket(INVALID_SOCKET), m_ip(ip), m_send(false), m_client(), m_server()
	{
#ifndef _NDEBUG
		std::string message;
		message += "Class NetworkServerInstance, Epoch: ";
		message += std::to_string(get_epoch());

		XPLICIT_INFO(message);
#endif

		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (m_socket == SOCKET_ERROR)
			throw NetworkError(NETERR_INTERNAL_ERROR);

		memset(&m_server, 0, sizeof(struct sockaddr_in));

		m_server.sin_family = AF_INET;
		inet_pton(AF_INET, ip, &m_server.sin_addr.S_un.S_addr);
		m_server.sin_port = htons(NETWORK_PORT);

		auto ret_bind = bind(m_socket, reinterpret_cast<SOCKADDR*>(&m_server), sizeof(m_server));

		if (ret_bind == SOCKET_ERROR)
			throw NetworkError(NETERR_INTERNAL_ERROR);

	}


	const char* NetworkServerInstance::name() noexcept { return ("NetworkServerInstance"); }
	NetworkServerInstance::INSTANCE_TYPE NetworkServerInstance::type() noexcept { return NETWORK; }
	void NetworkServerInstance::update() {}

	NetworkServerInstance::~NetworkServerInstance()
	{
#ifndef _NDEBUG
		std::string message;
		message += "Class NetworkServerInstance (delete), Epoch: ";
		message += std::to_string(get_epoch());

		XPLICIT_INFO(message);
#endif

		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);

		WSACleanup();
	}

	void NetworkServerInstance::queue(GenericPacket& packet) noexcept
	{
		if (m_send)
			return;

		m_packet.push_back(packet);
	}

	void NetworkServerInstance::target(struct sockaddr_in& inaddr)
	{
		m_client = inaddr;
	}

	void NetworkServerInstance::send() noexcept { m_send = true; }

	struct sockaddr_in NetworkServerInstance::get() { return m_client; }
	const char* NetworkServerInstance::ip() noexcept { return m_ip.c_str(); }

	// we need a way to tell which client is who.
	void NetworkServerTraits::recv(NetworkServerEvent* env, NetworkServerInstance* instance)
	{
		if (instance)
		{
			int len{ sizeof(struct sockaddr_in) };

			int res = recvfrom(instance->m_socket, (char*)&env->m_tmp_packet, 
				sizeof(GenericPacket), 0,
				(struct sockaddr*)&instance->m_client, &len);

			if (len > 0)
			{
				if (res == SOCKET_ERROR)
					return;
			}

			// check if something is queued.
			// if yes, sends what we have.

			if (instance->m_send)
			{
				for (size_t i = 0; i < instance->m_packet.size(); i++)
				{
					::sendto(instance->m_socket, (const char*)&
						instance->m_packet[i], sizeof(GenericPacket), 0, (struct sockaddr*)&instance->m_client, sizeof(struct sockaddr_in));

				}
				
				instance->m_packet.clear();
				instance->m_send = false;
			}
		}
	}

	NetworkServerEvent::NetworkServerEvent(NetworkServerInstance* instance)
		: m_instance(instance), m_tmp_packet()
	{
		if (!instance)
			throw NetworkError(NETERR_INTERNAL_ERROR);

#ifndef _NDEBUG
		std::string message;
		message += "Class NetworkServerInstance, Epoch: ";
		message += std::to_string(get_epoch());

		XPLICIT_INFO(message);
#endif
	}

	NetworkServerEvent::~NetworkServerEvent() {}

	void NetworkServerEvent::operator()()
	{
		NetworkServerTraits::recv(this, m_instance);
	}

	GenericPacket& NetworkServerEvent::get() { return m_tmp_packet; }
	const char* NetworkServerEvent::name() noexcept { return ("ServerEvent"); }
}