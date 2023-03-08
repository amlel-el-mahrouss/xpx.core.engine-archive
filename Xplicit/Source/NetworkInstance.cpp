/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: NetworkInstance.cpp
 *			Purpose: UDP Networking API
 *
 * =====================================================================
 */

#include "NetworkInstance.h"
#include "Irr.h"

namespace Xplicit {
	NetworkInstance::NetworkInstance()
		: Instance(), m_packet(), m_inaddr(), m_tmp_inaddr()
	{
		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (m_socket == SOCKET_ERROR)
			throw NetworkError(NETERR_INTERNAL_ERROR);

		u_long ul = 1;
		ioctlsocket(m_socket, FIONBIO, &ul);

#ifndef _NDEBUG
		XPLICIT_INFO("Created NetworkInstance");
#endif

	}

	NetworkInstance::~NetworkInstance()
	{
		// send a stop command.
		GenericPacket cmd = {  };
		cmd.CMD = NETWORK_CMD_STOP; // client is shutting down.

		this->send(cmd);

		// Shutdown and close socket if shutdown failed.
		int res = shutdown(m_socket, SD_BOTH);

		if (res == SOCKET_ERROR)
			closesocket(m_socket);

#ifndef _NDEBUG
		XPLICIT_INFO("~NetworkInstance, Epoch: " + std::to_string(get_epoch()));
#endif
	}

	bool NetworkInstance::connect(const char* ip)
	{
		if (m_socket == SOCKET_ERROR)
			return false;

		memset(&m_inaddr, 0, sizeof(SOCKADDR_IN));

		m_inaddr.sin_family = AF_INET;
		inet_pton(AF_INET, ip, &m_inaddr.sin_addr);
		m_inaddr.sin_port = htons(NETWORK_PORT);

		::bind(m_socket, (SOCKADDR*)&m_inaddr, sizeof(m_inaddr));

		int result = ::connect(m_socket, reinterpret_cast<SOCKADDR*>(&m_inaddr), sizeof(m_inaddr));

		if (result == SOCKET_ERROR)
			throw NetworkError(NETERR_INTERNAL_ERROR);

		return true;
	}

	bool NetworkInstance::send(GenericPacket& cmd)
	{
		m_packet = cmd;

		int res = ::sendto(m_socket, (const char*)&m_packet, sizeof(GenericPacket), 0,
			(SOCKADDR*)&m_inaddr, sizeof(SOCKADDR_IN));

		if (res == SOCKET_ERROR)
			throw NetworkError(NETERR_INTERNAL_ERROR);

		return true;
	}

	void NetworkInstance::update()
	{
		// This is only getting and sending packets, no logic needed here.
	}

	bool NetworkInstance::read(GenericPacket& packet)
	{
		int length{ sizeof(struct sockaddr_in) };

		// return false if length is zero (no packet)
		if (recvfrom(m_socket, (char*)&packet, sizeof(GenericPacket), MSG_PEEK,
			(SOCKADDR*)&m_tmp_inaddr, &length) == 0) return false;

		// fail here if the recvfrom is not from the host.
		if (m_tmp_inaddr.sin_addr.S_un.S_addr != m_inaddr.sin_addr.S_un.S_addr) 
			return false;

		return true;
	}

	void NetworkEvent::operator()()
	{
		auto net = InstanceManager::get_singleton_ptr()->find<NetworkInstance>("NetworkInstance");

		if (net)
		{
			GenericPacket packet = net->get_packet();

			if (packet.CMD == NETWORK_CMD_KICK || packet.CMD == NETWORK_CMD_STOP)
			{
				XPLICIT_INFO(packet.CMD == NETWORK_CMD_KICK ? "[CLIENT] Server kicked us!" :
							 "[CLIENT] Server shutdown!");

				InstanceManager::get_singleton_ptr()->remove<NetworkInstance>("NetworkInstance");
			}
		}
	}
}