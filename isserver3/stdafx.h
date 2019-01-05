// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif//_CRT_SECURE_NO_WARNINGS

#define  _CRT_SECURE_NO_WARNINGS
// Windows ͷ�ļ�:
#include <windows.h>
#include <BaseTyps.h>
#include <Unknwn.h>
#include <tchar.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "resource.h"

#include "../include/sinstar-i.h"
#include "../include/TextService-i.h"
#include "../include/unknown.h"

#define LOG_FILTER "sinstar3_core"

#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <event/NotifyCenter.h>
#include <commgr2.h>
#include <helper/SMenu.h>
#include <resprovider-zip/zipresprovider-param.h>

#include "resource.h"
#define R_IN_CPP	//�������������
#include "res\resource.h"
using namespace SOUI;

#include "../include/reg.h"
#include "../iscomm/isProtocol.h"
#include "../iscomm/iscomm.h"

#include "global_def.h"

#include "ime/inputContext.h"
#include "ime/CmdHandler.h"

#include "controls/SChatEdit.h"
#include "controls/SToggle2.h"
#include "controls/SCandView.h"
#include "controls/SSentView.h"
#include "controls/SEnglishCand.h"
#include "controls/SPhraseCand.h"
#include "controls/SCaption2.h"
#include "controls/SCompBackground.h"
#include "controls/SStatusBackground.h"
#include "controls/SMutexView.h"
#include "controls/SSpellView.h"
#include "controls/SFlagView.h"
#include "controls/SLineComp.h"
#include "controls/SApngPlayer.h"

#include "controls/SSkinAPNG.h"
#include "controls/SSkinPNGX.h"
#include "controls/SLineSkin.h"
#include "controls/SQrCtrl.h"
#include "controls/STabCtrlEx.h"

#include "dataCenter/datacenter.h"
#include "dataCenter/Settings.h"


extern SComMgr2 * g_ComMgr2;