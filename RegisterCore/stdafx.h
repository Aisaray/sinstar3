// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
// Windows ͷ�ļ�:
#include <windows.h>
#include <tchar.h>
#include <ShellAPI.h>
#include <malloc.h>
#include "../include/reg.h"
//copy from ../include/protocol.h
#define SINSTAR3_SERVER_HWND _T("sinstar3_server_wnd_{85B55CBC-7D48-4860-BA88-0BE4B073A94F}")
#define SINSTAR3_SHARE_BUF_NAME_FMT _T("sistart3_share_buffer_8085395F-E2FA-4F96-8BD0-FE5D7412CD22_%08x_2_%08x")
#define SINSTAR3_MUTEX _T("sinstar3_mutex_40AC2365-F592-4BE3-8B41-04A6B5337BFB")

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
