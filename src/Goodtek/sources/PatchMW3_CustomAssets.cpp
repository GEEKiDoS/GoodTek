#include "common_includes.hpp"
#include "hooks.hpp"

#include <filesystem>

#include "IW5.hpp"
#include "CustomAssetReader.hpp"

using namespace std;
using namespace IW5;

ZoneMemory* g_pCustomZoneMemory = new ZoneMemory(1024 * 400);

std::map<std::string, XAssetHeader> loadedWeapons;


HOOK_DETOUR_DECLARE(hkDB_AddXAsset);
NOINLINE char* __cdecl hkDB_AddXAsset(IW5::XAssetType type, IW5::XAssetHeader* header)
{
	switch (type)
	{
	case xanim:
	{
		const auto path = "userraw\\xanims\\"s + std::string(header->xanimparts->name) + ".xae2"s;

		if (std::filesystem::exists(path))
		{
			auto anim = ReadXAnimParts(path, g_pCustomZoneMemory);
			anim->name = strdup(header->xanimparts->name);

			printf("Loaded XAnim %s from userraw\n", anim->name);

			XAssetHeader newHeader;
			newHeader.xanimparts = anim;

			return HOOK_DETOUR_GET_ORIG(hkDB_AddXAsset)(type, &newHeader);
		}

		break;
	}
	case loaded_sound:
	{
		const auto path = "userraw\\loaded_sound\\"s + std::string(header->loadedsound->name);

		if (std::filesystem::exists(path))
		{
			auto sound = ReadLoadedSound(path, g_pCustomZoneMemory);
			
			if (sound)
			{
				memcpy(&header->loadedsound->sound, &sound->sound, sizeof(MssSound));

				printf("Loaded Sound %s from userraw\n", header->loadedsound->name);
				return HOOK_DETOUR_GET_ORIG(hkDB_AddXAsset)(type, header);
			}
		}

		break;
	}
	//case weapon:
	//{
	//	if (!header->weapon->hideTags)
	//		header->weapon->hideTags = new short[32];

	//	header->weapon->hideTags[0] = SL_AllocString("tag_dummy"s);

	//	break;
	//}
	//case sound: // for listing crap
	//{
	//	for (int i = 0; i < header->sound->count; i++)
	//	{
	//		if (header->sound->head[i].soundFile->type == 1)
	//		{
	//			printf("Sound alias: %s - %s\n", header->sound->head[i].aliasName, header->sound->head[i].soundFile->sound.loadSnd->name);
	//		}
	//	}

	//	break;
	//}

	//case font: // test
	//{
	//	Font_s* origFont = header->font;
	//	Font_s* newFont = new Font_s;

	//	memcpy(newFont, origFont, sizeof(Font_s));

	//	newFont->glyphs = (Glyph*)malloc(newFont->glyphCount * sizeof(Glyph));

	//	for (int i = 0; i < origFont->glyphCount; i++)
	//	{

	//		memcpy(&newFont->glyphs[i], &origFont->glyphs[0], sizeof(Glyph));
	//	}

	//	XAssetHeader newHeader;
	//	newHeader.font = newFont;

	//	return HOOK_DETOUR_GET_ORIG(hkDB_AddXAsset)(type, &newHeader);
	//}
	}

	return HOOK_DETOUR_GET_ORIG(hkDB_AddXAsset)(type, header);
}

std::map<std::string, LocalizeEntry *> customUIString = {
	{ "MENU_MULTIPLAYER_CAPS", new LocalizeEntry { "^1GOOD^2Tekno^7MW3", "MENU_MULTIPLAYER_CAPS" } },
	{ "MENU_MAIN_MENU_CAPS", new LocalizeEntry { "CHECK TEKNO UPDATE", "MENU_MAIN_MENU_CAPS" } }
};

#pragma optimize("", off)
HOOK_DETOUR_DECLARE(hkDB_FindXAssetHeader);
NOINLINE XAssetHeader __cdecl hkDB_FindXAssetHeader(XAssetType type, const char* name, bool flag)
{
	switch (type)
	{
	case localize:
	{
		if (customUIString.contains(name))
		{
			XAssetHeader header;
			header.localize = customUIString[name];

			return header;
		}

		break;
	}
	case weapon:
	{
		if (loadedWeapons.contains(name))
			return loadedWeapons[name];

		const auto path = "userraw\\weapons\\mp\\"s + std::string(name);

		if (std::filesystem::exists(path))
		{
			auto baseWeapon = HOOK_DETOUR_GET_ORIG(hkDB_FindXAssetHeader)(type, name, flag).weapon;

			auto weapon = ReadWeapon(path, g_pCustomZoneMemory, HOOK_DETOUR_GET_ORIG(hkDB_FindXAssetHeader));

			printf("Loaded Weapon %s from userraw\n", weapon->name);


			XAssetHeader newHeader;
			newHeader.weapon = weapon;

			loadedWeapons.emplace(name, newHeader);

			return newHeader;
		}

		/*if (std::string(name) == "iw5_p99_mp"s)
		{
			WeaponCompleteDef* weapon = HOOK_DETOUR_GET_ORIG(hkDB_FindXAssetHeader)(type, name, flag).weapon;

			weapon->WeaponDef->sprintLoopTime = 1291;

			XAssetHeader asset;
			asset.weapon = weapon;

			return asset;
		}*/

		break;
	}
	}

	return HOOK_DETOUR_GET_ORIG(hkDB_FindXAssetHeader)(type, name, flag);
}
#pragma optimize("", on)

void PatchMW3_CustomAssets()
{
	HOOK_DETOUR(0x5C8C60, hkDB_AddXAsset);
	HOOK_DETOUR(0x4B25C0, hkDB_FindXAssetHeader);
}