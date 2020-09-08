#include "common_includes.hpp"
#include "hooks.hpp"

#include "IW5.hpp"

namespace Client
{
	IW5::dvar_t* jump_slowdownEnable;

	HOOK_DETOUR_DECLARE(hkJump_ApplySlowdown);
	NOINLINE void __cdecl hkJump_ApplySlowdown(IW5::playerState_s* ps)
	{
		if (jump_slowdownEnable->current.boolean)
		{
			HOOK_DETOUR_GET_ORIG(hkJump_ApplySlowdown)(ps);
		}
		else
		{
			if (ps->pm_time <= 1800)
			{
				if (!ps->pm_time)
				{
					if ((float)(ps->jumpOriginZ + 18.0) <= ps->origin[2])
					{
						ps->pm_time = 1200;
					}
					else
					{
						ps->pm_time = 1800;
					}
				}
			}
			else
			{
				ps->pm_flags &= ~0x4000;
				ps->jumpOriginZ = 0.0;
			}
		}
	}

	HOOK_DETOUR_DECLARE(hkJump_CalcHeightScale);
	NOINLINE double __cdecl hkJump_CalcHeightScale(IW5::playerState_s* ps)
	{
		if (jump_slowdownEnable->current.boolean)
		{
			return HOOK_DETOUR_GET_ORIG(hkJump_CalcHeightScale)(ps);
		}
		else
		{
			return 1.0;
		}
	}

	double __stdcall Jump_CalcHeightScale_UsercallStub(IW5::playerState_s* ps)
	{
		return 1.0;
	}

	// mysterious usercall
	HOOK_DETOUR_DECLARE(hkJump_CalcHeightScale_Usercall);
	NOINLINE __declspec(naked) void hkJump_CalcHeightScale_Usercall()
	{
		__asm
		{
			push ecx
			push eax
		}

		if (jump_slowdownEnable->current.boolean)
		{
			__asm {
				pop eax
				pop ecx
				jmp _hkJump_CalcHeightScale_UsercallOrig
			}
		}

		__asm
		{
			call Jump_CalcHeightScale_UsercallStub

			pop ecx
			retn
		}
	}

	HOOK_DETOUR_DECLARE(hkJump_RegisterDvars);
	NOINLINE void __cdecl hkJump_RegisterDvars()
	{
		IW5::Dvar_RegisterFloat("jump_height", 39.0, 0.0, 1000.0, 0x140u, "The maximum height of a player's jump");
		IW5::Dvar_RegisterFloat("jump_stepSize", 18.0, 0.0, 64.0, 0x140u, "The maximum step up to the top of a jump arc");
		jump_slowdownEnable = IW5::Dvar_RegisterBool("jump_slowdownEnable", 1, 0x180u, "Slow player movement after jumping");
		IW5::Dvar_RegisterFloat("jump_ladderPushVel", 128.0, 0.0, 1024.0, 0x140u, "The velocity of a jump off of a ladder");
		IW5::Dvar_RegisterFloat("jump_spreadAdd", 64.0, 0.0, 512.0, 0x140u, "The amount of spread scale to add as a side effect of jumping");
	}

	void PatchMW3_Jump()
	{
		HOOK_DETOUR(0x429BD0, hkJump_ApplySlowdown);
		HOOK_DETOUR(0x4A93E0, hkJump_CalcHeightScale);
		HOOK_DETOUR(0x5745C0, hkJump_CalcHeightScale_Usercall);
		HOOK_DETOUR(0x54A1F0, hkJump_RegisterDvars);
	}
}