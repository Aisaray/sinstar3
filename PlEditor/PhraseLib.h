// PhraseLib.h: interface for the CPLEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_)
#define AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "inputstardoc.h"
#include "GroupManager.h"
#include <string>
#include <vector>
#include "SArray.hpp"

using namespace std;

#define FN_PLSYS	"default.spl"

#define SSID_CL		'LC'
#define SVCL_MAJOR1	2005
#define SVCL_MINOR2	2
#define SVCL_MINOR3	3	//change to unicode

#define FLAG_INDEX		0xAA	//�������־
#define MAX_WILDMATCH	100		//���ܼ�ʱ����������
#define MAX_RATE		250		//���������Ƶ��������Ƶ�������ض�����
#define MAX_PHRASE		250		//max phrase length
#define MAX_ASSO_CANDS	100		//����������
#define RATE_DELETE		255

//#define MAX_WORDLEN		30		//same as iscomm.h
typedef struct tagPHRASE2
{
	BYTE	byGroup;	//������
	BYTE	byRate;		//��Ƶ����ʼʱΪ0
	BYTE	cLen;		//�ʳ�
	WCHAR 	szWord[MAX_PHRASE+1];	//������
}PHRASE2,*PPHRASE2;

typedef struct tagPHRASEINFO
{
	BYTE	byGroup;	//������
	BYTE	byRate;		//��Ƶ����ʼʱΪ0
}PHRASEINFO;

struct IProgListener{
	virtual void OnStart(DWORD dwMax) = 0;
	virtual void OnProg(DWORD dwProg,DWORD dwMax) = 0;
	virtual void OnEnd(bool bUpdateUI) = 0;
};

class CPLEditor  :public CGroupManager
{
public:
	struct IQueryRate
	{
		virtual BYTE QueryPhraseRate(LPCWSTR pszPhrase,BYTE byLen) = 0;
	};

	CPLEditor();
	virtual ~CPLEditor();
	BOOL Save(LPCTSTR pszFileName);
	BOOL Load(LPCTSTR pszFileName);
	void Free();
	void SetProgCallBack(IProgListener *funCb);
	void SetQueryRate(IQueryRate * pQuery );
	BOOL AddWord(LPCWSTR pszWord,BYTE cLen=0xff,BYTE byRate=0,BOOL bCheckNeed=TRUE,BYTE byGroup=0);
	void LoadData(FILE *f);
	void WriteData(FILE *f);
	int Import2Group(LPCTSTR pszFile,BYTE byRateMin, BYTE byRateMax,BYTE byDefault,BYTE iGroup=0xFF);
	BOOL ExportGroup(LPCTSTR pszFile,BYTE iGroup);
	BOOL EraseGroup(BYTE iGroup);
private:
	BOOL ParseLine(LPCWSTR pszLine,int &nBegin,int &nEnd);

	SMap<SStringW,PHRASE2> m_mapPhrase;
	BOOL			m_bModify;
	IProgListener * m_funProgCB;	//���Ȼص�
	IQueryRate	  * m_funQueryRate;
};

typedef struct tagMYRANGE
{
	int	nMin;
	int nMax;
}MYRANGE;

typedef struct tagPHRASE{
	BYTE	byRate;		//��Ƶ����ʼʱΪ0
	BYTE	cLen;		//�ʳ�
	WCHAR 	szWord[MAX_PHRASE+1];	//������
}PHRASE,*PPHRASE;

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
	static BOOL CheckPhraseLib(LPCTSTR pszFileName);

	BOOL Load(LPCTSTR pszFileName);
	void LoadData(FILE *f);
	BOOL Save(LPCTSTR pszFileName);
	void WriteData(FILE *f);
	void Free();
	void GroupValid(BYTE byGroup,BOOL bValid);
	BOOL GroupValid(LPCWSTR pszGroupName, BOOL bValid);
	MYRANGE GetPhraseHeadRange(PHRASE *p,int nMin=-1,int nMax=-1);
	BYTE GetPhraseGroup(DWORD dwIndex);
	BOOL IsPhraseValid(DWORD dwIndex);
	PPHRASE GetPhrase(DWORD index);
	LPBYTE GetBuf(){return m_pbyBuf;}
	DWORD GetWords(){return m_dwCount;}
	int QueryAssociate(LPCWSTR pszHead, BYTE cSize,CSArray<PPHRASE> *pArray);

	PPHRASE IsPhraseExist2(LPCWSTR pszText,BYTE cSize);

	DWORD IsPhraseExist(LPCWSTR pszText,BYTE cSize);

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
