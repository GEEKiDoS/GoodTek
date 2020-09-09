#include "common_includes.hpp"
#include "hooks.hpp"

#include <filesystem>
#include <intrin.h>

#include "IW5.hpp"

#include "zstd.h"
#include "zlib.h"

namespace Client
{
	struct db_zstd_context_s
	{
		// original context
		char pad[24];

		// zstd context
		ZSTD_DCtx* DCtx;
		ZSTD_DStream* DStream;
	};

	struct db_z_stream_s
	{
		char* next_in;
		unsigned int avail_in;
		unsigned int total_in;
		char* next_out;
		unsigned int avail_out;
		unsigned int total_out;
		char* msg;
		db_zstd_context_s* state;
		char* (__cdecl* zalloc)(char*, unsigned int, unsigned int);
		void(__cdecl* zfree)(char*, char*);
		char* opaque;
		int data_type;
		unsigned int adler;
	};

	static void decrypt_data(char* _data, std::size_t _size)
	{
		auto fastfile = static_cast<std::string>(*reinterpret_cast<const char**>(0x135E190) + 4);
		// FileSystem::GetFastFile();

		auto encryptionKey = static_cast<std::string>(fastfile + ": This fastfile is property of the Plutonium Project."
			);
		auto dataptr = _data;

		auto keyPos = 0;
		for (int rounds = 3; rounds >= 0; rounds--)
		{
			for (auto i = 0u; i < _size; i++)
			{
				if (keyPos >= encryptionKey.size())
				{
					keyPos = 0;
				}

				dataptr[i] = dataptr[i] ^ rounds;
				dataptr[i] = dataptr[i] ^ encryptionKey[keyPos];
				dataptr[i] = 0xFF - dataptr[i];
				keyPos++;
			}
		}
	}

	int ffversion = 0;

	HOOK_DETOUR_DECLARE(hkDB_InflateInit);
	NOINLINE int __cdecl hkDB_InflateInit(db_z_stream_s* strm, int w, const char *version, int stream_size)
	{
		if (ffversion < 2000)
			return HOOK_DETOUR_GET_ORIG(hkDB_InflateInit)(strm, w, version, stream_size);

		// Allocate zstd context
		strm->state = reinterpret_cast<db_zstd_context_s*>(malloc(sizeof db_zstd_context_s));
		memset(strm->state, 0, sizeof db_zstd_context_s);

		// Create pointers needed for decompression
		strm->state->DCtx = ZSTD_createDCtx();
		strm->state->DStream = ZSTD_createDStream();

		// return ZLIB ok state
		return Z_OK;
	}

	HOOK_DETOUR_DECLARE(hkDB_Inflate);
	NOINLINE int __cdecl hkDB_Inflate(db_z_stream_s* strm, int f)
	{
		if (ffversion < 2000)
			return HOOK_DETOUR_GET_ORIG(hkDB_Inflate)(strm, f);

		// if there are no bytes available in stream..
		if (strm->avail_in <= 0)
		{
			return Z_OK;
		}

		ZSTD_inBuffer inBuf;
		ZSTD_outBuffer outBuf;

		inBuf.src = strm->next_in;
		inBuf.size = strm->avail_in;
		inBuf.pos = 0;

		outBuf.dst = strm->next_out;
		outBuf.size = strm->avail_out;
		outBuf.pos = 0;

		auto code = ZSTD_decompressStream(strm->state->DStream, &outBuf, &inBuf);
		if (ZSTD_isError(code))
		{
			printf("An error occured while decompressing zone: %s", ZSTD_getErrorName(code));
			return Z_STREAM_ERROR;
		}

		strm->next_out += outBuf.pos;
		strm->total_out += outBuf.pos;
		strm->avail_out -= outBuf.pos;

		strm->next_in += inBuf.pos;
		strm->total_in += inBuf.pos;
		strm->avail_in -= inBuf.pos;

		return Z_OK;
	}

	HOOK_DETOUR_DECLARE(hkDB_InflateEnd);
	NOINLINE int __cdecl hkDB_InflateEnd(db_z_stream_s* strm)
	{
		if (ffversion < 2000)
			return HOOK_DETOUR_GET_ORIG(hkDB_InflateEnd)(strm);

		// free decompression stream
		ZSTD_freeDCtx(strm->state->DCtx);
		ZSTD_freeDStream(strm->state->DStream);
		free(strm->state);

		return Z_OK;
	}

	void __stdcall SetFastfileVersionStub(int version)
	{
		ffversion = version;
		printf("Loading FF with version %d\n", ffversion);
	}

	HOOK_DETOUR_DECLARE(hkReadFFVersion);
	NOINLINE __declspec(naked) void hkReadFFVersion()
	{
		__asm
		{
			cmp eax, 1;
			je ok;

			cmp eax, 1000;
			jae ok;

			push 0x4E451A;
			retn;

		ok:
			push eax;
			call SetFastfileVersionStub;

			push 0x4E453D;
			retn;
		}
	}
	
	HOOK_DETOUR_DECLARE(hkLoad_Stream);
	NOINLINE int __cdecl hkLoad_Stream(bool atStreamStart, const void* ptr, int size)
	{
		int result = HOOK_DETOUR_GET_ORIG(hkLoad_Stream)(atStreamStart, ptr, size);

		if (ffversion < 2000)
			return result;

		int addr = (int)_ReturnAddress();

		switch (addr)
		{
		case 0x444E95: // weapon
		case 0x4A5F85: // techset
		{
			decrypt_data((char*)ptr, size);
			break;
		}
		}

		return result;
	}

	HOOK_DETOUR_DECLARE(hkStrNCmp);
	NOINLINE char __cdecl hkStrNCmp(char *source, const char *target, size_t size)
	{
		if (std::string(target) == "IWffu100"s)
			return true;

		return HOOK_DETOUR_GET_ORIG(hkStrNCmp)(source, target, size);
	}

	void LoadZone_f()
	{
		IW5::XZoneInfo zone = { "patch_goodtek", 20, 0 };
		IW5::DB_LoadXAssets(&zone, 1, 0);
	}

	HOOK_DETOUR_DECLARE(hkDB_LoadXAssets);
	NOINLINE void hkDB_LoadXAssets(IW5::XZoneInfo* zoneInfo, uint32_t zoneCount, int sync)
	{
		bool isCommonAssets = false;

		for (int i = 0; i < zoneCount; i++)
		{
			if (!zoneInfo[i].zone)
				continue;

			printf("Loading zone %s\n", zoneInfo[i].zone);

			if (std::string(zoneInfo[i].zone) == "patch_mp"s)
			{
				isCommonAssets = true;
			}
		}

		if (isCommonAssets)
		{
			std::vector<IW5::XZoneInfo> infos;
			for (int i = 0; i < zoneCount; i++)
			{
				infos.push_back(zoneInfo[i]);
			}

			if (filesystem::exists("zone\\goodtek\\"))
			{
				for (const auto& entry : std::filesystem::directory_iterator("zone\\goodtek\\"s))
				{
					if (entry.is_regular_file())
					{
						if (entry.path().extension().string() == ".ff")
						{
							auto zoneName = entry.path().filename();
							zoneName.replace_extension();

							zoneName = "..\\goodtek\\"s / zoneName;

							printf("Loading custom zone %s\n", entry.path().filename().string().c_str());

							IW5::XZoneInfo zone;
							zone.zone = strdup(zoneName.string().c_str());
							zone.loadFlags = 1;
							zone.unloadFlags = 0;

							infos.push_back(zone);
						}
					}
				}
			}

			HOOK_DETOUR_GET_ORIG(hkDB_LoadXAssets)(infos.data(), infos.size(), sync);
		}

		HOOK_DETOUR_GET_ORIG(hkDB_LoadXAssets)(zoneInfo, zoneCount, sync);
	}

	void PatchMW3_FFCompression()
	{
		// Allow loading of unsigned fastfiles
		std::array<uint8_t, 1> patch1 = { 0xEB };
		WriteProtectedMemory(patch1, 0x4E44FB);

		std::array<uint8_t, 2> patch2 = { 0x90, 0x90 };
		WriteProtectedMemory(patch2, 0x4D4057);

		// DB_InflateInit - 0x53EF90
		// DB_InflateEnd - 0x4459B0
		// DB_Inflate - 0x4E8E20
		HOOK_DETOUR(0x53EF90, hkDB_InflateInit);
		HOOK_DETOUR(0x4459B0, hkDB_InflateEnd);
		HOOK_DETOUR(0x4E8E20, hkDB_Inflate);

		// 0x4E4515 - We have to get the value of eax then jump back (ff version)
		HOOK_DETOUR(0x4E4515, hkReadFFVersion);

		// 0x4292F0 - load_stream
		HOOK_DETOUR(0x4C53A0, hkLoad_Stream);

		// 0x444E90 - Weapon
		// 0x4A5F80 - Techset

		// load all custom ffs after init
		HOOK_DETOUR(IW5::DB_LoadXAssets, hkDB_LoadXAssets);
	}
}