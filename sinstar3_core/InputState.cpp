#include "StdAfx.h"
#include "InputState.h"
#include <MMSystem.h>
#include <ShellAPI.h>
#pragma comment(lib,"Winmm.lib")

const BYTE KCompKey[] ={0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,        // 00-0F
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


BOOL Tips_Rand(BOOL bSpell,char *pszBuf)
{
// 	if(g_bOpLoaded)
// 	{
// 		if(bSpell)
// 		{
// 			if(g_nOpTipSpell||g_nOpTipAll)
// 			{
// 				nIndex=rand()%(g_nOpTipSpell+g_nOpTipAll);
// 				if(nIndex<g_nOpTipAll)
// 					pszOpTip=g_ppszOpTipAll[nIndex];
// 				else
// 					pszOpTip=g_ppszOpTipSpell[nIndex-g_nOpTipAll];
// 			}
// 		}else
// 		{
// 			if(g_nOpTipShape||g_nOpTipAll)
// 			{
// 				nIndex=rand()%(g_nOpTipShape+g_nOpTipAll);
// 				if(nIndex<g_nOpTipAll)
// 					pszOpTip=g_ppszOpTipAll[nIndex];
// 				else
// 					pszOpTip=g_ppszOpTipShape[nIndex-g_nOpTipAll];
// 			}
// 		}
// 	}
	strcpy(pszBuf,"test");
	return TRUE;
}


void SoundPlay(LPCTSTR pszSound)
{
	if(g_SettingsG.nSoundAlert==1)
	{
		SStringT strPath = SStringT().Format(_T("%s\\sound\\%s.wav"),theModule->GetDataPath(),pszSound);
		PlaySound(strPath,NULL,SND_ASYNC|SND_FILENAME);
	}else if(g_SettingsG.nSoundAlert==2)
	{
		MessageBeep(1000);
	}
}


BOOL CmdExecute(BYTE * pszBuf)
{
	UINT uRet=FALSE;
	char *pParam=NULL;
	char *pCmd=(char*)pszBuf+pszBuf[1]+2+1;
	if(pCmd[0]=='\"')
	{
		pParam=strstr(pCmd,"\" ");
		if(pParam) {pParam[1]=0;pParam+=2;}
	}else
	{
		pParam=strstr(pCmd," ");
		if(pParam) pParam[0]=0,pParam++;
	}
	uRet=(UINT)ShellExecuteA(NULL,"open",pCmd,pParam,NULL,SW_SHOWDEFAULT);
	if(uRet<=32) uRet=(UINT)ShellExecuteA(NULL,"explorer",pCmd,NULL,NULL,SW_SHOWDEFAULT);
	return uRet>32;
}


//���Ŵ���
//BYTE byInput:��������
//char *pBuf:ת��������
int Symbol_Convert(UINT byInput,char *pBuf,CONST LPBYTE lpbKeyState)
{
	int nRet=0;
	if(g_SettingsL.bCharMode)
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
	return nRet;
}

BOOL IsDigitEx(char c)
{
	if(isdigit(c)) return TRUE;
	return c=='.';
}

//�жϵ�ǰ״̬�����ּ����ǲ����нϱ���,�Զ���״̬���ʻ�����״̬��֧�����ּ��̵�������
BOOL KeyIn_IsNumCode(InputContext * lpCntxtPriv)
{
	return (lpCntxtPriv->inState==INST_USERDEF
		&& ((lpCntxtPriv->cInput>0 && IsDigitEx(lpCntxtPriv->szInput[0])))
		);
}

BOOL KeyIn_Code_IsMaxCode(InputContext * lpCntxtPriv,BYTE byType)
{
	if(lpCntxtPriv->cInput==0) return FALSE;
	return (lpCntxtPriv->cInput>=ISComm_GetCompInfo()->cCodeMax &&  byType==MCR_NORMAL);
}

BOOL KeyIn_Code_IsValidComp(InputContext * lpCntxtPriv,char cInput)
{
	BYTE byMask=0;
	if(lpCntxtPriv->cInput==MAX_COMP-1) return FALSE;
	if(g_SettingsG.bBlendUD) byMask|=MQC_USERDEF;
	if(g_SettingsG.bBlendSpWord) byMask|=MQC_SPCAND;
	lpCntxtPriv->szInput[lpCntxtPriv->cInput]=cInput;
	return ISComm_CheckComp(lpCntxtPriv->szInput,lpCntxtPriv->cInput+1,byMask);
}

//���һ�������Ƿ�Ϊ�������ַ��ͷ
BOOL KeyIn_IsWebHeader(const char *pszHeader,int nLen)
{
	BOOL bRet=FALSE;
	int i=0,iBegin=0,iEnd;
	while(g_SettingsG.szWebHeader[i])
	{
		iEnd=iBegin;
		for(;;)
		{
			if(g_SettingsG.szWebHeader[iEnd]==0) break;
			if(g_SettingsG.szWebHeader[iEnd]==0x20) break;
			iEnd++;
		}
		if(nLen==iEnd-iBegin && strncmp(pszHeader,g_SettingsG.szWebHeader+iBegin,nLen)==0)
		{
			bRet=TRUE;
			break;
		}
		if(g_SettingsG.szWebHeader[iEnd]==0) break;//������ַ�������
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
				bOpen=lpCntxtPriv->cInput!=0;
			}else//if(lpCntxtPriv->compMode==IM_SPELL)
			{//ƴ��
				bOpen=(lpCntxtPriv->bySyllables>1 || lpCntxtPriv->spellData[0].bySpellLen>0);
				if(g_SettingsG.compMode==IM_SHAPECODE) bOpen=TRUE;
			}
		}else if(lpCntxtPriv->sbState==SBST_SENTENCE)
			bOpen=TRUE;
	}else if(lpCntxtPriv->inState==INST_ENGLISH)
	{//Ӣ������
		bOpen=lpCntxtPriv->cInput!=0;
	}else if(lpCntxtPriv->inState==INST_USERDEF || lpCntxtPriv->inState==INST_LINEIME)
	{
		bOpen=TRUE;
	}
	return bOpen;
}

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
	SLOG_INFO("bCoding:"<<m_bCoding);
	if(m_bCoding) return;
	m_bCoding = TRUE;
	SASSERT(m_pListener);
	m_pListener->OnInputStart();
}

void CInputState::OnInputChange(const SStringT &strResult,byte byAstMask/*=0*/)
{
	SLOG_INFO("bCoding:"<<m_bCoding<<" result:"<<strResult<<" astMask:"<<byAstMask);

	SASSERT(m_pListener);
	m_pListener->OnInputChange(strResult);
	if(byAstMask!=0)
	{
		SStringA strResultA = S_CT2A(strResult);
		KeyIn_InputAndAssociate(&m_ctx,strResultA,(short)strResultA.GetLength(),byAstMask);
	}
	CDataCenter::GetAutoLockerInstance()->GetData().m_cInputCount+=strResult.GetLength();
}

void CInputState::OnInputChange(const SStringA &strResult,byte byAstMask/*=0*/)
{
	OnInputChange(S_CA2T(strResult),byAstMask);
}

void CInputState::OnInputEnd(BOOL bDelay)
{
	SLOG_INFO("bCoding:"<<m_bCoding<<" bDelay:"<<bDelay);

	SASSERT(m_pListener);
	m_pListener->OnInputEnd(bDelay?0:5000);
	ClearContext(CPC_ALL);
	m_bCoding = FALSE;
}

BOOL CInputState::HandleKeyDown(UINT uVKey,UINT uScanCode,const BYTE * lpbKeyState)
{
	SLOG_INFO("uVKey:"<<uVKey<<" uScanCode:"<<uScanCode);
	BOOL bHandle=FALSE;
	//����ʹ��VK�����ݼ������뷭ҳ��
	if(!bHandle && lpbKeyState[VK_CONTROL] & 0x80 )
	{//�����ݼ�: todo

	}

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
				&& (!g_SettingsG.bCandSelNoNum || g_SettingsG.compMode==IM_SPELL) //δ��ֹ����ѡ������
				)//��������ѡ������
				)
			{//����������ֵ�ASCII��
				if((uVKey>=VK_NUMPAD0 && uVKey<=VK_NUMPAD9))
					byCandIndex=uVKey-VK_NUMPAD0+'0';
				else
					byCandIndex=uVKey;
			}
		}else if(g_SettingsG.b23CandKey && !(lpbKeyState[VK_SHIFT]&0x80))
		{//SHIFT ģʽ�²�����
			UINT uVk=MapVirtualKey(uScanCode,1);
			if(uVk==g_SettingsG.by2CandVK && lpCntxtPriv->sCandCount>=2) byCandIndex='2';
			if(uVk==g_SettingsG.by3CandVK && lpCntxtPriv->sCandCount>=3) byCandIndex='3';
		}
		if(byCandIndex )
		{
			if(!((lpCntxtPriv->sbState==SBST_ASSOCIATE && g_SettingsG.byAstMode==AST_ENGLISH && !(lpbKeyState[VK_CONTROL]&0x80))//Ӣ������״ֻ̬�а���Ctrl�Ž������ѡ��
				|| (g_SettingsG.compMode==IM_SPELL && lpCntxtPriv->inState==INST_CODING && (uVKey==VK_SPACE || uVKey=='\'') ) )	//ƴ������״̬�µ���������ֶ�����  0xde=VkKeyScan('\'')
				)
			{
				bHandle=KeyIn_All_SelectCand(lpCntxtPriv,byCandIndex,0,lpbKeyState);
				if(bHandle) OnInputEnd(FALSE);
			}
		}
		if(!bHandle) bHandle=KeyIn_All_TurnCandPage(lpCntxtPriv,uVKey,lpbKeyState);
	}

	if(isprint(uVKey))
	{
		uVKey = tolower(uVKey);
	}

	//����ƴ���������ƶ�
	if(!bHandle && lpCntxtPriv->compMode==IM_SPELL && lpCntxtPriv->inState==INST_CODING)
	{
		bHandle=KeyIn_Spell_MoveCaret(lpCntxtPriv,uVKey,lpbKeyState);
		if(!bHandle && uVKey==VK_DELETE) bHandle=KeyIn_Spell_SyFix(lpCntxtPriv,uVKey,lpbKeyState);//����VK_DELETE
	}


	if(!bHandle && uVKey)
	{
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
					if(lpCntxtPriv->cInput==0)
					{
						bReadyEn=TRUE;
						bReadyDgt=TRUE;
					}
				}
			}else if(lpCntxtPriv->sbState==SBST_ASSOCIATE)
			{
				bReadyEn=TRUE;
				bReadyDgt=TRUE;
				if((g_SettingsG.byAstMode==AST_CAND ||(g_SettingsG.byAstMode==AST_ENGLISH &&(lpbKeyState[VK_CONTROL]&0x80))) && lpCntxtPriv->sCandCount)
					bReadyDgt=FALSE;
			}
			if(IsTempSpell() && (bReadyEn || bReadyDgt) && (isdigit(uVKey) || isupper(uVKey)))
			{//temp spell mode
				SoundPlay(_T("error"));
				return FALSE;
			}
			if((bReadyEn || bReadyDgt) && lpCntxtPriv->bShowTip) //�ر�tip
				lpCntxtPriv->bShowTip=FALSE;
			if(bReadyEn && uVKey>='a' && uVKey<='z')
			{//��д���룬���л���Ӣ��״̬
				ClearContext(CPC_ALL);
				if(g_SettingsL.bEnglish)
				{
					lpCntxtPriv->inState=INST_ENGLISH;
//					Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
					//ȷ�������봰��
					OnInputStart();
				}
			}else if(bReadyDgt && uVKey>='0' && uVKey<='9')
			{//�������룬������������״̬
				ClearContext(CPC_ALL);
				lpCntxtPriv->inState=INST_DIGITAL;
//				Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
				m_pListener->CloseInputWnd(0);
			}
		}
		if(lpCntxtPriv->inState==INST_CODING)
		{
			if(lpCntxtPriv->sbState==::SBST_NORMAL)
			{//��������
				if(lpCntxtPriv->compMode==IM_SPELL)
				{//ƴ��״̬
					if(lpCntxtPriv->bPYBiHua)
					{//�ʻ��������룬��֧�����ܼ�
						BOOL bInputKey=FALSE;
						if(uVKey>=VK_NUMPAD1&&uVKey<=VK_NUMPAD5)
						{//С��������
							bInputKey=TRUE;
						}else
						{//���õ�ת����
							int i;
							for( i=0;i<5;i++)
							{
								if(g_SettingsG.byLineKey[i]==uVKey)
								{
									uVKey=VK_NUMPAD1+i;
									bInputKey=TRUE;
									break;
								}
							}
							if(uVKey==VK_ESCAPE || uVKey==VK_BACK) bInputKey=TRUE;
						}
						bHandle=KeyIn_PYBiHua_ChangComp(lpCntxtPriv,uVKey,lpbKeyState);
						if(!bHandle) bHandle=KeyIn_Spell_InputText(lpCntxtPriv,uVKey,lpbKeyState);
					}else
					{
						bHandle=KeyIn_Spell_Normal(lpCntxtPriv,uVKey,lpbKeyState);
					}
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
					if(g_SettingsG.byLineKey[i]==uVKey)
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
		if(!bHandle) SoundPlay(_T("error"));
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


BOOL CInputState::KeyIn_Spell_SyFix(InputContext * lpCntxtPriv,UINT byInput,
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
			OnInputEnd(TRUE);
		}
	}else if(byInput==VK_ESCAPE)
	{
		OnInputEnd(TRUE);
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
		if(lpCntxtPriv->cInput)
		{
			strResult = SStringA(lpCntxtPriv->szInput,lpCntxtPriv->cInput);
		}
		OnInputChange(strResult);
		OnInputEnd(FALSE);

		//Plugin_TextInput(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,NULL,0,FALSE);

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
			GetShapeComp(lpCntxtPriv->szInput,lpCntxtPriv->cInput);
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
		//֪ͨӦ�ó����������
		OnInputChange(strResultA);
		OnInputEnd(TRUE);
		ISComm_KeyIn(strResultA,(short)strResultA.GetLength(),GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT),m_pListener->GetHwnd());
		bRet=TRUE;
	}
	return bRet;
}

//ͨ����д��������λ����
BOOL CInputState::KeyIn_Spell_Locate(InputContext *lpCntxtPriv,UINT byInput,
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
		if(lpCntxtPriv->cInput)
		{
			if(bGetSpID) ISComm_SpellMemoryEx(lpCntxtPriv->szInput,lpCntxtPriv->cInput,bySpellID);
			strResultA = SStringA(lpCntxtPriv->szInput,lpCntxtPriv->cInput);
		}
		//lpCompStr->dwResultStrLen+=Symbol_Convert(byInput,pResult+lpCompStr->dwResultStrLen,lpbKeyState);
		OnInputStart();
		OnInputChange(strResultA,0);
		OnInputEnd(FALSE);
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
						  CONST BYTE * lpbKeyState)
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

			if(lpCntxtPriv->inState==INST_CODING)
			{//��ͨ��������
				if(lpCntxtPriv->sbState==::SBST_NORMAL)
				{//��������״̬
					if(lpCntxtPriv->compMode==IM_SPELL)
					{//ƴ������״̬
						BYTE iWord=0;
						BYTE byPhraseLen=pData[1]/2;
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
							memcpy(lpCntxtPriv->szWord[iWord+byCaret],pData+2+iWord*2,2);
							lpCntxtPriv->bySelect[iWord+byCaret]=byPhraseLen;
							iWord++;
						}
						if(lpCntxtPriv->bPYBiHua)
						{//�˳��ʻ�����
							lpCntxtPriv->bPYBiHua=FALSE;
							lpCntxtPriv->szBiHua[0]=0;
						}
						if(byPhraseLen>lpCntxtPriv->bySyllables)
						{//���������볤������
							SStringA strResultA((char*)pData+2,pData[1]);
							BOOL isTempSpell = IsTempSpell();
							OnInputChange(strResultA,GetKeyinMask(!isTempSpell,MKI_ALL));
							OnInputEnd(TRUE);
							if(isTempSpell) 
							{//��ʱƴ��ģʽ��������ֵı���
								lpCntxtPriv->bShowTip=TRUE;
								GetShapeComp(strResultA,strResultA.GetLength());
							}
							if(lpCntxtPriv->bPYBiHua)
							{//�˳��ʻ�ѡ������״̬
								lpCntxtPriv->bPYBiHua=FALSE;
								lpCntxtPriv->szBiHua[0]=0;
							}
							if(isTempSpell)
							{//�˳���ʱƴ��
								lpCntxtPriv->compMode=IM_SHAPECODE;
								//Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
							}
						}else
						{
							KeyIn_Spell_Forecast(lpCntxtPriv,lpCntxtPriv->byCaret);
							if(byCaret+byPhraseLen<lpCntxtPriv->bySyllables)
							{
								lpCntxtPriv->byCaret+=byPhraseLen;
								KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
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
								char *pPhrase=(char*)pData+1;
								char *pComp=(char *)pPhrase+pPhrase[0]+1;
								char cCompLen=pComp[0];
								if(cCompLen==0)
									pComp=lpCntxtPriv->szInput,cCompLen=lpCntxtPriv->cInput;
								else
									pComp++;
								if(pPhrase[0]>2 || !g_SettingsG.bDisableDelWordCand)
								{
									if(ISComm_PhraseDel(pComp,cCompLen,pPhrase+1,pPhrase[0],m_pListener->GetHwnd())==ISACK_SUCCESS)
									{//ɾ������ɹ�,������ʾ
										memcpy(lpCntxtPriv->ppbyCandInfo+iCand,lpCntxtPriv->ppbyCandInfo+iCand+1,(lpCntxtPriv->sCandCount-iCand-1)*sizeof(LPBYTE));
										lpCntxtPriv->sCandCount--;
										goto end;
									}else
									{
										SoundPlay(_T("error"));
									}
								}else
								{
									SoundPlay(_T("error"));
								}
							}
							SoundPlay(_T("error"));
							goto end;
						}else if(pData[0]==RATE_USERCMD)
						{//����ֱͨ������
							CmdExecute(pData);
							byMask=0;
						}else
						{//��ͨ��ѡ������
							LPBYTE pComp=pData+2+pData[1];
							if(pData[0]==RATE_USERDEF)
							{//�Զ������
								if(pComp[0]!=0)	pData=pComp;
								else pData++;
								SStringA strResultA((char*)pData+1,pData[0]);
								OnInputChange(strResultA);
								//OnInputEnd(FALSE); todo
								byMask=0;
							}else
							{//�����Զ������
								SStringA strResultA((char*)pData+2,pData[1]);
								OnInputChange(strResultA);
								//OnInputEnd(FALSE); todo
//								Plugin_TextInput(pResult,lpCompStr->dwResultStrLen,pData+1+pData[1]+1,pData[1+pData[1]],pData[0]==RATE_WILD);
								if(pData[0]!=RATE_FORECAST)
								{//����Ԥ��ʣ���Ƶ����
									if(lpbKeyState[VK_CONTROL] & 0x80)
									{
										ISComm_RateAdjust((char*)pComp+1,pComp[0],(char*)pData+2,pData[1],RAM_FAST,m_pListener->GetHwnd());
									}else if(g_SettingsG.byRateAdjust) 
									{
										ISComm_RateAdjust((char*)pComp+1,pComp[0],(char*)pData+2,pData[1],g_SettingsG.byRateAdjust==1?RAM_AUTO:RAM_FAST,m_pListener->GetHwnd());
									}
								}else
								{//Ԥ��ʣ��Զ����
									ISComm_MakePhrase((char*)pData+2,pData[1]);
								}
							}
						}
					}
				}else //if(lpCntxtPriv->sbState==SBST_ASSOCIATE || lpCntxtPriv->sbState==SBST_SENTENCE)
				{
					if(g_SettingsG.byAstMode==AST_CAND)
					{//��ǰ�Ǵ�������
						OnInputStart();
						SStringA strResultA((char*)pData+3+pData[0],pData[2]-pData[0]);
						OnInputChange(strResultA);
						OnInputEnd(FALSE);
					}else if(g_SettingsG.byAstMode==AST_ENGLISH)
					{//��ǰ��Ӣ������
						OnInputStart();
						SStringA strResultA((char*)pData+1,pData[0]);
						OnInputChange(strResultA,byMask);
						OnInputEnd(TRUE);
						cCompLen=0;//�����м�������
					}
				}			
			}else if(lpCntxtPriv->inState==INST_ENGLISH)
			{//Ӣ�ĵ�������
				SStringA strResultA((char*)pData+1,pData[0]);
				byMask&=~MKI_ASTENGLISH;
				OnInputChange(strResultA,byMask);
				//OnInputEnd(TRUE);
				lpCntxtPriv->inState=INST_CODING;
//				Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
			}else if(lpCntxtPriv->inState==INST_USERDEF)
			{//�û��Զ�������
				if(pData[0]==RATE_USERCMD)
				{//����ֱͨ������
					CmdExecute(pData);
					byMask=0;
				}else
				{//һ����Զ���
					LPBYTE pComp=pData+2+pData[1];
					if(pComp[0]!=0) 
						pData=pComp;
					else
						pData++;
					SStringA strResultA((char*)pData+1,pData[0]);
					OnInputChange(strResultA);
					OnInputEnd(FALSE);
					lpCntxtPriv->inState=INST_CODING;
//					Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
					byMask=GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT);//������
				}
			}else if(lpCntxtPriv->inState==INST_LINEIME)
			{//�ʻ�����״̬
				SStringA strResultA((char*)pData+2,pData[1]);
				OnInputChange(strResultA,byMask);
				OnInputEnd(TRUE);

				lpCntxtPriv->inState=INST_CODING;
//				Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
			}
			ClearContext(CPC_ALL);
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
		//����״̬���Զ���״ֻ̬��ʹ�����¼�ͷ��ҳ,�Ա�������������ͻ
		if(byInput==VK_DOWN || (!(lpbKeyState[VK_SHIFT]&0x80) && byInput==g_SettingsG.byTurnPageDownVK&& lpCntxtPriv->sbState!=SBST_ASSOCIATE && lpCntxtPriv->inState!=INST_USERDEF))
		{
			bRet = m_pListener->GoNextCandidatePage();
		}else if(byInput==VK_UP || (!(lpbKeyState[VK_SHIFT]&0x80) && byInput==g_SettingsG.byTurnPageUpVK&& lpCntxtPriv->sbState!=SBST_ASSOCIATE && lpCntxtPriv->inState!=INST_USERDEF))
		{
			bRet = m_pListener->GoPrevCandidatePage();
		}
	}
	if(bRet){
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_ASSOCIATE)
		{//����״̬�ķ�ҳ,������ʱ��ʱ��
			m_pListener->CloseInputWnd(5000);
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
	if(g_SettingsG.compMode == IM_SPELL) return FALSE;
	return TRUE;
}

//ƴ���ıʻ���������ѡ��״̬:�������
BOOL CInputState::KeyIn_PYBiHua_ChangComp(InputContext * lpCntxtPriv,UINT byInput,
							 CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	int nLen=(int)strlen(lpCntxtPriv->szBiHua);

	if(byInput == VK_BACK)
	{
		if(nLen>0)
		{
			lpCntxtPriv->szBiHua[--nLen]=0;
			bRet=TRUE;
		}else
		{//�˳��ʻ�����״̬
			if(lpCntxtPriv->bPYBiHua) lpCntxtPriv->bPYBiHua=FALSE;
			return TRUE;
		}
	}else if(byInput>=VK_NUMPAD1 && byInput<=VK_NUMPAD6)
	{
		if(nLen<50)
		{
			lpCntxtPriv->szBiHua[nLen++]=byInput-VK_NUMPAD1+'1';
			lpCntxtPriv->szBiHua[nLen]=0;
		}
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE)
	{
		lpCntxtPriv->bPYBiHua=FALSE;
		lpCntxtPriv->szBiHua[0]=0;
		return TRUE;
	}
	if(bRet)
	{
		char *pszWordList=(char*)malloc(lpCntxtPriv->sCandCount*2);
		short i;
		for(i=0;i<lpCntxtPriv->sCandCount;i++)
		{
			LPBYTE pbyCand=lpCntxtPriv->ppbyCandInfo[i];
			memcpy(pszWordList+i*2,pbyCand+2,2);
		}
		if(ISComm_SortWordByBiHua(lpCntxtPriv->szBiHua,nLen,pszWordList,lpCntxtPriv->sCandCount)==ISACK_SUCCESS)
		{
			PMSGDATA pData=ISComm_GetData();
			for(i=0;i<lpCntxtPriv->sCandCount;i++)
			{
				LPBYTE pbyCand=lpCntxtPriv->ppbyCandInfo[i];
				memcpy(pbyCand+2,pData->byData+i*2,2);
			}
			memcpy(lpCntxtPriv->szWord+lpCntxtPriv->byCaret,pData->byData,2);//�Զ�ѡ���һ������
			lpCntxtPriv->bySelect[lpCntxtPriv->byCaret]=1;					//�趨Ϊ�Զ�ѡ�񣬲������Զ��޸�
		}else
		{
			SoundPlay(_T("error"));
		}
		free(pszWordList);
	}
	return bRet;
}

BOOL CInputState::KeyIn_Code_Normal(InputContext * lpCntxtPriv,UINT byInput,
					   CONST BYTE * lpbKeyState)
{
	BOOL bRet=KeyIn_Code_ChangeComp(lpCntxtPriv,byInput,lpbKeyState);
	if(!bRet) bRet=KeyIn_Code_Symbol(lpCntxtPriv,byInput,lpbKeyState);
	return bRet;
}

char		g_cHotKeyUD=0;			//�л����û�������������ؿ�ݼ�
char		g_cHotKeySent=0;		//�л������������������ؿ�ݼ�

//��ͨ����״̬�±���ı䴦��
BOOL CInputState::KeyIn_Code_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	if(byInput==ISComm_GetCompInfo()->cWildChar)
	{
		bCompChar=TRUE;
	}else
	{
		char i=0;
		while(ISComm_GetCompInfo()->szCode[i])
		{
			if(ISComm_GetCompInfo()->szCode[i]==byInput)
			{
				bCompChar=TRUE;
				break;
			}
			i++;
		}
	}
	if(bCompChar)
	{
		if(lpCntxtPriv->sbState==SBST_ASSOCIATE) 
		{
			ClearContext(CPC_ALL);
		}

		if(KeyIn_Code_IsMaxCode(lpCntxtPriv,MCR_NORMAL) 
			&& !KeyIn_Code_IsValidComp(lpCntxtPriv,byInput)
			&& g_SettingsG.bAutoInput 
			&& !lpCntxtPriv->bWebMode)
		{
			if(lpCntxtPriv->sCandCount)
			{
				BYTE byType=lpCntxtPriv->ppbyCandInfo[0][0];
				//��ֹ��������ʱ���ִ���:��㲻�����ױ���,�˳���ǰ����,�����㶥����������
				if((byInput<'a' || byInput>'z') && !ISComm_GetCompInfo()->bSymbolFirst) return FALSE;
				if(byType!=RATE_FORECAST && (byType!=RATE_GBK || g_SettingsG.nGbkMode==2)) 
				{
					KeyIn_All_SelectCand(lpCntxtPriv,'1',1,lpbKeyState);
					bRet=TRUE;
				}
			}
			lpCntxtPriv->cInput=0;
		}

		if(lpCntxtPriv->cInput==0 && !bRet)
		{
			if((byInput<'a' || byInput>'z') && (byInput!=g_cHotKeyUD || !g_SettingsG.bFastUMode) )
			{//��㣺Ҫô�����Զ���ģʽ��ݼ������߲�֧�ֿ���Զ���ģʽ�л�
				if(!ISComm_GetCompInfo()->bSymbolFirst || byInput==g_cHotKeyUD) return FALSE;//���Ŷ�������
			}
			if(g_SettingsG.bShowOpTip)
			{
				lpCntxtPriv->bShowTip=TRUE;
				Tips_Rand(FALSE,lpCntxtPriv->szTip);
			}
			//��ʼ��������,���ɿ�ʼ������Ϣ�Ի�ȡ������ʱ���봰�ڵ�����
			OnInputStart();
			if(byInput==g_cHotKeyUD && g_SettingsG.bFastUMode)
			{//�л����û��Զ�������״̬
				ClearContext(CPC_ALL);
				lpCntxtPriv->inState=INST_USERDEF;
				lpCntxtPriv->bShowTip=FALSE;
				//Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
				return TRUE;
			}
		}
		if(lpCntxtPriv->cInput<MAX_COMP)
		{
			lpCntxtPriv->szInput[lpCntxtPriv->cInput++]=byInput;
			bRet=TRUE;
		}
	}else if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->cInput>0) lpCntxtPriv->cInput--;
		if(lpCntxtPriv->bWebMode && lpCntxtPriv->cInput<=2) lpCntxtPriv->bWebMode=FALSE;
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE)
	{
		lpCntxtPriv->cInput=0;
		lpCntxtPriv->sbState=::SBST_NORMAL;
		lpCntxtPriv->bWebMode=FALSE;
		lpCntxtPriv->bShowTip=FALSE;
		bRet=TRUE;
	}else if(byInput==VK_RETURN)
	{
		BOOL bClearComp= lpCntxtPriv->bWebMode==FALSE 
			&& ((g_SettingsG.bEnterClear && !(lpbKeyState[VK_SHIFT]&0x80))||(!g_SettingsG.bEnterClear && lpbKeyState[VK_SHIFT]&0x80 ));
		if(bClearComp)
		{//��ձ���
			lpCntxtPriv->cInput=0;
			lpCntxtPriv->sbState=::SBST_NORMAL;
			bRet=TRUE;
		}else 
		{//��������
			if(lpCntxtPriv->bWebMode && lpbKeyState[VK_SHIFT]&0x80)
			{//��ַģʽ�´������
				lpCntxtPriv->szInput[lpCntxtPriv->cInput]=0;
				ShellExecuteA(NULL,"open",lpCntxtPriv->szInput,NULL,NULL,SW_SHOWDEFAULT);
			}else
			{
				BYTE byMask=0;
				SStringA strResultA(lpCntxtPriv->szInput,lpCntxtPriv->cInput);

				if(g_SettingsL.bRecord)
					byMask|=MKI_RECORD;
				if(g_SettingsG.bTTSInput)
					byMask|=MKI_TTSINPUT;
				ISComm_KeyIn(strResultA,(short)strResultA.GetLength(),byMask,m_pListener->GetHwnd());
				OnInputChange(strResultA);
				OnInputEnd(FALSE);
			}
			ClearContext(CPC_ALL);
			return TRUE;
		}
	}else if(byInput==g_cHotKeyUD)
	{//�û�������Զ���״̬�л���
		if(lpCntxtPriv->cInput==0 && g_SettingsG.bFastUMode)
		{//�л����û��Զ�������״̬
			ClearContext(CPC_ALL);
			lpCntxtPriv->inState=INST_USERDEF;
			//Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
			OnInputStart();
			return TRUE;
		}
	}else if(lpCntxtPriv->cInput < MAX_COMP)
	{
		lpCntxtPriv->szInput[lpCntxtPriv->cInput++]=byInput;
		if(lpCntxtPriv->bWebMode || (!g_SettingsG.bAutoInput && byInput>='a' && byInput<='z')) bRet=TRUE;
	}
	if(!lpCntxtPriv->bWebMode)
	{//������ַģʽת��
		if(!bCompChar)
		{
			if(KeyIn_IsWebHeader(lpCntxtPriv->szInput,lpCntxtPriv->cInput))
			{
				lpCntxtPriv->bWebMode=TRUE;
				bRet=TRUE;
			}else if(byInput!=VK_BACK && byInput!=VK_ESCAPE && byInput!=VK_RETURN && byInput!=g_cHotKeyUD)
			{
				lpCntxtPriv->cInput--;
			}
		}else
		{
			lpCntxtPriv->bWebMode=KeyIn_IsWebHeader(lpCntxtPriv->szInput,lpCntxtPriv->cInput);
		}
	}

	if(bRet)
	{
		LPBYTE lpbyCand=NULL;
		BYTE byType=0;
		BYTE byMask=MQC_NORMAL|g_SettingsG.byForecast;
		if(g_SettingsG.bAutoMatch) byMask|=MQC_MATCH;
		if(g_SettingsG.bBlendUD) byMask|=MQC_USERDEF;
		if(g_SettingsG.bBlendSpWord) byMask|=MQC_SPCAND;
		if(g_SettingsG.bAutoPrompt) byMask|=MQC_AUTOPROMPT;
		if(g_SettingsG.bOnlySimpleCode) byMask|=MQC_ONLYSC;

		ClearContext(CPC_CAND);

		if(lpCntxtPriv->cInput==0)
		{
			OnInputEnd(TRUE);
		}else
		{
			lpCntxtPriv->sbState=::SBST_NORMAL;	//�˳�����״̬
			if(ISComm_QueryCand(lpCntxtPriv->szInput,lpCntxtPriv->cInput,byMask,m_pListener->GetHwnd())==ISACK_SUCCESS)
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
					if(pCandData[1]==2) sSingleWords++;
					pCandData+=pCandData[1]+2;	//ƫ�ƴ�����Ϣ
					pCandData+=pCandData[0]+1;	//ƫ�Ʊ�����Ϣ					
				}
				pCandData=pbyData;
				for(i=0;i<sCount;i++)
				{
					if(g_SettingsG.nGbkMode!=0 || pCandData[0]!=RATE_GBK || sSingleWords<2)
					{//GBK��ʾ���߲���GBK����
						lpCntxtPriv->ppbyCandInfo[lpCntxtPriv->sCandCount++]=pCandData;
					}
					pCandData+=pCandData[1]+2;	//ƫ�ƴ�����Ϣ
					pCandData+=pCandData[0]+1;	//ƫ�Ʊ�����Ϣ
				}
			}
		}
		if(lpCntxtPriv->sCandCount)
		{
			byType=m_pbyMsgBuf[0];
			lpbyCand=lpCntxtPriv->ppbyCandInfo[0];
		}else if(lpCntxtPriv->bWebMode)
		{
			lpCntxtPriv->bShowTip=TRUE;
			strcpy(lpCntxtPriv->szTip,"��ַģʽ:\n �س�=��ַ����\n Shift+�س�=���");
		}
		if((byType&MCR_AUTOSELECT ||(KeyIn_Code_IsMaxCode(lpCntxtPriv,byType) && g_SettingsG.bAutoInput)) && !lpCntxtPriv->bWebMode)
		{
			if(lpCntxtPriv->sCandCount==1 && lpbyCand[0]!=RATE_FORECAST && (lpbyCand[0]!=RATE_GBK || g_SettingsG.nGbkMode!=1))
				KeyIn_All_SelectCand(lpCntxtPriv,'1',0,lpbKeyState);
			else
			{
				if(lpCntxtPriv->sCandCount>1)
					SoundPlay(_T("ChongMa"));
				else if(lpCntxtPriv->sCandCount==0)
					SoundPlay(_T("KongMa"));
				else
					SoundPlay(_T("LianXiang"));
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
			strResultA = SStringA((char*)pbyCand+2,pbyCand[1]);
		}
	}

//	strResultA = Symbol_Convert(byInput,strResultA,lpbKeyState);

// 	lpCompStr->dwResultStrLen+=Symbol_Convert(byInput,pResult+lpCompStr->dwResultStrLen,lpbKeyState);
	if(g_SettingsL.bRecord)
		byMask|=MKI_RECORD;
	if(g_SettingsG.bTTSInput)
		byMask|=MKI_TTSINPUT;

	OnInputStart();
	OnInputChange(strResultA,byMask);
	OnInputEnd(FALSE);
	return TRUE;
}

//����״̬�µ����봦��,����״̬�ı䴦��
BOOL CInputState::KeyIn_All_Associate(InputContext * lpCntxtPriv,UINT byInput,
						 CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	SASSERT(lpCntxtPriv->sbState==SBST_ASSOCIATE);
	if((lpCntxtPriv->compMode==IM_SHAPECODE && byInput==g_cHotKeySent) ||(lpCntxtPriv->compMode==IM_SPELL && byInput==';'))
	{
		if(lpCntxtPriv->sSentLen)
		{//�л����������״̬
			lpCntxtPriv->sbState=SBST_SENTENCE;
			lpCntxtPriv->sSentCaret=0;
			OnInputStart();
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
	}else if((lpCntxtPriv->compMode==IM_SHAPECODE && byInput==g_cHotKeySent) ||(lpCntxtPriv->compMode==IM_SPELL && byInput==';'))
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
		OnInputEnd(FALSE);
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
		if(g_SettingsL.bRecord)
			byMask|=MKI_RECORD;
		if(g_SettingsG.bTTSInput)
			byMask|=MKI_TTSINPUT;

		SStringA strResultA((char*)lpCntxtPriv->pbySentWord[0],
			(int)(lpCntxtPriv->pbySentWord[lpCntxtPriv->sSentCaret]-lpCntxtPriv->pbySentWord[0]));

		ClearContext(CPC_ALL);
		OnInputStart();
		OnInputChange(strResultA,byMask);
		OnInputEnd(FALSE);
	}
}


//Ӣ�ĵ���״̬�����봦��
BOOL CInputState::KeyIn_Code_English(InputContext * lpCntxtPriv,UINT byInput,
						CONST BYTE* lpbKeyState)
{
	SASSERT(lpCntxtPriv->inState==INST_ENGLISH);
	if((byInput>='A' && byInput<='Z')||(byInput>='a' && byInput<='z') || byInput==VK_BACK || byInput==VK_SPACE || byInput=='\'')
	{
		if(byInput==VK_BACK)
		{
			if(lpCntxtPriv->cInput) 
			{
				lpCntxtPriv->cInput--;
				if(lpCntxtPriv->cInput==0)
				{
					OnInputEnd(FALSE);
					lpCntxtPriv->inState=INST_CODING;
//					Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
				}
			}
		}else
		{
			lpCntxtPriv->szInput[lpCntxtPriv->cInput++]=byInput;
		}
		ClearContext(CPC_CAND);
		if(lpCntxtPriv->cInput)
		{//�����Ѿ��޸�
			if(ISComm_EnQueryCand(lpCntxtPriv->szInput,lpCntxtPriv->cInput)==ISACK_SUCCESS)
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
	}else if(byInput==VK_RETURN)
	{//���뵱ǰӢ��
		SStringA strResult;
		if(lpCntxtPriv->pbyEnSpell)
		{
			strResult = SStringA((char*)lpCntxtPriv->pbyEnSpell+1,lpCntxtPriv->pbyEnSpell[0]);
		}else
		{
			strResult = SStringA((char*)lpCntxtPriv->szInput,lpCntxtPriv->cInput);
		}
		if(g_SettingsL.bSound) ISComm_TTS(strResult,(char)strResult.GetLength(),MTTS_EN);
		if(lpbKeyState[VK_SHIFT]&0x80 && lpCntxtPriv->pbyEnPhontic && lpCntxtPriv->pbyEnPhontic[0])
		{//����������Ϣ
			//PutEnAdd2Clip(lpCntxtPriv->pbyEnPhontic);
		}
		//���뵥��
		OnInputChange(strResult);
		OnInputEnd(TRUE);

		//Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
	}else if(byInput==VK_ESCAPE)
	{//�������
//		Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		OnInputEnd(TRUE);
	}
	return TRUE;
}

//��������
BOOL CInputState::KeyIn_Digital_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
							  CONST BYTE* lpbKeyState)
{
	BOOL bRet=FALSE;
	if((byInput>='0' && byInput<='9') || ((byInput=='.' ||byInput==',') && g_SettingsG.bAutoDot) )
	{
		SStringA strResultA((char)byInput);
		OnInputStart();
		OnInputChange(strResultA,g_SettingsL.bRecord?MKI_RECORD:0);
		OnInputEnd(FALSE);
		bRet=TRUE;
	}else
	{
		lpCntxtPriv->inState=INST_CODING;
		//Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
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
		if(lpCntxtPriv->cInput>0)	lpCntxtPriv->cInput--;
	}else if(byInput==VK_ESCAPE)
	{
		ClearContext(CPC_ALL);
//		Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		OnInputEnd(FALSE);
	}else if(byInput==VK_RETURN)
	{
		if(lpCntxtPriv->cInput)
		{//�������
			//�л�������״̬
			SStringA strResultA(lpCntxtPriv->szInput,lpCntxtPriv->cInput);
			OnInputChange(strResultA);
			OnInputEnd(FALSE);
//			Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
			return TRUE;
		}
	}else
	{
		lpCntxtPriv->szInput[lpCntxtPriv->cInput++]=byInput;
		lpCntxtPriv->szInput[lpCntxtPriv->cInput]=0;
	}
	ClearContext(CPC_CAND);
	if(lpCntxtPriv->cInput && ISACK_SUCCESS==ISComm_UDQueryCand(lpCntxtPriv->szInput,lpCntxtPriv->cInput))
	{//��ȡ�Զ������,֧�ֶ������
		PMSGDATA pMsgData=ISComm_GetData();
		LPBYTE pbyData;
		short i;
		memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
		pbyData=m_pbyMsgBuf;
		pbyData+=pbyData[0]+1;//������������������Զ������ı�������
		memcpy(&lpCntxtPriv->sCandCount,pbyData,2);
		pbyData+=2;
		lpCntxtPriv->ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*lpCntxtPriv->sCandCount);
		for(i=0;i<lpCntxtPriv->sCandCount;i++)
		{
			lpCntxtPriv->ppbyCandInfo[i]=pbyData;
			pbyData+=pbyData[1]+2;//����
			pbyData+=pbyData[0]+1;//����
		}
	}
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
		if(lpCntxtPriv->cInput>0)
		{
			lpCntxtPriv->cInput--;
			bCompChar=TRUE;
		}
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE||byInput==VK_RETURN)
	{//�˳�״̬
		lpCntxtPriv->inState=INST_CODING;
		lpCntxtPriv->sbState=::SBST_NORMAL;
		lpCntxtPriv->cInput=0;
		//Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		OnInputEnd(FALSE);
		bRet=TRUE;
	}else if(byInput>=VK_NUMPAD1 && byInput<=VK_NUMPAD6)
	{//�༭
		lpCntxtPriv->szInput[lpCntxtPriv->cInput++]=byInput-VK_NUMPAD1+'1';
		lpCntxtPriv->szInput[lpCntxtPriv->cInput]=0;
		bCompChar=TRUE;
		bRet=TRUE;
	}
	if(bCompChar)
	{
		ClearContext(CPC_CAND);
		if(lpCntxtPriv->cInput && ISComm_LineQueryCand(lpCntxtPriv->szInput,lpCntxtPriv->cInput)==ISACK_SUCCESS)
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
				pBuf+=pBuf[1]+2;
			}
			lpCntxtPriv->sCandCount=sCount;
		}
	}
	return bRet;
}


BOOL CInputState::TestKeyDown(UINT uKey,LPARAM lKeyData,const BYTE * lpbKeyState)
{
	static BOOL bPressOther = FALSE;
	static BOOL bPressShift=FALSE;
	static BOOL bPressCtrl=FALSE;
	BOOL bRet=FALSE;
	if(!ISComm_IsSvrWorking()) return FALSE;
	if ((lKeyData & 0x80000000) && (uKey != VK_SHIFT && uKey !=VK_CONTROL))
		return FALSE;

	if(uKey==VK_CAPITAL)
	{
		if(lpbKeyState[VK_CAPITAL]&0x01)
		{
			m_pListener->CloseInputWnd(0);
		}
		return FALSE;
	}else if(uKey==VK_SHIFT)
	{
		if(bPressCtrl)
		{
			bPressOther=TRUE;
			return FALSE;
		}
		if(lKeyData & 0x80000000)//����SHIFT��
		{
			if(!bPressOther && bPressShift)//��SHIFT�����º�û�а�������������ʾʹ�ÿ�ݹرչ���
			{
				BYTE byKey=(BYTE)(lKeyData>>16);
				bPressShift=FALSE; 
				if(!(lpbKeyState[VK_SPACE]&0x80) &&
					g_SettingsG.bySwitchKey==byKey)
				{ //check the scan code
					BOOL bOpen = !m_pListener->GetOpenStatus();
					m_pListener->SetOpenStatus(bOpen);
				}
			}else//��ԭ״̬
			{
				bPressShift=FALSE;
				bPressOther=FALSE;
			}	
		}else
		{//����SHIFT������ʼ��״̬
			bPressShift=TRUE;
			bPressOther=FALSE;
		}
	}else if(uKey==VK_CONTROL)
	{
		if(!m_pListener->GetOpenStatus())
			return FALSE;
		if(bPressShift)
		{
			bPressOther=TRUE;
			return FALSE;
		}
		if(lKeyData & 0x80000000)//����Ctrl��
		{
			BYTE byKey=(BYTE)(lKeyData>>24);
			if(!bPressOther && bPressCtrl)//Ctrl�����º�û�а�������������ʾʹ�ÿ�ݹرչ���
			{
				if(g_SettingsG.byTempSpellKey==byKey)
				{//��ʱƴ����ƴ������
					if(g_SettingsG.compMode==IM_SHAPECODE)
					{//�������״̬��������ʱƴ��״̬
						m_ctx.compMode=IM_SPELL;
						ClearContext(CPC_ALL);
						m_ctx.bShowTip=TRUE;
						strcpy(m_ctx.szTip,"��ʱƴ��:����������ʾ");
						//MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATESTATUS|IMN_PRIV_OPENINPUTWND|IMN_PRIV_UPDATEINPUTWND);
					}else if(g_SettingsG.compMode==IM_SPELL)
					{//ƴ������״̬
						if(IsTempSpell() && m_ctx.bySyllables==1 && m_ctx.spellData[0].bySpellLen==0)
						{//�˳���ʱƴ��״̬
							m_ctx.compMode = IM_SHAPECODE;
//							Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
//							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATESTATUS|IMN_PRIV_CLOSEINPUTWND);
							ClearContext(CPC_ALL);
						}else if(m_ctx.sCandCount)
						{
							if(m_ctx.bPYBiHua)
							{//�˳��ʻ��븨������ģʽ
								m_ctx.szBiHua[0]=0;
								m_ctx.bPYBiHua=FALSE;
							}else
							{//����ʻ��븨������ģʽ
								short i,sCands=0;
								m_ctx.bPYBiHua=TRUE;
								m_ctx.szBiHua[0]=0;
								//��������е�������
								for(i=0;i<m_ctx.sCandCount;i++)
								{
									LPBYTE 	pbyCand=m_ctx.ppbyCandInfo[i];
									if(pbyCand[1]==2)
									{
										m_ctx.ppbyCandInfo[sCands++]=m_ctx.ppbyCandInfo[i];
									}
								}
								m_ctx.sCandCount=sCands;
							}
//							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
						}
					}
				}else if(!g_SettingsG.bDisableFnKey && ((g_SettingsG.byTempSpellKey==0 && byKey==0xC1)||g_SettingsG.byTempSpellKey!=0))
				{//���ܼ�
					if(!KeyIn_IsCoding(&m_ctx)) 
					{
						if(m_ctx.inState==INST_CODING && m_ctx.sbState==SBST_ASSOCIATE
							&& ((m_ctx.compMode==IM_SHAPECODE &&  m_ctx.sSentLen) || (m_ctx.compMode==IM_SPELL && m_ctx.bySyllables==1 && m_ctx.spellData[0].bySpellLen==0) )
							)
						{//�����������״̬
							m_ctx.sbState=SBST_SENTENCE;
							m_ctx.sSentCaret=0;
							OnInputStart();
							bRet=TRUE;
						}else
						{//�����û��Զ�������״̬
							ClearContext(CPC_ALL);
							m_ctx.inState=INST_USERDEF;
							//Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
							OnInputStart();
							bRet=TRUE;
						}
					}else
					{
						if(m_ctx.inState==INST_CODING && m_ctx.sbState==SBST_SENTENCE
							&& ((m_ctx.compMode==IM_SHAPECODE &&  m_ctx.sSentLen) || (m_ctx.compMode==IM_SPELL && m_ctx.bySyllables==1 && m_ctx.spellData[0].bySpellLen==0) )
							)
						{//�������״̬
							m_ctx.sbState=SBST_ASSOCIATE;
							OnInputEnd(TRUE);
							bRet=TRUE;
						}else if(m_ctx.inState==INST_USERDEF)
						{//״̬��ԭ
							m_ctx.inState=INST_CODING;
							//Plugin_StateChange(g_CompMode,m_ctx.inState,m_ctx.sbState,g_bTempSpell);
							OnInputEnd(FALSE);
							bRet=TRUE;
						}
					}
				}
				bPressCtrl=FALSE; 
			}else//��ԭ״̬
			{
				bPressCtrl=FALSE;
				bPressOther=FALSE;
			}		
		}else
		{//����Ctrl������ʼ��״̬
			bPressCtrl=TRUE;
			bPressOther=FALSE;
		}
	}else if(!(lpbKeyState[VK_CAPITAL]&0x01)) 
	{
		bPressOther=TRUE;
		if(m_pListener->GetOpenStatus())
		{
			if(lpbKeyState[VK_CONTROL]&0x80 && lpbKeyState[VK_SHIFT]&0x80)
			{//Ctrl + Shift
				bRet=(uKey==g_SettingsG.byHotKeyQuery || (uKey>='0' && uKey<='9'));
				//if(!bRet) bRet=Plugin_HotkeyInquire(uKey);
				return bRet;
			}else if(lpbKeyState[VK_CONTROL]&0x80) 
			{//Ctrl��ϼ�
				if(uKey==g_SettingsG.byHotKeyMode
					||uKey==g_SettingsG.byHotKeyEn
					||uKey==g_SettingsG.byHotKeyQuery
					||uKey==g_SettingsG.byHotKeyMakeWord
					||uKey==g_SettingsG.byHotKeyShowRoot
					||uKey==g_SettingsG.byHotKeyHideStatus
					||(uKey>='0' && uKey<='9') 
					||(uKey>=VK_NUMPAD0 && uKey<=VK_NUMPAD9) 
					) //�ȼ�����Ƶ���ּ�
					return TRUE;
				else
					return FALSE;
			}else if(lpbKeyState[VK_SHIFT]&0x80 && uKey==VK_SPACE)
			{//Shift + VK_SPACE:��Ӣ�ı���л�
//				MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,MAKELONG(IMN_PRIV_COMMAND,g_SettingsL.bCharMode?IDC_CHARMODE1:IDC_CHARMODE2));
				return TRUE;
			}else
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
						char i=0;
						if(cKey==ISComm_GetCompInfo()->cWildChar)
						{
							bRet=TRUE;
						}else
						{
							while(ISComm_GetCompInfo()->szCode[i])
							{
								if(ISComm_GetCompInfo()->szCode[i]==cKey)
								{
									bRet=TRUE;
									break;
								}
								i++;
							}
						}
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
						bRet= (uKey==g_SettingsG.by2CandVK 
							|| uKey==g_SettingsG.by3CandVK 
							|| uKey==g_SettingsG.byTurnPageDownVK 
							|| uKey==g_SettingsG.byTurnPageUpVK
							);
					}
					if(!bRet && bPressCtrl)
					{//����Ƿ����û�����ĸ��ֿ�ݼ�
						bRet=( uKey==g_SettingsG.byHotKeyEn
							|| uKey==g_SettingsG.byHotKeyMakeWord
							|| uKey==g_SettingsG.byHotKeyMode
							|| uKey==g_SettingsG.byHotKeyQuery
							|| uKey==g_SettingsG.byHotKeyShowRoot
							|| uKey==g_SettingsG.byHotKeyHideStatus
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
							//Plugin_StateChange(g_CompMode,m_ctx.inState,m_ctx.sbState,g_bTempSpell);
							//MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
							if(uKey==VK_ESCAPE)
							{
// 								DWORD dwStyle=GetWindowLongPtr(g_hWndMsg,GWL_STYLE);
// 								if(dwStyle&WS_VISIBLE)
// 								{
// 									ShowWindow(g_hWndMsg,SW_HIDE);
// 									bRet=TRUE;
// 								}
							}else if(uKey==VK_BACK) 
								ISComm_KeyIn("\b",1,0,m_pListener->GetHwnd());
							else
								ISComm_KeyIn(".",1,g_SettingsL.bRecord?MKI_RECORD:0,m_pListener->GetHwnd());
						}
					}
				}
//				if(!bRet && !bCoding)	MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
			}
		}
	}

	return bRet;
}
