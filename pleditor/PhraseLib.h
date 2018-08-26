// PhraseLib.h: interface for the CPLEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_)
#define AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "inputstardoc.h"
#include "sarray.hpp"
#include "GroupManager.h"
#include "progcallback.h"
#include "../iscomm/iscomm.h"

#define FN_PLSYS	"default.spl"
#define FN_PLUSER	"user.dat"
#define FN_PLIDXREV	"plIdxRev.dat"
#define FN_USERDEL	"userdel.txt"

#define SSID_CL		'LC'
#define SVCL_MAJOR1	2005
#define SVCL_MINOR2	2

#define FLAG_INDEX		0xAA	//�������־
#define MAX_WILDMATCH	100		//���ܼ�ʱ����������
#define MAX_RATE		250		//���������Ƶ��������Ƶ�������ض�����

typedef struct tagPHRASE{
	BYTE	byRate;		//��Ƶ����ʼʱΪ0
	BYTE	cLen;		//�ʳ�
	char 	szWord[MAX_WORDLEN+1];	//������
}PHRASE,*PPHRASE;

typedef struct tagPHRASE2
{
	BYTE	byGroup;	//������
	BYTE	byRate;		//��Ƶ����ʼʱΪ0
	BYTE	cLen;		//�ʳ�
	char 	szWord[MAX_WORDLEN+1];	//������
}PHRASE2,*PPHRASE2;


//�û��ʿ��ѡ����Ϣ
typedef struct tagPHRASECAND
{
	PPHRASE pPhrase;
	char szComp[MAX_COMPLEN];
	char cCompLen;
}PHRASECAND,*PPHRASECAND;

class CPLEditor  :public CGroupManager
{
public:
	CPLEditor();
	virtual ~CPLEditor();
	BOOL Save(LPCTSTR pszFileName);
	BOOL Load(LPCTSTR pszFileName);
	void Free();
	void SetProgCallBack(FunProgCallback funCb,LPARAM cbParam)
	{
		m_funProgCB=funCb;
		m_cbParam=cbParam;
	}
	BOOL AddWord(LPCTSTR pszWord,char cLen=-1,BYTE byRate=0,BOOL bCheckNeed=TRUE,BYTE byGroup=0);
	void LoadData(FILE *f);
	void WriteData(FILE *f);
private:
	CSArray<PHRASE2> m_arrPhrase;
	BOOL			m_bModify;
	FunProgCallback	m_funProgCB;	//���Ȼص�
	LPARAM			m_cbParam;		//�ص�Param
};

typedef struct tagMYRANGE
{
	int	nMin;
	int nMax;
}MYRANGE;

class CPLReader
{
public:

	CPLReader()
	{
		m_pbyBuf=NULL;
		m_dwBufSize=0;
		m_dwCount=0;
		m_byGroups=0;
		m_pGroupInfo=NULL;
	}

	~CPLReader()
	{
		Free();
	}

	BOOL Load(LPCTSTR pszFileName);
	void LoadData(FILE *f);
	BOOL Save(LPCTSTR pszFileName);
	void WriteData(FILE *f);
	void Free();
	void GroupValid(BYTE byGroup,BOOL bValid);
	MYRANGE GetPhraseHeadRange(PHRASE *p,int nMin=-1,int nMax=-1);
	BYTE GetPhraseGroup(DWORD dwIndex);
	BOOL IsPhraseValid(DWORD dwIndex);
	PPHRASE GetPhrase(DWORD index);
	LPBYTE GetBuf(){return m_pbyBuf;}
	DWORD GetWords(){return m_dwCount;}
	int QueryAssociate(LPCTSTR pszHead, char cSize,CSArray<PPHRASE> *pArray);
	
	PPHRASE IsPhraseExist2(LPCTSTR pszText,char cSize)
	{
		DWORD dwIndex=IsPhraseExist(pszText,cSize);
		if(dwIndex==-1) return NULL;
		if(!IsPhraseValid(dwIndex)) return NULL;
		PPHRASE pp=GetPhrase(dwIndex);
		if(pp->cLen!=cSize) return NULL;
		return pp;
	}

	DWORD IsPhraseExist(LPCTSTR pszText,char cSize);
	
	DWORD			m_dwID;		//�ʿ��ID���༭�ʿ�ʱ��GetTickCount����
	GROUPINFO		*	m_pGroupInfo;//�ʿ���
	BYTE			m_byGroups;	//�ʿ�������
private:
	DWORD			m_dwCount;
	BYTE		*	m_pbyBuf;	//������
	DWORD			m_dwBufSize;
	CSArray<DWORD>	m_arrIndex;	//��������������
};

#endif // !defined(AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_)
