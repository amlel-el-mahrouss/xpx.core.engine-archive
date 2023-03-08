/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Irr.h
 *			Purpose: Irrlicht definitions
 *
 * =====================================================================
 */

#pragma once

#include <irrlicht.h>
#include <uuid/uuid.h>

#include "Foundation.h"

// use these namespaces.
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

namespace Xplicit
{
	class XPLICIT_API InputReceiver : public irr::IEventReceiver
	{
	public:
		virtual bool OnEvent(const SEvent& event)
		{
			if (event.EventType == irr::EET_KEY_INPUT_EVENT)
				m_keys[event.KeyInput.Key] = event.KeyInput.PressedDown;

			return false;
		}

		virtual bool is_key_down(EKEY_CODE keyCode) const
		{
			return m_keys[keyCode];
		}

	public:
		InputReceiver()
		{
			for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
				m_keys[i] = false;
		}

		virtual ~InputReceiver() {}

		InputReceiver& operator=(const InputReceiver&) = default;
		InputReceiver(const InputReceiver&) = default;

	private:
		bool m_keys[KEY_KEY_CODES_COUNT];

	};

	class XPLICIT_API Application final
	{
	private:
		Application()
			: Keyboard(nullptr), Dev(nullptr), Reader(nullptr), Writer(nullptr)
		{}

		~Application() 
		{
			if (Dev)
				Dev->drop();

			if (Reader)
				Reader->drop();

			if (Writer)
				Writer->drop();
		}

		Application& operator=(const Application&) = default;
		Application(const Application&) = default;

	public:
		static Application& get_singleton()
		{
			static Application app;
			return app;
		}

		void set(irr::IrrlichtDevice* dev)
		{
			if (dev)
				Dev = dev;
		}

	public:
		irr::io::IIrrXMLReader<char, IReferenceCounted>* Reader;
		irr::io::IXMLWriter* Writer;
		irr::IrrlichtDevice* Dev;
		InputReceiver* Keyboard;

	};

	class GUID final
	{
	public:
		template <int ver>
		static uuids::uuid version()
		{
			switch (ver)
			{
			case 4:
			{
				std::random_device rd;
				auto seed_data = std::array<int, std::mt19937::state_size> {};
				std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
				std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
				std::mt19937 generator(seq);

				auto gen = uuids::uuid_random_generator(generator);

				return gen();
			}
			}

			return {};
		}

	};
}

#define XML Xplicit::Application::get_singleton().Reader
#define IRR  Xplicit::Application::get_singleton().Dev
#define KB  Xplicit::Application::get_singleton().Keyboard