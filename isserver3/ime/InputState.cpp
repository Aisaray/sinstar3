#include "StdAfx.h"
#include "InputState.h"
#include "Utils.h"
#include <ShellAPI.h>

#pragma warning(disable:4311 4302)
static const BYTE KCompKey[] ={0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,        // 00-0F
0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,        // 10-1F
1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,        // 20-2F
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,        // 30-3F
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,        // 40-4F
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,        // 50-5F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 60-6F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 70-7F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 80-8F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 90-9F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // A0-AF
0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,        // B0-BF
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // C0-CF
0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,        // D0-DF
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // E0-EF
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};       // F0-FF


static const int HKI_LoadDebugSkin	 = -2;
static const int HKI_UnloadDebugSkin = -3;
//���Ŵ���
//BYTE byInput:��������
SStringA Symbol_Convert(InputContext * lpCntxtPriv,UINT byInput,const BYTE * lpbKeyState)
{
	char pBuf[100];
	int nRet=0;
	if(g_SettingsUI->bCharMode)
	{
		char cType=3;
		if(byInput=='\'')
		{
			static BOOL bLeft1=TRUE;
			cType=bLeft1?1:2;
			bLeft1=!bLeft1;
		}else if(byInput=='\"')
		{
			static BOOL bLeft2=TRUE;
			cType=bLeft2?1:2;
			bLeft2=!bLeft2;
		}
		if(ISComm_SymbolConvert((char)byInput,cType)==ISACK_SUCCESS)
		{
			PMSGDATA pMsg=ISComm_GetData();
			if(pMsg->sSize<20)
			{
				nRet=pMsg->sSize-1;
				strcpy(pBuf,(char*)pMsg->byData);
				if(strncmp(pBuf+nRet-5,"$left",5)==0)
				{//���͹���ƶ�����
					nRet-=5;
					pBuf[nRet]=0;
					if(lpbKeyState[VK_SHIFT]&0x80) 	keybd_event(VK_SHIFT,MapVirtualKey(VK_SHIFT,0),KEYEVENTF_KEYUP,0);
					keybd_event(VK_LEFT,MapVirtualKey(VK_LEFT,0),0,0);
					keybd_event(VK_LEFT,MapVirtualKey(VK_LEFT,0),KEYEVENTF_KEYUP,0);
				}
			}
		}
	}
	if(!nRet)
	{
		pBuf[0]=byInput;
		nRet=1;
	}

	return SStringA(pBuf,nRet);
}

BOOL IsDigitEx(char c)
{
	if(isdigit(c)) return TRUE;
	return c=='.';
}

//�жϵ�ǰ״̬�����ּ����ǲ����нϱ���,�Զ���״̬���ʻ�����״̬��֧�����ּ��̵�������
BOOL KeyIn_IsNumCode(InputContext * lpCntxtPriv)
{
	return (lpCntxtPriv->inState == INST_USERDEF
		&& ((lpCntxtPriv->cComp>0 && IsDigitEx(lpCntxtPriv->szComp[0])) || lpCntxtPriv->cComp == 0)
		);
}

BOOL KeyIn_Code_IsMaxCode(InputContext * lpCntxtPriv,BYTE byType)
{
	if(lpCntxtPriv->cComp==0) return FALSE;
	return (lpCntxtPriv->cComp>=ISComm_GetCompInfo()->cCodeMax &&  byType==MCR_NORMAL);
}

BOOL KeyIn_Code_IsValidComp(InputContext * lpCntxtPriv,char cInput)
{
	BYTE byMask=0;
	if(lpCntxtPriv->cComp==MAX_COMP-1) return FALSE;
	if(g_SettingsG->bBlendUD) byMask|=MQC_USERDEF;
	if(g_SettingsG->bBlendSpWord) byMask|=MQC_SPCAND;
	lpCntxtPriv->szComp[lpCntxtPriv->cComp]=cInput;
	return ISComm_CheckComp(lpCntxtPriv->szComp,lpCntxtPriv->cComp+1,byMask);
}

//���һ�������Ƿ�Ϊ�������ַ��ͷ
BOOL KeyIn_IsWebHeader(const char *pszHeader,int nLen)
{
	BOOL bRet=FALSE;
	int i=0,iBegin=0,iEnd;
	while(g_SettingsG->szWebHeader[i])
	{
		iEnd=iBegin;
		for(;;)
		{
			if(g_SettingsG->szWebHeader[iEnd]==0) break;
			if(g_SettingsG->szWebHeader[iEnd]==0x20) break;
			iEnd++;
		}
		if(nLen==iEnd-iBegin && strncmp(pszHeader,g_SettingsG->szWebHeader+iBegin,nLen)==0)
		{
			bRet=TRUE;
			break;
		}
		if(g_SettingsG->szWebHeader[iEnd]==0) break;//������ַ�������
		iBegin=iEnd+1;
	}
	return bRet;
}

BOOL KeyIn_IsCoding(InputContext * lpCntxtPriv)
{
	BOOL bOpen=FALSE;
	if(lpCntxtPriv->inState==INST_CODING)
	{//��������
		if(lpCntxtPriv->sbState==::SBST_NORMAL)
		{
			if(lpCntxtPriv->compMode==IM_SHAPECODE)
			{//����
				bOpen=lpCntxtPriv->cComp!=0;
			}else//if(lpCntxtPriv->compMode==IM_SPELL)
			{//ƴ��
				bOpen=(lpCntxtPriv->bySyllables>1 || lpCntxtPriv->spellData[0].bySpellLen>0);
				if(g_SettingsG->compMode==IM_SHAPECODE) bOpen=TRUE;
			}
		}else if(lpCntxtPriv->sbState==SBST_SENTENCE)
			bOpen=TRUE;
	}else if(lpCntxtPriv->inState==INST_ENGLISH)
	{//Ӣ������
		bOpen=lpCntxtPriv->cComp!=0;
	}else if(lpCntxtPriv->inState==INST_USERDEF || lpCntxtPriv->inState==INST_LINEIME)
	{
		bOpen=TRUE;
	}
	return bOpen;
}

CInputState::CInputState(void)
:m_pListener(NULL)
,m_fOpen(FALSE)
,m_bUpdateTips(TRUE)
,m_bPressOther(FALSE)
,m_bPressShift(FALSE)
,m_bPressCtrl(FALSE)

{
	memset(&m_ctx,0,sizeof(InputContext));
	ClearContext(CPC_ALL);
	m_ctx.compMode  = (COMPMODE)g_SettingsG->compMode;
	m_ctx.bShowTip = FALSE;
	m_pbyMsgBuf=(LPBYTE)malloc(MAX_BUF_ACK);
}

CInputState::~CInputState(void)
{
	free(m_pbyMsgBuf);
}

int CInputState::TestHotKey(UINT uVk, const BYTE * lpbKeyState) const
{
	if (uVk == VK_CONTROL || uVk == VK_SHIFT || uVk == VK_MENU)
		return -1;
	if (!m_pListener->GetOpenStatus() || lpbKeyState[VK_CAPITAL]&0x01)
		return -1;
	int iRet = -1;
	for (int i = 0; i < HKI_COUNT; i++)
	{
		DWORD dwHotkey = g_SettingsG->dwHotkeys[i];
		WORD wModifier = HIWORD(dwHotkey);
		WORD wVk = LOWORD(dwHotkey);
		if (wVk == uVk)
		{
			if (((wModifier & MOD_ALT)!=0) ^ ((lpbKeyState[VK_MENU] & 0x80)!=0))
				continue;
			if (((wModifier & MOD_CONTROL) != 0) ^ ((lpbKeyState[VK_CONTROL] & 0x80) != 0))
				continue;
			if (((wModifier & MOD_SHIFT) != 0) ^ ((lpbKeyState[VK_SHIFT] & 0x80) != 0))
				continue;
			iRet = i;
			break;
		}
	}
	if(iRet != -1)
	{
		if (m_ctx.cComp > 0 || m_ctx.inState == INST_USERDEF)
			iRet = -1;
		if(iRet == HKI_UDMode && (g_SettingsG->compMode == IM_SPELL || IsTempSpell()))
			iRet = -1;
	}else//iRet == -1
	{
		if((lpbKeyState[VK_CONTROL] & 0x80) && (lpbKeyState[VK_MENU] & 0x80) && g_SettingsG->bEnableDebugSkin)
		{
			if(uVk == 'F')
			{//load debug skin
				return HKI_LoadDebugSkin;
			}else if(uVk='U')
			{
				return HKI_UnloadDebugSkin;
			}
		}
		
		if(m_ctx.sCandCount && ((uVk>='0' && uVk<='9')||(uVk>=VK_NUMPAD0 && uVk<=VK_NUMPAD9)))
		{//number
			if(lpbKeyState[VK_CONTROL] & 0x80)
			{
				iRet = (lpbKeyState[VK_SHIFT] & 0x80)?HKI_DelCandidate:HKI_AdjustRate;
			}
		}
	}

	return iRet;
}

void CInputState::Tips_Init()
{
	if (m_bUpdateTips)
	{
		for (int i = 0; i < TT_COUNT; i++)
		{
			m_tips[i].RemoveAll();
		}
		pugi::xml_document xmlTips;
		const wchar_t * groups[] = {
			L"all",L"spell",L"shape"
		};
		if (xmlTips.load_file(CDataCenter::getSingletonPtr()->GetDataPath() + _T("\\data\\tips.xml")))
		{
			pugi::xml_node tips = xmlTips.child(L"tips");
			for (int i = 0; i < 3; i++)
			{
				pugi::xml_node tip = tips.child(groups[i]).child(L"tip");
				while (tip)
				{
					m_tips[i].Add(S_CW2A(tip.attribute(L"value").as_string()));
					tip = tip.next_sibling(L"tip");
				}

			}
		}
		m_bUpdateTips = FALSE;
	}
}

int CInputState::Tips_Rand(BOOL bSpell, char *pszBuf)
{
	Tips_Init();
	if (bSpell)
	{
		int total = (int) (m_tips[TT_SPELL].GetCount() + m_tips[TT_BOTH].GetCount());
		int idx = rand() % total;
		if (idx < (int)m_tips[TT_SPELL].GetCount())
			strcpy(pszBuf, m_tips[TT_SPELL][idx]);
		else
			strcpy(pszBuf, m_tips[TT_BOTH][idx - m_tips[TT_SPELL].GetCount()]);
		return idx;
	}
	else
	{
		int total = (int) (m_tips[TT_SHAPE].GetCount() + m_tips[TT_BOTH].GetCount());
		int idx = rand() % total;
		if (idx < (int)m_tips[TT_SHAPE].GetCount())
			strcpy(pszBuf, m_tips[TT_SHAPE][idx]);
		else
			strcpy(pszBuf, m_tips[TT_BOTH][idx - m_tips[TT_SHAPE].GetCount()]);
		return idx;
	}
}

int CInputState::Tips_Next(BOOL bSpell,char *pszBuf, int iTip, bool bNext)
{
	Tips_Init();
	int idx = 0;
	if (bSpell)
	{
		int total = (int)(m_tips[TT_SPELL].GetCount() + m_tips[TT_BOTH].GetCount());
		if (iTip == -1)
		{
			idx = bNext ? 0 : total - 1;
		}
		else
		{
			idx = bNext ? (iTip + 1) : (iTip - 1);
		}
		idx = (idx+total)%total;
		if (idx < (int)m_tips[TT_SPELL].GetCount())
			strcpy_s(pszBuf, 200,m_tips[TT_SPELL][idx]);
		else
			strcpy_s(pszBuf, 200,m_tips[TT_BOTH][idx - m_tips[TT_SPELL].GetCount()]);
		return idx;
	}
	else
	{
		int total = (int)(m_tips[TT_SHAPE].GetCount() + m_tips[TT_BOTH].GetCount());
		if (iTip == -1)
		{
			idx = bNext ? 0 : total - 1;
		}
		else
		{
			idx = bNext ? (iTip + 1) : (iTip - 1);
		}
		idx = (idx+total)%total;
		if (idx < (int)m_tips[TT_SHAPE].GetCount())
			strcpy_s(pszBuf,200, m_tips[TT_SHAPE][idx]);
		else
			strcpy_s(pszBuf, 200,  m_tips[TT_BOTH][idx - m_tips[TT_SHAPE].GetCount()]);
	}
	SLOG_INFO("iTip:" << iTip << " bNext:" << bNext<<" idx:"<<idx);
	return idx;
}

void CInputState::GetShapeComp(const char *pInput,char cLen)
{
	if(ISComm_QueryComp(pInput,cLen)==ISACK_SUCCESS)
	{
		PMSGDATA pData=ISComm_GetData();
		pData->byData[pData->sSize]=0;
		sprintf(m_ctx.szTip,"��\"%s\"�ı���=%s",pInput,(char*)pData->byData);
	}else
	{
		sprintf(m_ctx.szTip,"��ѯ��\"%s\"�ı���ʧ��",pInput);
	}
}

#define MKI_ALL (MKI_RECORD|MKI_TTSINPUT|MKI_AUTOPICK)
BYTE CInputState::GetKeyinMask(BOOL bAssociate,BYTE byMask)
{
	BYTE byRet=0;
	if(g_SettingsG->bAutoMatch) byRet|=(MKI_AUTOPICK&byMask);
	if(g_SettingsUI->bRecord) byRet|=(MKI_RECORD&byMask);
	if(g_SettingsUI->bSound) byRet|=(MKI_TTSINPUT&byMask);
	if(bAssociate)
	{
		if(g_SettingsUI->bRecord) byRet|=MKI_ASTSENT;
		switch(g_SettingsG->byAstMode)
		{
		case AST_CAND:byRet |= MKI_ASTCAND; break;
		case AST_ENGLISH:byRet|=MKI_ASTENGLISH;break;
		case AST_PHRASEREMIND:byRet|=MKI_PHRASEREMIND;break;
		default:break;
		}
	}
	return byRet;
}

void CInputState::ClearContext(UINT dwMask)
{
	SLOG_INFO("dwMask:"<<dwMask);
	if(dwMask&CPC_COMP)
	{
		m_ctx.szComp[0]=0;
		m_ctx.cComp = 0;
	}
	if(dwMask&CPC_ENGLISH)
	{
		m_ctx.pbyEnSpell=NULL;
		m_ctx.pbyEnPhontic=NULL;
	}
	if(dwMask&CPC_STATE)
	{
		m_ctx.inState=INST_CODING;
		m_ctx.sbState=::SBST_NORMAL;
		m_ctx.bWebMode=FALSE;
	}
	if(dwMask&CPC_CAND)
	{
		if(m_ctx.ppbyCandInfo)
		{
			free(m_ctx.ppbyCandInfo);
			m_ctx.ppbyCandInfo=NULL;
		}
		m_ctx.sCandCount=0;
		m_ctx.iCandBegin = 0;
		m_ctx.iCandLast = -1;
		m_ctx.pbyEnAstPhrase=NULL;
	}
	if(dwMask&CPC_SENTENCE)
	{
		m_ctx.sSentCaret=m_ctx.sSentLen=0;
		m_ctx.sSentWords=0;
		memset(m_ctx.pbySentWord,0,sizeof(LPBYTE)*MAX_SENTLEN);
	}
	if(dwMask&CPC_SPELL)
	{
		BYTE i;
		for(i=0;i<m_ctx.bySyllables;i++)
		{
			if(m_ctx.pbyBlur[i]) free(m_ctx.pbyBlur[i]);
			m_ctx.pbyBlur[i]=NULL;
		}
		memset(m_ctx.szWord,VK_SPACE,2*MAX_SYLLABLES);
		memset(m_ctx.bySelect,0,sizeof(BOOL)*MAX_SYLLABLES);
		memset(m_ctx.spellData,0,sizeof(SPELLINFO)*MAX_SYLLABLES);
		m_ctx.bySyllables=1;
		m_ctx.bySyCaret=0xFF;
		m_ctx.byCaret=0;
	}
	if(dwMask & CPC_TIP)
	{
		m_ctx.szTip[0]=0;
		//m_ctx.iTip = -1;
	}
	if (dwMask & CPC_UDCOMP)
	{
		m_ctx.cCompACLen = 0;
	}
	if (dwMask & CPC_INPUT)
	{
		m_ctx.cInput = 0;
	}
}


void CInputState::InputStart()
{
	SLOG_INFO("");
	m_pListener->OnInputStart();

	DWORD tmCur = GetTickCount();
	if (tmCur - m_tmInputEnd > 10 * 1000)
	{
		m_tmInputStart = tmCur;
	}
	else
	{
		m_tmInputStart = m_tmInputEnd;
	}
}

void CInputState::InputResult(const SStringT &strResult,BYTE byAstMask)
{
	SLOG_INFO("result:"<<strResult<<" astMask:"<<byAstMask);

	SASSERT(m_pListener);
	SStringT strTemp = strResult;
	if (g_SettingsUI->bInputBig5)
	{
		int nLen = CUtils::GB2GIB5(strResult, strResult.GetLength(), NULL, 0);
		TCHAR *pBig5 = new TCHAR[nLen+1];
		CUtils::GB2GIB5(strResult, strResult.GetLength(), pBig5, nLen+1);
		strTemp = SStringW(pBig5);
		delete[]pBig5;
	}
	{
		m_pListener->OnInputResult(strTemp);
		_tcscpy(m_ctx.szInput, strTemp);
		m_ctx.cInput = strTemp.GetLength();
	}

	if(byAstMask!=0)
	{
		SStringA strResultA = S_CT2A(strTemp);
		KeyIn_InputAndAssociate(&m_ctx,strResultA,(short)strResultA.GetLength(),byAstMask);
	}else
	{
		InputHide(FALSE);
	}

	m_tmInputEnd = GetTickCount();
	CDataCenter::getSingletonPtr()->GetData().m_tmInputSpan += m_tmInputEnd - m_tmInputStart;
	CDataCenter::getSingletonPtr()->GetData().m_cInputCount+= strTemp.GetLength();
}

void CInputState::InputResult(const SStringA &strResult,BYTE byAstMask)
{
	InputResult(S_CA2T(strResult,CP_GB),byAstMask);
}

void CInputState::InputEnd()
{
	SLOG_INFO("");
	m_pListener->OnInputEnd();
}

void CInputState::InputUpdate()
{
	SLOG_INFO("");
	m_pListener->UpdateInputWnd();
}


void CInputState::InputOpen()
{
	SLOG_INFO("");
	m_pListener->OpenInputWnd();

}

void CInputState::InputHide(BOOL bDelay)
{
	SLOG_INFO("delay:"<<bDelay);
	if(m_pListener->IsCompositing()) 
		m_pListener->OnInputEnd();
	m_pListener->CloseInputWnd(bDelay);
}

void CInputState::StatusbarUpdate()
{
	m_pListener->OnCommand(CMD_UPDATEMODE,0);
}

BOOL CInputState::HandleKeyDown(UINT uVKey,UINT uScanCode,const BYTE * lpbKeyState)
{
	SLOG_INFO("uVKey:"<<uVKey<<" uScanCode:"<<uScanCode);
	//����ʹ��VK�����ݼ������뷭ҳ��
	int iHotKey = TestHotKey(uVKey, lpbKeyState);
	if (iHotKey != -1)
	{
		if(iHotKey == HKI_LoadDebugSkin || iHotKey == HKI_UnloadDebugSkin)
		{//load debug skin
			return TRUE;
		}
		if(iHotKey == HKI_UDMode)
		{//�л����û��Զ�������״̬
			ClearContext(CPC_ALL);
			m_ctx.inState = INST_USERDEF;
			if (!m_pListener->IsCompositing())
			{
				InputStart();
				InputOpen();
			}
			InputUpdate();
		}
		if(iHotKey != HKI_AdjustRate && iHotKey != HKI_DelCandidate)//��Ƶ��ɾ�ʽ��������߼�����
			return TRUE;
	}

	BOOL bHandle=FALSE;
	InputContext * lpCntxtPriv = &m_ctx;
	if(!bHandle && lpCntxtPriv && lpCntxtPriv->sCandCount && lpCntxtPriv->sbState!=SBST_SENTENCE)
	{//��������
		BYTE byCandIndex=0;
		if(uVKey==VK_SPACE) 
		{//�ո�
			byCandIndex='1';
		}else if((uVKey>='0' && uVKey<='9') || (uVKey>=VK_NUMPAD0 && uVKey<=VK_NUMPAD9))
		{//���ּ�
			if((lpbKeyState[VK_CONTROL]&0x80) ||
				(!(lpbKeyState[VK_SHIFT]&0x80) //δ����Shift�����
				&& !KeyIn_IsNumCode(lpCntxtPriv) //���ֲ��Ǳ���״̬
				&& (!g_SettingsG->bCandSelNoNum || g_SettingsG->compMode==IM_SPELL) //δ��ֹ����ѡ������
				)//��������ѡ������
				)
			{//����������ֵ�ASCII��
				if((uVKey>=VK_NUMPAD0 && uVKey<=VK_NUMPAD9))
					byCandIndex=uVKey-VK_NUMPAD0+'0';
				else
					byCandIndex=uVKey;
			}
		}else if(g_SettingsG->b23CandKey && !(lpbKeyState[VK_SHIFT]&0x80))
		{//SHIFT ģʽ�²�����
			UINT uVk=MapVirtualKey(uScanCode,1);
			if(uVk==g_SettingsG->by2CandVK && lpCntxtPriv->sCandCount>=2) byCandIndex='2';
			if(uVk==g_SettingsG->by3CandVK && lpCntxtPriv->sCandCount>=3) byCandIndex='3';
		}
		if(byCandIndex )
		{
			if(!((lpCntxtPriv->sbState==SBST_ASSOCIATE && g_SettingsG->byAstMode==AST_ENGLISH && !(lpbKeyState[VK_CONTROL]&0x80))//Ӣ������״ֻ̬�а���Ctrl�Ž������ѡ��
				|| (m_ctx.compMode==IM_SPELL && lpCntxtPriv->inState==INST_CODING && (uVKey==VK_SPACE || uVKey=='\'') ) )	//ƴ������״̬�µ���������ֶ�����  0xde=VkKeyScan('\'')
				)
			{
				bHandle=KeyIn_All_SelectCand(lpCntxtPriv,byCandIndex,0,lpbKeyState);
			}
		}
		if(!bHandle) bHandle=KeyIn_All_TurnCandPage(lpCntxtPriv,uVKey,lpbKeyState);
	}


	//����ƴ���������ƶ�
	if(!bHandle && lpCntxtPriv->compMode==IM_SPELL && lpCntxtPriv->inState==INST_CODING)
	{
		bHandle=KeyIn_Spell_MoveCaret(lpCntxtPriv,uVKey,lpbKeyState);
		if(!bHandle && uVKey==VK_DELETE) bHandle=KeyIn_Spell_SyFix(lpCntxtPriv,uVKey,lpbKeyState);//����VK_DELETE
	}

	if(uVKey==VK_RETURN && lpbKeyState[VK_SHIFT]&0x80)
	{
		bHandle=KeyIn_RepeatInput(lpCntxtPriv);
	}

	WCHAR wChar = 0;
	int test = ToUnicode(uVKey,uScanCode,lpbKeyState,&wChar,1,0);

	if(!bHandle && uVKey && test!=0)
	{
		uVKey = wChar;
		if(lpCntxtPriv->inState==INST_CODING)
		{//����״̬ת��ǰ����
			BOOL bReadyEn=FALSE;
			BOOL bReadyDgt=FALSE;
			if(lpCntxtPriv->sbState==::SBST_NORMAL)
			{
				if(lpCntxtPriv->compMode==IM_SPELL)
				{
					if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
					{
						bReadyEn=TRUE;
						bReadyDgt=TRUE;
					}
				}else
				{
					if(lpCntxtPriv->cComp==0)
					{
						bReadyEn=TRUE;
						bReadyDgt=TRUE;
					}
				}
			}else if(lpCntxtPriv->sbState==SBST_ASSOCIATE)
			{
				bReadyEn=TRUE;
				bReadyDgt=TRUE;
				if((g_SettingsG->byAstMode==AST_CAND ||(g_SettingsG->byAstMode==AST_ENGLISH &&(lpbKeyState[VK_CONTROL]&0x80))) && lpCntxtPriv->sCandCount)
					bReadyDgt=FALSE;
			}
			if(IsTempSpell() && (bReadyEn || bReadyDgt) && (isdigit(uVKey) || isupper(uVKey)))
			{//temp spell mode
				CUtils::SoundPlay(_T("error"));
				return FALSE;
			}
			if((bReadyEn || bReadyDgt) && lpCntxtPriv->bShowTip) //�ر�tip
				lpCntxtPriv->bShowTip=FALSE;
			if(bReadyEn && uVKey>='A' && uVKey<='Z' && (lpbKeyState[VK_CAPITAL]&0x01)== 0)
			{//��д���룬���л���Ӣ��״̬
				ClearContext(CPC_ALL);
				if(g_SettingsUI->bEnglish)
				{
					lpCntxtPriv->inState=INST_ENGLISH;
					//ȷ�������봰��
					InputStart();
					InputOpen();
				}
			}else if(bReadyDgt && uVKey>='0' && uVKey<='9')
			{//�������룬������������״̬
				ClearContext(CPC_ALL);
				lpCntxtPriv->inState=INST_DIGITAL;
				InputHide(FALSE);
			}
		}
		if(lpCntxtPriv->inState==INST_CODING)
		{
			if(lpCntxtPriv->sbState==::SBST_NORMAL)
			{//��������
				if(lpCntxtPriv->compMode==IM_SPELL)
				{//ƴ��״̬
					bHandle=KeyIn_Spell_Normal(lpCntxtPriv,uVKey,lpbKeyState);
				}else
				{
					bHandle=KeyIn_Code_Normal(lpCntxtPriv,uVKey,lpbKeyState);
				}
			}else if(lpCntxtPriv->sbState==SBST_ASSOCIATE)
			{//����״̬
				bHandle=KeyIn_All_Associate(lpCntxtPriv,uVKey,lpbKeyState);
			}else if(lpCntxtPriv->sbState==SBST_SENTENCE)
			{//����״̬
				bHandle=KeyIn_All_Sentence(lpCntxtPriv,uVKey,lpbKeyState);
				if(!bHandle) KeyIn_Code_Symbol(lpCntxtPriv,uVKey,lpbKeyState);
			}
		}else if(lpCntxtPriv->inState==INST_ENGLISH)
		{//Ӣ�ĵ���״̬
			bHandle=KeyIn_Code_English(lpCntxtPriv,uVKey,lpbKeyState);
		}else if(lpCntxtPriv->inState==INST_DIGITAL)
		{//��������״̬
			bHandle=KeyIn_Digital_ChangeComp(lpCntxtPriv,uVKey,lpbKeyState);
		}else if(lpCntxtPriv->inState==INST_USERDEF)
		{//�û��Զ�������״̬
			bHandle=KeyIn_UserDef_ChangeComp(lpCntxtPriv,uVKey,lpbKeyState);
		}else if(lpCntxtPriv->inState==INST_LINEIME)
		{//�ʻ�����״̬
			BOOL bInputKey=FALSE;
			if(uVKey>=VK_NUMPAD1&&uVKey<=VK_NUMPAD6)
			{//С��������
				bInputKey=TRUE;
			}else
			{//���õ�ת����
				int i;
				for( i=0;i<6;i++)
				{
					if(g_SettingsG->byLineKey[i]==uVKey)
					{
						uVKey=VK_NUMPAD1+i;
						bInputKey=TRUE;
						break;
					}
				}
				if(uVKey==VK_ESCAPE || uVKey==VK_RETURN || uVKey==VK_BACK) bInputKey=TRUE;
			}
			if(bInputKey) bHandle=KeyIn_Line_ChangeComp(lpCntxtPriv,uVKey,lpbKeyState);
		}
		if(!bHandle) CUtils::SoundPlay(_T("error"));
	}
	return bHandle;
}

//ƴ�����������ƶ�
void SpellBuf_Move(InputContext * lpCntxtPriv,BYTE byBegin,BYTE bySize,char cDistance)
{
	memmove(lpCntxtPriv->pbyBlur+byBegin+cDistance,lpCntxtPriv->pbyBlur+byBegin,sizeof(LPBYTE)*bySize);
	memmove(lpCntxtPriv->spellData+byBegin+cDistance,lpCntxtPriv->spellData+byBegin,sizeof(SPELLINFO)*bySize);
	memmove(lpCntxtPriv->bySelect+byBegin+cDistance,lpCntxtPriv->bySelect+byBegin,sizeof(BYTE)*bySize);
	memmove(lpCntxtPriv->szWord+byBegin+cDistance,lpCntxtPriv->szWord+byBegin,2*bySize);
}

//���ָ�����ڵ�����
void SpellBuf_ClearSyllable(InputContext * lpCntxtPriv,BYTE bySyllable)
{
	memset(lpCntxtPriv->pbyBlur+bySyllable,0,sizeof(LPBYTE));
	memset(lpCntxtPriv->spellData+bySyllable,0,sizeof(SPELLINFO));
	memset(lpCntxtPriv->bySelect+bySyllable,0,sizeof(BYTE));
	memset(lpCntxtPriv->szWord+bySyllable,VK_SPACE,2);
}

BOOL CInputState::KeyIn_RepeatInput(InputContext *  lpCntxtPriv)
{
	if (lpCntxtPriv->cInput == 0)
		return FALSE;
	InputStart();
	InputResult(SStringT(lpCntxtPriv->szInput,lpCntxtPriv->cInput), GetKeyinMask(FALSE, MKI_ALL));
	InputEnd();
	return TRUE;
}

//ƴ��״̬�¸��º�ѡ�ʴ���
void CInputState::KeyIn_Spell_UpdateCandList(InputContext *  lpCntxtPriv,BYTE byCaret)
{
	DWORD dwRet=ISACK_ERROR;
	BYTE byMask=0;
	ClearContext(CPC_CAND);
	if(lpCntxtPriv->spellData[byCaret].bySpellLen==0) return;
	if(g_SettingsG->bPYPhraseFirst) byMask=0x01;
	if(byCaret==lpCntxtPriv->bySyllables-1)
	{
		byMask|=0x02;	//�������������ѯ����
		if(lpCntxtPriv->bySyllables>=4) byMask|=0x04;	//��������������
		dwRet=ISComm_SpellQueryCandEx(lpCntxtPriv->pbyBlur,lpCntxtPriv->bySyllables,byMask);
	}else
	{
		if(byCaret==0 && lpCntxtPriv->bySyllables>=4) byMask|=0x04;//��������������
		dwRet=ISComm_SpellQueryCandEx(lpCntxtPriv->pbyBlur+byCaret,lpCntxtPriv->bySyllables-byCaret,byMask);
	}

	if(dwRet==ISACK_SUCCESS)
	{
		PMSGDATA pMsgData=ISComm_GetData();
		short i,sCount=0,sValidCount=0;
		LPBYTE pBuf=m_pbyMsgBuf;
		memcpy(pBuf,pMsgData->byData,pMsgData->sSize);
		memcpy(&sCount,pBuf,2);
		pBuf+=2;
		lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
		for(i=0;i<sCount;i++)
		{
			char nLen = pBuf[2];
			if(byCaret==(lpCntxtPriv->bySyllables-1))
			{
				if(strncmp((char*)(lpCntxtPriv->szWord+lpCntxtPriv->bySyllables-nLen/2),(char*)pBuf+3,nLen)!=0) //ȥ����Ԥ����ͬ������
					lpCntxtPriv->ppbyCandInfo[sValidCount++]=pBuf;
			}else
			{
				if(strncmp((char*)(lpCntxtPriv->szWord+byCaret),(char*)pBuf+3,nLen)!=0)
					lpCntxtPriv->ppbyCandInfo[sValidCount++]=pBuf;
			}
			pBuf+=nLen+3;
		}
		lpCntxtPriv->sCandCount=sValidCount;
	}
}

//ƴ��״̬���Ƶ��༭����
BOOL CInputState::KeyIn_Spell_MoveCaret(InputContext *lpCntxtPriv, UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=TRUE;
	if(lpCntxtPriv->bySyCaret==0xFF)
	{
		switch(byInput)
		{
		case VK_LEFT:
			if(lpCntxtPriv->byCaret>0)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret-1,1,1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret-1);
				}
				lpCntxtPriv->byCaret--;
			}
			break;
		case VK_HOME:
			if(lpCntxtPriv->byCaret>0)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,0,lpCntxtPriv->byCaret,1);
					SpellBuf_ClearSyllable(lpCntxtPriv,0);
				}
				lpCntxtPriv->byCaret=0;
			}
			break;
		case VK_RIGHT:
			if(lpCntxtPriv->byCaret<lpCntxtPriv->bySyllables-1)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret+1,1,-1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret+1);
				}
				lpCntxtPriv->byCaret++;
			}
			break;
		case VK_END:
			if(lpCntxtPriv->byCaret<lpCntxtPriv->bySyllables-1)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret+1,lpCntxtPriv->bySyllables-(lpCntxtPriv->byCaret+1),-1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->bySyllables-1);
				}
				lpCntxtPriv->byCaret=lpCntxtPriv->bySyllables-1;
			}
			break;
		default:
			bRet=FALSE;
			break;
		}
		if(bRet) 
		{
			KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
		}
	}else
	{
		bRet=FALSE;
		//�������
		if(byInput==VK_LEFT)
		{
			if(lpCntxtPriv->bySyCaret>1) 
				lpCntxtPriv->bySyCaret--;
			else
				lpCntxtPriv->bySyCaret = lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen;
			bRet=TRUE;
		}else if(byInput==VK_RIGHT)
		{
			if(lpCntxtPriv->bySyCaret<lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen)
				lpCntxtPriv->bySyCaret++;
			else
				lpCntxtPriv->bySyCaret = 1;
			bRet=TRUE;
		}else if(byInput==VK_HOME)
		{
			lpCntxtPriv->bySyCaret=1;
			bRet=TRUE;
		}else if(byInput==VK_END)
		{
			lpCntxtPriv->bySyCaret=lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen;
			bRet=TRUE;
		}
	}
	return bRet;
}

//��byStartPos���ڿ�ʼԤ�⣬�����ʼ���ڵ�ǰһ���������û�ѡ��ĵ��֣�����ֻ���ΪԤ���ǰ׺����������
void CInputState::KeyIn_Spell_Forecast(InputContext * lpCntxtPriv,BYTE byStartPos)
{
	BYTE byEnd=byStartPos;
	while(byEnd<lpCntxtPriv->bySyllables)
	{
		//�ֱ��ҵ�Ԥ�����ʼ��ͽ�����
		BYTE byBegin=byEnd;
		while(lpCntxtPriv->bySelect[byBegin]!=0 && byBegin<lpCntxtPriv->bySyllables) byBegin++;
		byEnd=byBegin;
		while(lpCntxtPriv->bySelect[byEnd]==0 && byEnd<lpCntxtPriv->bySyllables) byEnd++;
		if(byEnd>byBegin)
		{
			char szPrefix[2]={0};
			if(byBegin>0 && lpCntxtPriv->bySelect[byBegin-1]==1)
				memcpy(szPrefix,lpCntxtPriv->szWord[byBegin-1],2);
			if(ISComm_SpellForecast(szPrefix,lpCntxtPriv->pbyBlur+byBegin,byEnd-byBegin)==ISACK_SUCCESS)
			{
				PMSGDATA pMsgData=ISComm_GetData();
				BYTE byPrefix=pMsgData->byData[0];
				BYTE *pBuf=pMsgData->byData+1;
				BYTE iPhrase,byPhrases=*pBuf++;
				char *pszWord=(char*)(lpCntxtPriv->szWord+byBegin);
				if(byPrefix==1)
				{//ǰ׺��Ч
					memcpy(pszWord,pBuf+2+2,pBuf[1]-2);
					pszWord+=pBuf[1]-2;
					pBuf+=pBuf[1]+2;
					byPhrases--;
				}
				for(iPhrase=0;iPhrase<byPhrases;iPhrase++)
				{//��������
					memcpy(pszWord,pBuf+2,pBuf[1]);
					pszWord+=pBuf[1];
					pBuf+=pBuf[1]+2;
				}
			}
		}
	}
}


BOOL CInputState::KeyIn_Spell_SyFix(InputContext * lpCntxtPriv,UINT byInput,
									CONST BYTE * lpbKeyState)
{
	if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0) return FALSE;
	if(lpCntxtPriv->bySyCaret==0xFF)
	{
		if(byInput==VK_RETURN && !(lpbKeyState[VK_SHIFT]&0x80))
		{
			lpCntxtPriv->bySyCaret=lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen;
			return TRUE;
		}
	}else
	{
		BOOL bRet=FALSE;
		if(byInput==VK_RETURN)
		{//�˳������޸�״̬
			if(lpCntxtPriv->sCandCount)
				lpCntxtPriv->bySyCaret=0xFF;
			else
			{
				lpCntxtPriv->bShowTip=TRUE;
				strcpy(lpCntxtPriv->szTip,"���ڴ���!");
			}
			return TRUE;
		}else
		{//�����޸ļ�
			BOOL bModified=FALSE;
			SPELLINFO * lpSpi=lpCntxtPriv->spellData+lpCntxtPriv->byCaret;
			if(byInput==VK_BACK)
			{
				if(lpCntxtPriv->bySyCaret>0)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret-1,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret);
					lpSpi->bySpellLen--;
					lpCntxtPriv->bySyCaret--;
					bModified=TRUE;
				}
				bRet=TRUE;
			}else if(byInput==VK_DELETE)
			{
				if(lpCntxtPriv->bySyCaret<lpSpi->bySpellLen)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret+1,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret-1);
					lpSpi->bySpellLen--;
					bModified=TRUE;
				}
				bRet=TRUE;
			}else if(byInput>='a' && byInput<='z')
			{//��������
				if(lpSpi->bySpellLen<6)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret+1,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret);
					lpSpi->szSpell[lpCntxtPriv->bySyCaret]=byInput;
					lpSpi->bySpellLen++;
					lpCntxtPriv->bySyCaret++;
					bModified=TRUE;
				}
				bRet=TRUE;
			}
			if(bModified)
			{//��ǰ���ڱ��޸ģ���������
				lpCntxtPriv->bShowTip=FALSE;
				if(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret])
				{//ɾ��ԭ������
					free(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]);
					lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]=NULL;
					memset(lpCntxtPriv->szWord[lpCntxtPriv->byCaret],0x20,2);
					ClearContext(CPC_CAND);//�����ǰ����
				}
				lpCntxtPriv->bySelect[lpCntxtPriv->byCaret]=0;
				if(lpSpi->bySpellLen==0)
				{//�Զ��˳����ڱ༭״̬
					lpCntxtPriv->bySyCaret=0xFF;
				}else if(ISComm_Spell2ID(lpSpi->szSpell,lpSpi->bySpellLen)==ISACK_SUCCESS)
				{//���µ�ǰ���ڱ༭������
					PMSGDATA pData=NULL;
					ISComm_SpellGetBlur(lpSpi->szSpell,lpSpi->bySpellLen);
					pData=ISComm_GetData();
					lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]=(LPBYTE)malloc(pData->sSize);
					memcpy(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret],pData->byData,pData->sSize);
					//����Ԥ�Ȿ��ĺ�ѡ��
					KeyIn_Spell_Forecast(lpCntxtPriv,0);
					//��ȡ��ǰ�����λ�õ�����
					KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
				}
			}
			return bRet;
		}
	}
	return FALSE;
}

//ƴ��״̬�¸ı����
BOOL CInputState::KeyIn_Spell_ChangeComp(InputContext* lpCntxtPriv,UINT byInput,
										 CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	SPELLINFO* pSpInfo=lpCntxtPriv->spellData+lpCntxtPriv->byCaret;
	char (*pComp)[2]=lpCntxtPriv->szWord+lpCntxtPriv->byCaret;
	BYTE byCaret=lpCntxtPriv->byCaret;
	if(byInput>='a' && byInput<='z')
	{
		bCompChar=TRUE;
		if(pSpInfo->bySpellLen==0)
		{
			if(byInput=='u' || byInput=='v')
			{
				if(!IsTempSpell() && lpCntxtPriv->bySyllables==1)
				{//�л����û��Զ���ģʽ
					ClearContext(CPC_ALL);
					lpCntxtPriv->inState=INST_USERDEF;
					InputOpen();
					InputUpdate();
					InputStart();
				}
				bCompChar=FALSE;
			}else if(byInput=='i')
			{
				if(!IsTempSpell() && lpCntxtPriv->bySyllables==1)
				{//�л����ʻ�����״̬
					ClearContext(CPC_ALL);
					lpCntxtPriv->inState=INST_LINEIME;
					InputOpen();
					InputUpdate();
					InputStart();
				}
				bCompChar=FALSE;
			}else
			{
				if(g_SettingsG->bShowOpTip && !IsTempSpell())
				{
					lpCntxtPriv->bShowTip=TRUE;
					lpCntxtPriv->iTip=Tips_Rand(TRUE,lpCntxtPriv->szTip);
				}
			}
		}

		if(!bCompChar)	return lpCntxtPriv->bySyllables==1;
	}else if(byInput=='\'' && pSpInfo->bySpellLen)
	{
		bCompChar=TRUE;
	}

	if(bCompChar)
	{
		PMSGDATA pData;
		if(lpCntxtPriv->sbState==SBST_ASSOCIATE) 
		{
			ClearContext(CPC_ALL);
		}
		if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
		{//��ʼ��������,���ɿ�ʼ������Ϣ�Ի�ȡ������ʱ���봰�ڵ�����
			InputOpen();
			InputStart();
		}
		pSpInfo->szSpell[pSpInfo->bySpellLen++]=byInput;
		if(ISComm_Spell2ID(pSpInfo->szSpell,pSpInfo->bySpellLen)==ISACK_SUCCESS)
		{//���µ�ǰ���ڱ༭������
			ISComm_SpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen);
			pData=ISComm_GetData();
			if(lpCntxtPriv->pbyBlur[byCaret]) free(lpCntxtPriv->pbyBlur[byCaret]);
			lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
			memcpy(lpCntxtPriv->pbyBlur[byCaret],pData->byData,pData->sSize);
			lpCntxtPriv->bySelect[byCaret]=0;
		}else
		{//����һ��������
			pSpInfo->bySpellLen--;
			if(lpCntxtPriv->bySyllables<MAX_SYLLABLES)
			{
				//����ǰ�����������ں���һ������,������һ�����ڿռ�
				SpellBuf_Move(lpCntxtPriv,
					byCaret+1,
					lpCntxtPriv->bySyllables-(byCaret+1),
					1);
				lpCntxtPriv->bySyllables++;
				lpCntxtPriv->byCaret++;
				//��յ�ǰ��������
				SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret);
				pSpInfo++;
				pComp++;
				byCaret++;
				//���ݵ�ǰ������д��ǰ��������
				if(byInput!='\'')
				{
					pSpInfo->szSpell[pSpInfo->bySpellLen++]=byInput;
					if(ISComm_Spell2ID(pSpInfo->szSpell,1)==ISACK_SUCCESS)
					{
						pData=ISComm_GetData();
					}else
					{//����ƴ����������ͨ������ĸ�γ�ƴ��
						pData=ISComm_GetData();
						pData->byData[0]=0;
					}
					if(pData->byData[0]==0 && 
						lpCntxtPriv->bySyllables>1 &&
						byCaret==lpCntxtPriv->bySyllables-1 && 
						lpCntxtPriv->spellData[byCaret-1].bySpellLen>1 )
					{//���ڱ༭���һ�����ڣ���ǰ����Ĳ�����ĸ���ж��Ƿ���Ҫ��ǰһ�����ڽ�һ����ĸ
						SPELLINFO siPrev,siCur;
						siPrev=lpCntxtPriv->spellData[byCaret-1];
						siCur=lpCntxtPriv->spellData[byCaret];
						siCur.szSpell[0]=siPrev.szSpell[siPrev.bySpellLen-1];
						siCur.szSpell[1]=byInput;
						siCur.bySpellLen=2;
						siPrev.bySpellLen--;
						if(ISComm_Spell2ID(siPrev.szSpell,siPrev.bySpellLen)==ISACK_SUCCESS &&
							ISComm_Spell2ID(siCur.szSpell,siCur.bySpellLen)==ISACK_SUCCESS)
						{//����ĸ�ɹ�,�޸�ǰһ��������
							_ASSERT(lpCntxtPriv->pbyBlur[byCaret-1]);
							free(lpCntxtPriv->pbyBlur[byCaret-1]);	//ע���ͷ�ԭƴ��ռ�õĶ��ڴ�
							SpellBuf_ClearSyllable(lpCntxtPriv,byCaret-1);
							ISComm_SpellGetBlur(siPrev.szSpell,siPrev.bySpellLen);
							pData=ISComm_GetData();
							lpCntxtPriv->pbyBlur[byCaret-1]=(LPBYTE)malloc(pData->sSize);
							memcpy(lpCntxtPriv->pbyBlur[byCaret-1],pData->byData,pData->sSize);
							lpCntxtPriv->spellData[byCaret-1]=siPrev;
							lpCntxtPriv->spellData[byCaret]=siCur;
						}
					}
					if(ISComm_SpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen)==ISACK_SUCCESS)
					{
						pData=ISComm_GetData();
						lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
						memcpy(lpCntxtPriv->pbyBlur[byCaret],
							pData->byData,
							pData->sSize);
					}else
					{//�����ʱ����
						pSpInfo->bySpellLen=0;
					}
				}
			}
		}
		bRet=TRUE;
	}else if(byInput==VK_BACK)
	{
		if(pSpInfo->bySpellLen)
		{
			pSpInfo->bySpellLen--;
			free(lpCntxtPriv->pbyBlur[byCaret]);
			lpCntxtPriv->pbyBlur[byCaret]=NULL;
			lpCntxtPriv->bySelect[byCaret]=0;
			memset(lpCntxtPriv->szWord+byCaret,0x20,2);
			if(pSpInfo->bySpellLen)
			{
				PMSGDATA pData;
				ISComm_SpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen);
				pData=ISComm_GetData();
				lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
				memcpy(lpCntxtPriv->pbyBlur[byCaret],
					pData->byData,
					pData->sSize);
			}else if(byCaret==lpCntxtPriv->bySyllables-1 && lpCntxtPriv->bySyllables>1)
			{//ɾ�����һ�����ڵ�Ψһ��ĸ
				lpCntxtPriv->bySyllables--;
				lpCntxtPriv->byCaret--;
				pSpInfo--;
				byCaret--;
			}
			bRet=TRUE;
		}else if(lpCntxtPriv->bySyllables>1)//���ٱ�֤Ҫ��һ�����ڿռ�
		{//�����ǰ�Ŀհ�����
			SpellBuf_Move(lpCntxtPriv,
				lpCntxtPriv->byCaret+1,
				lpCntxtPriv->bySyllables-(lpCntxtPriv->byCaret+1),
				-1);
			SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->bySyllables-1);
			lpCntxtPriv->bySyllables--;
			if(lpCntxtPriv->byCaret==lpCntxtPriv->bySyllables)
			{
				lpCntxtPriv->byCaret--;
				pSpInfo--;
				byCaret--;
			}
			bRet=TRUE;
		}else if(g_SettingsG->compMode!=IM_SPELL)
		{//temp spell mode will ignore VK_BACK key.
			bRet=TRUE;
		}
		if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
		{
			InputEnd();
			if(!IsTempSpell())
				InputHide(TRUE);
		}
	}else if(byInput==VK_ESCAPE)
	{
		InputEnd();
		InputHide(TRUE);
		ClearContext(CPC_ALL);
		if(IsTempSpell())
		{//restore shape code input mode
			m_ctx.compMode=IM_SHAPECODE;
			StatusbarUpdate();
		}	
		bRet=TRUE;
	}

	if(bRet )
	{
		if(lpCntxtPriv->bySyllables>1) 
			m_ctx.bShowTip=FALSE;
		ClearContext(CPC_CAND);//�����ǰ����
		if(pSpInfo->bySpellLen)
		{
			//����Ԥ�Ȿ��ĺ�ѡ��
			KeyIn_Spell_Forecast(lpCntxtPriv,0);
			//��ȡ��ǰ�����λ�õ�����
			KeyIn_Spell_UpdateCandList(lpCntxtPriv,byCaret);
		}
	}
	return bRet;
}

BOOL CInputState::KeyIn_Spell_GetSpellInput(InputContext * lpCntxtPriv,BYTE bySpellID[MAX_SYLLABLES][2])
{
	BOOL bRet=TRUE;
	BYTE i,j,k,iWord;
	PMSGDATA pData=NULL;

	for(i=0,iWord=0;i<lpCntxtPriv->bySyllables;i++)
	{
		BOOL  bFind=FALSE;
		if(lpCntxtPriv->spellData[i].bySpellLen==0) continue;
		//�ҵ���ǰ�û�ѡ��ĺ��ֵ�ƴ����ͨ����������ģ��������ƥ����
		ISComm_QueryWordSpell(lpCntxtPriv->szWord[i]);//��ú��ֵ�����ƴ����ID
		pData=ISComm_GetData();

		for(j=0;j<pData->byData[0];j++)
		{
			short sBlur=0;
			BYTE  *pbyBlur=lpCntxtPriv->pbyBlur[i];
			memcpy(&sBlur,pbyBlur,2);//ģ��������
			pbyBlur+=4;//ģ����ID,�����������ȫƥ��������
			for(k=0;k<sBlur;k++)
			{
				if(memcmp(pData->byData+1+2*j,pbyBlur+2*k,2)==0)
				{//�õ�������ƴ��ID
					memcpy(bySpellID[iWord],pbyBlur+2*k,2);
					bFind=TRUE;
				}
			}
			if(bFind) break;
		}
		if(!bFind) bRet=FALSE;
		memcpy(lpCntxtPriv->szComp+iWord*2,lpCntxtPriv->szWord[i],2);
		iWord++;
	}
	lpCntxtPriv->cComp=iWord*2;
	return bRet;
}


//ƴ��״̬�½�������뵽�༭����
BOOL CInputState::KeyIn_Spell_InputText(InputContext* lpCntxtPriv,UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if(lpCntxtPriv->bySyllables<=1 && lpCntxtPriv->spellData[0].bySpellLen==0) return FALSE;

	if(byInput==VK_SPACE)
	{
		BYTE bySpellID[MAX_SYLLABLES][2];//�û�����ĺ��ֵ�ƴ��������ID
		SStringA strResult;
		BOOL bGetSpID=KeyIn_Spell_GetSpellInput(lpCntxtPriv,bySpellID);
		if(lpCntxtPriv->cComp)
		{
			strResult = SStringA(lpCntxtPriv->szComp,lpCntxtPriv->cComp);
		}
		ClearContext(CPC_ALL);
		InputResult(strResult,GetKeyinMask(!IsTempSpell(),MKI_ALL));
		InputEnd();
		InputHide(TRUE);

		if (IsTempSpell())
		{//��ʱƴ��ģʽ�Զ��������ƴ���ı���
			lpCntxtPriv->bShowTip = TRUE;
			lpCntxtPriv->iTip = -1;
			GetShapeComp(strResult, strResult.GetLength());
			lpCntxtPriv->compMode = IM_SHAPECODE;
			StatusbarUpdate();
		}

		//���û������ύ������������
		if(bGetSpID) ISComm_SpellMemoryEx(strResult,strResult.GetLength(),bySpellID);
		bRet=TRUE;
	}else if ( byInput == VK_RETURN && g_SettingsG->compMode == IM_SPELL)
	{//�س��������

		SStringA strResultA;
		BYTE i;
		for(i=0;i<lpCntxtPriv->bySyllables;i++)
		{
			if(lpCntxtPriv->spellData[i].bySpellLen)
			{
				strResultA += SStringA(lpCntxtPriv->spellData[i].szSpell,lpCntxtPriv->spellData[i].bySpellLen);
			}
		}
		//֪ͨӦ�ó����������
		InputResult(strResultA,GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT));
		InputEnd();
		InputHide(FALSE);
		ClearContext(CPC_ALL);
		bRet=TRUE;
	}
	return bRet;
}

//ͨ����д��������λ����
BOOL CInputState::KeyIn_Spell_Locate(InputContext *lpCntxtPriv,UINT byInput,
									 CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if(byInput>='A' && byInput<='Z' && lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen!=0)
	{//��д����,�Ǳʻ�����״̬���ǿ���״̬
		BYTE byCaret=lpCntxtPriv->byCaret+1;
		byInput+=0x20;
		while(byCaret<lpCntxtPriv->byCaret+lpCntxtPriv->bySyllables)
		{
			if(lpCntxtPriv->spellData[byCaret%lpCntxtPriv->bySyllables].szSpell[0]==byInput)
			{
				lpCntxtPriv->byCaret=byCaret%lpCntxtPriv->bySyllables;
				KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
				bRet=TRUE;
				break;
			}
			byCaret++;
		}
	}
	return bRet;
}

//�������
BOOL CInputState::KeyIn_Spell_Symbol(InputContext* lpCntxtPriv,UINT byInput,
									 CONST BYTE* lpbKeyState)
{
	BOOL bRet=FALSE;
	if(!IsTempSpell() && (lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen!=0 || lpCntxtPriv->bySyllables==1))
	{//�������
		BYTE bySpellID[MAX_SYLLABLES][2];
		BOOL bGetSpID=KeyIn_Spell_GetSpellInput(lpCntxtPriv,bySpellID);
		SStringA strResultA;
		if(lpCntxtPriv->cComp)
		{
			if(bGetSpID) ISComm_SpellMemoryEx(lpCntxtPriv->szComp,lpCntxtPriv->cComp,bySpellID);
			strResultA = SStringA(lpCntxtPriv->szComp,lpCntxtPriv->cComp);
		}
		strResultA += Symbol_Convert(&m_ctx,byInput,lpbKeyState);
		InputStart();
		InputResult(strResultA,0);
		InputEnd();
		InputHide(FALSE);
		ClearContext(CPC_ALL);
		bRet=TRUE;
	}
	return bRet;
}

BOOL CInputState::KeyIn_Spell_Normal(InputContext * lpCntxtPriv,UINT byInput,
									 CONST BYTE * lpbKeyState)
{
	BOOL bHandle=KeyIn_Spell_SyFix(lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_ChangeComp(lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_InputText(	lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_Locate(lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_Symbol(lpCntxtPriv,byInput,lpbKeyState);
	return bHandle;
}


//���벢������
BOOL CInputState::KeyIn_InputAndAssociate(InputContext * lpCntxtPriv,const char *pszInput,short sLen,BYTE byMask)
{
	ISComm_KeyIn(pszInput,sLen,byMask,m_pListener->GetHwnd());
	lpCntxtPriv->sbState=SBST_ASSOCIATE;
	lpCntxtPriv->sCandCount=0;
	return TRUE;
}

//ѡ��һ������
//BYTE byInput: Virtual Key
BOOL CInputState::KeyIn_All_SelectCand(InputContext * lpCntxtPriv,UINT byInput,char cCompLen,
									   CONST BYTE * lpbKeyState,bool bKeepVisible)
{
	BOOL bRet=FALSE;

	short iCand = -1;
	if(byInput>='0' && byInput<='9')
	{
		iCand = m_pListener->SelectCandidate((byInput-'0'+9)%10);		
	}
	if(iCand != -1)
	{//��Ч������
		BYTE byMask=GetKeyinMask(cCompLen==0,MKI_ALL);
		LPBYTE pData=lpCntxtPriv->ppbyCandInfo[iCand];
		bRet=TRUE;
		SStringA strResultA;
		if(lpCntxtPriv->inState==INST_CODING)
		{//��ͨ��������
			if(lpCntxtPriv->sbState==::SBST_NORMAL)
			{//��������״̬
				if(lpCntxtPriv->compMode==IM_SPELL)
				{//ƴ������״̬
					BYTE iWord=0;
					BYTE byPhraseLen=pData[2]/2;
					BYTE byCaret=lpCntxtPriv->byCaret;
					if(byCaret==lpCntxtPriv->bySyllables-1)
					{
						if(byPhraseLen>lpCntxtPriv->bySyllables)
							byCaret=0;//��������������
						else
							byCaret-=byPhraseLen-1;
					}
					while(iWord<byPhraseLen)
					{
						memcpy(lpCntxtPriv->szWord[iWord+byCaret],pData+3+iWord*2,2);
						lpCntxtPriv->bySelect[iWord+byCaret]=byPhraseLen;
						iWord++;
					}
					if(byPhraseLen>lpCntxtPriv->bySyllables)
					{//���������볤������
						SStringA strResultA((char*)pData+3,pData[2]);
						BOOL isTempSpell = IsTempSpell();

						ClearContext(CPC_ALL);
						InputResult(strResultA,GetKeyinMask(!isTempSpell,MKI_ALL));
						InputEnd();

						if(isTempSpell) 
						{//��ʱƴ��ģʽ��������ֵı���
							lpCntxtPriv->bShowTip=TRUE;
							lpCntxtPriv->iTip = -1;
							GetShapeComp(strResultA,strResultA.GetLength());
						}
						if(isTempSpell)
						{//�˳���ʱƴ��
							lpCntxtPriv->compMode=IM_SHAPECODE;
							InputUpdate();
						}
					}
					else
					{
						KeyIn_Spell_Forecast(lpCntxtPriv,lpCntxtPriv->byCaret);
						if(byCaret+byPhraseLen<lpCntxtPriv->bySyllables)
						{
							lpCntxtPriv->byCaret+=byPhraseLen;
							KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
							InputUpdate();
						}else
						{
							KeyIn_Spell_InputText(lpCntxtPriv,VK_SPACE,lpbKeyState);
						}
					}
					goto end; //ƴ������Ƚ�����
				}else
				{//��ƴ��
					if( (lpbKeyState[VK_CONTROL]&0x80) && (lpbKeyState[VK_SHIFT]&0x80) )
					{//Ctrl+Shift+���ּ�:����ɾ��,������״̬֧��
						if(lpCntxtPriv->sCandCount>1)
						{//ֻ���ж�������ʱ�����Ч
							char *pPhrase=(char*)pData+2;
							char *pComp=(char *)pPhrase+pPhrase[0]+1;
							char cCompLen=pComp[0];
							if(cCompLen==0)
								pComp=lpCntxtPriv->szComp,cCompLen=lpCntxtPriv->cComp;
							else
								pComp++;
							if(pPhrase[0]>2 || !g_SettingsG->bDisableDelWordCand)
							{
								if(ISComm_PhraseDel(pComp,cCompLen,pPhrase+1,pPhrase[0],m_pListener->GetHwnd())==ISACK_SUCCESS)
								{//ɾ������ɹ�,������ʾ
									memcpy(lpCntxtPriv->ppbyCandInfo+iCand,lpCntxtPriv->ppbyCandInfo+iCand+1,(lpCntxtPriv->sCandCount-iCand-1)*sizeof(LPBYTE));
									lpCntxtPriv->sCandCount--;
									InputUpdate();
									goto end;
								}else
								{
									CUtils::SoundPlay(_T("error"));
								}
							}else
							{
								CUtils::SoundPlay(_T("error"));
							}
						}
						CUtils::SoundPlay(_T("error"));
						goto end;
					}else if(pData[0]==RATE_USERCMD)
					{//����ֱͨ������
						CUtils::CmdExecute(pData);
						byMask=0;
					}else
					{//��ͨ��ѡ������
						LPBYTE pCand=pData+2;//rate+gbkFlag
						LPBYTE pComp=pCand+1+pCand[0];
						if(pData[0]==RATE_USERDEF)
						{//�Զ������
							if(pComp[0]!=0)	
								strResultA = SStringA((char*)pComp + 1, pComp[0]);
							else 
								strResultA = SStringA((char*)pCand + 1, pCand[0]);
							byMask=0;
						}else
						{//�����Զ������
							strResultA = SStringA((char*)pCand+1,pCand[0]);
							if(pData[0]!=RATE_FORECAST)
							{//����Ԥ��ʣ���Ƶ����
								if(lpbKeyState[VK_CONTROL] & 0x80)
								{
									ISComm_RateAdjust((char*)pComp+1,pComp[0],(char*)pCand+1,pCand[0],RAM_FAST,m_pListener->GetHwnd());
								}else if(g_SettingsG->byRateAdjust) 
								{
									ISComm_RateAdjust((char*)pComp+1,pComp[0],(char*)pCand+1,pCand[0],g_SettingsG->byRateAdjust==1?RAM_AUTO:RAM_FAST,m_pListener->GetHwnd());
								}
							}else
							{//Ԥ��ʣ��Զ����
								ISComm_MakePhrase((char*)pCand+1,pCand[0]);
							}
						}
					}
				}
			}else //if(lpCntxtPriv->sbState==SBST_ASSOCIATE || lpCntxtPriv->sbState==SBST_SENTENCE)
			{
				if(g_SettingsG->byAstMode==AST_CAND)
				{//��ǰ�Ǵ�������
					InputStart();
					strResultA= SStringA((char*)pData+3+pData[0],pData[2]-pData[0]);
				}else if(g_SettingsG->byAstMode==AST_ENGLISH)
				{//��ǰ��Ӣ������
					InputStart();
					strResultA =SStringA((char*)pData+1,pData[0]);
					cCompLen=0;//�����м�������
				}
			}			
		}else if(lpCntxtPriv->inState==INST_ENGLISH)
		{//Ӣ�ĵ�������
			strResultA = SStringA((char*)pData+1,pData[0]);
			byMask&=~MKI_ASTENGLISH;
		}else if(lpCntxtPriv->inState==INST_USERDEF)
		{//�û��Զ�������
			if(pData[0]==RATE_USERCMD)
			{//����ֱͨ������
				CUtils::CmdExecute(pData);
				byMask=0;
			}else
			{//һ����Զ���
				LPBYTE pCand = pData + 2;
				LPBYTE pComp= pCand +1+pCand[0];
				if(pComp[0]!=0) 
					strResultA = SStringA((char*)pComp + 1, pComp[0]);
				else
					strResultA = SStringA((char*)pCand + 1, pCand[0]);
				byMask=GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT);//������
			}
		}else if(lpCntxtPriv->inState==INST_LINEIME)
		{//�ʻ�����״̬
			strResultA=SStringA((char*)pData+3,pData[2]);
		}
		ClearContext(CPC_ALL);
		InputResult(strResultA,byMask);
		InputEnd();

		InputUpdate();
		if (!bKeepVisible)
		{
			InputHide(byMask &(MKI_ASTENGLISH | MKI_ASTCAND | MKI_ASTSENT | MKI_PHRASEREMIND) || lpCntxtPriv->bShowTip);
		}
	}
end:
	if(bRet && lpCntxtPriv->bWebMode) lpCntxtPriv->bWebMode=FALSE;
	return bRet;
}


//���뷭ҳ����
//BYTE byInput:Vitual Key
BOOL CInputState::KeyIn_All_TurnCandPage(InputContext * lpCntxtPriv,UINT byInput,
										 CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if(lpCntxtPriv->sCandCount )
	{
		//����״̬����������״ֻ̬��ʹ�����¼�ͷ��ҳ,�Ա�������������ͻ
		if(byInput==VK_DOWN || byInput ==VK_PRIOR 
			|| (!(lpbKeyState[VK_SHIFT]&0x80) && byInput==g_SettingsG->byTurnPageDownVK && lpCntxtPriv->sbState!=SBST_ASSOCIATE && lpCntxtPriv->inState != INST_ENGLISH))
		{
			bRet = m_pListener->GoNextCandidatePage();
		}else if(byInput==VK_UP || byInput == VK_NEXT
			|| (!(lpbKeyState[VK_SHIFT]&0x80) && byInput==g_SettingsG->byTurnPageUpVK&& lpCntxtPriv->sbState!=SBST_ASSOCIATE && lpCntxtPriv->inState != INST_ENGLISH))
		{
			bRet = m_pListener->GoPrevCandidatePage();
		}
	}
	if(bRet){
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_ASSOCIATE)
		{//����״̬�ķ�ҳ,������ʱ��ʱ��
			InputHide(TRUE);
		}
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->compMode==IM_SPELL)
		{//ƴ��״̬�Զ����µ�ǰ�ַ�
			short iCand = m_pListener->SelectCandidate(0);
			if(iCand!=-1)
			{//����ҳ������µ�Ԥ�⣬��ͬ���ֶ�ѡ��
				LPBYTE pCand=lpCntxtPriv->ppbyCandInfo[iCand];
				memcpy(lpCntxtPriv->szWord+lpCntxtPriv->byCaret,pCand+2,2);
				lpCntxtPriv->bySelect[lpCntxtPriv->byCaret]=1;
				KeyIn_Spell_Forecast(lpCntxtPriv,lpCntxtPriv->byCaret);
			}
		}
	}

	return bRet;
}

BOOL CInputState::IsTempSpell() const
{
	if(m_ctx.compMode != IM_SPELL) return FALSE;
	if(g_SettingsG->compMode == IM_SPELL) return FALSE;
	return TRUE;
}


BOOL CInputState::KeyIn_Code_Normal(InputContext * lpCntxtPriv,UINT byInput,
									CONST BYTE * lpbKeyState)
{
	BOOL bRet=KeyIn_Code_ChangeComp(lpCntxtPriv,byInput,lpbKeyState);
	if(!bRet) bRet=KeyIn_Code_Symbol(lpCntxtPriv,byInput,lpbKeyState);
	return bRet;
}

//��ͨ����״̬�±���ı䴦��
BOOL CInputState::KeyIn_Code_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	if(byInput==CDataCenter::getSingletonPtr()->GetData().m_compInfo.cWild)
	{
		bCompChar=TRUE;
	}else
	{
		bCompChar = CDataCenter::getSingletonPtr()->GetData().m_compInfo.IsCompChar(byInput);
	}
	if(bCompChar)
	{
		if(lpCntxtPriv->sbState==SBST_ASSOCIATE) 
		{
			ClearContext(CPC_ALL);
		}

		if(KeyIn_Code_IsMaxCode(lpCntxtPriv,MCR_NORMAL) 
			&& !KeyIn_Code_IsValidComp(lpCntxtPriv,byInput)
			&& g_SettingsG->bAutoInput 
			&& !lpCntxtPriv->bWebMode)
		{
			if(lpCntxtPriv->sCandCount)
			{
				BYTE *p = lpCntxtPriv->ppbyCandInfo[0];
				BYTE byType= p[0];
				bool bGbk = p[1]!=0;
				//��ֹ��������ʱ���ִ���:��㲻�����ױ���,�˳���ǰ����,�����㶥����������
				if((byInput<'a' || byInput>'z') && !ISComm_GetCompInfo()->bSymbolFirst) return FALSE;
				if(byType!=RATE_FORECAST && (!bGbk || g_SettingsG->nGbkMode== CSettingsGlobal::GBK_SHOW_NORMAL)) 
				{
					KeyIn_All_SelectCand(lpCntxtPriv,'1',1,lpbKeyState,true);
				}
			}
			lpCntxtPriv->cComp=0;
		}

		if(lpCntxtPriv->cComp==0)
		{
			if(byInput<'a' || byInput>'z')
			{//��㣺Ҫô�����Զ���ģʽ��ݼ������߲�֧�ֿ���Զ���ģʽ�л�
				if(!ISComm_GetCompInfo()->bSymbolFirst) return bRet;//���Ŷ�������
			}
			if(g_SettingsG->bShowOpTip)
			{//�б��������ʾ������ʾ
				lpCntxtPriv->bShowTip=TRUE;
				lpCntxtPriv->iTip = Tips_Rand(FALSE,lpCntxtPriv->szTip);
			}
			//��ʼ��������,���ɿ�ʼ������Ϣ�Ի�ȡ������ʱ���봰�ڵ�����
			InputStart();
			if(!bRet) InputOpen();
		}
		if(lpCntxtPriv->cComp<MAX_COMP)
		{
			lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput;
			bRet=TRUE;
		}
	}else if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->cComp>0) lpCntxtPriv->cComp--;
		if(lpCntxtPriv->bWebMode && lpCntxtPriv->cComp<=2) lpCntxtPriv->bWebMode=FALSE;
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE)
	{
		lpCntxtPriv->cComp=0;
		lpCntxtPriv->sbState=::SBST_NORMAL;
		lpCntxtPriv->bWebMode=FALSE;
		lpCntxtPriv->bShowTip=FALSE;
		bRet=TRUE;
	}else if(byInput==VK_RETURN)
	{
		BOOL bClearComp= lpCntxtPriv->bWebMode==FALSE 
			&& ((g_SettingsG->bEnterClear && !(lpbKeyState[VK_SHIFT]&0x80))||(!g_SettingsG->bEnterClear && lpbKeyState[VK_SHIFT]&0x80 ));
		if(bClearComp)
		{//��ձ���
			lpCntxtPriv->cComp=0;
			lpCntxtPriv->sbState=::SBST_NORMAL;
			bRet=TRUE;
		}else 
		{//��������
			if(lpCntxtPriv->bWebMode && lpbKeyState[VK_SHIFT]&0x80)
			{//��ַģʽ�´������
				lpCntxtPriv->szComp[lpCntxtPriv->cComp]=0;
				ShellExecuteA(NULL,"open",lpCntxtPriv->szComp,NULL,NULL,SW_SHOWDEFAULT);
			}else
			{
				BYTE byMask=0;
				SStringA strResultA;
				if (lpCntxtPriv->cComp == 1 && (lpCntxtPriv->szComp[0]<'a' || lpCntxtPriv->szComp[0]>'z'))
				{
					strResultA += Symbol_Convert(&m_ctx, lpCntxtPriv->szComp[0], lpbKeyState);
				}
				else
				{
					strResultA=SStringA(lpCntxtPriv->szComp, lpCntxtPriv->cComp);
				}

				if(g_SettingsUI->bRecord)
					byMask|=MKI_RECORD;
				if(g_SettingsUI->bSound)
					byMask|=MKI_TTSINPUT;
				InputResult(strResultA,byMask);
			}
			InputEnd();
			InputHide(FALSE);
			ClearContext(CPC_ALL);
			return TRUE;
		}
	}else if(lpCntxtPriv->cComp < MAX_COMP)
	{
		lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput;
		if(lpCntxtPriv->bWebMode || (!g_SettingsG->bAutoInput && byInput>='a' && byInput<='z')) bRet=TRUE;
	}
	if(!lpCntxtPriv->bWebMode)
	{//������ַģʽת��
		if(!bCompChar)
		{
			if(KeyIn_IsWebHeader(lpCntxtPriv->szComp,lpCntxtPriv->cComp))
			{
				lpCntxtPriv->bWebMode=TRUE;
				bRet=TRUE;
			}else if(byInput!=VK_BACK && byInput!=VK_ESCAPE && byInput!=VK_RETURN)
			{
				lpCntxtPriv->cComp--;
			}
		}else
		{
			lpCntxtPriv->bWebMode=KeyIn_IsWebHeader(lpCntxtPriv->szComp,lpCntxtPriv->cComp);
		}
	}

	if(bRet)
	{
		LPBYTE lpbyCand=NULL;
		BYTE byType=0;
		BYTE byMask=MQC_NORMAL|g_SettingsG->byForecast;
		if(g_SettingsG->bAutoMatch) byMask|=MQC_MATCH;
		if(g_SettingsG->bBlendUD) byMask|=MQC_USERDEF;
		if(g_SettingsG->bBlendSpWord) byMask|=MQC_SPCAND;
		if(g_SettingsG->bAutoPrompt) byMask|=MQC_AUTOPROMPT;
		if(g_SettingsG->bOnlySimpleCode) byMask|=MQC_ONLYSC;

		ClearContext(CPC_CAND);

		if(lpCntxtPriv->cComp==0)
		{
			InputEnd();
			InputHide(TRUE);
		}else
		{
			lpCntxtPriv->sbState=::SBST_NORMAL;	//�˳�����״̬
			if(ISComm_QueryCand(lpCntxtPriv->szComp,lpCntxtPriv->cComp,byMask,m_pListener->GetHwnd())==ISACK_SUCCESS)
			{
				PMSGDATA pMsgData=ISComm_GetData();
				LPBYTE pbyData,pCandData;
				short i,sCount,sSingleWords=0;
				memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
				pbyData=m_pbyMsgBuf+1;
				memcpy(&sCount,pbyData,2);
				pbyData+=2;
				lpCntxtPriv->ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*sCount);
				lpCntxtPriv->sCandCount=0;
				pCandData=pbyData;
				//���ҳ���������
				for(i=0;i<sCount;i++)
				{
					//format: rate+bGbk+candLen+cand+compLen+comp
					if(pCandData[2]==2) sSingleWords++;
					pCandData+=pCandData[2]+3;	//ƫ�ƴ�����Ϣ
					pCandData+=pCandData[0]+1;	//ƫ�Ʊ�����Ϣ					
				}
				pCandData=pbyData;
				for(i=0;i<sCount;i++)
				{
					if (pCandData[1]!=0)
					{
						if (!g_SettingsUI->bFilterGbk && (g_SettingsG->nGbkMode !=  CSettingsGlobal::GBK_HIDE ||  sSingleWords<2))
						{//GBK��ʾ���߲���GBK����
							lpCntxtPriv->ppbyCandInfo[lpCntxtPriv->sCandCount++] = pCandData;
						}
					}
					else
					{
						lpCntxtPriv->ppbyCandInfo[lpCntxtPriv->sCandCount++] = pCandData;
					}
					pCandData+=pCandData[2]+3;	//ƫ�ƴ�����Ϣ
					pCandData+=pCandData[0]+1;	//ƫ�Ʊ�����Ϣ
				}
			}
			InputUpdate();
		}
		if(lpCntxtPriv->sCandCount)
		{
			byType=m_pbyMsgBuf[0];
			lpbyCand=lpCntxtPriv->ppbyCandInfo[0];
		}else if(lpCntxtPriv->bWebMode)
		{
			lpCntxtPriv->bShowTip=TRUE;
			lpCntxtPriv->iTip = -1;
			strcpy(lpCntxtPriv->szTip,"��ַģʽ:\n �س�=��ַ����\n Shift+�س�=���");
		}
		if((byType&MCR_AUTOSELECT ||(KeyIn_Code_IsMaxCode(lpCntxtPriv,byType) && g_SettingsG->bAutoInput)) && !lpCntxtPriv->bWebMode)
		{
			if(lpCntxtPriv->sCandCount==1 && lpbyCand[0]!=RATE_FORECAST && (lpbyCand[1]!=0 || g_SettingsG->nGbkMode!=CSettingsGlobal::GBK_SHOW_MANUAL))
			{
				KeyIn_All_SelectCand(lpCntxtPriv,'1',0,lpbKeyState);
			}else
			{
				if(lpCntxtPriv->sCandCount>1)
					CUtils::SoundPlay(_T("ChongMa"));
				else if(lpCntxtPriv->sCandCount==0)
					CUtils::SoundPlay(_T("KongMa"));
				else
					CUtils::SoundPlay(_T("LianXiang"));
			}
		}
	}
	return bRet;
}

BOOL CInputState::KeyIn_Code_Symbol(InputContext * lpCntxtPriv,UINT byInput,
									CONST BYTE * lpbKeyState)
{
	BYTE byMask=0;

	SStringA strResultA;
	if(lpCntxtPriv->sCandCount && 
		lpCntxtPriv->inState==INST_CODING && 
		lpCntxtPriv->sbState==::SBST_NORMAL)
	{
		short iCand = m_pListener->SelectCandidate(0);
		if(iCand!=-1)
		{
			LPBYTE pbyCand=lpCntxtPriv->ppbyCandInfo[iCand];
			strResultA = SStringA((char*)pbyCand+3,pbyCand[2]);
		}
	}

	strResultA += Symbol_Convert(&m_ctx,byInput,lpbKeyState);

	if(g_SettingsUI->bRecord)
		byMask|=MKI_RECORD;
	if(g_SettingsUI->bSound)
		byMask|=MKI_TTSINPUT;

	ClearContext(CPC_ALL);
	InputStart();
	InputResult(strResultA,byMask);
	InputEnd();
	InputHide(FALSE);
	return TRUE;
}

//����״̬�µ����봦��,����״̬�ı䴦��
BOOL CInputState::KeyIn_All_Associate(InputContext * lpCntxtPriv,UINT byInput,
									  CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	SASSERT(lpCntxtPriv->sbState==SBST_ASSOCIATE);

	if((lpCntxtPriv->compMode==IM_SHAPECODE && byInput==g_SettingsG->bySentMode) ||(lpCntxtPriv->compMode==IM_SPELL && byInput==';'))
	{
		if(lpCntxtPriv->sSentLen)
		{//�л����������״̬
			lpCntxtPriv->sbState=SBST_SENTENCE;
			ClearContext(CPC_CAND);
			lpCntxtPriv->sSentCaret=0;
			InputStart();
			InputOpen();
			if(g_SettingsG->bShowOpTip)
			{
				m_ctx.bShowTip=TRUE;
				m_ctx.iTip = -1;
				strcpy(m_ctx.szTip,"��������ѡ��,�ո�����;���ȫ������");
			}

			bRet=TRUE;
		}
	}

	if(!bRet)
	{
		if(lpCntxtPriv->compMode==IM_SPELL)
			bRet=KeyIn_Spell_Normal(lpCntxtPriv,byInput,lpbKeyState);
		else
			bRet=KeyIn_Code_Normal(lpCntxtPriv,byInput,lpbKeyState);
	}
	return bRet;
}

//�������״̬�µ����봦��
BOOL CInputState::KeyIn_All_Sentence(InputContext * lpCntxtPriv,UINT byInput,
									 CONST BYTE * lpbKeyState)
{
	_ASSERT(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_SENTENCE);
	if(byInput==',')
	{
		if(lpCntxtPriv->sSentCaret<lpCntxtPriv->sSentWords)
			lpCntxtPriv->sSentCaret++;
	}else if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->sSentCaret>0)
			lpCntxtPriv->sSentCaret--;
	}else if((lpCntxtPriv->compMode==IM_SHAPECODE && byInput==g_SettingsG->bySentMode) ||(lpCntxtPriv->compMode==IM_SPELL && byInput==';'))
	{
		if(lpCntxtPriv->sSentCaret==0) return FALSE;//����FALSE,�Ա��������������
	}else if(byInput=='.')
	{
		lpCntxtPriv->sSentCaret=lpCntxtPriv->sSentWords;
		KeyIn_Sent_Input(lpCntxtPriv);
	}else if(byInput==VK_SPACE)
	{
		if(lpCntxtPriv->sSentCaret==0) lpCntxtPriv->sSentCaret=lpCntxtPriv->sSentWords;
		KeyIn_Sent_Input(lpCntxtPriv);
	}else if(byInput==VK_ESCAPE)
	{
		ClearContext(CPC_ALL);
		InputEnd();
		InputHide(FALSE);
	}

	if(byInput>='0' && byInput<='9')
		return FALSE;//��������������ѡ��ӿڼ���ִ��
	return TRUE;
}

void  CInputState::KeyIn_Sent_Input(InputContext* lpCntxtPriv)
{
	if(lpCntxtPriv->sSentLen && lpCntxtPriv->sSentCaret)
	{
		BYTE byMask=0;
		if(g_SettingsUI->bRecord)
			byMask|=MKI_RECORD;
		if(g_SettingsUI->bSound)
			byMask|=MKI_TTSINPUT;

		SStringA strResultA((char*)lpCntxtPriv->pbySentWord[0],
			(int)(lpCntxtPriv->pbySentWord[lpCntxtPriv->sSentCaret]-lpCntxtPriv->pbySentWord[0]));

		ClearContext(CPC_ALL);
		InputStart();
		InputResult(strResultA,byMask);
		InputEnd();
		InputHide(FALSE);
	}
}


//Ӣ�ĵ���״̬�����봦��
BOOL CInputState::KeyIn_Code_English(InputContext * lpCntxtPriv,UINT byInput,
									 CONST BYTE* lpbKeyState)
{
	SASSERT(lpCntxtPriv->inState==INST_ENGLISH);
	if(byInput==VK_RETURN)
	{//���뵱ǰӢ��
		SStringA strResult = SStringA((char*)lpCntxtPriv->szComp, lpCntxtPriv->cComp);
		if(g_SettingsUI->bSound) ISComm_TTS(strResult,(char)strResult.GetLength(),MTTS_EN);
		InputResult(strResult,0);
		InputEnd();
		InputHide(FALSE);
		ClearContext(CPC_ALL);
	}else if(byInput==VK_ESCAPE)
	{//�������
		InputEnd();
		InputHide(TRUE);
		ClearContext(CPC_ALL);
	}else
	{
		if(byInput==VK_BACK)
		{
			if(lpCntxtPriv->cComp) 
			{
				lpCntxtPriv->cComp--;
				if(lpCntxtPriv->cComp==0)
				{
					InputEnd();
					InputHide(FALSE);
					lpCntxtPriv->inState=INST_CODING;
				}
			}
		}else
		{
			lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput;
		}
		ClearContext(CPC_CAND);
		if(lpCntxtPriv->cComp)
		{//�����Ѿ��޸�
			if(ISComm_EnQueryCand(lpCntxtPriv->szComp,lpCntxtPriv->cComp)==ISACK_SUCCESS)
			{
				PMSGDATA pData=ISComm_GetData();
				LPBYTE  pBuf=m_pbyMsgBuf;
				BYTE i,byCount=0;
				memcpy(m_pbyMsgBuf,pData->byData,pData->sSize);
				lpCntxtPriv->pbyEnSpell=pBuf;
				pBuf+=1+pBuf[0];
				lpCntxtPriv->pbyEnPhontic=pBuf;
				pBuf+=1+pBuf[0];
				byCount=*pBuf++;
				lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(byCount*sizeof(LPBYTE));
				for(i=0;i<byCount;i++)
				{
					lpCntxtPriv->ppbyCandInfo[i]=pBuf;
					pBuf+=1+pBuf[0];
				}
				lpCntxtPriv->sCandCount=byCount;
			}else
			{
				lpCntxtPriv->pbyEnSpell=NULL;
				lpCntxtPriv->pbyEnPhontic=NULL;
				lpCntxtPriv->pbyEnAstPhrase=NULL;
			}
		}
	}
	return TRUE;
}

//��������
BOOL CInputState::KeyIn_Digital_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										   CONST BYTE* lpbKeyState)
{
	BOOL bRet=FALSE;
	if((byInput>='0' && byInput<='9') || ((byInput=='.' ||byInput==',') && g_SettingsG->bAutoDot) )
	{
		SStringA strResultA((char)byInput);
		InputStart();
		InputResult(strResultA,g_SettingsUI->bRecord?MKI_RECORD:0);
		InputEnd();
		bRet=TRUE;
	}else
	{
		lpCntxtPriv->inState=INST_CODING;
		if(lpCntxtPriv->compMode==IM_SPELL)
			bRet=KeyIn_Spell_Normal(lpCntxtPriv,byInput,lpbKeyState);
		else
			bRet=KeyIn_Code_Normal(lpCntxtPriv,byInput,lpbKeyState);
	}
	return bRet;
}

//�û��Զ�������
BOOL CInputState::KeyIn_UserDef_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										   CONST BYTE* lpbKeyState)
{
	if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->cComp>0)	lpCntxtPriv->cComp--;
	}else if(byInput==VK_ESCAPE)
	{
		ClearContext(CPC_ALL);
		InputEnd();
		InputHide(FALSE);
	}else if(byInput==VK_RETURN)
	{
		if(lpCntxtPriv->cComp)
		{//�������
			//�л�������״̬
			SStringA strResultA(lpCntxtPriv->szComp,lpCntxtPriv->cComp);
			InputResult(strResultA,0);
			InputEnd();
			InputHide(FALSE);
			ClearContext(CPC_ALL);
			return TRUE;
		}
	}else
	{
		lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput;
		lpCntxtPriv->szComp[lpCntxtPriv->cComp]=0;
	}
	ClearContext(CPC_CAND);
	if(lpCntxtPriv->cComp && ISACK_SUCCESS==ISComm_UDQueryCand(lpCntxtPriv->szComp,lpCntxtPriv->cComp))
	{//��ȡ�Զ������,֧�ֶ������
		PMSGDATA pMsgData=ISComm_GetData();
		LPBYTE pbyData;
		short i;
		memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
		pbyData=m_pbyMsgBuf;
		//save auto complete composition string
		lpCntxtPriv->cCompACLen = pbyData[0];
		memcpy(lpCntxtPriv->szCompAutoComplete, pbyData + 1, pbyData[0]);
		pbyData+=pbyData[0]+1;
		memcpy(&lpCntxtPriv->sCandCount,pbyData,2);
		pbyData+=2;
		lpCntxtPriv->ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*lpCntxtPriv->sCandCount);
		for(i=0;i<lpCntxtPriv->sCandCount;i++)
		{
			lpCntxtPriv->ppbyCandInfo[i]=pbyData;
			pbyData+=pbyData[2]+3;//����
			pbyData+=pbyData[0]+1;//����
		}
	}
	InputUpdate();
	return TRUE;
}

//�ʻ�����״̬
BOOL CInputState::KeyIn_Line_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	if(byInput==VK_BACK)
	{//����
		if(lpCntxtPriv->cComp>0)
		{
			lpCntxtPriv->cComp--;
			bCompChar=TRUE;
		}
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE||byInput==VK_RETURN)
	{//�˳�״̬
		ClearContext(CPC_ALL);
		InputUpdate();
		bRet=TRUE;
	}else if(byInput>=VK_NUMPAD1 && byInput<=VK_NUMPAD6)
	{//�༭
		lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput-VK_NUMPAD1+'1';
		lpCntxtPriv->szComp[lpCntxtPriv->cComp]=0;
		bCompChar=TRUE;
		bRet=TRUE;
	}
	if(bCompChar)
	{
		ClearContext(CPC_CAND);
		if(lpCntxtPriv->cComp && ISComm_LineQueryCand(lpCntxtPriv->szComp,lpCntxtPriv->cComp)==ISACK_SUCCESS)
		{//��ѯ����
			PMSGDATA pMsgData=ISComm_GetData();
			short i,sCount=0;
			LPBYTE pBuf=m_pbyMsgBuf;
			memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
			memcpy(&sCount,pBuf,2);
			pBuf+=2;
			lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
			for(i=0;i<sCount;i++)
			{
				lpCntxtPriv->ppbyCandInfo[i]=pBuf;
				pBuf+=pBuf[2]+3;
			}
			lpCntxtPriv->sCandCount=sCount;
		}
	}
	return bRet;
}


BOOL CInputState::TestKeyDown(UINT uKey,LPARAM lKeyData,const BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if ((lKeyData & 0x80000000) && (uKey != VK_SHIFT && uKey !=VK_CONTROL))
		return FALSE;

	//����ݼ�
	int iHotKey = TestHotKey(uKey, lpbKeyState);
	if (iHotKey != -1)
	{
		bRet = TRUE;
		if ( m_pListener)
		{
			switch (iHotKey)
			{
			case HKI_LoadDebugSkin:
				{
					SStringT debugSkin=g_SettingsG->strDebugSkinPath;
					m_pListener->OnCommand(CMD_CHANGESKIN, (LPARAM)&debugSkin);
				}
				break;
			case HKI_UnloadDebugSkin:
				{
					SStringT curSkin=g_SettingsG->strSkin;
					m_pListener->OnCommand(CMD_CHANGESKIN, (LPARAM)&curSkin);
				}
				break;
			case HKI_CharMode:
				m_pListener->OnCommand(CMD_HOTKEY_CHARMODE, 0);
				break;
			case HKI_MakePhrase:
				m_pListener->OnCommand(CMD_HOTKEY_MAKEPHRASE, 0);
				break;
			case HKI_EnSwitch:
				m_pListener->OnCommand(CMD_HOTKEY_ENGLISHMODE, 0);
				break;
			case HKI_Mode:
				m_pListener->OnCommand(CMD_HOTKEY_INPUTMODE, 0);
				break;
			case HKI_ShowRoot:
				m_pListener->OnCommand(CMD_HOTKEY_KEYMAP, 0);
				break;
			case HKI_HideStatus:
				m_pListener->OnCommand(CMD_HOTKEY_HIDESTATUSBAR, 0);
				break;
			case HKI_Query:
				m_pListener->OnCommand(CMD_HOTKEY_QUERYINFO, 0);
				break;
			case HKI_FilterGbk:
				m_pListener->OnCommand(CMD_HOTKEY_FILTERGBK, 0);
				break;
			case HKI_TTS:
				m_pListener->OnCommand(CMD_HOTKEY_TTS, 0);
				break;
			case HKI_Record:
				m_pListener->OnCommand(CMD_HOTKEY_RECORD, 0);
				break;
			}
		}
		m_bPressOther = TRUE;
		if(bRet) return TRUE;
	}

	if(uKey==VK_CAPITAL)
	{
		if(lpbKeyState[VK_CAPITAL]&0x01)
		{
			ClearContext(CPC_ALL);
			InputEnd();
			InputHide(FALSE);
		}
		return FALSE;
	}else if(uKey==VK_SHIFT)
	{
		if(m_bPressCtrl)
		{
			m_bPressOther=TRUE;
			return FALSE;
		}
		if(lKeyData & 0x80000000)//����SHIFT��
		{
			if(!m_bPressOther && m_bPressShift)//��SHIFT�����º�û�а�������������ʾʹ�ÿ�ݹرչ���
			{
				BYTE byKey=(BYTE)(lKeyData>>16);
				m_bPressShift=FALSE; 
				if(!(lpbKeyState[VK_SPACE]&0x80) &&
					g_SettingsG->bySwitchKey==byKey)
				{ //check the scan code
					BOOL bOpen = !m_pListener->GetOpenStatus();
					m_pListener->SetOpenStatus(bOpen);
					if (bOpen)
					{//��������
						if (KeyIn_IsCoding(&m_ctx))
						{
							//��������봰��
							InputOpen();
						}
					}
					else
					{//�ر����룬�����ǰ����������,�򽫵�ǰ����������������༭����
						if (m_ctx.cComp != 0)
						{
							SStringA result(m_ctx.szComp, m_ctx.cComp);
							InputResult(result,0);
							ClearContext(CPC_ALL);
						}
						InputEnd();
						InputHide(FALSE);
					}

				}
			}else//��ԭ״̬
			{
				m_bPressShift=FALSE;
				m_bPressOther=FALSE;
			}	
		}else
		{//����SHIFT������ʼ��״̬
			m_bPressShift=TRUE;
			m_bPressOther=FALSE;
		}
	}else if(uKey==VK_CONTROL)
	{
		if(!m_pListener->GetOpenStatus())
			return FALSE;
		if(m_bPressShift)
		{
			m_bPressOther=TRUE;
			return FALSE;
		}
		if(lKeyData & 0x80000000)//����Ctrl��
		{
			BYTE byKey=(BYTE)(lKeyData>>24);
			if(!m_bPressOther && m_bPressCtrl)//Ctrl�����º�û�а�������������ʾʹ�ÿ�ݹرչ���
			{
				if(g_SettingsG->byTempSpellKey==byKey)
				{//��ʱƴ��
					if(m_ctx.compMode==IM_SPELL)
					{//ƴ������״̬
						if(IsTempSpell())
						{//�˳���ʱƴ��״̬
							m_ctx.compMode = IM_SHAPECODE;
							InputHide(FALSE);
							InputEnd();
							StatusbarUpdate();
							ClearContext(CPC_ALL);
						}
					}else if(g_SettingsG->compMode==IM_SHAPECODE && m_ctx.inState==INST_CODING)
					{//�������״̬��������ʱƴ��״̬
						ClearContext(CPC_ALL);
						m_ctx.compMode=IM_SPELL;
						m_ctx.bShowTip=TRUE;
						m_ctx.iTip = -1;
						strcpy(m_ctx.szTip,"��ʱƴ��:�������Զ���ʾ����");
						InputOpen();
						InputUpdate();
						StatusbarUpdate();
						SLOG_INFO("");
						if (!m_pListener->IsCompositing())
						{//query cursor position
							InputStart();
							InputEnd();
						}
					} 
				}else if(!g_SettingsG->bDisableFnKey && ((g_SettingsG->byTempSpellKey==0 && byKey==0xC1)||g_SettingsG->byTempSpellKey!=0))
				{//���ܼ�, 0xC1=��Ctrl
					if(!KeyIn_IsCoding(&m_ctx)) 
					{
						if(m_ctx.inState==INST_CODING && m_ctx.sbState==SBST_ASSOCIATE
							&& ((m_ctx.compMode==IM_SHAPECODE &&  m_ctx.sSentLen) || (m_ctx.compMode==IM_SPELL && m_ctx.bySyllables==1 && m_ctx.spellData[0].bySpellLen==0) )
							)
						{//�����������״̬
							m_ctx.sbState=SBST_SENTENCE;
							ClearContext(CPC_CAND);//clear candidate list
							m_ctx.sSentCaret=0;
							InputStart();
							InputOpen();
							if(g_SettingsG->bShowOpTip)
							{
								m_ctx.bShowTip=TRUE;
								m_ctx.iTip = -1;
								strcpy(m_ctx.szTip,"�������ģʽ");
							}

							bRet=TRUE;
						}else
						{//�����û��Զ�������״̬
							ClearContext(CPC_ALL);
							m_ctx.inState=INST_USERDEF;
							InputStart();
							bRet=TRUE;
						}
					}else
					{
						if(m_ctx.inState==INST_CODING && m_ctx.sbState==SBST_SENTENCE
							&& ((m_ctx.compMode==IM_SHAPECODE &&  m_ctx.sSentLen) || (m_ctx.compMode==IM_SPELL && m_ctx.bySyllables==1 && m_ctx.spellData[0].bySpellLen==0) )
							)
						{//�������״̬
							m_ctx.sbState=SBST_ASSOCIATE;
							InputEnd();
							bRet=TRUE;
						}else if(m_ctx.inState==INST_USERDEF)
						{//״̬��ԭ
							m_ctx.inState=INST_CODING;
							InputEnd();
							bRet=TRUE;
						}
					}
				}
				m_bPressCtrl=FALSE; 
			}else//��ԭ״̬
			{
				m_bPressCtrl=FALSE;
				m_bPressOther=FALSE;
			}		
		}else
		{//����Ctrl������ʼ��״̬
			m_bPressCtrl=TRUE;
			m_bPressOther=FALSE;
		}
	}else if(!(lpbKeyState[VK_CAPITAL]&0x01)) 
	{
		m_bPressOther=TRUE;
		if(m_pListener->GetOpenStatus())
		{
			if(lpbKeyState[VK_CONTROL]&0x80 && lpbKeyState[VK_SHIFT]&0x80)
			{//Ctrl + Shift
				bRet=(uKey>='0' && uKey<='9');
				return bRet;
			}else if(!(lpbKeyState[VK_CONTROL]&0x80 || lpbKeyState[VK_MENU]&0x80))
			{
				BOOL bCoding=KeyIn_IsCoding(&m_ctx);
				if(uKey>='A' && uKey<='Z')
				{//��ĸ����
					if(m_ctx.compMode==IM_SPELL)
					{
						bRet=TRUE;
					}else 
					{
						char cKey=uKey+0x20;	//��VKת�����ַ�
						bRet = CDataCenter::getSingleton().GetData().m_compInfo.IsCompChar(cKey);
					}
				}else if(uKey>=VK_NUMPAD0 && uKey<=VK_NUMPAD9)
				{//С������������
					bRet=KeyIn_IsNumCode(&m_ctx) || m_ctx.sCandCount;
				}else if(uKey>=VK_MULTIPLY && uKey<=VK_DIVIDE)
				{//С���̹�ʽ����
					bRet=KeyIn_IsNumCode(&m_ctx);
				}else
				{
					bRet=KCompKey[uKey];
					if(!bRet && m_ctx.sCandCount)
					{//����Ƿ����û�����ĸ���������ؿ�ݼ�
						bRet= (uKey==g_SettingsG->by2CandVK 
							|| uKey==g_SettingsG->by3CandVK 
							|| uKey==g_SettingsG->byTurnPageDownVK 
							|| uKey==g_SettingsG->byTurnPageUpVK
							);
					}
					if(!bRet && m_ctx.bySyCaret!=0xFF)
						bRet= uKey==VK_DELETE;
					//!������ص��л��������ǿɴ�ӡ�ַ�,��˲���Ҫ�ж�
				}
				if(!bCoding) 
				{//���Ǳ����������
					if(uKey==VK_BACK || uKey==VK_RETURN || uKey==VK_ESCAPE 
						|| uKey==VK_LEFT || uKey==VK_RIGHT 
						|| uKey==VK_UP || uKey==VK_DOWN //���뷭ҳ��������Ҫ��ҳ��ʱ��
						|| uKey==VK_HOME || uKey==VK_END )//�༭���ƶ���꣬������ǰ��
					{
						if(uKey==VK_RETURN && lpbKeyState[VK_SHIFT]&0x80)
						{//Shift+Return:�ظ�����
							bRet=TRUE;
						}else
						{//�༭���ƶ����
							bRet=FALSE;
							ClearContext(CPC_ALL);
							InputHide(FALSE);
							if(uKey==VK_BACK) 
								ISComm_KeyIn("\b",1,0,m_pListener->GetHwnd());
							else
								ISComm_KeyIn(".",1,g_SettingsUI->bRecord?MKI_RECORD:0,m_pListener->GetHwnd());
						}
					}
				}
				if(!bRet && !bCoding)
					InputHide(FALSE);
			}
		}
	}

	return bRet;
}

void CInputState::OnImeSelect(BOOL bSelect)
{
	SLOG_INFO("fOpen:" << bSelect);
	m_fOpen = bSelect;
}


BOOL CInputState::OnSvrNotify(UINT wp, PMSGDATA pMsg)
{
	SLOG_INFO("code="<<wp<<",m_fOpen:"<<m_fOpen);
	if(wp == NT_KEYIN)
	{//����ʱ���ص���������
		if(m_fOpen)
		{
			InputContext * ctx = &m_ctx;
			if(INST_CODING== ctx->inState && SBST_ASSOCIATE==ctx->sbState)
			{//��֤��ǰ״̬�ǵȴ���������״̬

				ClearContext(CPC_CAND);
				if(pMsg->sSize)
				{
					LPBYTE pbyData=pMsg->byData;
					if(pbyData[0]==MKI_ASTCAND)
					{//ڼ������
						short iCand=0,sCount=0;
						pbyData++;
						memcpy(&sCount,pbyData,2);
						pbyData+=2;
						ctx->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
						for(iCand=0;iCand<sCount;iCand++)
						{//ö������������飺��ͷ����(1BYTE)+����
							ctx->ppbyCandInfo[iCand]=pbyData;
							pbyData+=pbyData[2]+3;
						}
						ctx->sCandCount=sCount;
						SLOG_INFO("��������:"<<ctx->sCandCount);
					}
					if(pbyData-pMsg->byData<pMsg->sSize && pbyData[0]==MKI_ASTENGLISH)
					{//Ӣ������
						BYTE i;
						pbyData++;
						ctx->pbyEnAstPhrase=pbyData;
						pbyData+=1+pbyData[0];
						ctx->sCandCount=*pbyData++;
						ctx->ppbyCandInfo=(LPBYTE*)malloc(ctx->sCandCount*sizeof(LPBYTE));
						for(i=0;i<ctx->sCandCount;i++)
						{
							ctx->ppbyCandInfo[i]=pbyData;
							pbyData+=pbyData[0]+1;
							pbyData+=pbyData[0]+1;						
						}
						SLOG_INFO("Ӣ������:"<<ctx->sCandCount);
					}
					if(pbyData-pMsg->byData<pMsg->sSize && pbyData[0]==MKI_PHRASEREMIND)
					{//���д�����ʾ
						ctx->bShowTip=TRUE;
						ctx->iTip = -1;
						strcpy(ctx->szTip,"ϵͳ����:");
						memcpy(ctx->szTip+9,pbyData+2,pbyData[1]);
						ctx->szTip[9+pbyData[1]]=0;
						pbyData+=2+pbyData[1];
						SLOG_INFO("���д�����ʾ:"<<ctx->szTip);
					}
					if(pbyData-pMsg->byData<pMsg->sSize && pbyData[0]==MKI_ASTSENT)
					{//��������
						short sLen=0,iWord=0,sOffset=0;
						pbyData++;
						memcpy(&sLen,pbyData,2);
						pbyData+=2;
						while(sOffset<sLen)
						{
							ctx->pbySentWord[iWord++]=pbyData;
							sOffset+=(pbyData[0]&0x80)?2:1;
							pbyData+=(pbyData[0]&0x80)?2:1;
						}
						ctx->pbySentWord[iWord]=pbyData;
						ctx->sSentWords=iWord;
						ctx->sSentCaret=0;
						ctx->sSentLen=sLen;
						SLOG_INFO("��������:"<<SStringA((char*)ctx->pbySentWord[0],sLen));
					}
				}
				if(ctx->bShowTip || ctx->sCandCount || ctx->sSentLen)
				{//�������������������
					SLOG_INFO("Update Input Window");
					if (ctx->sCandCount == 0 && g_SettingsG->bShowOpTip && !ctx->bShowTip)
					{//û�к�ѡʱ,�ں�ѡλ����ʾ������ʾ
						ctx->bShowTip=TRUE;
						if(ctx->sSentLen )
							strcpy(ctx->szTip,"�ֺŽ����������"),ctx->iTip = -1;
						else
							ctx->iTip = Tips_Rand(ctx->compMode == IM_SPELL, ctx->szTip);
					}
					InputUpdate();
				}else
				{//�رմ���
					SLOG_INFO("Close Input Window");
					InputHide();
				}

			}else
			{
				SLOGFMTI("IME state is not waitting for notify,inState=%d,sbState=%d",ctx->inState,ctx->sbState);
			}
		}else
		{
			SLOG_INFO("Close Input Window");
			InputHide();
		}
	}
	return FALSE;
}
