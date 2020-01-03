//	PhraseLib.cpp: implementation of the CPLEditor class.
//	�ʿ��Ϊ2���֣�	1������������Ĵʿ�;
//					2��ÿһ�����ڿ��е�ƫ������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPLEditor::CPLEditor()
{
	m_bModify=FALSE;
	m_funProgCB=NULL;
	m_cbParam=0;
}

CPLEditor::~CPLEditor()
{
	Free();
}

void CPLEditor::Free()
{
	CGroupManager::Free();
	m_mapPhrase.RemoveAll();
	m_bModify=FALSE;
}

int Phrase2Cmp(void *ctx,const void *p1,const void *p2)
{
	const PHRASE2 *pSour=(const PHRASE2*)p1;
	const PHRASE2 *pDest = (const PHRASE2 *)p2;
	return strcmp(pSour->szWord,pDest->szWord);
}

//-------------------------------------------------
//	���ʿ������һ���´ʻ�,-1:dest phrase has existed and failed
//	BOOL bCheckNeed:����Ƿ���ڵĿ���,����������ʱ����ΪFALSE
//	BYTE byGroup:�����Ĵʿ���
//---------------------------------------------------
BOOL CPLEditor::AddWord(LPCSTR pszWord,char cLen/*=-1*/,BYTE byRate/*=0*/,BOOL bCheckNeed/*=TRUE*/,BYTE byGroup/*=0*/)
{
	if(byGroup>=m_arrGroup.size()) return FALSE;
	if(cLen==-1) cLen=strlen(pszWord);
	if(cLen>MAX_WORDLEN) return FALSE;

	PHRASE2 p={byGroup,byRate,cLen};
	strncpy(p.szWord,pszWord,cLen);
	p.szWord[cLen]=0;

	SStringA key(p.szWord,cLen);
	if(bCheckNeed)
	{
		if(m_mapPhrase.Lookup(key)!=NULL)
			return FALSE;
	}
	m_mapPhrase[key]=p;
	m_arrGroup[byGroup].dwCount++;
	m_bModify=TRUE;
	return TRUE;
}


void CPLEditor::LoadData(FILE *f)
{
	CGroupManager::Read(f);
	DWORD dwCount=CGroupManager::GetCount();
	BYTE byRate;
	char cLen;
	char szWord[MAX_WORDLEN];
	fseek(f,sizeof(DWORD),SEEK_CUR);//���dwDataSize
	BYTE byGroup=0;
	if(m_funProgCB) m_funProgCB->OnStart(dwCount);
	for(int j=0;j<m_arrGroup.size();j++)
	{
		m_arrGroup[j].dwCount=0;
	}
	for(DWORD i=0;i<dwCount;i++)
	{
		fread(&byGroup,1,1,f);
		fread(&byRate,1,1,f);
		fread(&cLen,1,1,f);
		fread(szWord,1,cLen,f);
		AddWord(szWord,cLen,byRate,FALSE,byGroup);
		if(m_funProgCB) m_funProgCB->OnProg(i,dwCount);
	}
	if(m_funProgCB) m_funProgCB->OnEnd();
	m_bModify=FALSE;
}


//---------------------------------------------------------
//	format:head+indexPos+data+index
//---------------------------------------------------------
BOOL CPLEditor::Load(LPCTSTR pszFileName)
{
	Free();
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2))
	{
		fclose(f);
		return FALSE;
	}
	
	LoadData(f);

	fclose(f);
	return TRUE;
}

void CPLEditor::WriteData(FILE *f)
{
	CGroupManager::Write(f);//д��ʿ�����Ϣ
	DWORD dwDataSize=0;	//���ݳ���
	fwrite(&dwDataSize,sizeof(DWORD),1,f);
	//д�뾭��������Ĵʿ�����
	UINT nCount = m_mapPhrase.GetCount();
	PHRASE2 *pArr = (PHRASE2*) malloc(sizeof(PHRASE2)*nCount);
	SPOSITION pos = m_mapPhrase.GetStartPosition();
	int i=0;
	while(pos)
	{
		pArr[i++]=m_mapPhrase.GetNextValue(pos);
	}
	qsort_s(pArr,m_mapPhrase.GetCount(),sizeof(PHRASE2),Phrase2Cmp,NULL);

	DWORD *pdwOffset=(DWORD*)malloc(sizeof(DWORD)*nCount);
	LONG lBegin=ftell(f);
	if(m_funProgCB) m_funProgCB->OnStart(nCount);
	for(UINT i=0;i<nCount;i++)
	{
		pdwOffset[i]=ftell(f)-lBegin;
		const PHRASE2 &p=pArr[i];
		fwrite(&p.byGroup,1,1,f);
		fwrite(&p.byRate,1,1,f);
		fwrite(&p.cLen,1,1,f);
		fwrite(p.szWord,1,p.cLen,f);
	 	if(m_funProgCB) m_funProgCB->OnProg(i,nCount);
	}
	//�������ݳ���
	dwDataSize=ftell(f)-lBegin;
	if(m_funProgCB) m_funProgCB->OnEnd();
	//д��������
	fwrite(pdwOffset,sizeof(DWORD),nCount,f);
	free(pdwOffset);
	//��д���ݳ���
	fseek(f,lBegin-sizeof(DWORD),SEEK_SET);
	fwrite(&dwDataSize,sizeof(DWORD),1,f);
	fseek(f,0,SEEK_END);
	
	free(pArr);
}

BOOL CPLEditor::Save(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("wb"));
	if(!f) return FALSE;
	DWORD dwID=GetTickCount();
	ISDOC_WriteHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2,dwID);
	WriteData(f);
	fclose(f);
	return TRUE;
}

void CPLEditor::SetProgCallBack(IProgListener *funCb)
{
	m_funProgCB=funCb;
}
