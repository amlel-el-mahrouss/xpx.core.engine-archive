/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Server.cpp
 *			Purpose: Server Main Procedure
 *
 * =====================================================================
 */

#include "PlayerJoinLeaveEvent.h"

static void xplicit_update_position(Xplicit::NetworkServerEvent* env, Xplicit::NetworkServerInstance* server);
static void xplicit_attach_mono();
static void xplicit_load_cfg();

static auto g_instance_manager = Xplicit::InstanceManager::get_singleton_ptr();
static auto g_event_dispatcher = Xplicit::EventDispatcher::get_singleton_ptr();

static int64_t g_frametime = 0;

static void xplicit_update_position(Xplicit::NetworkServerEvent* env, Xplicit::NetworkServerInstance* server)
{
	if (!env) return;
	if (!server) return;

	auto vec_actor = g_instance_manager->find_all<Xplicit::ActorInstance>("ActorInstance");
	Xplicit::GenericPacket& cur_packet = env->get();

	auto sockaddr = server->get();

	for (size_t i = 0; i < vec_actor.size(); i++)
	{
		if (sockaddr.sin_addr.S_un.S_addr == vec_actor[i]->sockaddr().sin_addr.S_un.S_addr)
		{
			if (cur_packet.CMD == Xplicit::NETWORK_CMD_POS)
			{
				vec_actor[i]->set(cur_packet.X, cur_packet.Y, cur_packet.Z);
			}
		}
	}
}

static void xplicit_load_cfg()
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
	
	auto mono = g_instance_manager->find<Xplicit::MonoEngineInstance>("MonoEngineInstance");

	const char* argv[] = {"XplicitNgin"};

	// read until nothing cant be read.
	while (XML->read())
	{
		switch (XML->getNodeType())
		{
			//we found a new element
		case irr::io::EXN_ELEMENT:
		{
			if (std::string(XML->getNodeName()) == "PLUGIN")
			{
				try
				{
					auto dllPath = XML->getAttributeValue("VFS");
					if (!dllPath)
						throw std::runtime_error("Bad DLL path!");

					std::string str_dllPath = data_dir;

					str_dllPath += "\\Lib\\";
					str_dllPath += dllPath;

					auto assembly_file = mono->open(str_dllPath.c_str());

					if (assembly_file)
					{
						XPLICIT_INFO("Loaded DLL: " + str_dllPath);
						mono->run(assembly_file, 1, argv);
					}
				}
				catch (...)
				{
					XPLICIT_INFO("Bad DLL, skiping..");
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
					XPLICIT_INFO(std::string(err.what()).append(": Bad Scene!, exiting.."));
					std::exit(3);
				}
			}

			break;
		}
		}
	}
}

// Load the C# DLL 'Xplicit.dll' to let other programs run.
// This must not fail!

static void xplicit_attach_mono()
{
	auto mono = g_instance_manager->add<Xplicit::MonoEngineInstance>();

	char data_dir[4096];
	memset(data_dir, 0, 4096);

	GetEnvironmentVariableA("XPLICIT_DATA_DIR", data_dir, 4096);

	std::string path = data_dir;
	path += "\\Lib\\Xplicit.dll";

	// we need a script instance, in order to create classes.
	// a script instance is literally a context for our plugin engine.

	g_instance_manager->add<Xplicit::MonoScriptInstance>(path.c_str());
	g_event_dispatcher->add<Xplicit::MonoUpdateEvent>();
}

static void xplicit_create_common()
{
	g_event_dispatcher->add<Xplicit::PlayerJoinLeaveEvent>();
	g_event_dispatcher->add<Xplicit::PhysicsActorEvent>();
	g_event_dispatcher->add<Xplicit::NetworkActorEvent>();
	g_event_dispatcher->add<Xplicit::PlayerActorEvent>();
}

static void xplicit_on_close()
{
	auto server = g_instance_manager->find<Xplicit::NetworkServerInstance>("NetworkServerInstance");
	if (!server)
		return;

	Xplicit::GenericPacket packet;
	packet.CMD = Xplicit::NETWORK_CMD_STOP;

	server->queue(packet);
	server->send();

	g_instance_manager->update();
	g_event_dispatcher->update();

}

int main(int argc, char** argv)
{
	try
	{
		atexit(xplicit_on_close);

		WSADATA wsa;
		Xplicit::init_winsock(&wsa);

		Xplicit::Application::get_singleton().set(irr::createDevice(irr::video::EDT_NULL));

		// the address is located in the XPLICIT_SERVER_ADDR variable.
		char* addr = getenv("XPLICIT_SERVER_ADDR");

		if (!addr) return 1;

		auto server = g_instance_manager->add<Xplicit::NetworkServerInstance>(addr);
		auto env = g_event_dispatcher->add<Xplicit::NetworkServerEvent>(server);

		xplicit_attach_mono();
		xplicit_load_cfg();
		xplicit_create_common();

		while (g_instance_manager && g_event_dispatcher)
		{
			xplicit_update_position(env, server);

			g_instance_manager->update();
			g_event_dispatcher->update();
		}

		return 0;
	}
	catch (std::runtime_error err)
	{
#ifndef _NDEBUG
		std::string msg;
		msg += "C++ Runtime Error: ";
		msg += err.what();

		XPLICIT_CIRITICAL(msg);
#endif

		return -1;
	}
	catch (Xplicit::NetworkError err)
	{
#ifndef _NDEBUG
		std::string msg;
		msg += "A network Error occured, application will now quit..";

		XPLICIT_CIRITICAL(msg);
#endif

		return -1;
	}
}