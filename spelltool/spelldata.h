#pragma once

#include "skeytree.h"
#include "sarray.hpp"

#define SIZE_TUNE	24		//��ĸ����
#define SIZE_RHYME	33		//��ĸ����
#define SIZE_SPELL	407		//����ƴ������

#define SPELLID	'PSIS'

#define FLAG_REVS	0xFE	//������������־
#define FLAG_SPELL	0xFD	//����ƴ������
#define FLAG_RHYME	0xFC	//��ĸ������
#define FLAG_PHRASE	0xFB	//����������

#define MAX_SENTLEN		50		//�����󳤶�


typedef struct tagSPELLINFO
{
	char szSpell[7];
	BYTE byID[2];
}SPELLINFO,*PSPELLINFO;

typedef struct tagSPELLINDEX
{
	BYTE byID[2];
	short sIndex;
}SPELLINDEX;

typedef struct tagMYSPTEXT
{
	char cLen;
	char szText[5];
}MYSPTEXT;

class CSpellData
{	
public:
	CSpellData();
	virtual ~CSpellData();

	BOOL DetectSpell(LPCSTR pszSpell,int nLen);
	BOOL IsSpellValid(BYTE byID[2]);
	BYTE MakeupSpell(BYTE byTune,BYTE byRhyme[SIZE_RHYME]);
	short GetIDIndex(const BYTE byID[2]);
	BOOL GetRhymeID(LPCSTR pszRhyme,int nLen,BYTE *pbyRhymeID);
	BOOL GetRhymeID(LPCWSTR pszRhyme,int nLen,BYTE *pbyRhymeID);
	BOOL GetTuneID(LPCSTR pszTune,int nLen,BYTE *pbyTuneID);
	BOOL GetTuneID(LPCWSTR pszTune,int nLen,BYTE *pbyTundID);

	BOOL ID2Spell(BYTE byID[2],char szSpell[7]);
	BOOL Spell2ID(LPCSTR pszSpell,int nLen,BYTE byID[2]);
	BOOL Spell2ID(LPCWSTR pszSpell,int nLen,BYTE byID[2]);
	int Spell2IDEx(LPCSTR pszSpell,int nLen,BYTE byID[2]);
	int SplitSpell2Rhymes(LPCSTR pszSpell, int nLen, int cSpells[MAX_SENTLEN/2],bool bTestJP);
	int GuessRhymeIDs(const BYTE byID[2],BYTE outIDs[][2],int nLen);
private:
	int ExtractSpell(LPCSTR pszSpell, int nLen,BYTE byID[2]);
private:
	CSArray<SPELLINFO> m_arrSpInfo;
	CSArray<SPELLINDEX> m_arrSpIndex;
	CSArray<MYSPTEXT>		m_arrSpTune;
	CSArray<MYSPTEXT>	m_arrSpRhyme;
};

extern MYSPTEXT g_szTune[SIZE_TUNE];
extern MYSPTEXT g_szRhyme[SIZE_RHYME];
extern SPELLINFO g_SpellInfo[SIZE_SPELL];
extern CSpellData g_SpellData;