#include "stdafx.h"
#include "CmdHandler.h"
#include "ui/STipWnd.h"

using namespace SOUI;

CCmdHandler::CCmdHandler():m_pTipWnd(NULL)
{
}


CCmdHandler::~CCmdHandler()
{
}

void CCmdHandler::SetTipWnd(STipWnd * pTipWnd)
{
	m_pTipWnd = pTipWnd;
}

void CCmdHandler::OnMakeWord(LPARAM lp)
{
	if (OpenClipboard(m_pTipWnd->m_hWnd))
	{
		HGLOBAL hglb = GetClipboardData(CF_TEXT);
		if (hglb)
		{
			LPSTR lpstr = (char *)GlobalLock(hglb);
			if (lpstr)
			{
				char szBuf[200];
				int len = strlen(lpstr);
				if (len < 127 && ISComm_MakePhrase(lpstr, (char)len) == ISACK_SUCCESS)
				{
					PMSGDATA pMsg = ISComm_GetData();
					if (pMsg->byData[0] == 1)
						sprintf(szBuf, "��\"%s\"�Ѿ�����", lpstr);
					else
						sprintf(szBuf, "��\"%s\"����ʿ�", lpstr);
				}
				else
				{
					sprintf(szBuf, "���\"%s\"ʧ��", lpstr);
				}
				SStringT msg = S_CA2T(szBuf);
				m_pTipWnd->SetTip(_T("���"), msg);
			}
			GlobalUnlock(hglb);
		}
		CloseClipboard();
	}
}
