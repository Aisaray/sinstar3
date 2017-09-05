#include "StdAfx.h"
#include "InputState.h"

CInputState::CInputState(void):m_pListener(NULL),m_bCoding(FALSE)
{
	memset(&m_ctx,0,sizeof(InputContext));
	ClearContext(CPC_ALL);
	m_ctx.compMode  = g_SettingsG.compMode;
	m_ctx.bShowTip = FALSE;
	m_pbyMsgBuf=(LPBYTE)malloc(MAX_BUF_ACK);
}

CInputState::~CInputState(void)
{
	free(m_pbyMsgBuf);
}

#define MKI_ALL (MKI_RECORD|MKI_TTSINPUT|MKI_AUTOPICK)
BYTE CInputState::GetKeyinMask(BOOL bAssociate,BYTE byMask)
{
	BYTE byRet=0;
	if(g_SettingsG.bAutoMatch) byRet|=(MKI_AUTOPICK&byMask);
	if(g_SettingsL.bRecord) byRet|=(MKI_RECORD&byMask);
	if(g_SettingsG.bTTSInput) byRet|=(MKI_TTSINPUT&byMask);
	if(bAssociate)
	{
		if(g_SettingsG.bAstSent) byRet|=MKI_ASTSENT;
		switch(g_SettingsG.byAstMode)
		{
		case AST_ENGLISH:byRet|=MKI_ASTENGLISH;break;
		case AST_CAND:byRet|=MKI_ASTCAND;break;
		case AST_PHRASEREMIND:byRet|=MKI_PHRASEREMIND;break;
		default:break;
		}
	}
	return byRet;
}

void CInputState::ClearContext(UINT dwMask)
{
	if(dwMask&CPC_COMP)
	{
		m_ctx.szInput[0]=0;
		m_ctx.cInput = 0;
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
		m_ctx.bPYBiHua=FALSE;
	}
}


void CInputState::OnInputStart()
{
	if(m_bCoding) return;
	m_bCoding = TRUE;
	SASSERT(m_pListener);
	m_pListener->OnInputStart();
}

void CInputState::OnInputEnd(const SStringT &strResult)
{
	SASSERT(m_pListener);
	m_pListener->OnInputEnd(strResult);
	ClearContext(CPC_ALL);
	m_bCoding = FALSE;
}

BOOL CInputState::HandleKeyDown(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState)
{
	if(isprint(vKey))
	{
		vKey = tolower(vKey);
	}

	if(UpdateInputMode(vKey,uScanCode,lpbKeyState))
		return TRUE;

	if(HandleShapeCodeKeyDown(vKey,uScanCode,lpbKeyState))
		return TRUE;
	
	if(HandleSpellKeyDown(vKey,uScanCode,lpbKeyState))
		return TRUE;

	return FALSE;
}

BOOL CInputState::UpdateInputMode(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState)
{
	if( m_ctx.compMode == IM_SHAPECODE
		&& m_ctx.inState == INST_CODING
		&& m_ctx.cInput == 0 
		&& vKey == CDataCenter::GetAutoLockerInstance()->GetData().m_compInfo.cWild)
	{
		m_ctx.inState = INST_USERDEF;
		OnInputStart();
		return TRUE;
	}
	if(m_ctx.inState == INST_USERDEF
		&& vKey == VK_ESCAPE)
	{
		ClearContext(CPC_ALL);
		m_ctx.inState = INST_CODING;
		OnInputEnd(SStringT());
		return TRUE;
	}
	if(m_ctx.compMode == IM_SHAPECODE
		&& vKey == VK_CONTROL
		&& m_ctx.cInput == 0)
	{
		m_ctx.compMode = IM_SPELL;
		return TRUE;
	}
	if(m_ctx.compMode == IM_SPELL
		&& g_SettingsG.compMode == IM_SHAPECODE
		&& vKey == VK_ESCAPE)
	{
		ClearContext(CPC_SPELL|CPC_COMP|CPC_CAND);
		m_ctx.compMode = IM_SHAPECODE;
		return TRUE;
	}
	if(g_SettingsG.compMode == IM_SPELL && !m_bCoding)
	{
		if(vKey == 'u' || vKey == 'v')
		{
			m_ctx.inState = INST_USERDEF;
			OnInputStart();
			return TRUE;
		}
		if(vKey == 'i')
		{
			m_ctx.inState = INST_LINEIME;
			m_pListener->OnInputStart();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CInputState::HandleShapeCodeKeyDown(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState)
{
	if(m_ctx.compMode != IM_SHAPECODE)
		return FALSE;

	if(isprint(vKey))
	{
		if(!CDataCenter::GetAutoLockerInstance()->GetData().m_compInfo.IsCompChar(vKey))
		{
			return FALSE;
		}
		OnInputStart();
		m_ctx.szInput[m_ctx.cInput++]=((char)vKey);
	}else if(vKey == VK_BACK)
	{
		m_ctx.cInput -- ;
	}else if(vKey == VK_ESCAPE)
	{
		m_ctx.cInput = 0;
		OnInputEnd(_T(""));
		return TRUE;
	}

	BYTE byMask=MQC_NORMAL|g_SettingsG.byForecast;
	if(g_SettingsG.bAutoMatch) byMask|=MQC_MATCH;
	if(g_SettingsG.bBlendUD) byMask|=MQC_USERDEF;
	if(g_SettingsG.bBlendSpWord) byMask|=MQC_SPCAND;
	if(g_SettingsG.bAutoPrompt) byMask|=MQC_AUTOPROMPT;
	if(g_SettingsG.bOnlySimpleCode) byMask|=MQC_ONLYSC;

	SStringA strCompA(m_ctx.szInput,m_ctx.cInput);
	if(ISACK_SUCCESS == ISComm_QueryCand(strCompA,strCompA.GetLength(),byMask,m_pListener->GetHwnd()))
	{
		PMSGDATA pMsgData=ISComm_GetData();
		LPBYTE pbyData,pCandData;
		short i,sCount,sSingleWords=0;
		memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
		pbyData=m_pbyMsgBuf+1;
		memcpy(&sCount,pbyData,2);
		pbyData+=2;
		m_ctx.ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*sCount);
		m_ctx.sCandCount=0;
		pCandData=pbyData;
		//���ҳ���������
		for(i=0;i<sCount;i++)
		{
			if(pCandData[1]==2) sSingleWords++;
			pCandData+=pCandData[1]+2;	//ƫ�ƴ�����Ϣ
			pCandData+=pCandData[0]+1;	//ƫ�Ʊ�����Ϣ					
		}
		pCandData=pbyData;
		for(i=0;i<sCount;i++)
		{
			if(g_SettingsG.nGbkMode!=0 || pCandData[0]!=RATE_GBK || sSingleWords<2)
			{//GBK��ʾ���߲���GBK����
				m_ctx.ppbyCandInfo[m_ctx.sCandCount++]=pCandData;
			}
			pCandData+=pCandData[1]+2;	//ƫ�ƴ�����Ϣ
			pCandData+=pCandData[0]+1;	//ƫ�Ʊ�����Ϣ
		}
	}
	return TRUE;
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

//ƴ��״̬�¸��º�ѡ�ʴ���
void CInputState::KeyIn_Spell_UpdateCandList(InputContext *  lpCntxtPriv,BYTE byCaret)
{
	DWORD dwRet=ISACK_ERROR;
	BYTE byMask=0;
	ClearContext(CPC_CAND);
	if(lpCntxtPriv->spellData[byCaret].bySpellLen==0) return;
	if(g_SettingsG.bPYPhraseFirst) byMask=0x01;
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
			if(byCaret==(lpCntxtPriv->bySyllables-1))
			{
				if(strncmp((char*)(lpCntxtPriv->szWord+lpCntxtPriv->bySyllables-pBuf[1]/2),(char*)pBuf+2,pBuf[1])!=0) //ȥ����Ԥ����ͬ������
					lpCntxtPriv->ppbyCandInfo[sValidCount++]=pBuf;
			}else
			{
				if(strncmp((char*)(lpCntxtPriv->szWord+byCaret),(char*)pBuf+2,pBuf[1])!=0)
					lpCntxtPriv->ppbyCandInfo[sValidCount++]=pBuf;
			}
			pBuf+=pBuf[1]+2;
		}
		lpCntxtPriv->sCandCount=sValidCount;
	}
}

//ƴ��״̬���Ƶ��༭����
BOOL CInputState::KeyIn_Spell_MoveCaret(InputContext *lpCntxtPriv, BYTE byInput,
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
			if(lpCntxtPriv->bPYBiHua)
			{
				lpCntxtPriv->bPYBiHua=FALSE;
				lpCntxtPriv->szBiHua[0]=0;
			}
			KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
		}
	}else
	{
		bRet=FALSE;
		//�������
		if(byInput==VK_LEFT)
		{
			if(lpCntxtPriv->bySyCaret>0) 
				lpCntxtPriv->bySyCaret--;
			bRet=TRUE;
		}else if(byInput==VK_RIGHT)
		{
			if(lpCntxtPriv->bySyCaret<lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen)
				lpCntxtPriv->bySyCaret++;
			bRet=TRUE;
		}else if(byInput==VK_HOME)
		{
			lpCntxtPriv->bySyCaret=0;
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


BOOL CInputState::KeyIn_Spell_SyFix(InputContext * lpCntxtPriv,BYTE byInput,
					   CONST BYTE * lpbKeyState)
{
	if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0) return FALSE;
	if(lpCntxtPriv->bySyCaret==0xFF)
	{
		if(!lpCntxtPriv->bPYBiHua && byInput==VK_RETURN && !(lpbKeyState[VK_SHIFT]&0x80))
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
// 				strcpy(g_szTip,"���ڴ���!");
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
BOOL CInputState::KeyIn_Spell_ChangeComp(InputContext* lpCntxtPriv,BYTE byInput,
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
			OnInputStart();
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
		}else if(g_SettingsG.compMode!=IM_SPELL)
		{//temp spell mode will ignore VK_BACK key.
			bRet=TRUE;
		}
		if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
		{
			OnInputEnd(_T(""));
//			if(!g_bTempSpell) MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SETDELAY);
		}
	}else if(byInput==VK_ESCAPE)
	{
		ClearContext(CPC_ALL);
		OnInputEnd(_T(""));
		//MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,(g_bTempSpell?IMN_PRIV_UPDATESTATUS:0)|IMN_PRIV_SETDELAY);
		if(g_SettingsG.compMode != IM_SPELL)
		{//restore shape code input mode
			m_ctx.compMode=IM_SHAPECODE;
			//Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
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
		memcpy(lpCntxtPriv->szInput+iWord*2,lpCntxtPriv->szWord[i],2);
		iWord++;
	}
	lpCntxtPriv->cInput=iWord*2;
	return bRet;
}


//ƴ��״̬�½�������뵽�༭����
BOOL CInputState::KeyIn_Spell_InputText(InputContext* lpCntxtPriv,BYTE byInput,
						   CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if(lpCntxtPriv->bySyllables<=1 && lpCntxtPriv->spellData[0].bySpellLen==0) return FALSE;

	if(byInput==VK_SPACE)
	{
		BYTE bySpellID[MAX_SYLLABLES][2];//�û�����ĺ��ֵ�ƴ��������ID
		SStringA strResult;
		BOOL bGetSpID=KeyIn_Spell_GetSpellInput(lpCntxtPriv,bySpellID);
		if(lpCntxtPriv->cInput)
		{
			strResult = SStringA(lpCntxtPriv->szInput,lpCntxtPriv->cInput);
		}
		OnInputEnd(S_CA2T(strResult));

		//Plugin_TextInput(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,NULL,0,FALSE);

		ClearContext(CPC_ALL);
		//���û������ύ������������
		if(bGetSpID) ISComm_SpellMemoryEx(strResult,strResult.GetLength(),bySpellID);
		//KeyIn_InputAndAssociate(lpCntxtPriv,lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,GetKeyinMask(!g_bTempSpell,MKI_ALL));
		if(lpCntxtPriv->bPYBiHua)
		{//�˳��ʻ�ѡ������״̬
			lpCntxtPriv->bPYBiHua=FALSE;
			lpCntxtPriv->szBiHua[0]=0;
		}		
		if(g_SettingsG.compMode != IM_SPELL) 
		{//��ʱƴ��ģʽ�Զ��������ƴ���ı���
			lpCntxtPriv->bShowTip=TRUE;
			//GetShapeComp(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen);
			lpCntxtPriv->compMode = IM_SHAPECODE;
//			Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		}
		bRet=TRUE;
	}else if ( byInput == VK_RETURN && g_SettingsG.compMode == IM_SPELL && !lpCntxtPriv->bPYBiHua)
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
		ClearContext(CPC_ALL);

		//֪ͨӦ�ó����������
		OnInputEnd(S_CA2T(strResultA));
		ISComm_KeyIn(strResultA,(short)strResultA.GetLength(),GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT),m_pListener->GetHwnd());
		bRet=TRUE;
	}
	return bRet;
}

//ͨ����д��������λ����
BOOL CInputState::KeyIn_Spell_Locate(InputContext *lpCntxtPriv,BYTE byInput,
						CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if(byInput>='A' && byInput<='Z' && !lpCntxtPriv->bPYBiHua && lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen!=0)
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


BOOL CInputState::KeyIn_Spell_Normal(InputContext * lpCntxtPriv,BYTE byInput,
						CONST BYTE * lpbKeyState)
{
	BOOL bHandle=KeyIn_Spell_SyFix(lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_ChangeComp(lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_InputText(	lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_Locate(lpCntxtPriv,byInput,lpbKeyState);
//	if(!bHandle) bHandle=KeyIn_Spell_Symbol(lpCntxtPriv,byInput,lpbKeyState);
	return bHandle;
}


BOOL CInputState::HandleSpellKeyDown(UINT byInput,UINT uScanCode,const BYTE * lpbKeyState)
{
	if(m_ctx.compMode != IM_SPELL)
		return FALSE;

	//����ƴ���������ƶ�
	BOOL bHandle=KeyIn_Spell_MoveCaret(&m_ctx,byInput,lpbKeyState);
	if(!bHandle && byInput==VK_DELETE) bHandle=KeyIn_Spell_SyFix(&m_ctx,byInput,lpbKeyState);//����VK_DELETE
	
	if(m_ctx.inState != INST_CODING)
		return bHandle;

	if(m_ctx.sbState==::SBST_NORMAL)
	{
		if(m_ctx.bPYBiHua)
		{//�ʻ��������룬��֧�����ܼ�
			BOOL bInputKey=FALSE;
			if(byInput>=VK_NUMPAD1&&byInput<=VK_NUMPAD5)
			{//С��������
				bInputKey=TRUE;
			}else
			{//���õ�ת����
				int i;
				for( i=0;i<5;i++)
				{
					if(g_SettingsG.byLineKey[i]==byInput)
					{
						byInput=VK_NUMPAD1+i;
						bInputKey=TRUE;
						break;
					}
				}
				if(byInput==VK_ESCAPE || byInput==VK_BACK) bInputKey=TRUE;
			}
// 			bHandle=KeyIn_PYBiHua_ChangComp(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
// 			if(!bHandle) bHandle=KeyIn_Spell_InputText(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
		}else
		{
			bHandle=KeyIn_Spell_Normal(&m_ctx,byInput,lpbKeyState);
		}
	}
	return bHandle;
}



