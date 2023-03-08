/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Client.cpp
 *			Purpose: Client sources
 *
 * =====================================================================
 */

#include "Client.h"

static auto g_instance_manager = Xplicit::InstanceManager::get_singleton_ptr();
static auto g_event_dispatcher = Xplicit::EventDispatcher::get_singleton_ptr();

static void xplicit_init_instances(Xplicit::NetworkInstance* net)
{
	Xplicit::GenericPacket packet_spawn{};
	packet_spawn.CMD = Xplicit::NETWORK_CMD_BEGIN;

	net->send(packet_spawn);

	g_instance_manager->add<Xplicit::CameraInstance>();
	g_instance_manager->add<Xplicit::LocalPlayerInstance>();
	g_event_dispatcher->add<Xplicit::LocalPlayerMoveEvent>();
}

static void xplicit_init(Xplicit::NetworkInstance* net)
{
	char data_dir[4096];
	memset(data_dir, 0, 4096);

	GetEnvironmentVariableA("XPLICIT_DATA_DIR", data_dir, 4096);

	if (*data_dir == 0)
		throw std::runtime_error("getenv: no such variable!");

	std::string path = data_dir;
	path += "\\MANIFEST.xml";

	XML = IRR->getFileSystem()->createXMLReaderUTF8(path.c_str());

	if (!XML)
		throw std::runtime_error("No XML provided..");

	// read until nothing cant be read.
	while (XML->read())
	{
		switch (XML->getNodeType())
		{
			//we found a new element
		case irr::io::EXN_ELEMENT:
		{
			if (std::string(XML->getNodeName()) == "DNS")
			{
				if (net->connect(XML->getAttributeValue("IP")))
				{
					xplicit_init_instances(net);
				}
			}
			else if (std::string(XML->getNodeName()) == "SCENE")
			{
				try
				{
					std::string scene_path = data_dir;
					scene_path += "\\";
					scene_path += XML->getAttributeValue("VFS");

					if (!IRR->getSceneManager()->loadScene(scene_path.c_str()))
						throw std::runtime_error("SceneException");
				}
				catch (std::runtime_error& err)
				{
#ifndef _NDEBUG
					XPLICIT_INFO(std::string(err.what()).append(": Bad Scene!, exiting.."));
#endif

					std::exit(3);
				}
			}

			break;
		}
		}
	}
}

static void xplicit_fini()
{
	if (!g_instance_manager)
		return;

	g_instance_manager->remove<Xplicit::NetworkInstance>("NetworkInstance");
	g_instance_manager->remove<Xplicit::CameraInstance>("CameraInstance");
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	atexit(xplicit_fini);

	try
	{
		WSADATA wsa;

		Xplicit::init_winsock(&wsa);

#ifndef _NDEBUG
		Xplicit::open_terminal();
#endif

		KB = new Xplicit::InputReceiver();

		if (!KB)
			throw std::bad_alloc();

		dimension2du dim2d = dimension2du(800, 600);

		Xplicit::Application::get_singleton().set(irr::createDevice(irr::video::EDT_DIRECT3D9,
			dim2d,
			16U,
			false,
			false,
			false,
			KB));

		auto net = g_instance_manager->add<Xplicit::NetworkInstance>();
		g_event_dispatcher->add<Xplicit::NetworkEvent>();

		xplicit_init(net);

		IRR->setWindowCaption(L"Xplicit Client");

		while (IRR->run() && g_instance_manager && g_event_dispatcher)
		{
			IRR->getVideoDriver()->beginScene(true, true, irr::video::SColor(0, 38, 100, 100));

			IRR->getSceneManager()->drawAll();
			IRR->getGUIEnvironment()->drawAll();

			g_event_dispatcher->update();
			g_instance_manager->update();

			IRR->getVideoDriver()->endScene();
		}

		return 0;
	}
	catch (...)
	{
		Xplicit::GUI::message_box(L"Uh oh!", L"Something did go wrong, we're sorry!", MB_OK);

		return -1;
	}
}
