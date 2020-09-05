#pragma once

namespace IW5
{
	typedef short (__cdecl* SL_AllocString_t)(const char *);
	extern SL_AllocString_t SL_AllocString_ORIG;

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
}

inline static short SL_AllocString(const std::string& str)
{
	return IW5::SL_AllocString_ORIG(str.c_str());
}