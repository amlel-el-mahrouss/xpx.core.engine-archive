/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: MonoInstance.h
 *			Purpose: C# Foundations
 *
 * =====================================================================
 */

#pragma once

#include "Instance.h"
#include "Foundation.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace Xplicit
{
	class MonoScriptInstance;
	class MonoEngineInstance;

	class XPLICIT_API MonoEngineInstance : public Instance
	{
	public:
		MonoEngineInstance();
		virtual ~MonoEngineInstance();

		MonoEngineInstance& operator=(const MonoEngineInstance&) = default;
		MonoEngineInstance(const MonoEngineInstance&) = default;

		MonoAssembly* open(const char* assembly_file);

		MonoObject* run(MonoAssembly* in, const char* env);
		int run(MonoAssembly* in, int argc, const char** argv);

		virtual const char* name() noexcept override;
		virtual INSTANCE_TYPE type() noexcept override;
		virtual void update() override;

		MonoClass* make(Ref<MonoScriptInstance*>& assembly, const char* namespase, const char* klass);
		MonoDomain* domain() noexcept;

	private:
		MonoDomain* m_app_domain;
		MonoDomain* m_domain;

	};

	class XPLICIT_API MonoScriptInstance : public Instance
	{
	public:
		MonoScriptInstance(const char* filename);
		virtual ~MonoScriptInstance();

		MonoScriptInstance& operator=(const MonoScriptInstance&) = default;
		MonoScriptInstance(const MonoScriptInstance&) = default;

		MonoObject* run(const char* method_name);
		virtual void update() override;

		virtual const char* name() noexcept override;
		virtual INSTANCE_TYPE type() noexcept override;

		Ref<MonoEngineInstance*>& get();

	private:
		Ref<MonoEngineInstance*> m_engine_ref;
		std::basic_string<char> m_filename;
		MonoAssembly* m_assembly;

		friend MonoEngineInstance;

	};

	XPLICIT_API void xplicit_register_class(const char* namespase, const char* klass);
}
