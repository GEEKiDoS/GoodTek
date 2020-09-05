#include "common_includes.hpp"
#include <thread>

void PatchMW3();
void PatchTeknoGods();

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
		CreateDebugConsole();

		PatchTeknoGods();
		PatchMW3();
	}

	return true;
}