#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <tlhelp32.h>

int findMyProc(const char* procname)
{
	HANDLE         hSnapshot;
	PROCESSENTRY32 pe;
	int			   pid = 0;
	BOOL		   hResult; 

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return 0;

	pe.dwSize = sizeof(PROCESSENTRY32);
	hResult   = Process32First(hSnapshot, &pe);

	while(hResult)
	{
		if (strcmp(procname, (const char*)pe.szExeFile) == 0)
		{
			pid = pe.th32DefaultHeapID;
			break;
		}
		hResult = Process32Next(hSnapshot, &pe);
	}

	CloseHandle(hSnapshot);
	return pid;
}
int main(int argc, char* argv[])
{
	int pid = 0;
	pid = findMyProc(argv[1]);
	if (pid)
	{
		printf("PID = %d\n", pid);
	}
	return 0;
}




