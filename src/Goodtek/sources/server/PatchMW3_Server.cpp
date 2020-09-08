#include "common_includes.hpp"
#include "hooks.hpp"

using namespace std;

namespace Server
{
	void PatchMW3_Jump();
	void PatchMW3_CustomAssets();

	void PatchMW3()
	{
		// enable userraw
		std::array<uint8_t, 1> userrawPatch = { 0 };

		WriteProtectedMemory(userrawPatch, 0x5121EC);
		WriteProtectedMemoryWithString("userraw", 0x5121F1);

		// change console font to consolas
		WriteProtectedMemoryWithString("Consolas", 0x528640);

		PatchMW3_Jump();
		PatchMW3_CustomAssets();
	}
}