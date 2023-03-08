/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Protocol.h
 *			Purpose: Network Protocol Definitions
 *
 * =====================================================================
 */

#pragma once

#include "Foundation.h"

#ifndef NETWORK_PORT
#define NETWORK_PORT (35596)
#endif // !NETWORK_PORT

namespace Xplicit {
    enum NETWORK_CMD : int 
    {
        NETWORK_CMD_BEGIN = 9, // start network, handshake
        NETWORK_CMD_STOP, // abort connection
        // PvP
        NETWORK_CMD_DEAD,
        NETWORK_CMD_DAMAGE,
        NETWORK_CMD_SPAWN,
        NETWORK_CMD_ERROR,
        // tools
        NETWORK_CMD_TOOL_MOUNT,
        NETWORK_CMD_TOOL_UNMOUNT,
        NETWORK_CMD_TOOL_USE,
        // position
        NETWORK_CMD_POS,
        // script update command.
        NETWORK_CMD_SCRIPT,
        // accept current connection
        NETWORK_CMD_ACCEPT,
        NETWORK_CMD_REFUSE,
        // communication packets
        NETWORK_CMD_MESSAGE,
        NETWORK_CMD_VOICE_CHAT_BEGIN,
        NETWORK_CMD_VOICE_CHAT_END,
        // kick
        NETWORK_CMD_KICK,
        NETWORK_CMD_INVALID = 0xFFFFFFF,
    };

    PACKED_STRUCT
    (
    class GenericPacket
    {
    public:
        NETWORK_CMD CMD; /* The Command. */
        int64_t ID;

    public:
        float X; /* X position */
        float Y; /* Y position */
        float Z; /* Z position */

    }
    );

    PACKED_STRUCT
    (
    class GenericPacketHeader
    {
    public:
        char PlayerName[128];
        char UUID[128];

    }
    );
}