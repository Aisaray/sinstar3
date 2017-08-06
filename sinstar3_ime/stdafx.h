// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
#define ENABLE_LANGUAGEBAR

// Windows ͷ�ļ�:
#define NOIME
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>

#include "imm.h"
#pragma comment(lib,"imm32.lib")

#include "imeContext.h"

#include "../include/version.h"
#include "../include/sinstar-i.h"

#include "../core_loader/core_loader.h"

extern HINSTANCE g_hInst;
extern DWORD g_dwSystemInfoFlags;

void Helper_Trace(LPCTSTR pszFormat,...);
