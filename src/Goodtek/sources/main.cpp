#include "common_includes.hpp"
#include <thread>

void PatchMW3();
void PatchTeknoGods();

BOOL APIENTRY DllMain(HMODULE, DWORD uCallReason, LPVOID)
{
	if (uCallReason == DLL_PROCESS_ATTACH)
	{
		PatchTeknoGods();
		PatchMW3();
	}

	return true;
}