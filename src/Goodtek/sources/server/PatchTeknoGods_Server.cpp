#include "common_includes.hpp"
#include "hooks.hpp"

#include <regex>

#define CONFIG_NAME ".\\teknomw3ext.ini"

namespace Server
{
	typedef void(__cdecl* DebugOutput_t)(const char* format, ...);
	DebugOutput_t DebugOutput = (DebugOutput_t)0x10001550;

	static bool g_bReplaceGameType = false;
	static bool g_bReplaceMapName = false;
	static bool g_bFakePlayerNumbers = false;

	static int g_iUpdateFakePlayerNumberInterval = 10000;

	static std::string* g_sCustomGameType;

	std::string mapnameLookup(std::string mapName)
	{
		char result[0x20];

		GetPrivateProfileStringA("MapNames", mapName.c_str(), mapName.c_str(), result, 0x20, CONFIG_NAME);

		return std::string(result);
	}

	static int players = 0, playersMin = 0, playersMax = 256;

	HWND* consoleWindow = (HWND*)0x592C828;
	char consoleTitleBuffer[0x100];
	void SetMW3ConsoleTitle(const char* text, ...)
	{
		va_list arglist;
		va_start(arglist, text);
		_vsnprintf(consoleTitleBuffer, 0x100, text, arglist);
		va_end(arglist);

		SetWindowText(*consoleWindow, consoleTitleBuffer);
	}

	char serverName[100];
	char mapName[100];
	int realPlayers;
	int realPlayersMax;
	bool needUpdateMapName = false;

	HOOK_DETOUR_DECLARE(hkUpdateServerStatus);
	NOINLINE void __stdcall hkUpdateServerStatus(int cPlayers, int cPlayersMax, int cBotPlayers,
		const char* pchServerName, const char* pSpectatorServerName,
		const char* pchMapName)
	{
		if (!serverName[0])
			strcpy(serverName, pchServerName);

		if (needUpdateMapName)
			strcpy(mapName, pchMapName);

		realPlayers = cPlayers;
		realPlayersMax = cPlayersMax;

		if(g_bFakePlayerNumbers)
			HOOK_DETOUR_GET_ORIG(hkUpdateServerStatus)(players, playersMax, cBotPlayers, pchServerName, pSpectatorServerName, pchMapName);
		else
			HOOK_DETOUR_GET_ORIG(hkUpdateServerStatus)(cPlayers, cPlayersMax, cBotPlayers, pchServerName, pSpectatorServerName, pchMapName);
	}

	std::regex gameModeRegex("gt\\\\.*?\\\\");
	std::regex mapNameRegex("m\\\\.*?\\\\");

	HOOK_DETOUR_DECLARE(hkSetGameTags);
	NOINLINE void __fastcall hkSetGameTags(int _this, void* edx, const char* pszGameTags)
	{
		needUpdateMapName = true;

		char* serverTags = (char*)_this + 2133;

		std::string sGameTags(pszGameTags);

		if (g_bReplaceGameType)
		{
			std::string gameType("gt\\");
			gameType += *g_sCustomGameType;
			gameType += "\\";

			sGameTags = std::regex_replace(sGameTags, gameModeRegex, gameType);
		}

		if (g_bReplaceMapName)
		{
			std::smatch result;
			std::string::const_iterator iter = sGameTags.begin();
			std::string::const_iterator iterEnd = sGameTags.end();

			std::regex_search(iter, iterEnd, result, mapNameRegex);

			std::string mapCode = result[0];
			mapCode = mapCode.substr(2, mapCode.size() - 3);

			std::string mapName("m\\");
			mapName += mapnameLookup(mapCode);
			mapName += "\\";

			sGameTags = std::regex_replace(sGameTags, mapNameRegex, mapName);
		}

		DebugOutput("[TeknoMW3 2805b Ext] Changing GameTags, new Tags: %s\n", sGameTags.c_str());
		snprintf(serverTags, 512, "%s", sGameTags.c_str());
	}

	DWORD WINAPI UpdateServerInfoThread(LPVOID lpArguments)
	{
		for (;;)
		{
			if (g_bFakePlayerNumbers)
			{
				SetMW3ConsoleTitle("TeknoMW3 Dedicated Server [%s on %s] %d/%d (fake players: %d/%d)", serverName, mapName, realPlayers, realPlayersMax, players, playersMax);
			}
			else
			{
				SetMW3ConsoleTitle("TeknoMW3 Dedicated Server [%s on %s] %d/%d", serverName, mapName, realPlayers, realPlayersMax);
			}

			Sleep(1000);
		}

		return 0;
	}

	DWORD WINAPI UpdatePlayerNumThread(LPVOID lpArguments)
	{
		for (;;)
		{
			players = playersMin + rand() % (playersMax - playersMin);

			Sleep(g_iUpdateFakePlayerNumberInterval);
		}

		return 0;
	}

	void PatchTeknoGods()
	{
		srand(time(0));

		g_bReplaceGameType = GetPrivateProfileIntA("Config", "ReplaceGameType", 0, CONFIG_NAME);
		g_bReplaceMapName = GetPrivateProfileIntA("Config", "ReplaceMapName", 0, CONFIG_NAME);
		g_bFakePlayerNumbers = GetPrivateProfileIntA("Config", "FakePlayerNumbers", 0, CONFIG_NAME);

		if (g_bFakePlayerNumbers)
		{
			g_iUpdateFakePlayerNumberInterval = GetPrivateProfileIntA("Config", "UpdateFakePlayerNumberInterval", 10000, CONFIG_NAME);
			playersMax = GetPrivateProfileIntA("Config", "MaxPlayers", 18, CONFIG_NAME);
			playersMin = GetPrivateProfileIntA("Config", "MinPlayers", 0, CONFIG_NAME);

			CloseHandle(CreateThread(nullptr, NULL, UpdatePlayerNumThread, nullptr, NULL, nullptr));
		}

		HOOK_DETOUR(0x10022AD0, hkUpdateServerStatus);

		if (g_bReplaceGameType)
		{
			char gameType[0x20];
			GetPrivateProfileStringA("Config", "CustomGameType", "MyGameType", gameType, 0x20, ".\\teknomw3ext.ini");

			g_sCustomGameType = new std::string(gameType);
		}

		if (g_bReplaceGameType || g_bReplaceMapName)
		{
			HOOK_DETOUR(0x10010BC0, hkSetGameTags);
		}

		CloseHandle(CreateThread(nullptr, NULL, UpdateServerInfoThread, nullptr, NULL, nullptr));
	}
}