#include "StdAfx.h"
#include "WordRate.h"
#include "TxtReader.h"

#pragma pack(push,1)
struct WordRate{
	wchar_t wIndex;
	BYTE    byRate;
};
#pragma pack(pop)

static const char KRateHeader[4]="SWR";
#define VER_MAJOR 3
#define VER_MINOR 0
static const WORD KRateVer=(VER_MAJOR<<8)|VER_MINOR;

CWordRate::CWordRate(void)
{
}

CWordRate::~CWordRate(void)
{
}

CWordRate & CWordRate::GetInstance()
{
	static CWordRate s_instance;
	return s_instance;
}


BOOL CWordRate::_LoadFromFile(LPCTSTR pszRateFile)
{
	FILE *f = _tfopen(pszRateFile,_T("rb"));
	if(!f)
		return FALSE;
	char szHeader[sizeof(KRateHeader)]={0};
	WORD wVer = 0;
	//read header and ver
	fread(szHeader,1,sizeof(KRateHeader),f);
	fread(&wVer,sizeof(WORD),1,f);
	if(strncmp(szHeader,KRateHeader,sizeof(KRateHeader))!=0
		|| wVer!=KRateVer)
	{
		fclose(f);
		return FALSE;
	}
	m_mapRate.clear();
	//read count
	DWORD dwCount=0;
	fread(&dwCount,sizeof(DWORD),1,f);
	WordRate *pWordRate = new WordRate[dwCount];
	BOOL bRet = FALSE;
	if(pWordRate)
	{
		fread(pWordRate,sizeof(WordRate),dwCount,f);
		for(DWORD i=0;i<dwCount;i++)
		{
			m_mapRate.insert(std::make_pair(pWordRate[i].wIndex,pWordRate[i].byRate));
		}
		delete []pWordRate;
		bRet = TRUE;
	}
	fclose(f);

	return bRet;
}

BOOL CWordRate::LoadFromFile(LPCTSTR pszRateFile)
{
	return GetInstance()._LoadFromFile(pszRateFile);
}

BYTE CWordRate::_GetWordRate(wchar_t wIndex) const
{
	RATEMAP::const_iterator it = m_mapRate.find(wIndex);
	if(it==m_mapRate.end())
		return 0;
	return it->second;
}

BYTE CWordRate::GetWordRate(wchar_t wIndex)
{
	return GetInstance()._GetWordRate(wIndex);
}

BOOL CWordRate::_SaveToFile(LPCTSTR pszRateFile)
{
	FILE *f = _tfopen(pszRateFile,_T("wb"));
	if(!f)
		return FALSE;
	fwrite(KRateHeader,1,sizeof(KRateHeader),f);
	fwrite(&KRateVer,sizeof(KRateVer),1,f);
	DWORD dwCount = m_mapRate.size();
	fwrite(&dwCount,sizeof(DWORD),1,f);
	RATEMAP::const_iterator it = m_mapRate.begin();
	while(it!=m_mapRate.end())
	{
		WordRate wordRate={it->first,it->second};
		fwrite(&wordRate,sizeof(wordRate),1,f);
		it++;
	}
	fclose(f);
	return TRUE;
}

BOOL CWordRate::SaveToFile(LPCTSTR pszRateFile)
{
	return GetInstance()._SaveToFile(pszRateFile);
}

BOOL CWordRate::Export(LPCTSTR pszTxtFile)
{
	return GetInstance()._Export(pszTxtFile);
}

int CWordRate::Import(LPCTSTR pszTxtFile)
{
	return GetInstance()._Import(pszTxtFile);
}

static int WordRateCmpByRate( const void *arg1, const void *arg2 )
{
	const WordRate *p1=(const WordRate*)arg1;
	const WordRate *p2=(const WordRate*)arg2;
	return (int)p2->byRate-(int)p1->byRate;
}

BOOL CWordRate::_Export(LPCTSTR pszTxtFile) const
{
	FILE *f = _tfopen(pszTxtFile,_T("wb"));
	if(!f) return FALSE;
	
	const BYTE BOM[2]={0xff,0xfe};
	WordRate *pWordRate = new WordRate[m_mapRate.size()];
	UINT i=0;
	for(RATEMAP::const_iterator it=m_mapRate.begin();it!=m_mapRate.end();it++,i++)
	{
		pWordRate[i].wIndex=it->first;
		pWordRate[i].byRate=it->second;
	}
	qsort(pWordRate,m_mapRate.size(),sizeof(WordRate),WordRateCmpByRate);

	fwrite(BOM,1,2,f);
	for(i=0;i<m_mapRate.size();i++)
	{
		fwprintf(f,L"%c %u\r\n",pWordRate[i].wIndex,pWordRate[i].byRate);
	}
	fclose(f);
	delete []pWordRate;
	return TRUE;
}

int CWordRate::_Import(LPCTSTR pszTxtFile)
{
	CTxtReader reader;
	if(!reader.Open(pszTxtFile))
		return 0;
	WCHAR szBuf[50];
	int nCount = 0;
	while(reader.getline(szBuf,50))
	{
		WCHAR wWord;
		int nRate;
		if(szBuf[1]==VK_SPACE)
		{
			wWord=szBuf[0];
			nRate = _wtoi(szBuf+2);
			if(nRate>MAX_RATE)
				nRate=MAX_RATE;
			m_mapRate[wWord]=(BYTE)nRate;
			nCount ++;
		}
	}
	return nCount;
}


BOOL CWordRate::SetWordRate(WCHAR wIndex,BYTE byRate)
{
	if(byRate>MAX_RATE)
		return FALSE;
	GetInstance().m_mapRate[wIndex]=byRate;
	return TRUE;
}

void CWordRate::UpdateWordRate(WCHAR wIndex,int change)
{
	BYTE byRate = GetWordRate(wIndex);
	int sRate = byRate;
	sRate+=change;
	if(sRate>MAX_RATE)
		sRate=MAX_RATE;
	if(sRate<0)
		sRate=0;
	SetWordRate(wIndex,(BYTE)sRate);
}

void CWordRate::Clear()
{
	GetInstance()._Clear();
}

void CWordRate::_Clear()
{
	m_mapRate.clear();
}

int CWordRate::GetSize()
{
	return GetInstance()._GetSize();
}

int CWordRate::_GetSize() const
{
	return m_mapRate.size();
}

/*
������λ���估˳�� 
GBK �����˫�ֽڱ�ʾ��������뷶ΧΪ 8140-FEFE�����ֽ��� 81-FE ֮�䣬β�ֽ��� 40-FE ֮�䣬�޳� xx7F һ���ߡ��ܼ� 23940 ����λ�������� 21886 �����ֺ�ͼ�η��ţ����к��֣��������׺͹�����21003 ����ͼ�η��� 883 ���� 
ȫ�������Ϊ���󲿷֣� 

1. �������������� 
a. GB 2312 ���������� GBK/2: B0A1-F7FE����¼ GB 2312 ���� 6763 ������ԭ˳�����С� 
b. GB 13000.1 ���人������������ 
(1) GBK/3: 8140-A0FE����¼ GB 13000.1 �е� CJK ���� 6080 ���� 
(2) GBK/4: AA40-FEA0����¼ CJK ���ֺ������ĺ��� 8160 ����CJK ������ǰ���� UCS �����С���У������ĺ��֣��������׺͹������ں󣬰��������ֵ䡷��ҳ�룯��λ���С� 

2. ͼ�η������������� 
a. GB 2312 �Ǻ��ַ��������� GBK/1: A1A1-A9FE�����г� GB 2312 �ķ����⣬���� 10 ��Сд�������ֺ� GB 12345 �����ķ��š��Ʒ��� 717 ���� 
b. GB 13000.1 ����Ǻ��������� GBK/5: A840-A9A0��BIG-5 �Ǻ��ַ��š��ṹ���͡��������ڴ������Ʒ��� 166 ���� 
*/
#define SIZE_GBK1	846		//(A9-A1+1)*(FE-A1+1) ͼ�η�����
#define SIZE_GBK2	6768	//(F7-B0+1)*(FE-A1+1) GB 2312 ������
#define SIZE_GBK3	6112	//(A0-81+1)*(FE-40+1) CJK ���� 
#define SIZE_GBK4	8245	//(FE-AA+1)*(A0-40+1) ��¼ CJK ���ֺ������ĺ���
#define SIZE_GBK5	194		//(A9-A8+1)*(A0-40+1)


int CWordID::GetWordType(WCHAR wIndex)
{
	BYTE szWord[2];
	if(IsAlphabit(wIndex))
		return ASCII;
	if(!WideCharToMultiByte(936,0,&wIndex,1,(char*)szWord,2,NULL,NULL))
		return GBK_UNDEFINE;
	BYTE byLow=szWord[0],byHei=szWord[1];

	int nRet= GBK_UNDEFINE;
	if(byHei>=0xA1 && byLow<=0xFE)
	{
		if(byLow>=0x81 && byLow<=0xA0)
		{
			nRet=GBK3;
		}
		else if(byLow>=0xA1 && byLow<=0xA9)
		{
			nRet=GBK1;
		}
		else if(byLow>=0xB0 && byLow<=0xF7)
		{
			nRet=GBK2;
		}
	}
	else if(byHei>=0x40 && byHei<=0xA0)
	{
		if(byLow>=0x81 && byLow<=0xA0)
		{
			nRet=GBK3;
		}
		else if(byLow>=0xA8 && byLow<=0xA9)
		{
			nRet=GBK5;
		}
		else if(byLow>=0xAA && byLow<=0xFE)
		{
			nRet=GBK4;
		}
	}

	return nRet;
}

bool CWordID::IsHanzi(WCHAR wWord)
{
	return wWord>=128;
}

bool CWordID::IsAlphabit(WCHAR wWord)
{
	return wWord<128;
}

bool CWordID::IsValidWord(WCHAR wWord)
{
	return wWord<0xD800;
}

bool CWordID::IsGBK(WCHAR wIndex)
{
	int type = GetWordType(wIndex);
	return type > GBK2;
}
