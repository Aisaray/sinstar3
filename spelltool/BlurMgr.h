// BlurMgr.h: interface for the CBlurMgr class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <afxtempl.h>

typedef struct tagBLURSYLLABLE
{//����ģ��
	BYTE bySyllable1;
	BYTE bySyllable2;
}BLURSYLLABLE;

typedef struct tagBLURSPELL
{//ƴ��ģ��
	BYTE bySpell1[2];
	BYTE bySpell2[2];
}BLURSPELL;


class CBlurMgr  
{
public:
	CBlurMgr();
	virtual ~CBlurMgr();

	void AddBlurTune(BYTE byTune1,BYTE byTune2);
	void AddBlurRhyme(BYTE byRhyme1,BYTE byRhyme2);
	void AddBlurSpell(BYTE bySpell1[2],BYTE bySpell2[2]);

	int GetTuneBLurs(BYTE byTune, BYTE byBlurTune[/*SIZE_TUNE*/]);
	int GetRhymeBLurs(BYTE byRhyme, BYTE byBlurRhyme[/*SIZE_RHYME*/]);
	int GetSpellBlurs(BYTE byID[2],BYTE byBlurSpells[/*SIZE_SPELL*/][2]);

	CArray<BLURSYLLABLE,BLURSYLLABLE&>	m_arBlurTune;	//��ĸģ��
	CArray<BLURSYLLABLE,BLURSYLLABLE&>	m_arBlurRhyme;	//��ĸģ��
	CArray<BLURSPELL,BLURSPELL&>		m_arBlurSpell;	//ȫƴ��ģ��
};

