#include "common_includes.hpp"
#include <thread>

#include "IW5.hpp";

namespace Client 
{
	void PatchMW3();
	void PatchTeknoGods();
}

namespace Server
{
	void PatchMW3();
	void PatchTeknoGods();
}

void CreateDebugConsole()
{
	AllocConsole();

	auto ret = freopen("CONIN$", "r", stdin);
	ret = freopen("CONOUT$", "w", stdout);
	ret = freopen("CONOUT$", "w", stderr);

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
}

BOOL APIENTRY DllMain(HMODULE, DWORD uCallReason, LPVOID)
{
	if (uCallReason == DLL_PROCESS_ATTACH)
	{
		// Client
		if (*(uint32_t*)0x822E30 == 0x20355749) // "IW5\x20"
		{
			IW5::InitFunc_Client14();

			CreateDebugConsole();

			Client::PatchTeknoGods();
			Client::PatchMW3();
		}
		// Server
		else if (*(uint32_t*)0x6EA8B4 == 0x20355749)
		{
			IW5::InitFunc_Server14();

			Server::PatchTeknoGods();
			Server::PatchMW3();
		}
		else
		{
			MessageBox(0, "Unsupported version!", "GoodTek - Error", MB_OK);
			exit(-1);
		}
	}

	return true;
}