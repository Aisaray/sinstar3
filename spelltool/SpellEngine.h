// SpellEngine.h: interface for the CSpellEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELLENGINE_H__792C3673_A29B_46E6_BB16_BFF5C19D5FE2__INCLUDED_)
#define AFX_SPELLENGINE_H__792C3673_A29B_46E6_BB16_BFF5C19D5FE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "inputstardoc.h"
#include "spelldata.h"
#include "spellmap.h"
#include "sarray.hpp"

#define FLAG_SPELLENGINE	'PS'
#define SPVER_MAJON			2007
#define SPVER_MINOR			1
#define SPVER_MINOR2		2 //unicode

#define NAME_SPELL			_T("spell.dat")

typedef struct tagPPCONTEXT
{
	DWORD dwIndex;	//���������б��е�����
	char  cLocate;	//�����ֵ�λ��
}PPCONTEXT;

typedef struct tagPPCONTEXT2
{
	char cLocate;
	WCHAR szPhrase[50];
}PPCONTEXT2;

typedef struct tagPHONEINFO
{
	BYTE byPhone[2];	//����
	union
	{
		CSArray<PPCONTEXT> *pArrCntxt;//�����ı�
		CSArray<PPCONTEXT2> *pArrCntxt2;//�ڶ�����ʽ�������� 
	};
}PHONEINFO;

#define MAX_WORDPHONE	6	//һ���������Ķ�����
typedef struct tagWORDPHONE
{
	WCHAR wWord;		//����
	BYTE byPhone[2];	//���ֵ�Ĭ�϶���
	PHONEINFO pi[MAX_WORDPHONE];	//���ֵĶ�������Ϣ
	char cpp;			//���ֵĶ����ָ���
}WORDPHONE;

typedef struct tagWString
{
	char cLen;
	WCHAR szBuf[50];
}WString;

typedef struct tagAString
{
	char cLen;
	char szBuf[50];
}AString;

#define MAX_SENTPP	10	//һ���������������ָ���
#include "strarray.h"
class CSpellEngine  
{
	typedef struct tagSPELLPHRASE
	{
		WCHAR szBuf[50];		//����
		char cLen;			//���鳤��
		char cPPSize;		//�����ָ���
		char cPPPos[MAX_SENTPP];	//ÿ�������ֵ�λ��
		char cPPSp[MAX_SENTPP][7];	//ÿ�������ֵĶ���
	}SPELLPHRASE;



public:
	CSpellEngine();
	virtual ~CSpellEngine();

	BOOL SaveSpellMap(LPCTSTR pszFileName);
	BOOL GetWordSpell(WCHAR wChar,char *pszSpell);
	WORDPHONE * GetWordPhone(WCHAR wChar);

	
	BOOL Open(LPCTSTR pszFileName);
	BOOL Save(LPCTSTR pszFileName);

	int GetPhraseSpell(LPCWSTR pszPhrase,int nLen,char szSpell[][7]);
	int Context_Export(LPCTSTR pszFileName);
	int Context_Import(LPCTSTR pszFileName);
#ifdef SPELLENGINE_EDITOR
	int ImportWord2Spell(LPCTSTR pszFileName);
	int ImportSpell2Word(LPCTSTR pszFileName);
	BOOL ImportWordRate(LPCTSTR pszFileName);
#endif// SPELLENGINE_EDITOR

	int MakeSpell(LPCWSTR pszSent,int nLen,BYTE szSpellIDs[][2],int nSize,BOOL bPolyPhoneDef=TRUE);
	int MakeSpell(LPCWSTR pszSent, int nLen,char szSpells[][7],int nSize,BOOL bPolyPhoneDef=TRUE);


	CSpellMap		m_spellMap;		//ƴ��������
	CSArray<WORDPHONE> m_arrWord;	//���ֵ�ƴ����Ϣ
	CSArray<WString>	m_arrContext;	//�����ı�
	CWStrArray	m_arrCntxtRemark;	//�����ı�ע�����԰������޸�ʱ�䣬�޸��˵�
private:
	BOOL CalcPolyPhone(LPCWSTR pszSent,int nLen,int iWord,BYTE byPhone[2],BOOL bPolyPhoneDef=TRUE);
	static void MakeSpellPhrase(LPCWSTR pszLine,SPELLPHRASE *pPhrase);
};
int WordPhoneCmp(WORDPHONE *spWord1,WORDPHONE *spWord2,LPARAM lParam);
int TextCmp(WString *pt1,WString *pt2,LPARAM lParam);
#endif // !defined(AFX_SPELLENGINE_H__792C3673_A29B_46E6_BB16_BFF5C19D5FE2__INCLUDED_)
