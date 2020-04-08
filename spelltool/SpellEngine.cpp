// SpellEngine.cpp: implementation of the CSpellEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpellEngine.h"
#include "TxtReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int WordPhoneCmp(WORDPHONE *spWord1,WORDPHONE *spWord2,LPARAM lParam)
{
	return (int)spWord1->wWord - (int)spWord2->wWord;
}

int TextCmp(WString *pt1, WString *pt2, LPARAM lParam)
{
	if(pt1->cLen!=pt2->cLen) return pt1->cLen-pt2->cLen;
	return wcsncmp(pt1->szBuf,pt2->szBuf,pt1->cLen);
}

CSpellEngine::CSpellEngine()
:m_arrWord(500),m_arrContext(500)
{

}

CSpellEngine::~CSpellEngine()
{
	for(int i=0;i<m_arrWord.GetSize();i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		for(int j=0;j<pwp->cpp;j++)
		{
			if(pwp->pi[j].pArrCntxt) delete pwp->pi[j].pArrCntxt;
		}
	}
}

#ifdef SPELLENGINE_EDITOR
//����ƴ����������ӳ���
//���ʽΪ�ı���	�硡huang\t�ƻ�...
//������ע���� �� #ע��
int CSpellEngine::ImportSpell2Word(LPCTSTR pszFileName)
{
	WCHAR szLine[1000];
	CTxtReader reader('#');
	if(!reader.Open(pszFileName)) return 0;
	int nRet=0;
	BYTE byID[2];
	BOOL bRead=reader.getline(szLine,1000);
	while(bRead)
	{
		WCHAR *pszCand=wcsstr(szLine,L"\t");
		ASSERT(pszCand);
		if(g_SpellData.Spell2ID(szLine,pszCand-szLine,byID))
		{
			short sIndex=g_SpellData.GetIDIndex(byID);
			pszCand++;
			m_spellMap.SetSpellCands(sIndex,pszCand,wcslen(pszCand));
			nRet++;
		}
		bRead=reader.getline(szLine,1000);
	}
	reader.Close();
	return nRet;
}


//���뺺��->ƴ����Ϣ
//��ʽ�� ˵ shuo #yue;shui;
//#��Ϊ������Ϣ
//��#��ͷΪע����
int CSpellEngine::ImportWord2Spell(LPCTSTR pszFileName)
{
	WCHAR szLine[100];
	CTxtReader reader('#');
	if(!reader.Open(pszFileName)) return 0;
	int nRet=0;
	WCHAR szDefSpell[7];
	WCHAR szOtherSpell[80];
	WORDPHONE word;
	BOOL bRead=reader.getline(szLine,100);
	while(bRead)
	{
		ASSERT(szLine[1]==0x20);
		memset(&word,0,sizeof(word));
		word.wWord = szLine[0];
		szDefSpell[0]=szOtherSpell[0]=0;
		swscanf(szLine+2,L"%s #%s",szDefSpell,szOtherSpell);
		g_SpellData.Spell2ID(szDefSpell,-1,word.byPhone);
		//��Ĭ�϶���Ҳ��������ֵ�һ����
		memcpy(word.pi[0].byPhone,word.byPhone,2);
		word.cpp=1;

		LPWSTR pszBegin=szOtherSpell;
		LPWSTR pszEnd=wcsstr(pszBegin,L";");
		while(pszEnd)
		{
			g_SpellData.Spell2ID(pszBegin,pszEnd-pszBegin,word.pi[word.cpp++].byPhone);
			pszBegin=pszEnd+1;
			pszEnd=wcsstr(pszBegin,L";");
		}

		int nInsert=m_arrWord.SortInsert(word,FALSE,WordPhoneCmp);
		ASSERT(nInsert!=-1);
		nRet++;
		bRead = reader.getline(szLine,100);
	}
	reader.Close();
	return nRet;	
}

//������Ƶ�ļ�
//��ʽ�� �� 225
//��Ƶ������250
BOOL CSpellEngine::ImportWordRate(LPCTSTR pszFileName)
{
	CSArray<WORDRATE> arrWordRate(500);
	WCHAR szLine[100];
	CTxtReader reader('#');
	if(!reader.Open(pszFileName)) return FALSE;
	WORDRATE wr;
	BOOL bRead=reader.getline(szLine,100);
	while(bRead)
	{
		ASSERT(szLine[1]=0x20);
		wr.wWord = szLine[0];
		wr.byRate=(BYTE)_wtoi(szLine+2);
		if(wr.byRate>MAX_RATE) wr.byRate=MAX_RATE;
		arrWordRate.SortInsert(wr,FALSE,WordRateCmp);
		bRead=reader.getline(szLine,100);
	}
	reader.Close();
	m_spellMap.InitRate(&arrWordRate);
	return TRUE;
}
#endif// SPELLENGINE_EDITOR

//��һ������������ƴ���Ĵ���ֽ�Ϊ����ʹ�õĴ���
//��: ����yue
void CSpellEngine::MakeSpellPhrase(LPCWSTR pszLine,SPELLPHRASE *pPhrase)
{
	int c=0;
	int i=0;
	char cpps=0;
	while(pszLine[c])
	{
		if(CWordID::IsHanzi(pszLine[c]))
		{//����
			pPhrase->szBuf[i++]=pszLine[c++];
		}else
		{//����һ�������ֵĶ���
			char cBegin=c;
			while(pszLine[c] && CWordID::IsAlphabit(pszLine[c])) c++;
			ASSERT(c-cBegin<7);
			pPhrase->cPPPos[cpps]=i-1;
			CUtils::CopyCompFormWStr2AStr(pPhrase->cPPSp[cpps],pszLine+cBegin,c-cBegin);
			cpps++;
		}
	}
	pPhrase->szBuf[i]=0;
	pPhrase->cLen=i;
	pPhrase->cPPSize=cpps;
}

int CSpellEngine::Context_Export(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("w"));
	if(!f)
	{
		return -1;
	}
	fwrite("\xff\xfe",1,2,f);//write utf16 bom

	int nRet=0;
	int i,j;
	for( i=0;i<m_arrCntxtRemark.GetSize();i++)
	{
		fwrite(L"@",sizeof(WCHAR),1,f);	//ע����ʼ����
		fwrite(m_arrCntxtRemark[i].pszBuf,sizeof(WCHAR),m_arrCntxtRemark[i].nLen,f);
		fwrite(L"\n",sizeof(WCHAR),1,f);
	}
	char szSpell[100][7];

	for( i=0;i<m_arrContext.GetSize();i++)
	{
		WString *pt=m_arrContext.GetPtAt(i);
		int nWord=MakeSpell(pt->szBuf,pt->cLen,szSpell,100,TRUE);
		if(nWord!=pt->cLen) continue;
		for( j=0;j<pt->cLen;j++)
		{
			WORDPHONE *pwp=GetWordPhone(pt->szBuf[j]);
			fwrite(pt->szBuf+j,sizeof(WCHAR),1,f);		//д�뺺��
			if(pwp->cpp!=1) //��������д��ƴ��
			{
				WCHAR wzSpell[7];
				int nLen = strlen(szSpell[j]);
				CUtils::CopyCompFromAStr2WStr(wzSpell,szSpell[j],nLen);
				fwrite(wzSpell,sizeof(WCHAR),nLen,f);
			}
		}
		fwrite(L"\n",sizeof(WCHAR),1,f);
		nRet++;
	}
	fclose(f);
	return nRet;
}

//���������������
//�ı� ��:��chong��
int CSpellEngine::Context_Import(LPCTSTR pszFileName)
{
	WCHAR szLine[1000];
	CTxtReader reader('#');
	if(!reader.Open(pszFileName)) return -1;
	int i,j;
	//�����ǰ��������
	m_arrCntxtRemark.RemoveAll();
	m_arrContext.RemoveAll();
	for( i=0;i<m_arrWord.GetSize();i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		for( j=0;j<pwp->cpp;j++)
		{
			if(pwp->pi[j].pArrCntxt)
			{
				delete pwp->pi[j].pArrCntxt;
				 pwp->pi[j].pArrCntxt=NULL;
			}
		}
	}

	int nRet=0;
	BOOL bRead=reader.getline(szLine,100);
	//�����������б���һЩ�໥������������ȥ��
	SPELLPHRASE spPhrase;
	while(bRead)
	{
		if(szLine[0]=='@')
		{//ע����
			m_arrCntxtRemark.AddString(szLine+1);
		}else
		{//������
			nRet++;
			memset(&spPhrase,0,sizeof(spPhrase));
			MakeSpellPhrase(szLine,&spPhrase);
			for(char c=0;c<spPhrase.cPPSize;c++)
			{//Ϊ�������е����ж����ָ����������б�
				WORDPHONE wp;
				wp.wWord = spPhrase.szBuf[spPhrase.cPPPos[c]];
				int idx=m_arrWord.SortFind(&wp,WordPhoneCmp,0);//�ڵ��ֱ����ҵ��ö����֡�
				if(idx!=-1)
				{
					BYTE byPhone[2];
					WORDPHONE *pwp=m_arrWord.GetPtAt(idx);
					if(pwp->cpp>1 && g_SpellData.Spell2ID(spPhrase.cPPSp[c],-1,byPhone))
					{//ƴ������Ҫ��Ч
						for(char j=0;j<pwp->cpp;j++)
						{
							if(memcmp(pwp->pi[j].byPhone,byPhone,2)==0)
							{//�ҵ�����λ��,ʹ�õ�ǰ�������������¸ö�����������
								PPCONTEXT2 newContext;
								newContext.cLocate=spPhrase.cPPPos[c];
								wcscpy(newContext.szPhrase,spPhrase.szBuf);
								//�Ƚϵ�ǰ�������뼺�������ĵĹ�ϵ
								if(pwp->pi[j].pArrCntxt2==NULL) 
									pwp->pi[j].pArrCntxt2=new CSArray<PPCONTEXT2>;

								CSArray<PPCONTEXT2> *pCntxt=pwp->pi[j].pArrCntxt2;

								BOOL bExist=FALSE;
								for(short k=pCntxt->GetSize()-1;k>=0;k--)
								{
									PPCONTEXT2 *pcntxt=pCntxt->GetPtAt(k);
									WCHAR *pResult=wcsstr(newContext.szPhrase,pcntxt->szPhrase);
									if(pResult && newContext.cLocate-(pResult-newContext.szPhrase) == pcntxt->cLocate )
									{//�µ�����������Ч�����Ѿ�����
										bExist=TRUE;
										break;
									}
									pResult=wcsstr(pcntxt->szPhrase,newContext.szPhrase);
									if(pResult && pcntxt->cLocate-(pResult-pcntxt->szPhrase) == newContext.cLocate )
									{//�µ������İ���ԭ�����ĵ���Ч����,ɾ��ԭ������
										pCntxt->RemoveAt(k);
									}
								}
								if(!bExist)	pCntxt->Add(newContext);
								break;
							}
						}
					}
				}
			}
		}
		bRead=reader.getline(szLine,100);
	}
	reader.Close();

	//�ҳ����ж����ֵ������Ĳ�������뵽m_arrContext��
	WString t;
	for(i=0;i<m_arrWord.GetSize();i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		for(char j=0;j<pwp->cpp;j++)
		{
			if(!pwp->pi[j].pArrCntxt2) continue;
			for(short k=0;k<pwp->pi[j].pArrCntxt2->GetSize();k++)
			{
				PPCONTEXT2 cntxt2=pwp->pi[j].pArrCntxt2->GetAt(k);
				wcscpy(t.szBuf,cntxt2.szPhrase);
				t.cLen=wcslen(t.szBuf);
				m_arrContext.SortInsert(t,FALSE,TextCmp);
			}
		}
	}	
	//�����ж����ֵ���������Ϣ��һ������������
	for(i=0;i<m_arrWord.GetSize();i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		for(char j=0;j<pwp->cpp;j++)
		{
			if(!pwp->pi[j].pArrCntxt2) continue;
			CSArray<PPCONTEXT> *pArrCntxt=new CSArray<PPCONTEXT>;
			for(short k=0;k<pwp->pi[j].pArrCntxt2->GetSize();k++)
			{
				PPCONTEXT2 cntxt2=pwp->pi[j].pArrCntxt2->GetAt(k);
				t.cLen=wcslen(cntxt2.szPhrase);
				wcscpy(t.szBuf,cntxt2.szPhrase);
				int nIdx=m_arrContext.SortFind(&t,TextCmp);
				ASSERT(nIdx!=-1);
				PPCONTEXT cntxt;
				cntxt.cLocate=cntxt2.cLocate;
				cntxt.dwIndex=nIdx;
				pArrCntxt->Add(cntxt);
			}
			delete pwp->pi[j].pArrCntxt2;
			pwp->pi[j].pArrCntxt=pArrCntxt;
		}
	}
	return nRet;
}

BOOL CSpellEngine::Save(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("wb"));
	if(!f) return FALSE;
	ISDOC_WriteHeader(f,FLAG_SPELLENGINE,SPVER_MAJON,SPVER_MINOR2,0);
	m_spellMap.WriteData(f);
	//д���������Ϣ
	int i,nCount=m_arrWord.GetSize();
	fwrite(&nCount,sizeof(int),1,f);
	for(i=0;i<nCount;i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		fwrite(&pwp->wWord,2,1,f);
		fwrite(pwp->byPhone,1,2,f);
		fwrite(&pwp->cpp,1,1,f);
		for(int j=0;j<pwp->cpp;j++)
		{
			fwrite(pwp->pi[j].byPhone,1,2,f);
			short sCount=0;
			if(pwp->pi[j].pArrCntxt) sCount=pwp->pi[j].pArrCntxt->GetSize();
			fwrite(&sCount,sizeof(short),1,f);
			for(short k=0;k<sCount;k++)
			{
				PPCONTEXT *pCntxt=pwp->pi[j].pArrCntxt->GetPtAt(k);
				fwrite(pCntxt,sizeof(PPCONTEXT),1,f);
			}
		}
	}
	//д��������
	nCount=m_arrContext.GetSize();
	fwrite(&nCount,sizeof(int),1,f);
	for(i=0;i<nCount;i++)
	{
		WString *pt=m_arrContext.GetPtAt(i);
		fwrite(&pt->cLen,1,1,f);
		fwrite(pt->szBuf,sizeof(WCHAR),pt->cLen,f);
	}
	//Save remark
	nCount=m_arrCntxtRemark.GetSize();
	fwrite(&nCount,sizeof(int),1,f);
	for(i=0;i<nCount;i++)
	{
		BYTE bySize=(BYTE)m_arrCntxtRemark[i].nLen;
		fwrite(&bySize,1,1,f);
		fwrite(m_arrCntxtRemark[i].pszBuf,sizeof(WCHAR),bySize,f);
	}
	fclose(f);
	return TRUE;	
}

BOOL CSpellEngine::Open(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,FLAG_SPELLENGINE,SPVER_MAJON,SPVER_MINOR2))
	{
		fclose(f);
		return FALSE;
	}
	//����ƴ������ӳ���
	m_spellMap.LoadData(f);
	
	//�����������Ϣ
	int i,nCount=0;
	fread(&nCount,sizeof(int),1,f);
	m_arrWord.SetSize(nCount);
	WORDPHONE wp;
	for(i=0;i<nCount;i++)
	{
		memset(&wp,0,sizeof(wp));
		fread(&wp.wWord,2,1,f);
		fread(wp.byPhone,1,2,f);
		fread(&wp.cpp,1,1,f);
		for(int j=0;j<wp.cpp;j++)
		{
			fread(wp.pi[j].byPhone,1,2,f);
			short sCount=0;
			fread(&sCount,sizeof(short),1,f);
			if(sCount){
				wp.pi[j].pArrCntxt=new CSArray<PPCONTEXT>(sCount);
				wp.pi[j].pArrCntxt->SetSize(sCount);
			}
			PPCONTEXT cntxt;
			for(short k=0;k<sCount;k++)
			{
				fread(&cntxt,sizeof(cntxt),1,f);
				wp.pi[j].pArrCntxt->SetAt(k,cntxt);
			}
		}
		m_arrWord.SetAt(i,wp);
	}
	//����������
	nCount=0;
	fread(&nCount,sizeof(int),1,f);
	m_arrContext.SetSize(nCount);
	WString t;
	for(i=0;i<nCount;i++)
	{
		memset(&t,0,sizeof(t));
		fread(&t.cLen,1,1,f);
		fread(t.szBuf,sizeof(WCHAR),t.cLen,f);
		m_arrContext.SetAt(i,t);
	}
	//load remark
	nCount=0;
	fread(&nCount,sizeof(int),1,f);
	WCHAR szBuf[256];
	for(i=0;i<nCount;i++)
	{
		BYTE bySize=0;
		fread(&bySize,1,1,f);
		if(fread(szBuf,sizeof(WCHAR),bySize,f)!=bySize) break;
		szBuf[bySize]=0;
		m_arrCntxtRemark.AddString(szBuf);
	}

	fclose(f);
	return TRUE;
}

//����һ�仰��һ���ֵĶ���������Ƕ���������������Ĳ�����ƥ��Ķ���
//LPCSTR pszSent,int nLen,int iWord:����ľ��Ӽ�Ŀ�������ڵ�λ��
//BYTE byPhone[2]:����ƴ��ID
//BOOL bPolyPhoneDef:û���ҵ�������ʱ�Ƿ�ʹ��Ĭ�϶������
BOOL CSpellEngine::CalcPolyPhone(LPCWSTR pszSent,int nLen,int iWord,BYTE byPhone[2],BOOL bPolyPhoneDef/*=TRUE*/)
{
	//�ҵ����־ݶ�����Ϣ
	WORDPHONE wp;
	wp.wWord = pszSent[iWord];
	int nIdx=m_arrWord.SortFind(&wp,WordPhoneCmp);
	if(nIdx==-1) return FALSE;
	WORDPHONE *pwp=m_arrWord.GetPtAt(nIdx);
	if(pwp->cpp==1)
	{//���Ƕ�����
		memcpy(byPhone,pwp->byPhone,2);
		return TRUE;
	}
	if(bPolyPhoneDef)
		memcpy(byPhone,pwp->byPhone,2);
	else
		memset(byPhone,0xFF,2);
	
	char cMatchLen=0;	//ƥ�䳤��
	for(char i=0;i<pwp->cpp;i++)
	{
		if(!pwp->pi[i].pArrCntxt) continue;	//�����ж�����������
		BOOL bFind=FALSE;
		for(int j=0;j<pwp->pi[i].pArrCntxt->GetSize();j++)
		{//����������
			PPCONTEXT *pCntxt=pwp->pi[i].pArrCntxt->GetPtAt(j);
			WString *pt=m_arrContext.GetPtAt(pCntxt->dwIndex);
			if(pt->cLen<cMatchLen || pCntxt->cLocate>iWord || nLen-(iWord-pCntxt->cLocate)<pt->cLen) continue;
			if(wcsncmp(pszSent+(iWord-pCntxt->cLocate),pt->szBuf,pt->cLen)==0)
			{
				memcpy(byPhone,pwp->pi[i].byPhone,2);
				cMatchLen=pt->cLen;
				break;
			}
		}
	}
	return cMatchLen>0 || bPolyPhoneDef;
}

//---------------------------------------------------------------
//	��ȡһ�����ӵ�ƴ��
//	LPCSTR pszSent:Ŀ�����
//	BYTE byPhoneIDs[][2]:���ƴ��ID
//	int nSize:�������
//	BOOL bPolyPhoneDef:û�ж�����������ʱ�Ƿ�ʹ��Ĭ��ƴ������
//---------------------------------------------------------------
int CSpellEngine::MakeSpell(LPCWSTR pszSent,int nLen, BYTE byPhoneIDs[][2],int nSize,BOOL bPolyPhoneDef/*=TRUE*/)
{
	if(nLen==-1) nLen=wcslen(pszSent);
	int nWords=nLen;
	int nRet=0;
	for(int i=0;i<nWords;i++)
	{
		if(i==nSize)
			break;
		if(!CWordID::IsHanzi(pszSent[i]))//ascii code
			break;
		if(!CalcPolyPhone(pszSent,nLen,i,byPhoneIDs[i],bPolyPhoneDef)) 
			break;
		nRet++;
	}
	return nRet;
}

int CSpellEngine::MakeSpell(LPCWSTR pszSent,int nLen, char szSpells[][7],int nSize,BOOL bPolyPhoneDef/*=TRUE*/)
{
	ASSERT(nSize<200);
	BYTE byIDs[200][2];
	int nRet=MakeSpell(pszSent,nLen,byIDs,nSize,bPolyPhoneDef);
	for(int i=0;i<nRet;i++)
	{
		if(memcmp(byIDs[i],"\0xFF\0xFF",2)==0)
			strcpy(szSpells[i],"none");
		else
			g_SpellData.ID2Spell(byIDs[i],szSpells[i]);
	}
	return nRet;
}

//����һ�������ƴ��,�����п��԰����������ַ�
//LPCSTR pszPhrase,int nLen:����
//char szSpell[][7]:����ƴ��,��������Ƕ�����ͬʱȱ��ƥ���������ʱ��ƴ��Ϊ��,������ֲ������Ļ�����ƴ�����ձ���û��ƴ��ҲΪ��
//���ػ��ƴ���ĵ�����
int CSpellEngine::GetPhraseSpell(LPCWSTR pszPhrase,int nLen,char szSpell[][7])
{
	LPCWSTR pSent=pszPhrase;
	int i=0,iWord=0,words=0;
	BYTE byID[2];
	int nRet=0;
	while(i<nLen)
	{
		if(pszPhrase[i]>127)
		{
			if(CalcPolyPhone(pSent,nLen-(pSent-pszPhrase),iWord,byID,FALSE))
			{
				g_SpellData.ID2Spell(byID,szSpell[words]);
				nRet++;
			}else
			{
				szSpell[words][0]=0;
			}
			i+=2;
			iWord++;
		}else
		{
			i++;
			pSent+=iWord*2+1;
			iWord=0;
			szSpell[words][0]=0;
		}
		words++;
	}
	return nRet;
}

//��ȡһ�����ֵ�ȫ������
BOOL CSpellEngine::GetWordSpell(WCHAR wChar, char *pszSpell)
{
	WORDPHONE *pwp=GetWordPhone(wChar);
	if(!pwp) return FALSE;
	char szSpell[7];
	for(char c=0;c<pwp->cpp;c++)
	{
		pszSpell[0]=0x20;
		pszSpell++;
		g_SpellData.ID2Spell(pwp->pi[c].byPhone,szSpell);
		strcpy(pszSpell,szSpell);
		pszSpell+=strlen(szSpell);
	}
	pszSpell[0]=0;
	return TRUE;
}

WORDPHONE * CSpellEngine::GetWordPhone(WCHAR wChar)
{
	WORDPHONE wp;
	wp.wWord = wChar;
	int nIdx=m_arrWord.SortFind(&wp,WordPhoneCmp);
	if(nIdx==-1) return NULL;
	return m_arrWord.GetPtAt(nIdx);
}

//ֻ����ƴ�������ֵ���Ϣ
BOOL CSpellEngine::SaveSpellMap(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("r+b"));
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,FLAG_SPELLENGINE,SPVER_MAJON,SPVER_MINOR2))
	{
		fclose(f);
		return FALSE;
	}
	//����ƴ������ӳ���
	m_spellMap.WriteData(f);
	fclose(f);
	return TRUE;
}
