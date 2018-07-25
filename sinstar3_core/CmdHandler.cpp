#include "stdafx.h"
#include "CmdHandler.h"
#include "Sinstar3Impl.h"
#include "ui/STipWnd.h"
#include "Utils.h"

using namespace SOUI;
#define MAX_UDICTKEY	40	//���ؼ��ֳ���
#define MAX_UDICTPHRASE	1000

CCmdHandler::CCmdHandler(CSinstar3Impl * pSinstar3)
	:m_pSinstar3(pSinstar3)
	,m_pTipWnd(NULL)
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
	WCHAR szBuf[MAX_INPUT];
	int nRet =CUtils::GetClipboardText(m_pSinstar3->m_hWnd, szBuf, MAX_INPUT);
	if (nRet > 0)
	{
		SStringA str = S_CW2A(SStringW(szBuf, nRet));
		char szMsg[300];
		if (str.GetLength()<127 && ISComm_MakePhrase(str, (char)str.GetLength()) == ISACK_SUCCESS)
		{
			PMSGDATA pMsg = ISComm_GetData();
			if (pMsg->byData[0] == 1)
				sprintf(szMsg, "��\"%s\"�Ѿ�����", (LPCSTR)str);
			else
				sprintf(szMsg, "��\"%s\"����ʿ�", (LPCSTR)str);
		}
		else
		{
			sprintf(szMsg, "���\"%s\"ʧ��", (LPCSTR)str);
		}
		SStringT msg = S_CA2T(szMsg);
		m_pTipWnd->SetTip(_T("���"), msg);
	}
}

void CCmdHandler::OnKeyMap(LPARAM lp)
{
	ISComm_SendMsg(CT_SHOWKEYMAP, 0, 0, 0);
}

void CCmdHandler::OnHideStatusBar(LPARAM lp)
{
	g_SettingsL.bHideStatus = !g_SettingsL.bHideStatus;
	m_pSinstar3->m_pStatusWnd->Show(!g_SettingsL.bHideStatus);
}

void CCmdHandler::OnQueryInfo(LPARAM lp)
{
	WCHAR szBuf[MAX_INPUT] = { 0 };
	int  nGet = CUtils::GetClipboardText(m_pSinstar3->m_hWnd, szBuf, MAX_INPUT);
	if(nGet==0)
	{
		InputContext *pCtx = m_pSinstar3->m_inputState.GetInputContext();
		if (pCtx->cInput)
		{
			SStringT strInput(pCtx->szInput, pCtx->cInput);
			SStringW strInputW = S_CT2W(strInput);
			wcscpy(szBuf, strInputW);
			nGet = strInputW.GetLength();
		}
	}
	if (nGet)
	{
		SStringA strBuf = S_CW2A(szBuf);
		char szRet[128 + 300 + 300 + MAX_UDICTPHRASE] = { 0 };//�ؼ���+ƴ��+���+�û��ʵ�
		char *p = szRet;
		if (strBuf[0] & 0x80)
		{//���Ĺؼ��ʣ����Բ�ѯ��������
			p += sprintf(p, "�ؼ���=%s", (LPCSTR)strBuf);
			if (ISComm_QueryComp(strBuf, nGet) == ISACK_SUCCESS)
			{
				PMSGDATA pData = ISComm_GetData();
				pData->byData[pData->sSize] = 0;
				p += sprintf(p, "\r\n%s=%s", ISComm_GetCompInfo()->szName, (char*)pData->byData);
			}
			else
			{
				p += sprintf(p, "\r\n%s=��ѯʧ��", ISComm_GetCompInfo()->szName);
			}

			if (ISComm_SpellQueryComp(strBuf, nGet) == ISACK_SUCCESS)
			{
				PMSGDATA pData = ISComm_GetData();
				short i, sCount = 0;
				BYTE *pbyData = pData->byData;
				p += sprintf(p, "\r\nƴ��=");
				memcpy(&sCount, pbyData, 2);
				pbyData += 2;
				if (sCount>10) sCount = 10;//ֻȡǰ��10��ƴ��
				for (i = 0; i<sCount; i++)
				{
					char cSize = pbyData[0];
					pbyData++;
					memcpy(p, pbyData, cSize);
					pbyData += cSize;
					p += cSize;
					*p++ = ' ';
				}
				*p = 0;
			}
			else
			{
				p += sprintf(p, "\r\nƴ��=��ѯʧ��");
			}
		}
		else
		{//��ѯӢ�ĵ���
			p += sprintf(p, "\r\n����=%s", (LPCSTR)strBuf);
			if (ISComm_En2Ch(strBuf, nGet) == ISACK_SUCCESS)
			{
				PMSGDATA pData = ISComm_GetData();
				LPBYTE pbyData = pData->byData;
				BYTE i = 0, byItems = *pbyData++;
				pbyData += pbyData[0] + 1;//skip phonetic
				p += sprintf(p, "\r\n��������");
				while (i<byItems)
				{
					p += sprintf(p, "\r\n\t%d:", i + 1);
					memcpy(p, pbyData + 1, pbyData[0]); p += pbyData[0]; pbyData += pbyData[0] + 1;
					i++;
				}
				*p = 0;
			}
			else
			{
				p += sprintf(p, "\r\n���Ĵʿ��ѯʧ��!");
			}
		}
		if (ISComm_QueryUserDict(strBuf, nGet) == ISACK_SUCCESS)
		{//��ѯ�û��ʵ�ɹ�
			PMSGDATA pData = ISComm_GetData();
			if (pData->sSize<1000 + 100)//�ؼ��ʳ������100��������󳤶�1000
			{
				char *pbuf = (char*)pData->byData;
				p += sprintf(p, "\r\n�û��ʵ�[%s]:%s", ISComm_GetUserDict(), pbuf);
				pbuf += strlen(pbuf) + 1;
				p += sprintf(p, "\r\n\t%s", pbuf);
				if (OpenClipboard(m_pSinstar3->m_hWnd))
				{//���û��ʵ����ݱ��浽������
					int nLen = strlen(pbuf);
					HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, nLen + 1);
					LPSTR pMem = (LPSTR)GlobalLock(hMem);
					strcpy(pMem, pbuf);
					GlobalUnlock(hMem);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, hMem);
					CloseClipboard();
				}
			}
		}
		SStringT msg = S_CA2T(szRet);
		m_pTipWnd->SetTip(_T("��ѯ"), msg);
	}
	else
	{
		CUtils::SoundPlay(_T("error"));
	}
}
