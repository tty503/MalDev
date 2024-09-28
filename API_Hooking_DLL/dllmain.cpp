// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C"
{
    __declspec(dllexport) int _cdecl Cat(LPCTSTR say)
    {
        MessageBox(NULL, say, (LPCTSTR)"=^..^=", MB_OK);
        return 1;
    }
}

extern "C"
{
    __declspec(dllexport) int _cdecl Mouse(LPCTSTR say)
    {
        MessageBox(NULL, say, (LPCTSTR)"<:3()~~", MB_OK);
        return 1;
    }
}

extern "C"
{
    __declspec(dllexport) int _cdecl Frog(LPCTSTR say)
    {
        MessageBox(NULL, say, (LPCTSTR)"<(-)", MB_OK);
        return 1;
    }
}

extern "C"
{
    __declspec(dllexport) int _cdecl Bird(LPCTSTR say)
    {
        MessageBox(NULL, say, (LPCTSTR)"8)~", MB_OK);
        return 1;
    }
}