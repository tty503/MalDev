// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  nReason,
                       LPVOID lpReserved
                     )
{
    switch (nReason)
    {
    case DLL_PROCESS_ATTACH:
        MessageBox(NULL, (LPCWSTR)L"Meow from DLL! by ProcessAttach", (LPCWSTR)L"=^..^=", MB_OK);
        break;
    case DLL_THREAD_ATTACH:
        MessageBox(NULL, (LPCWSTR)L"Meow from DLL! by ThreadAttach", (LPCWSTR)L"=^..^=", MB_OK);
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

