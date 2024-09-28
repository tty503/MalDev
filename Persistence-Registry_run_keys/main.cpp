#include <windows.h>
#include <string.h>

int main(int argc, char* argv[])
{
	HKEY hkey = NULL;
	const char* exe = "DISK:\\Path\\name.exe";

	// Start Up 
	LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, (LPCSTR)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hkey);
	if (res == ERROR_SUCCESS)
	{
		// Creamos un nuevo registro 
		RegSetValueEx(hkey, (LPCWSTR)"name", 0, REG_SZ, (unsigned char*)exe, strlen(exe));
		RegCloseKey(hkey);
	}
	return 0;
}