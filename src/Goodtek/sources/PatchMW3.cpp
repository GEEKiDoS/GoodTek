#include "common_includes.hpp"
#include "hooks.hpp"

#include <thread>
#include <map>
#include <filesystem>

void PatchMW3_CustomAssets();
void PatchMW3_CustomBinding();

void PatchMW3()
{
	// Enable userraw
	std::array<uint8_t, 1> userrawPatch = { 0 };
	WriteProtectedMemory(userrawPatch, 0x64627C);
	WriteProtectedMemoryWithString("userraw", 0x646281);

	// Color table
	DWORD * colorTable = (DWORD *)0x840B60;
	PLH::MemoryProtector ctmp(0x840B60, 28, PLH::R | PLH::W | PLH::X);

	colorTable[1] = RGB(255, 49, 49);
	colorTable[2] = RGB(134, 192, 0);
	colorTable[3] = RGB(255, 173, 34);
	colorTable[4] = RGB(0, 135, 193);
	colorTable[5] = RGB(32, 197, 255);
	colorTable[6] = RGB(151, 80, 221);

	std::thread([]()
		{
			uint32_t* pSemiColor = (uint32_t*)0x5F9CB70;
			PLH::MemoryProtector hcmp(0x5F9CB70, 4, PLH::R | PLH::W | PLH::X);

			std::array colors{ RGB(255,0,0), RGB(255, 117,0), RGB(255,255,0),RGB(51, 219, 54), RGB(0, 245, 250), RGB(13, 0, 176), RGB(156, 0, 196) };

			for (uint32_t i = 0; true; i = (i < colors.size() - 1 ? i + 1 : 0))
			{
				*pSemiColor = colors[i];

				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
	).detach();

	// RCE remote execution patch
	std::array<uint8_t, 4> rcePatch{ 0x33, 0xc0, 0xc3, 0x90 };
	WriteProtectedMemory(rcePatch, 0x4B5740);

	PatchMW3_CustomAssets();
	PatchMW3_CustomBinding();

	// party_minplayers
	std::array<uint8_t, 1> partyPatch{ 0x1 };
	WriteProtectedMemory(partyPatch, 0x4CF991);
}