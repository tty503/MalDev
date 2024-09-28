#include <windows.h>

typedef int(__cdecl* CatProc)(LPCTSTR say);
typedef int(__cdecl* BirdProc)(LPCTSTR say);

int main(void)
{
	HINSTANCE DLL;
	CatProc   catFunc;
	BirdProc  birdFunc;
	BOOL	  freeRes;

	DLL = LoadLibrary((LPCWSTR)"C:\\Users\\pc\\source\\repos\\MalDev\\x64\\Debug\\API_Hooking_DLL.dll");
	if (DLL != NULL)
	{
		catFunc  = (CatProc)GetProcAddress(DLL, "Cat");
		birdFunc = (BirdProc)GetProcAddress(DLL, "Bird");
		if ((catFunc != NULL) && (birdFunc != NULL))
		{
			(catFunc)((LPCTSTR)"MEOOOW");
			(birdFunc)((LPCTSTR)"Tweet");
		}
		freeRes = FreeLibrary(DLL);
	}
	return 0; 
}