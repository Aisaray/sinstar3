#ifndef ISCOMM_H
#define ISCOMM_H

#include "commfile.h"

//����֮��ͨѶʹ�õ��ڴ��ļ�ӳ�������
#define NAME_REQ_MAPFILE	_T("_sinstar3 req map file")	//�ͻ�����������������������ͨ��
#define MAX_BUF_REQ_MAP		1<<12						//4K						
#define MAX_BUF_REQ			(MAX_BUF_REQ_MAP)									
#define NAME_ACK_MAPFILE	_T("_sinstar3 ack map file")	//��������ͻ��˷���ָʾ������ͨ��
#define MAX_BUF_ACK_MAP		1<<15						//32K
#define MAX_BUF_ACK			((MAX_BUF_ACK_MAP)-sizeof(HWND))

//����֮��ͨѶʹ�õ�ͬ���¼�����,�ͻ���ͨ������lock��Mutex����֤�ܹ��������д��Ȩ�ޣ��Է�ֹ����ͻ���ͬʱд��
#define NAME_REQ_SYNCOMMUTEX	_T("_sinstar3 req synchronization mutex")

#define MSG_NAME_SINSTAR3		_T("_sinstar3 communicate msg")	//ͨѶʱʹ�õ���Ϣ���ƣ�ȡ���ϰ汾�̶�����ϢID�������������������

#define SVR_PROXY_NAME			TEXT("_sinstar3_svr_proxy_window_{AC7451C1-3528-42E4-ADF8-E35F3CF24646}")

#pragma pack(push,1)

typedef struct tagMSGDATA{
	short	sSize;		//�������ݳ���
	BYTE	byData[1];	//����������ʼ��ַ
}MSGDATA,*PMSGDATA;

typedef struct tagIMEFLAGDATA
{
	char * pData;
	unsigned int    nLen;
}IMEFLAGDATA;

typedef struct tagCOMPINFO
{
	char szName[50];	//����
	char szCode[50];	//��Ԫ
	char cWildChar;		//���ܼ�
	char cCodeMax;		//����볤
	char szUrl[50];		//�����ַ
	DWORD dwWords;		//�Ѿ������ַ���
	BOOL bSymbolFirst;	//����ױ���
	DWORD dwUnused:4;	//������ʹ�õı�־�������뷨���û������
	DWORD dwReserved:28;	//����,��ʼ��Ϊ0

	IMEFLAGDATA * pImeFlagData;
}COMPINFO,*PCOMPINFO;

typedef struct tagFLMINFO
{
	char szName[50];
	char szAddition[50];
	char szAddFont[LF_FACESIZE];
}FLMINFO,*PFLMINFO;
#pragma pack(pop)

#define CISIZE_BASE		164		//����������Ϣ������

#define MAX_SENTLEN		50		//�����󳤶�
#define MAX_WORDLEN		30		//������󳤶�
#define MAX_COMPLEN		20		//ϵͳ֧�ֵ�����볤

#define RATE_USERDEF	0xFF	//�Զ������ʹ�õĴ�Ƶ
#define RATE_FORECAST	0xFE	//����Ԥ��õ��Ĵ�ʹ�õĴ�Ƶ
#define RATE_GBK		0xFD	//GBK flag
#define RATE_USERCMD	0xFC	//Command ID
#define RATE_MIXSP		0xFB	//��ƴ����
#define RATE_WILD		0xFA	//���ܼ���ѯ���
#define RATE_ASSOCIATE	0xF9	//��������

//ϵͳ��Ӧ
#define ISACK_SUCCESS	0
#define ISACK_DELAY		1	
#define ISACK_UNKNOWN	100		//����ʶ�����Ϣ
#define ISACK_ERRCLIENT	101		//����Ŀͻ����ھ��
#define ISACK_SVRCANCEL	102		//�������쳣�˳�
#define ISACK_UNINIT	103		//ͨѶû��������ʼ��
#define ISACK_ERRBUF	104		//���ݳ��ȴ���
#define ISACK_ERROR		105		//һ���Դ���
	
//��ȡ��������ⷵ��ֵ
#define MCR_NORMAL		0		//������ȡ����
#define MCR_ASSOCIATE	1		//������,����Ԥ�⼰������ʾ
#define MCR_LONGERCOMP	(1<<1)	//�����Ե�ǰ����Ϊǰ׺�ĳ��Զ������
#define MCR_MIXSP		(1<<2)	//��ϼ�ƴ
#define MCR_USERDEF		(1<<3)	//����Զ�������
#define MCR_SPCAND		(1<<4)	//����ƴ������
#define MCR_WILD		(1<<6)	//���ܼ��Ĳ�ѯ
#define MCR_AUTOSELECT	(1<<7)	//Ψһ������

//����mask
#define MKI_ASTCAND		1		//��������
#define MKI_ASTSENT		(1<<1)	//��������
#define MKI_ASTENGLISH	(1<<2)	//Ӣ������
#define MKI_RECORD		(1<<3)	//��¼����
#define MKI_AUTOPICK	(1<<4)	//����ѡ��ʱ����������ӵĿ���
#define MKI_TTSINPUT	(1<<5)	//�ʶ���������
#define MKI_PHRASEREMIND (1<<6)	//��������

//��ѯ����mask
#define MQC_NORMAL		0		//��ѯ��ͨ����
#define MQC_FORECAST	1		//��ѯ�����Ԥ����
#define MQC_FCNOCAND	(1<<1)	//�޺�ѡ��ʱԤ��
#define MQC_MATCH		(1<<2)	//�Զ�����ƥ���������Ϊ��ѡ��
#define MQC_ONLYSC		(1<<3)	//���򲻳�ȫ
#define MQC_USERDEF		(1<<4)	//ͬʱ��ѯ�Զ������
#define MQC_AUTOPROMPT	(1<<5)	//���ܱ�����ʾ
#define MQC_SPCAND		(1<<6)	//����Զ�������

//TTSѡ��
#define MTTS_EN			1		//ʹ��Ӣ������	1
#define MTTS_CH			2		//ʹ����������

//rateAdjust mode
#define RAM_AUTO		0		//���ܵ�Ƶ
#define RAM_FAST		1		//���ٵ�Ƶ

#ifdef __cplusplus
extern "C"{
#endif//__cplusplus

BOOL ISComm_GetDataPathW(LPWSTR pszDataPath,int nLength);
BOOL ISComm_GetDataPathA(LPSTR pszDataPath, int nLength);

void ISComm_SetSvrPath(LPCTSTR pszPath);

void ISComm_FreeImeFlagData(IMEFLAGDATA *pData);

const UINT ISComm_GetCommMsgID();

DWORD ISComm_UpdateUserDefData(int nType, LPCSTR pszFilename);
DWORD ISComm_FatctUserDefFileName(int nType);

DWORD ISComm_SendMsg(DWORD dwType,const LPVOID pData,short sSize,HWND hWnd);
DWORD ISComm_PostMsg(DWORD dwType,const LPVOID pData,short sSize,HWND hWnd);
PMSGDATA ISComm_GetData();

BOOL ISComm_Login(HWND hWnd);
void  ISComm_Logout(HWND hWnd);

PCOMPINFO ISComm_GetCompInfo();
PFLMINFO  ISComm_GetFlmInfo();
char *    ISComm_GetUserDict();

PMSGDATA ISComm_OnSeverNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

DWORD ISComm_KeyIn(LPCSTR pszText,short sSize,BYTE byMask,HWND hWnd);

DWORD ISComm_QueryCand(LPCSTR pszComp,char cCompLen,BYTE byMask,HWND hWnd);
DWORD ISComm_QueryComp(LPCSTR pszPhrase,char cPhraseLen);
DWORD ISComm_Forecast(LPCSTR pszComp,char cCompLen);

DWORD ISComm_SpellQueryComp(LPCSTR pszPhrase,char cPhraseLen);
DWORD ISComm_SpellForecast(char szPrefix[2],LPBYTE *pbyBlurs,BYTE bySyllables);
DWORD ISComm_Spell2ID(LPCSTR pszSpell,char cCompLen);
DWORD ISComm_SpellGetBlur(LPCSTR pszSpell,char cCompLen);
DWORD ISComm_SpellQueryCandEx(LPBYTE *pbyBlur,BYTE bySyllables,BYTE byFlag);
DWORD ISComm_En2Ch(LPCSTR pszWord,char cWordLen);
DWORD ISComm_Ch2En(LPCSTR pszWord,char cWordLen);
DWORD ISComm_UDQueryCand(LPCSTR pszComp,char cCompLen);

DWORD ISComm_TTS(LPCSTR pszText,char cTextLen,BYTE byMask);
DWORD ISComm_RateAdjust(LPCSTR pszComp,char cCompLen,LPCSTR pszPhrase,char cPhraseLen,BYTE byMode,HWND hWnd);
DWORD ISComm_EnQueryCand(LPCSTR pszText,char cTextLen);
DWORD ISComm_MakePhrase(LPCSTR pszText,char cTextLen);
DWORD ISComm_ShowServer(LPCSTR pszPageName,char cTextLen);
DWORD ISComm_ServerVersion();
DWORD ISComm_PhraseDel(LPCSTR pszComp,char cCompLen,LPCSTR pszPhrase,char cPhraseLen,HWND hWnd);
DWORD ISComm_LineQueryCand(LPCSTR pszComp,char cCompLen);
int ISComm_PhraseRate(LPCSTR pszPhrase,char cPhraseLen);

DWORD ISComm_QueryUserDict(LPCSTR pszkey,char cKeyLen);
DWORD ISComm_SortWordByBiHua(LPCSTR pszBiHua,char cBiHuaLen,LPCSTR pszWordList,short nWords);

DWORD ISComm_Blur_Set(BOOL bBlur);
DWORD ISComm_Blur_Add(const char *pszSpell1, const char *pszSpell2);
DWORD ISComm_Blur_Del(const char *pszSpell1, const char *pszSpell2);
DWORD ISComm_Blur_Query();
DWORD ISComm_Blur_DelAll();

DWORD ISComm_QueryWordSpell(char szWord[2]);
DWORD ISComm_SpellMemoryEx(LPCSTR pszText,char cTextLen,BYTE (*pbySpellID)[2]);
DWORD ISComm_SymbolConvert(char cSymbol,char cType);

DWORD ISComm_UserDict_List();
DWORD ISComm_UserDict_Open(LPCSTR pszUserDict);
DWORD ISComm_UserDict_Max();
DWORD ISComm_UserDict_Item(DWORD dwItem);
DWORD ISComm_UserDict_Locate(LPCTSTR pszkey,char cKeyLen);

DWORD ISComm_Comp_List();
DWORD ISComm_Comp_Open(LPCSTR pszComp);
BOOL ISComm_IsSvrWorking();
DWORD ISComm_SkinChange(HWND hUIWnd);

BOOL  ISComm_CheckComp(LPCSTR pszComp,char cComplen,BYTE byMask);
DWORD ISComm_Bldsp_Get(BOOL *pbCe2,BOOL *pbCe3,BOOL *pbCa4);
DWORD ISComm_Bldsp_Set(BYTE byMask,BOOL bCe2,BOOL bCe3,BOOL bCa4);

BOOL  ISComm_SvrTray_Get();
void  ISComm_SvrTray_Set(BOOL bTray);

LPCSTR ISComm_Svr_Pages();


DWORD ISComm_GetTtsTokens();
DWORD ISComm_SetTtsToken(char bCh, int iToken);
DWORD ISComm_GetTtsSpeed();
DWORD ISComm_SetTtsSpeed(int nSpeed);

DWORD ISComm_BlurZCS(int bBlur);

DWORD ISComm_GetMaxPhrasePredictLength();
DWORD ISComm_SetMaxPhrasePredictLength(int nLen);

DWORD ISComm_GetMaxPhraseAssociateDeepness();
DWORD ISComm_SetMaxPhraseAssociateDeepness(int nDeepness);

DWORD ISComm_GetSentRecordMax();
DWORD ISComm_SetSentRecordMax(int nSentRecordMax);

DWORD ISComm_GetSvrAutorun();
DWORD ISComm_SetSvrAutorun(int bAutoRun);

DWORD ISComm_InstallPhraseLib(LPCSTR pszPlnameUtf8);

DWORD ISComm_QueryPhraseGroup();
DWORD ISComm_EnablePhraseGroup(LPCSTR pszGroupName, char bEnable);

DWORD ISComm_ImportUserLib(LPCSTR pszUserLibUtf8);
DWORD ISComm_ExportUserLib(LPCSTR pszUserLibUtf8);

DWORD ISComm_InstallComp(LPCSTR pszCompNameUtf8,char bApplyNow);

DWORD ISComm_Flm_List();
DWORD ISComm_Flm_Open(LPCSTR pszFlmUtf8);
DWORD ISComm_Flm_GetInfo();
DWORD ISComm_Flm_EnableGroup(LPCSTR pszGroup,char bEnable);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//ISCOMM_H