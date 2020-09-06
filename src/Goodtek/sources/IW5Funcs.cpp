#include "common_includes.hpp"
#include "IW5.hpp"

namespace IW5
{
	SL_AllocString_t		SL_AllocString_ORIG;
	SL_ConvertToString_t	SL_ConvertToString_ORIG;
	Cbuf_AddText_t			Cbuf_AddText;
	FS_Printf_t				FS_Printf;
	Key_KeynumToString_t	Key_KeynumToString;
	StartPrivateParty_t		StartPrivateParty;
	DB_FindXAssetHeader_t	DB_FindXAssetHeader;

	void InitFunc_Client14()
	{
		SL_AllocString_ORIG = (SL_AllocString_t)0x473490;
		Cbuf_AddText = (Cbuf_AddText_t)0x4C1030;
		FS_Printf = (FS_Printf_t)0x438120;
		Key_KeynumToString = (Key_KeynumToString_t)0x4CCFF0;
		StartPrivateParty = (StartPrivateParty_t)0x51CA50;
		DB_FindXAssetHeader = (DB_FindXAssetHeader_t)0x4B25C0;
	}

	void InitFunc_Server14()
	{
		DB_FindXAssetHeader = (DB_FindXAssetHeader_t)0x455860;

		SL_AllocString_ORIG = (SL_AllocString_t)0x4D83C0;
		SL_ConvertToString_ORIG = [](std::uint16_t index) 
		{
			return reinterpret_cast<const char*>(*reinterpret_cast<char**>(0x1CBB980) + 12 * index + 4);
		};
	}
}