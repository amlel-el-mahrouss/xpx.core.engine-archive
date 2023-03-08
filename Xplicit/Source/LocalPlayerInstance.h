/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: LocalPLayerInstance.h
 *			Purpose: Client-side Player Instance
 *
 * =====================================================================
 */

#pragma once

#include "Irr.h"
#include "Instance.h"
#include "CameraInstance.h"
#include "NetworkInstance.h"


namespace Xplicit
{
	constexpr const int XPLICIT_NETWORK_DELAY = 1000;
	constexpr const int XPLICIT_SPEED = 20.f;

	class XPLICIT_API LocalPlayerInstance : public Instance
	{
	public:
		LocalPlayerInstance()
			: Instance(), m_then(0), m_cam(nullptr)
		{
			m_cam = InstanceManager::get_singleton_ptr()->find<CameraInstance>("CameraInstance");
			m_then = IRR->getTimer()->getTime();

#ifdef _NDEBUG
			XPLICIT_INFO("LocalPlayerInstance::LocalPlayerInstance");
#endif
		}

		virtual ~LocalPlayerInstance()
		{
#ifdef _NDEBUG
			XPLICIT_INFO("LocalPlayerInstance::~LocalPlayerInstance");
#endif
		}

		LocalPlayerInstance& operator=(const LocalPlayerInstance&) = default;
		LocalPlayerInstance(const LocalPlayerInstance&) = default;

		virtual INSTANCE_TYPE type() noexcept override { return CAMERA; }
		virtual const char* name() noexcept override { return ("LocalPlayerInstance"); }

		virtual void update() override
		{
			if (!m_cam)
				m_cam = InstanceManager::get_singleton_ptr()->find<CameraInstance>("CameraInstance");

			const u32 now = IRR->getTimer()->getTime();
			const f32 delta = (f32)(now - m_then) / 1000.f; // Time in seconds

			m_then = now;

			if (KB->is_key_down(m_key_code_forward))
			{
				if (m_cam)
				{
					auto pos = m_cam->get()->getPosition();
					pos.Z += XPLICIT_SPEED * delta;

					m_cam->get()->setPosition(pos);
				}
			}
			else if (KB->is_key_down(m_key_code_backwards))
			{
				if (m_cam)
				{
					auto pos = m_cam->get()->getPosition();
					pos.Z -= XPLICIT_SPEED * delta;

					m_cam->get()->setPosition(pos);
				}
			}
			else if (KB->is_key_down(m_key_code_right))
			{
				if (m_cam)
				{
					auto pos = m_cam->get()->getPosition();
					pos.X += XPLICIT_SPEED * delta;

					m_cam->get()->setPosition(pos);
				}
			}
			else if (KB->is_key_down(m_key_code_left))
			{
				if (m_cam)
				{
					auto pos = m_cam->get()->getPosition();
					pos.X -= XPLICIT_SPEED * delta;

					m_cam->get()->setPosition(pos);
				}
			}
		}

	private:
		irr::EKEY_CODE m_key_code_backwards = irr::KEY_KEY_S;
		irr::EKEY_CODE m_key_code_forward = irr::KEY_KEY_W;
		irr::EKEY_CODE m_key_code_right = irr::KEY_KEY_D;
		irr::EKEY_CODE m_key_code_left = irr::KEY_KEY_A;

	private:
		CameraInstance* m_cam;
		u32 m_then;

	};

	class XPLICIT_API LocalPlayerMoveEvent : public Event
	{
	public:
		LocalPlayerMoveEvent() : m_network_delay(0) {}
		virtual ~LocalPlayerMoveEvent() {}

		LocalPlayerMoveEvent& operator=(const LocalPlayerMoveEvent&) = default;
		LocalPlayerMoveEvent(const LocalPlayerMoveEvent&) = default;

		const char* name() noexcept;
		virtual void operator()() override;

	private:
		irr::core::vector3df m_saved_position;
		int64_t m_network_delay;

	};
}
