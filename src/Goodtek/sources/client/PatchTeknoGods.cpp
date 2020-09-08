#include "common_includes.hpp"
#include "hooks.hpp"

namespace Client 
{
	void PatchTeknoGods()
	{
		// Remove debugger check
		std::array<uint8_t, 1> jmp{ 0xEB };
		WriteProtectedMemory(jmp, 0x100169E6);

		// Kill remote code edit backdoor, also prevent it from editing color table which I will edit in the PatchMW3 later
		std::array<uint8_t, 42> patch1;

		// fill with nops
		for (auto& byte : patch1)
			byte = 0x90;

		WriteProtectedMemory(patch1, 0x10016C0C);

		// Pump that bass!
		WriteProtectedMemoryWithString("^;Pump that Bass!", 0x100164EF);
		WriteProtectedMemoryWithString("^2IW5MP 1.4.382\n^1TeknoMW3 2.8.0.4\n^4GOODTEK 0.1", 0x1000E8B9);

		// Remove compassSize check, I mean, why?
		WriteProtectedMemoryWithString("sec_sv_blocked_dvars;sec_cfg_blocked_dvars", 0x1004E654);

		std::array<uint8_t, 2> patch2{ 0x90, 0x90 };
		WriteProtectedMemory(patch2, 0x1001688A);

		// Why exit process there? NO!
		WriteProtectedMemory(jmp, 0x1000CD6B);
		WriteProtectedMemory(jmp, 0x1000CD7F);

		// Hey, you forgot the '\n', so We add extra branding to it lol.
		WriteProtectedMemoryWithString("TeknoMW3 v%f by Stte&Simon with GOODTEK by GEEKiDoS. \n", 0x100178A1);
	}
}