#include <windows.h>
// Enlazamos la libreria necesaria para trabajar con spooler de impresion de Windows, add / gestion de monitores de puerto
#pragma comment(lib, "winspool")

int main(int argc, char* argv)
{
	// Informacion del monitor
	MONITOR_INFO_2 mi;
	mi.pName        = (LPWSTR)"Monitor";
	mi.pEnvironment = (LPWSTR)"Windows x64";
	mi.pDLLName     = (LPWSTR)"name.dll";
	AddMonitor(NULL, 2, (LPBYTE)&mi);
	return 0;
}