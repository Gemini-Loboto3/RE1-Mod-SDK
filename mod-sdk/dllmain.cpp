// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

void Modsdk_init(HMODULE hExe);
void Modsdk_close();

// standard entry point, do here your DLL initialization
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:	// initialize
		Modsdk_init(hModule);
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:	// deinitialize
		Modsdk_close();
        break;
    }
    return TRUE;
}

