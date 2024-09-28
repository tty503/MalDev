#include <windows.h>
#include <cstdio>

typedef int(__cdecl* MeowProc)();

int main(void)
{
	HINSTANCE meowDll;
	MeowProc  meowFunc;

	meowDll    = LoadLibrary(TEXT("C:\\Users\\pc\\source\\repos\\MalDev\\x64\\Debug\\Classic_DLL_Injection_SetWindowsHookEx-DLL.dll"));
	meowFunc   = (MeowProc)GetProcAddress(meowDll, "Meow");

	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)meowFunc, meowDll, 0);

	Sleep(5 * 1000);

	UnhookWindowsHookEx(hook);
	return 0;
}