// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "Windows.h"
#include "DefineTableService.h"
#include "PrototypeService.h"
#pragma warning(disable : 4996)

BEGIN_TABLESERVICE
ENTRYSERVICE("Echo", EchoServer),
ENTRYSERVICE("Time", TimeServer),
ENTRYSERVICE("Random", RandomServer),
END_TABLESERVICE;

extern "C" __declspec(dllexport) HANDLE  SSS(char* id, LPVOID prm)
{
    HANDLE rc = NULL;
    int  i = 0;
    while (i < SIZETS && strcmp(TABLESERVICE_ID(i), id) != 0)i++;
    if (i < SIZETS)
        rc = CreateThread(NULL, NULL,
            TABLESERVICE_FN(i), prm, NULL, NULL);
    return rc;
};

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

