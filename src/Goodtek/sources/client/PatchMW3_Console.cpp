#include "common_includes.hpp"
#include "hooks.hpp"

#include "IW5.hpp"

namespace Client
{
	std::vector<std::string> commandHistory;
	int historyPos = 0;

	HOOK_DETOUR_DECLARE(hkWndProc_ConsoleInput);
	LRESULT __stdcall hkWndProc_ConsoleInput(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		const HWND* inputHwnd = (HWND*)0x5A86340;

		if (Msg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_RETURN:
			{
				char inputBuffer[0x100];

				if (GetWindowText(*inputHwnd, inputBuffer, 100))
				{
					historyPos = commandHistory.size();
					commandHistory.push_back(strdup(inputBuffer));

					IW5::Cbuf_AddText(0, inputBuffer);
					SetWindowText(*inputHwnd, "");
				}
				break;
			}
			case VK_UP:
			{
				if (commandHistory.size() == 0)
					break;

				historyPos--;
				if (historyPos < 0)
					historyPos = commandHistory.size() - 1;

				auto cmd = commandHistory[historyPos];
				SetWindowText(*inputHwnd, cmd.c_str());
				
				break;
			}
			case VK_DOWN:
			{
				if (commandHistory.size() == 0)
					break;

				historyPos++;
				if (historyPos >= commandHistory.size())
					historyPos = 0;

				auto cmd = commandHistory[historyPos];
				SetWindowText(*inputHwnd, cmd.c_str());
				break;
			}
			}
		}

		// I really dont care rest of them
		return HOOK_DETOUR_GET_ORIG(hkWndProc_ConsoleInput)(hWnd, Msg, wParam, lParam);
	}

	void PatchMW3_Console()
	{
		// enable it
		IW5::Sys_ShowConsole();

		// handle the input
		HOOK_DETOUR(0x4AA7C0, hkWndProc_ConsoleInput);
		
		// restore quit
		static IW5::cmd_function_s quitFunc;
		IW5::Cmd_AddCommand("quit", (void(*)())0x556060, &quitFunc);
	}
}