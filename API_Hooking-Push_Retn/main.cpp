#include <windows.h> 

char	originalBytes[6];
FARPROC hookedAddress;

int __stdcall myFunc(LPCSTR lpCmdLine, UINT uCmdShow)
{
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookedAddress, originalBytes, 6, NULL);
	return WinExec("mspaint", uCmdShow);
}

void setMySuperHook()
{
	HINSTANCE hLib;
	VOID*     myFuncAddress;
	DWORD*    rOffSet;
	DWORD*    hookAddress;
	DWORD     src;
	DWORD     dst;
	CHAR	  patch[6] = { 0 };

	hLib          = LoadLibraryA("kernel32.dll");
	hookedAddress = GetProcAddress(hLib, "WinExec");

	ReadProcessMemory(GetCurrentProcess(), (LPCVOID)hookedAddress, originalBytes, 6, NULL);
	myFuncAddress = &myFunc; 

	memcpy_s(patch, 1, "\x68", 1);
	memcpy_s(patch + 1, 4, &myFuncAddress, 4);
	memcpy_s(patch + 5, 1, "\xC3", 1);

	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookedAddress, patch, 6, NULL);
}

int main()
{
	WinExec("notepad", SW_SHOWDEFAULT);
	setMySuperHook(); 
	WinExec("notepad", SW_SHOWDEFAULT);
}