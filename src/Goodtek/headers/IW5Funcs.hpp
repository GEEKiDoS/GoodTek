#pragma once

namespace IW5
{
	typedef short (__cdecl* SL_AllocString_t)(const char *);
	extern SL_AllocString_t SL_AllocString_ORIG;

	typedef const char* (__cdecl* SL_ConvertToString_t)(std::uint16_t index);
	extern SL_ConvertToString_t SL_ConvertToString_ORIG;

	typedef void(__cdecl* Cbuf_AddText_t)(int controller, const char* command);
	extern Cbuf_AddText_t Cbuf_AddText;

	typedef void(__cdecl* FS_Printf_t)(int file, char* fmt, ...);
	extern FS_Printf_t FS_Printf;

	typedef char* (__cdecl* Key_KeynumToString_t)(int key, int a2);
	extern Key_KeynumToString_t Key_KeynumToString;

	typedef char* (__cdecl* StartPrivateParty_t)(int unk1, int unk2);
	extern StartPrivateParty_t StartPrivateParty;
	
	typedef XAssetHeader(__cdecl* DB_FindXAssetHeader_t)(XAssetType type, const char* name, bool flag);
	extern DB_FindXAssetHeader_t DB_FindXAssetHeader;

	typedef void(__cdecl* Com_Printf_t)(const char* format, ...);
	extern Com_Printf_t Com_Printf;

	typedef dvar_t* (__cdecl* Dvar_RegisterBool_t)(const char* name, unsigned char defaultVal, int flags, const char* help);
	extern Dvar_RegisterBool_t Dvar_RegisterBool;

	typedef dvar_t* (__cdecl* Dvar_RegisterFloat_t)(const char* name, float defaultVal, float min, float max, int flags, const char* help);
	extern Dvar_RegisterFloat_t Dvar_RegisterFloat;

	typedef void (__cdecl* Sys_ShowConsole_t)();
	extern Sys_ShowConsole_t Sys_ShowConsole;

	typedef void(__cdecl* Cmd_AddCommand_t)(const char*, void (*)(), cmd_function_s*);
	extern Cmd_AddCommand_t Cmd_AddCommand;

	typedef int(__cdecl* Load_Stream_t)(bool atStreamStart, const void* ptr, int size);
	extern Load_Stream_t Load_Stream;

	typedef void(__cdecl* DB_LoadXAssets_t)(XZoneInfo* zoneInfo, unsigned int zoneCount, int sync);
	extern DB_LoadXAssets_t DB_LoadXAssets;

	void InitFunc_Client14();
	void InitFunc_Server14();
}

inline static short SL_AllocString(const std::string& str)
{
	return IW5::SL_AllocString_ORIG(str.c_str());
}

inline static std::string SL_ConvertToString(std::uint16_t index)
{
	return IW5::SL_ConvertToString_ORIG(index);
}