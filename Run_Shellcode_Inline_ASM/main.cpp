#include <windows.h>
#include <stdio.h>

int main()
{	
	printf("=^..^= meow meow\n");
	// Esto es sintaxis para gcc
	// asm(".byte 0x90,0x90,0x90,0x90\n\t"
	//	"ret \n\t");

	// El compilador de Microsoft solo soporta x86, de lo contrario no admite el inline assembly en x64 почему?
	_asm 
	{
		nop
		nop
		nop
		nop
	};

	return 0;
}