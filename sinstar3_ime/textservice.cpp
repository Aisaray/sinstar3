#include "stdafx.h"
#include "UiWnd.h"
#include "CompStrEx.h"
#include "../include/sinstar3_guids.h"



BOOL CUiWnd::InputStringW(LPCWSTR pszBuf, int nLen)
{
	if (IsCompositing())
	{
		return FALSE;
	}
	LPVOID pCtx = GetImeContext();
	if (!pCtx) return FALSE;
	StartComposition(pCtx);
	UpdateResultAndCompositionStringW(pCtx, pszBuf, nLen, NULL, 0);
	EndComposition(pCtx);
	ReleaseImeContext(pCtx);
	return TRUE;
}

BOOL CUiWnd::IsCompositing() const
{
	return m_bCompositing;
}

void CUiWnd::StartComposition(LPVOID lpImeContext)
{
	if (!lpImeContext) return;
	if(!IsCompositing())
	{
		CImeContext *pCtx=(CImeContext *)lpImeContext;
		CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
		_ASSERT(pCompStr);
		ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
		pCtx->GenerateMessage(WM_IME_STARTCOMPOSITION,0,0);
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_SETCOMPOSITIONWINDOW,0);
		if(m_pSinstar3) m_pSinstar3->OnCompositionStarted();
		m_bCompositing = TRUE;
	}
}

//ʹ��ָ�������滻��ǰ���봮
// nLeft,nRight: [-1,-1]:�ڵ�ǰ���λ�ò���
//				 [0,-1]:�滻ȫ������
//				 [left,right]:�滻�÷�Χ��������Ϊ��ֵ
void CUiWnd::ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)
{
	if (!lpImeContext) return;
#ifdef _UNICODE
	_ASSERT(m_pSinstar3);
	if(!IsCompositing()) StartComposition(lpImeContext);
	CImeContext *pCtx=(CImeContext *)lpImeContext;
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	if(pCompStr)
	{
		pCompStr->Insert(m_pSinstar3,nLeft,nRight,wszComp,nLen);
		pCtx->GenerateMessage(WM_IME_COMPOSITION,0,GCS_CURSORPOS|GCS_COMP|GCS_COMPREAD);
		if(m_pSinstar3)
		{
			m_pSinstar3->OnCompositionChanged();
			if(pCompStr->GetTargetPos()==-1)
			{
				POINT pt={-1,-1};
				m_pSinstar3->OnSetFocusSegmentPosition(pt,0);
			}
		}
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_CHANGECANDIDATE,0);
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_OPENCANDIDATE,0);

		if(pCompStr->dwCompStrLen==0) 	EndComposition(lpImeContext);

	}else
	{
		SLOGFMTI("ReplaceSelCompositionW: lock compstr failed");
	}
	ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
#else
	#error "have not been implemented ReplaceSelCompositionA"
#endif//_UNICODE

}

void CUiWnd::UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{
	if (!lpImeContext) return;
#ifdef _UNICODE
	_ASSERT(m_pSinstar3);
	if(!IsCompositing()) StartComposition(lpImeContext);
	CImeContext *pCtx=(CImeContext *)lpImeContext;
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	if(pCompStr)
	{
		pCompStr->Init();
		pCompStr->Insert(m_pSinstar3,0,-1,wszCompStr,nCompStrLen);
		pCompStr->dwResultStrLen=nResStrLen;
		_tcsncpy(pCompStr->szResultStr,wszResultStr,nResStrLen);
		pCtx->GenerateMessage(WM_IME_COMPOSITION,0,GCS_CURSORPOS|GCS_COMP|GCS_COMPREAD|GCS_RESULTSTR);
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_CHANGECANDIDATE,0);
		if(m_pSinstar3)
		{
			m_pSinstar3->OnCompositionChanged();
			POINT pt={-1,-1};
			m_pSinstar3->OnSetFocusSegmentPosition(pt,0);
		}
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_SETCOMPOSITIONWINDOW,0);
		ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
	}else
	{
		SLOGFMTI("ReplaceSelCompositionW: lock compstr failed");
	}
#else
	SLOGFMTI("not implemented ReplaceSelCompositionA");
#endif//_UNICODE
}

void CUiWnd::EndComposition(LPVOID lpImeContext)
{
	if (!lpImeContext) return;
	CImeContext *pCtx = (CImeContext *)lpImeContext;
	CCompStrEx *pCompStr = (CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	if (pCompStr && pCompStr->dwCompStrLen)
	{
		TCHAR szResult[MAXCOMPSIZE * 2];
		DWORD dwResStrLen = pCompStr->dwCompStrLen;
		_tcsncpy(szResult, pCompStr->szCompStr, pCompStr->dwCompStrLen);
		pCompStr->Init();
		_tcsncpy(pCompStr->szResultStr, szResult, dwResStrLen);
		pCompStr->dwResultStrLen = dwResStrLen;
		pCtx->GenerateMessage(WM_IME_COMPOSITION, 0, GCS_RESULTSTR | GCS_COMP);
	}
	ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
	pCtx->GenerateMessage(WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 0);
	pCtx->GenerateMessage(WM_IME_ENDCOMPOSITION, 0, 0);
	m_bCompositing = FALSE;
}


LPVOID CUiWnd::GetImeContext()
{
	if(m_pCurContext) return m_pCurContext;
	CImeContext *pContext=new CImeContext;
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	if(pContext->Init(hIMC)) return pContext;
	delete pContext;
	return NULL;
}

BOOL   CUiWnd::ReleaseImeContext(LPVOID lpImeContext)
{
	if(m_pCurContext == (CImeContext*)lpImeContext) return TRUE;
	if(!lpImeContext) return FALSE;
	delete (CImeContext*)lpImeContext;
	return TRUE;
}

void  CUiWnd::SetConversionMode(EInputMethod mode)
{
	CImeContext *pCtx=(CImeContext *)GetImeContext();
	if(pCtx)
	{
		DWORD dwMode=0;
		switch( mode)
		{
		case HalfAlphanumeric:
			dwMode = IME_CMODE_ROMAN;						
			break;

		case HalfNative:
			dwMode = IME_CMODE_ROMAN | IME_CMODE_NATIVE;	
			break;

		case FullNative:
			dwMode = IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE;
			break;

		}
		//���ȱ�֤����״̬ͬ��
		if(mode != HalfAlphanumeric)
		{
			if(!GetOpenStatus(pCtx)) SetOpenStatus(pCtx,TRUE);
			ImmSetConversionStatus(pCtx->_hIMC,dwMode,0);
		}else
		{
			if(GetOpenStatus(pCtx)) SetOpenStatus(pCtx,FALSE);
		}
	}
	ReleaseImeContext(pCtx);
}

EInputMethod CUiWnd::GetConversionMode()
{
	EInputMethod eInputMode =m_pSinstar3?m_pSinstar3->GetDefInputMode():FullNative;

	CImeContext *pCtx=(CImeContext *)GetImeContext();
	if(!pCtx) return HalfAlphanumeric;
	DWORD dwMode=0,dwSent=0;
	ImmGetConversionStatus(pCtx->_hIMC,&dwMode,&dwSent);
	ReleaseImeContext(pCtx);

	switch( dwMode)
	{
	case IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE| IME_CMODE_NATIVE:	
		eInputMode = FullNative;
		break;

	case IME_CMODE_ROMAN| IME_CMODE_NATIVE:	
		eInputMode = HalfNative;
		break;

	case IME_CMODE_ROMAN:																				// 0x10
		eInputMode = HalfAlphanumeric;
		break;
	}
	return eInputMode;
}

BOOL CUiWnd::SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)
{
	if(!lpImeContext) return FALSE;
	return ImmSetOpenStatus(((CImeContext *)lpImeContext)->_hIMC,bOpen);
}

BOOL CUiWnd::GetOpenStatus(LPVOID lpImeContext) const
{
	if(!lpImeContext) return FALSE;
	return ImmGetOpenStatus(((CImeContext *)lpImeContext)->_hIMC);
}
