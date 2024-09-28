#include <windows.h>
#include <string.h>

int reg_key_compare(HKEY hKeyRoot, char* lpSubKey, char* regVal, char* compare)
{
	HKEY	hKey = nullptr;
	LONG	ret;
	char    value[1024];
	DWORD   size = sizeof(value);

	ret = RegOpenKeyExA(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);
	if (ret == ERROR_SUCCESS)
	{
		RegQueryValueExA(hKey, regVal, NULL, NULL, (LPBYTE)value, &size);
		if (ret == ERROR_SUCCESS)
		{
			if (strcmp(value, compare) == 0)
			{
				return TRUE; 
			}
		}
	}
	return FALSE; 
}

int main(int argc, char* argv[])
{
	HKEY		hKey = NULL; 
	const char* exe  = "Z:\\Path\\name.exe";
	const char* ts   = "10";
	const char* aact = "1";

	// Start Up
	LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, (LPCSTR)"Control Panel\\Desktop", 0, KEY_WRITE, &hKey);
	if (res == ERROR_SUCCESS)
	{
		// Crear un nuevo registro 
		RegSetValueEx(hKey, (LPCSTR)"ScreenSaveActive", 0, REG_SZ, (unsigned char*)aact, strlen(aact));
		RegSetValueEx(hKey, (LPCSTR)"ScreenSaveTimeout", 0, REG_SZ, (unsigned char*)ts, strlen(ts));
		RegSetValueEx(hKey, (LPCSTR)"SCRNSAVE.EXE", 0, REG_SZ, (unsigned char*)exe, strlen(exe));
		RegCloseKey(hKey);
	}
	return 0;
}