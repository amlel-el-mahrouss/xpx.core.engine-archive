/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Foundation.h
 *			Purpose: Xplicit Foundation Header
 *
 * =====================================================================
 */

#pragma once

// C standard headers
#include <ctime>
#include <cstdio>
#include <clocale>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdint>

// network sockets.
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

// common control dll
#include <CommCtrl.h>
#pragma comment(lib,"comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// C++ standard headers
#include <tuple>
#include <array>
#include <thread>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <concepts>
#include <stdexcept>
#include <algorithm>
#include <filesystem>

// spdlog
#include <spdlog/spdlog.h>

// windows headers
#include <windows.h>
#include <tlhelp32.h>
#include <shellapi.h>

#ifdef __XPLICIT_AS_SDK__
#ifdef __EXPORT_XPLICIT__
#define XPLICIT_API __declspec(dllexport)
#else
#define XPLICIT_API __declspec(dllimport)
#endif
#else
#define XPLICIT_API
#endif

#ifdef END_OF_BUFFER
#undef END_OF_BUFFER
#endif // !END_OF_BUFFER

#define END_OF_BUFFER '\0'

// some spdlog helpers
#define XPLICIT_CIRITICAL(XMSG) spdlog::critical(XMSG)
#define XPLICIT_ERROR(XMSG) spdlog::error(XMSG)
#define XPLICIT_INFO(XMSG) spdlog::info(XMSG)

XPLICIT_API size_t fstrlen(const char* deflated);
XPLICIT_API time_t get_epoch(void);
XPLICIT_API FILE* get_logger(void);
XPLICIT_API void log(const char* msg);
XPLICIT_API bool open_logger(void);

#ifdef _MSC_VER
#define PACKED_STRUCT( decl ) __pragma( pack(push, 1) ) decl; __pragma( pack(pop))
#endif

namespace Xplicit 
{
	class Win32Error : public std::runtime_error 
	{
	public:
		Win32Error(const std::string& what) : std::runtime_error(what) { m_hr = GetLastError(); }
		~Win32Error() = default; // let the ABI define that.

		Win32Error& operator=(const Win32Error&) = default;
		Win32Error(const Win32Error&) = default;

		HRESULT hr() { return m_hr; }

	private:
		HRESULT m_hr;

	};

	template <typename T>
	class ModuleType;
	class Win32Helpers;
	class ModuleManagerWin32;

	template <typename T>
	class ModuleType final 
	{
		ModuleType() = default;
		friend ModuleManagerWin32;

	public:
		ModuleType(T t) : m_ptr(t) {}

		T get() { return m_ptr; }
		T operator->() { return m_ptr; }

	private:
		T m_ptr;

	};

	class Win32Helpers final 
	{
	public:
		static HWND find_wnd(const char* compare) 
		{
			if (!compare) return nullptr;

			HWND hCurWnd = nullptr;
			char string[256];

			do
			{
				ZeroMemory(&string, 255);

				hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
				GetWindowTextA(hCurWnd, string, 256);

				if (string == compare || !strcmp(compare, string))
					return hCurWnd;
			} while (hCurWnd != nullptr);

			return nullptr;
		}

	};

	class ModuleManagerWin32 final 
	{
	public:
		ModuleManagerWin32() = delete;

		explicit ModuleManagerWin32(const std::string& module_name) 
		{ 
			assert(!module_name.empty());
			hMod = LoadLibraryExA(module_name.c_str(), nullptr, 0);

			if (!hMod) 
				throw Win32Error("LoadLibraryExA failed!");
		}

		~ModuleManagerWin32() 
		{
			if (hMod)
				FreeLibrary(hMod);
		}

		ModuleManagerWin32& operator=(const ModuleManagerWin32&) = default;
		ModuleManagerWin32(const ModuleManagerWin32&) = default;

		template <typename T>
		ModuleType<T> get(const std::string& ordinal)
		{
			if (!hMod) return {};
			if (ordinal.empty()) return {};

			T the{ reinterpret_cast<T>(GetProcAddress(hMod, ordinal.c_str())) };

			return the;
		}

	private:
		HMODULE hMod{};

	};

	using ModuleManager = ModuleManagerWin32;

	template<typename T>
	concept IsPointer = (std::is_pointer<T>::value);

	template<typename T>
	concept NotNull = (!std::is_same<T, std::nullptr_t>::value);

	template<typename T>
	concept IsPointerAndNotNull = (IsPointer<T> && NotNull<T>);

	template <typename T>
	class Ref 
	{
	public:
		Ref() : m_ptr(nullptr) {}

		Ref(IsPointerAndNotNull auto ptr) 
			: m_ptr(ptr) 
		{
			m_ref_cnt = new int32_t(0);
			assert(m_ref_cnt);

			this->_inc_ref();
		}

		~Ref()
		{
			if (m_ref_cnt && *m_ref_cnt == 0)
			{
				if (m_ptr)
				{
					delete m_ptr;
				}
			}
		}

		int32_t count() { return *m_ref_cnt; }

		T get() { return m_ptr; }
		T operator->() { return m_ptr; }

	public:
		Ref& operator=(const Ref& ref)
		{
			m_ptr = ref.m_ptr;
			this->m_ref_cnt = ref.m_ref_cnt;

			return *this;
		}

		Ref(const Ref& ref)
		{
			m_ptr = ref.m_ptr;
			this->m_ref_cnt = ref.m_ref_cnt;
		}

		operator bool() { return m_ptr; }

		void _inc_ref() 
		{
			if (m_ref_cnt)
				++*(m_ref_cnt); 
		}

	private:
		T m_ptr;
		int32_t* m_ref_cnt{ nullptr };

	};

	template <typename T>
	static inline Ref<T> make_from(Ref<T> cls)
	{
		Ref<T> ref{ cls };
		ref._inc_ref();

		return ref;
	}

	template <typename T>
	static inline Ref<T> make_from(T cls)
	{
		Ref<T> ref{ cls };

		return ref;
	}

	using PrivShellData = HWND;

	constexpr const char* SHELL_MANAGER_EXEC_OPEN = "open";

	class ShellManager 
	{
	public:
		static Ref<HINSTANCE> open(std::string& appname, PrivShellData priv) 
		{
			if (appname.empty()) 
				return {};

			Ref<HINSTANCE> instance = ShellExecuteA(priv, SHELL_MANAGER_EXEC_OPEN, appname.c_str(), nullptr, nullptr, SW_SHOW);
			if (!instance) throw Win32Error("HINSTANCE has a problem, check hr()");

			return instance;
		}

		static Ref<HINSTANCE> open(const char* appname, PrivShellData priv) {
			if (!appname) 
				return {};

			Ref<HINSTANCE> instance = ShellExecuteA(priv, SHELL_MANAGER_EXEC_OPEN, appname, nullptr, nullptr, SW_SHOW);
			if (!instance) throw Win32Error("HINSTANCE has a problem, check hr()");

			return instance;
		}

	};

	template <typename Fn, typename... Args>
	class AsyncAction final 
	{
	public:
		using FnT = Fn;

	public:
		AsyncAction() = delete;

		AsyncAction(FnT fn, Args&&... args) 
		{
			if (!fn)
				throw std::bad_alloc();

			m_pThread = std::thread(fn, args...);
		}

		~AsyncAction() {}

		void join() 
		{
			m_pThread.join();
		}

	public:
		AsyncAction& operator=(const AsyncAction&) = default;
		AsyncAction(const AsyncAction&) = default;

	private:
		std::thread m_pThread;

	};

	static inline std::string get_at_current_path(const std::string& program_name) 
	{
		auto path = std::filesystem::current_path();
		path /= program_name;

		try {
			std::ifstream io(path);

			// if it aint exist, throw and error
			if (!io.is_open())
				throw std::bad_alloc();

		}
		catch (...) {
			return "";
		}

		return path.string() + program_name;
	}

	static inline bool init_winsock(WSADATA* dat) 
	{
		int result = WSAStartup(MAKEWORD(2, 2), dat);
		if (result != 0) {
			return false;
		}

		return true;
	}

	template <typename T, typename D>
	using OwnPtr = std::unique_ptr<T, D>;

	// opens a new terminal
	// with conout as stdout, so we can write to it.

	static inline void open_terminal(FILE* fp = stdout)
	{
		AllocConsole();
		(void)freopen("CONOUT$", "w", fp);
	}

	class GUI
	{
	public:
		static int32_t message_box(LPCWSTR title, LPCWSTR message, int flags)
		{
			if (!title)
				return -1;

			if (!message)
				return -1;

			return MessageBoxExW(nullptr, message, title, flags, LANG_ENGLISH);
		}

		static int32_t message_box(LPCWSTR title, LPCWSTR header, LPCWSTR message, PCWSTR icon, _TASKDIALOG_COMMON_BUTTON_FLAGS buttonFlags)
		{
			if (!title)
				return -1;

			if (!header)
				return -1;

			if (!message)
				return -1;

			TASKDIALOGCONFIG config = { sizeof(TASKDIALOGCONFIG) };
			int32_t clicked_button = 0;
		
			config.pszContent = message;
			config.pszMainInstruction = header;
			config.pszWindowTitle = title;
			config.pszMainIcon = icon;
			config.dwCommonButtons = TDCBF_YES_BUTTON;
			config.hwndParent = nullptr;

			TaskDialogIndirect(&config, &clicked_button, nullptr, nullptr);

			return clicked_button;
		}

	};
}
