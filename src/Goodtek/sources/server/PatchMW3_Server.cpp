#include "common_includes.hpp"
#include "hooks.hpp"

#include <filesystem>

#include "IW5.hpp"
#include "CustomAssetReader.hpp"

#include <zlib.h>

using namespace std;
using namespace IW5;

extern ZoneMemory* g_pCustomZoneMemory;

extern std::map<std::string, XAssetHeader> loadedWeapons;

namespace Server
{
	typedef void(__cdecl* DebugOutput_t)(const char* format, ...);
	extern DebugOutput_t DebugOutput;

	ZoneMemory* weaponPool = new ZoneMemory(100 * 1024 * 1024);

	HOOK_DETOUR_DECLARE(hkDB_AddXAsset);
	NOINLINE char* __cdecl hkDB_AddXAsset(XAssetType type, XAssetHeader* asset)
	{
		if (type == map_ents)
		{
			auto ents = asset->mapents;

			auto file = "userraw/"s + ents->name;

			if (std::filesystem::exists(file))
			{
				auto newEnts = (MapEnts*)malloc(sizeof(MapEnts));
				memcpy(newEnts, ents, sizeof(MapEnts));

				std::stringstream buffer;
				std::ifstream ifs;
				ifs.open(file);
				buffer << ifs.rdbuf();
				ifs.close();

				auto entStr = buffer.str();

				newEnts->entityString = strdup(entStr.c_str());
				newEnts->numEntityChars = entStr.size();

				XAssetHeader newHeader;
				newHeader.mapents = newEnts;

				DebugOutput("[TeknoMW3 2805b Ext] Loaded map ent %s from userraw\n", ents->name);

				return HOOK_DETOUR_GET_ORIG(hkDB_AddXAsset)(type, &newHeader);
			}
			else
			{
				FILE* write = fopen(file.c_str(), "w");
				if (write)
				{
					fprintf(write, ents->entityString);
					fclose(write);

					DebugOutput("[TeknoMW3 2805b Ext] Dumped map ent %s\n", ents->name);
				}
			}
		}

		return HOOK_DETOUR_GET_ORIG(hkDB_AddXAsset)(type, asset);
	}

	HOOK_DETOUR_DECLARE(hkDB_FindXAssetHeaderServer);
	NOINLINE XAssetHeader __cdecl hkDB_FindXAssetHeaderServer(XAssetType type, const char* name, bool flag)
	{
		if (type == weapon)
		{
			//DebugOutput("Reading weapon %s\n", name);

			if (loadedWeapons.contains(name))
				return loadedWeapons[name];

			const auto path = "userraw\\weapons\\mp\\"s + std::string(name);

			if (std::filesystem::exists(path))
			{
				auto weapon = ReadWeapon(path, weaponPool, HOOK_DETOUR_GET_ORIG(hkDB_FindXAssetHeaderServer));

				printf("Loaded Weapon %s from userraw\n", weapon->name);


				XAssetHeader newHeader;
				newHeader.weapon = weapon;

				loadedWeapons.emplace(name, newHeader);

				return newHeader;
			}
		}

		return HOOK_DETOUR_GET_ORIG(hkDB_FindXAssetHeaderServer)(type, name, flag);
	}

	HOOK_DETOUR_DECLARE(hkOnServerSpawned);
	NOINLINE void __fastcall hkOnServerSpawned(void *_this, void *edx)
	{
		DebugOutput("Cleared weapon data\n");

		loadedWeapons.clear();
		weaponPool->Free();
		weaponPool = new ZoneMemory(100 * 1024 * 1024);

		return HOOK_DETOUR_GET_ORIG(hkOnServerSpawned)(_this, edx);
	}

	void PatchMW3()
	{
		std::array<uint8_t, 1> userrawPatch = { 0 };

		WriteProtectedMemory(userrawPatch, 0x5121EC);
		WriteProtectedMemoryWithString("userraw", 0x5121F1);
		WriteProtectedMemoryWithString("Consolas", 0x528640);

		HOOK_DETOUR(0x455E90, hkDB_AddXAsset);
		HOOK_DETOUR(IW5::DB_FindXAssetHeader, hkDB_FindXAssetHeaderServer);
		HOOK_DETOUR(0x4D97B0, hkOnServerSpawned);
	}
}