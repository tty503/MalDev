#include <windows.h>
typedef int(__cdecl* CatProc)(LPCTSTR say);

char originalBytes[5];
FARPROC hookedAddress;

int __stdcall myFunc(LPCTSTR say)
{
	HINSTANCE DLL;
	CatProc   catFunc;

	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookedAddress, originalBytes, 5, NULL);

	DLL     = LoadLibrary((LPCWSTR)"C:\\Users\\pc\\source\\repos\\MalDev\\x64\\Debug\\API_Hooking_DLL.dll");
	catFunc = (CatProc)GetProcAddress(DLL, "Cat");

	return (catFunc)((LPCWSTR)"Meow!!!");
}

void setMySuperHook()
{
	HINSTANCE	hLib;
	VOID*       myFuncAddress;
	DWORD*		rOffset;
	DWORD		src; 
	DWORD		dst;
	CHAR		patch[5] = {0};
	
	hLib          = LoadLibraryA((LPCWSTR)"C:\\Users\\pc\\source\\repos\\MalDev\\x64\\Debug\\API_Hooking_DLL.dll");
	hookedAddress = GetProcAddress(hLib, "Cat");

	ReadProcessMemory(GetCurrentProcess(), (LPCVOID)hookedAddress, originalBytes, 5, NULL);

	myFuncAddress = &myFunc; 

	src     = (DWORD)hookedAddress + 5;
	dst     = (DWORD)myFuncAddress;
	rOffset = (DWORD*)(dst - src);

	// \xE9 - instruccion jump 
	memcpy(patch, "\xE9", 1);
	memcpy(patch + 1, &rOffset, 4);

	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookedAddress, patch, 5, NULL);
}

int main()
{
	HINSTANCE DLL; 
	CatProc   catFunc;

	DLL = LoadLibrary((LPCWSTR)"C:\\Users\\pc\\source\\repos\\MalDev\\x64\\Debug\\API_Hooking_DLL.dll");
	catFunc = (CatProc)GetProcAddress(DLL, "Cat");

	(catFunc)((LPCWSTR)"Meoooow");

	setMySuperHook();

	(catFunc)((LPCWSTR)"meow mewo");
}