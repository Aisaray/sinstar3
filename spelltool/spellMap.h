#pragma once

#include "spelldata.h"
#include "skeytreeEx.h"
#include "sarray.hpp"

//<!һ��ƴ���������Ϣ
typedef struct tagSPELLCAND{
	BYTE byRate;	//����ʹ��Ƶ��
	BYTE byLen;		//==1��Ϊ������鱣��һ��
	WCHAR wWord;	//����
}SPELLCAND,*PSPELLCAND;

typedef struct tagWORDRATE
{
	WCHAR wWord;
	BYTE byRate;
}WORDRATE;

typedef struct tagSPELLCANDINFO
{
	WORD wCount;
	SPELLCAND *pCands;
}SPELLCANDINFO,*PSPELLCANDINFO;

int WordRateCmp(WORDRATE *pwr1,WORDRATE *pwr2,LPARAM lParam);

//!ƴ���뺺�ֱ��ӳ���ֻ�������֣���������
class CSpellMap
{
public:
	CSpellMap(){memset(m_SpellMap,0,sizeof(m_SpellMap));}
	~CSpellMap(){Free();}

	void SetSpellCands(short nIdxSpell,LPCWSTR pszCand,int nLen);

	void LoadData(FILE *f);

	void WriteData(FILE *f);

	void Free();


	PSPELLCAND GetFirstCand(short nIndex);

	SPELLCANDINFO * GetSpellMap(short sIdx)
	{
		ASSERT(sIdx>=0 && sIdx<SIZE_SPELL);
		return m_SpellMap+sIdx;
	}
	//-------------------------------------------------------------
	//	ͨ����Ƶ������ʼ�����ֵ���Ƶ
	//	CSArray<WORDRATE> *pArrWordRate:��Ƶ��
	//-------------------------------------------------------------
	void InitRate(CSArray<WORDRATE> *pArrWordRate);
private:
	SPELLCANDINFO m_SpellMap[SIZE_SPELL];	//ƴ�������,ÿ��ƴ��һ���б�
};
