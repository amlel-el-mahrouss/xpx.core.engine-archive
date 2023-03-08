/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: LocalPLayerInstance.cpp
 *			Purpose: Client-side Player Instance
 *
 * =====================================================================
 */

#include "LocalPlayerInstance.h"

namespace Xplicit
{
	const char* LocalPlayerMoveEvent::name() noexcept { return ("LocalPlayerMoveEvent"); }

	void LocalPlayerMoveEvent::operator()()
	{
		if (!InstanceManager::get_singleton_ptr())
			return;

		auto net = InstanceManager::get_singleton_ptr()->find<NetworkInstance>("NetworkInstance");

		if (!net)
			return;

		if (m_network_delay > 0)
		{
			--m_network_delay;
			return;
		}

		auto cam = InstanceManager::get_singleton_ptr()->find<CameraInstance>("CameraInstance");

		if (cam->get()->getPosition() == m_saved_position)
			return;

		auto pos = cam->get()->getPosition();

		GenericPacket packet{ };

		packet.CMD = NETWORK_CMD_POS;

		packet.X = pos.X;
		packet.Y = pos.Y;
		packet.Z = pos.Z;

		net->send(packet);

		m_saved_position = pos;
		m_network_delay = XPLICIT_NETWORK_DELAY;
	}
}