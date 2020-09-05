#include "common_includes.hpp"
#include "hooks.hpp"

#include "IW5.hpp"

std::map<int, std::string> commandTable;

HOOK_DETOUR_DECLARE(hkCommandToIndex);
NOINLINE int hkCommandToIndex(const char* command)
{
	int result = HOOK_DETOUR_GET_ORIG(hkCommandToIndex)(command);

	if (result == 0)
	{
		std::string cmd(command);

		for (auto  &kv : commandTable)
		{
			if (kv.second == cmd)
				return kv.first;
		}

		result = 91 + commandTable.size();
		commandTable.emplace(result, command);
	}

	return result;
}

HOOK_DETOUR_DECLARE(hkExecuteBindedCommand);
NOINLINE void hkExecuteBindedCommand(int a1, int cmdIndex, int a3)
{
	if (cmdIndex >= 91)
	{
		if (commandTable.contains(cmdIndex))
		{
			std::string cmd = commandTable[cmdIndex];

			IW5::Cbuf_AddText(0, (cmd + "\n"s).c_str());
		}
	}

	return HOOK_DETOUR_GET_ORIG(hkExecuteBindedCommand)(a1, cmdIndex, a3);
}

HOOK_DETOUR_DECLARE(hkWriteBindings);
NOINLINE void hkWriteBindings(int a1, int file)
{
	const char** commandWhiteList = (const char**)0x8AF1E0;
	const int* keybindings = (const int*)0xB3A398;

	for (int i = 0; i < 256; i++)
	{
		char* key = IW5::Key_KeynumToString(i, 0);
		int value = keybindings[3 * i];
		if (value != 0 && value < 91)
		{
			IW5::FS_Printf(file, "bind %s \"%s\"\n", key, commandWhiteList[value]);
		}
		else if (value >= 91)
		{
			if (commandTable.contains(value))
			{
				IW5::FS_Printf(file, "bind %s \"%s\"\n", key, commandTable[value].c_str());
			}
		}
	}
}

void PatchMW3_CustomBinding()
{
	HOOK_DETOUR(0x47D300, hkCommandToIndex);
	HOOK_DETOUR(0x535540, hkExecuteBindedCommand);
	HOOK_DETOUR(0x4F9A10, hkWriteBindings);
}