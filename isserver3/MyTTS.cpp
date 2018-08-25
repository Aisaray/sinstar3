// MyTTS.cpp: implementation of the CMyTTS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyTTS.h"
#include <sphelper.h>

#define ASSERT SASSERT
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const TCHAR * KConfigIni = _T("config.ini");
const TCHAR * KTtsEntry = _T("tts");
const TCHAR * KTtsSpeed = _T("speed");
const TCHAR * KTtsChVoice = _T("ChVoice");
const TCHAR * KTtsEnVoice = _T("EnVoice");

CMyTTS::CMyTTS(LPCTSTR pszDataPath):m_bInitOK(FALSE), m_CurVoice(VOICE_NULL)
{
	m_strConfigIni = pszDataPath;
	m_strConfigIni += _T("\\");
	m_strConfigIni += KConfigIni;
	BeginThread();
}

CMyTTS::~CMyTTS()
{
	StopThread();
	::PostMessage(m_hWnd, WM_QUIT, 0, 0);// quit get message.
	JonThread();
}

//*****************************************
//	��ʼ��TTS���棬�ɹ�����TRUE,ʧ�ܷ���FALSE
//******************************************
BOOL CMyTTS::Init()
{
	CoInitialize(NULL);
	HWND hWnd = Create(_T("sinstar3_server_tts_host"), WS_POPUP, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL);
	ASSERT(hWnd);

	try
	{
		do {
			SComPtr<ISpObjectTokenCategory> cpSpCategory;
			HRESULT     hr;
			hr = m_cpVoiceEn.CoCreateInstance(CLSID_SpVoice);
			if (hr != S_OK) break;
			hr = m_cpVoiceCh.CoCreateInstance(CLSID_SpVoice);
			if (hr != S_OK) break;
			hr = SpGetCategoryFromId(SPCAT_VOICES, &cpSpCategory);
			if (hr != S_OK) break;
			hr = cpSpCategory->EnumTokens(L"Language=804", NULL, &m_cpChTokens);
			if (hr != S_OK) break;
			hr = cpSpCategory->EnumTokens(L"Language=9", NULL, &m_cpEnTokens);
			if (hr != S_OK) break;
			m_bInitOK = TRUE;

			m_iChVoice = GetPrivateProfileInt(KTtsEntry, KTtsChVoice, 0, m_strConfigIni);
			m_iEnVoice = GetPrivateProfileInt(KTtsEntry, KTtsEnVoice, 0, m_strConfigIni);
			m_nSpeed = GetPrivateProfileInt(KTtsEntry, KTtsSpeed, 0, m_strConfigIni);

			_SetSpeed(m_nSpeed);
			_SetVoice(TRUE, m_iChVoice);
			_SetVoice(FALSE, m_iEnVoice);

			SetMsgOwner(SPEI_END_INPUT_STREAM, m_hWnd, UM_TTS_FINISH);
		} while (0);

		if (!m_bInitOK)
		{
			Uninit();
		}
		return m_bInitOK;
	}catch(...)
	{
		Uninit();
		return FALSE;
	}
}

void CMyTTS::Uninit()
{
	WritePrivateProfileString(KTtsEntry, KTtsSpeed, SStringT().Format(_T("%d"),m_nSpeed),m_strConfigIni);
	WritePrivateProfileString(KTtsEntry, KTtsChVoice, SStringT().Format(_T("%d"), m_iChVoice), m_strConfigIni);
	WritePrivateProfileString(KTtsEntry, KTtsEnVoice, SStringT().Format(_T("%d"), m_iEnVoice), m_strConfigIni);

	m_cpVoiceEn = NULL;
	m_cpVoiceCh = NULL;
	m_cpChTokens = NULL;
	m_cpEnTokens = NULL;

	DestroyWindow();
	CoUninitialize();
}

void CMyTTS::SpeakWText(const WCHAR * pwcText,int nLen,BOOL bCh)
{
	if (nLen == -1) nLen = wcslen(pwcText);
	SStringW *pStr = new SStringW(pwcText,nLen);
	::PostMessage(m_hWnd, UM_FUN_SPEAK, bCh, (LPARAM)pStr);
}

void CMyTTS::_SpeakText(WPARAM wp, LPARAM lp)
{
	BOOL bCh = (BOOL)wp;
	SStringW *pStr = (SStringW*)lp;
	if (IsTTSBusy())
		_Stop();
	m_CurVoice = bCh ? VOICE_CH : VOICE_EN;
	HRESULT	hr = (m_CurVoice == VOICE_CH ? m_cpVoiceCh : m_cpVoiceEn)->Speak((*pStr), SPF_ASYNC | SPF_IS_NOT_XML, 0);
	delete pStr;
}

void CMyTTS::SpeakText(LPCSTR pszText,int nLen,BOOL bCh)
{
	if(nLen==-1) nLen=strlen(pszText);
	SStringA strA(pszText, nLen);
	SStringW strW = S_CA2W(strA);
	SpeakWText(strW, strW.GetLength(), bCh);
}


BOOL CMyTTS::IsTTSBusy()
{
	ASSERT(m_bInitOK);
	if(m_CurVoice==VOICE_NULL) return FALSE;
	SPVOICESTATUS  spVoiceStatus;
	(m_CurVoice==VOICE_CH?m_cpVoiceCh:m_cpVoiceEn)->GetStatus(&spVoiceStatus,NULL);
	BOOL bBusy=spVoiceStatus.dwRunningState!=SPRS_DONE;
	return bBusy;
}


int CMyTTS::GetVoice(BOOL bCh)
{
	return bCh?m_iChVoice:m_iEnVoice;
}


void CMyTTS::SetVoice(BOOL bCh, int nToken)
{
	::PostMessage(m_hWnd, UM_FUN_SETVOICE, (WPARAM)bCh, (LPARAM)nToken);
}

BOOL CMyTTS::_SetVoice(WPARAM wp, LPARAM lp)
{
	BOOL bCh = (BOOL)wp;
	int nToken = (int)lp;
	if (m_bInitOK) return FALSE;

	
	SComPtr<IEnumSpObjectTokens> pTokens = bCh ? m_cpChTokens : m_cpEnTokens;
	ISpObjectToken* pToken = NULL;
	ULONG count;
	HRESULT hr = pTokens->GetCount(&count);
	if (nToken < 0 || nToken >= count)
		return FALSE;

	hr = pTokens->Item(nToken, &pToken);
	if (hr != S_OK) 
		return FALSE;
	if (bCh)
	{
		hr = m_cpVoiceCh->SetVoice(pToken);
		m_iChVoice = nToken;
	}
	else
	{
		hr = m_cpVoiceEn->SetVoice(pToken);
		m_iEnVoice = nToken;
	}
	pToken->Release();
	return TRUE;
}

int CMyTTS::GetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize)
{
	return ::SendMessage(m_hWnd, UM_FUN_GETTOKENINFO, MAKEWPARAM(bCh,nBufSize), (LPARAM)token);
}

int CMyTTS::_GetTokensInfo(WPARAM wp,LPARAM lp)
{
	BOOL bCh = LOWORD(wp);
	int nBufSize = HIWORD(wp);

	wchar_t(*token)[MAX_TOKEN_NAME_LENGHT] = (wchar_t(*)[MAX_TOKEN_NAME_LENGHT])lp;

	if(!m_bInitOK) return 0;
	ULONG  count;
	SComPtr<IEnumSpObjectTokens> pTokens=bCh?m_cpChTokens:m_cpEnTokens;
	HRESULT hr=pTokens->GetCount(&count);
	if (token == NULL) return count;
	if(count==0) return 0;
	if (nBufSize < count) return 0;

	for(int i=0;i<(int)count;i++)
	{
		ISpObjectToken * pToken;
		hr=pTokens->Item(i,&pToken);
        CSpDynamicString dstrDesc;
        hr = SpGetDescription(pToken, &dstrDesc);
		pToken->Release();
		wcscpy(token[i], dstrDesc);
	}
	return count;
}


void CMyTTS::Stop()
{
	::PostMessage(m_hWnd, UM_FUN_STOP, 0, 0);
}

void CMyTTS::_Stop()
{
	if (m_CurVoice != VOICE_NULL)
	{
		(m_CurVoice == VOICE_CH ? m_cpVoiceCh : m_cpVoiceEn)->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
	}
}


void CMyTTS::SetMsgOwner(ULONGLONG ullEvent, HWND hWnd, UINT uMsg)
{
	_ASSERT(m_bInitOK);
	m_cpVoiceCh->SetInterest( ullEvent,ullEvent );
	m_cpVoiceCh->SetNotifyWindowMessage( hWnd, uMsg, 0, 0 );
	m_cpVoiceEn->SetInterest( ullEvent,ullEvent );
	m_cpVoiceEn->SetNotifyWindowMessage( hWnd, uMsg, 1, 0 );
}

int CMyTTS::GetSpeed()
{
	return m_nSpeed;
}

void CMyTTS::SetSpeed(int nSpeed)
{
	PostMessage(UM_FUN_SETSPEED, (WPARAM)nSpeed, 0);
}

void CMyTTS::_SetSpeed(WPARAM lp)
{
	if(!m_bInitOK) return;
	int nSpeed = (int)lp;
	m_cpVoiceEn->SetRate(nSpeed);
	m_cpVoiceCh->SetRate(nSpeed);
}


UINT CMyTTS::Run(LPARAM lp)
{
	if (!Init())
		return -1;
	
	MSG msg;
	while (!IsStoped())
	{
		if (GetMessage(&msg, 0, 0, 0))
		{
			DispatchMessage(&msg);
		}
	}
	Uninit();
	return 0;
}

LRESULT CMyTTS::OnTTSMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet= 1;
	switch (uMsg)
	{
	case UM_TTS_FINISH:
		STRACEA("UM_TTS_FINISH");
		break;
	case UM_FUN_SPEAK:
		_SpeakText(wParam, lParam);
		break;
	case UM_FUN_STOP:
		_Stop();
		break;
	case UM_FUN_SETVOICE:
		_SetVoice(wParam, lParam);
		break;
	case UM_FUN_SETSPEED:
		_SetSpeed(wParam);
		break;
	case UM_FUN_GETTOKENINFO:
		lRet = _GetTokensInfo(wParam, lParam);
		break;
	default:
		break;
	}
	return lRet;
}
