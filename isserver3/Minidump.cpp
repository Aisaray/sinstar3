#include "stdafx.h"
#include "Minidump.h"
#include <windows.h>
#include <DbgHelp.h>
#include <ShellAPI.h>
#pragma comment(lib, "DbgHelp.lib")

static TCHAR s_dumpFile[MAX_PATH]=_T("minidump.dmp");
static TCHAR s_crashReporter[MAX_PATH]={0};
// ����Dump�ļ�  
//   
void CreateDumpFile(LPCTSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// ����Dump�ļ�  
	//  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Dump��Ϣ  
	//  
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = FALSE;

	// д��Dump�ļ�����  
	//  
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);
	ShellExecute(NULL,_T("open"),s_crashReporter,NULL,NULL,SW_SHOWNORMAL);
}

// ����Unhandled Exception�Ļص�����  
//  
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	// ���ﵯ��һ������Ի����˳�����  
	//  
	//FatalAppExit(-1, _T("*** Unhandled Exception! ***"));
	CreateDumpFile(s_dumpFile, pException);
	return EXCEPTION_EXECUTE_HANDLER;
}

void CMinidump::Enable()
{
	// ���ô���Unhandled Exception�Ļص�����  
	//   
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
}

void CMinidump::SetDumpFile(LPCTSTR pszDumpFile)
{
	_tcscpy_s(s_dumpFile,MAX_PATH,pszDumpFile);
}

void CMinidump::SetCrashReporter(LPCTSTR pszCrashReporter)
{
	_tcscpy_s(s_crashReporter,MAX_PATH,pszCrashReporter);
}
