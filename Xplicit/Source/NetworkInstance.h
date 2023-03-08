/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: NetworkInstance.h
 *			Purpose: UDP Networking API
 *
 * =====================================================================
 */

#pragma once

#include "Event.h"
#include "Protocol.h"
#include "Instance.h"
#include "Foundation.h"

#define XPLICIT_NET_ASSERT(EXPR) if (!(EXPR)) { MessageBoxA(nullptr, #EXPR, "C++ Network Exception", 0); std::terminate(); }

#ifndef XPLICIT_SLEEP
#define XPLICIT_SLEEP Sleep
#endif

namespace Xplicit {
    enum NETWORK_ERR : int
    {
        NETERR_BAD_CHALLENGE,
        NETERR_PING_TOO_HIGH,
        NETERR_INTERNAL_ERROR,
        NETERR_BAN,
        NETERR_KICK,
        NETERR_COUNT,
    };

    class XPLICIT_API NetworkError final : public std::runtime_error 
    {
    public:
        NetworkError(const int what = NETERR_INTERNAL_ERROR) : std::runtime_error("Network Error") {}
        virtual ~NetworkError() = default; // let the ABI define that.

        NetworkError& operator=(const NetworkError&) = default;
        NetworkError(const NetworkError&) = default;

    public:
        int error() const noexcept { return m_iErr; }

    private:
        int m_iErr{ 200 };

    };

    class XPLICIT_API NetworkInstance : public Instance
    {
    public:
        NetworkInstance();

        virtual ~NetworkInstance();

        NetworkInstance& operator=(const NetworkInstance& ctx) = default;
        NetworkInstance(const NetworkInstance& ctx) = default;

        virtual const char* name() noexcept override { return ("NetworkInstance"); }
        virtual INSTANCE_TYPE type() noexcept override { return NETWORK; }

        virtual void update() override;

        bool connect(const char* ip);
        bool send(GenericPacket& cmd);
        bool read(GenericPacket& packet);

        GenericPacket get_packet() noexcept { return m_packet; }

    private:
        SOCKET m_socket;

    private:
        GenericPacket m_packet;

    private:
        struct sockaddr_in m_tmp_inaddr; // temp var.
        struct sockaddr_in m_inaddr; // socket descriptor.

        friend class NetworkEvent;

    };

    class XPLICIT_API NetworkEvent : public Event
    {
    public:
        NetworkEvent() {}
        virtual ~NetworkEvent() {}

        NetworkEvent& operator=(const NetworkEvent&) = default;
        NetworkEvent(const NetworkEvent&) = default;

        virtual void operator()();

        virtual const char* name() noexcept { return ("NetworkEvent"); }

    };
}