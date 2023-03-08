/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: CameraInstance.h
 *			Purpose: Camera Controller
 *
 * =====================================================================
 */

#pragma once

#include "Irr.h"
#include "Instance.h"
#include "NetworkInstance.h"

namespace Xplicit
{
	class XPLICIT_API CameraInstance : public Instance
	{
	public:
		CameraInstance() 
			: Instance()
		{
			m_camera = IRR->getSceneManager()->addCameraSceneNodeFPS(nullptr, 60.F, 0.2F);
			IRR->getCursorControl()->setVisible(false);

			m_camera->setName("CameraInstance");

			const char* data_dir = getenv("XPLICIT_DATA_DIR");
			
			if (!data_dir)
				throw std::runtime_error("Bad EnvPath");

			std::string idle_path = data_dir;
			idle_path += "/idle.png";

			m_idle_tex = IRR->getVideoDriver()->getTexture(idle_path.c_str());

			std::string moving_path = data_dir;
			moving_path += "/moving.png";

			m_moving_tex = IRR->getVideoDriver()->getTexture(moving_path.c_str());

			std::string typing_path = data_dir;
			typing_path += "/typing.png";

			m_typing_tex = IRR->getVideoDriver()->getTexture(typing_path.c_str());

		}

		virtual ~CameraInstance() 
		{
			// m_camera will be cleaned out by the OS. as we remove it when we exit the program.
			// except if some dumb hackers tries to do dumb shit.
		}

		CameraInstance& operator=(const CameraInstance&) = default;
		CameraInstance(const CameraInstance&) = default;

		virtual INSTANCE_TYPE type() noexcept override { return CAMERA; }
		virtual const char* name() noexcept override { return ("CameraInstance"); }

		virtual void update() override 
		{
			if (KB->is_key_down(irr::KEY_KEY_W))
			{
				IRR->getVideoDriver()->draw2DImage(m_moving_tex, IRR->getCursorControl()->getPosition(),
					core::rect<s32>(0, 0, 38, 38), 0,
					video::SColor(255, 255, 255, 255), true);
			}
			else if (KB->is_key_down(irr::KEY_KEY_T))
			{
				IRR->getVideoDriver()->draw2DImage(m_typing_tex, IRR->getCursorControl()->getPosition(),
					core::rect<s32>(0, 0, 38, 38), 0,
					video::SColor(255, 255, 255, 255), true);
			}
			else
			{
				IRR->getVideoDriver()->draw2DImage(m_idle_tex, IRR->getCursorControl()->getPosition(),
					core::rect<s32>(0, 0, 38, 38), 0,
					video::SColor(255, 255, 255, 255), true);
			}

			if (KB->is_key_down(irr::KEY_ESCAPE))
			{
				IRR->closeDevice();
			}
		}

		irr::scene::ICameraSceneNode* get() { return m_camera; }

	private:
		irr::scene::ICameraSceneNode* m_camera;

	private:
		irr::video::ITexture* m_moving_tex;
		irr::video::ITexture* m_typing_tex;
		irr::video::ITexture* m_idle_tex;

	};
}
