// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "sinstar3_core.h"
#ifdef ENABLE_VLD
#include <vld.h>
#endif

CSinstar3Core	*theModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		theModule = new CSinstar3Core((HINSTANCE)hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		delete theModule;
		theModule = NULL;
		break;
	}
	return TRUE;
}

