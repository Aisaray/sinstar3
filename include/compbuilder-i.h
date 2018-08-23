#pragma once

#ifdef ISCORE_EXPORTS
#define COMPBUILDER_API __declspec(dllexport)
#else
#define COMPBUILDER_API __declspec(dllimport)
#endif


#define RULE_MAX	8

struct ICodingRule
{
	virtual ~ICodingRule() {};

	virtual short MakeComp(LPBYTE *ppComps, char cSize, BYTE *pbyRet, short sBufSize) = 0;

	//************************************************
	//�������ʹ����ַ���
	//pszRule format:ce2=pij+pkl+nij+nkl
	//p:����n:����
	//***********************************************
	virtual BOOL ParseRule(LPCSTR pszRule) = 0;
	virtual BOOL ParseRules(char cCodeMax, BYTE byRules, char *pszBuf) = 0;

	//***************************************************
	//	���ɹ��ʹ����ַ���
	//***************************************************
	virtual BOOL MakeRule(int iRule, char *pszRule) = 0;

	//************************************************
	virtual void WriteEx(FILE *f) = 0;
	//*******************************************************
	//	�Ӵ��̶�ȡ
	//******************************************************
	virtual void Load(FILE *f) = 0;
	virtual void LoadEx(FILE *f) = 0;

};

typedef struct tagCOMPHEAD
{
	char szName[50];	//����
	char szCode[50];	//��Ԫ
	char cWildChar;		//���ܼ�
	char cCodeMax;		//����볤
	char szUrl[50];		//�����ַ
	DWORD dwWords;		//�Ѿ������ַ���
	BOOL bSymbolFirst;	//����ױ���
	DWORD dwEncrypt : 1;	//���ܱ�־
	DWORD dwYinXingMa : 1;	//�������Ż���־
	DWORD dwPhraseCompPart : 1;	//������벻һ����ȫ��
	DWORD dwAutoSelect : 1;	//��һ�����Զ�����
	DWORD dwLicense : 1;		//������Ҫ��Ȩ����ʹ��
	DWORD dwReserved : 27;	//����,��ʼ��Ϊ0
}COMPHEAD;

struct ICompBuilder
{
	virtual ~ICompBuilder() {}

	virtual ICodingRule * CreateCodingRule() = 0;
	virtual void DestroyCodingRule(ICodingRule * pRule) = 0;

	virtual BOOL Make(LPCSTR pszOutput,	//����ļ���
		COMPHEAD headInfo,	//������Ϣ
		ICodingRule *pRule,	//��ʹ���
		LPCSTR pszIconFile,//ͼ���ļ�
		COLORREF crKey,		//ͼ��͸��ɫ
		LPCSTR pszKeyMap,	//�ָ����ļ�
		const char	szKeyDllMD5[32]//�����㷨dll��MD5ֵ,32�ֽ�
	) = 0;

	virtual BOOL AddWord(char szWord[2], LPCSTR pszComp, char cCompLen = -1) = 0;
	virtual DWORD GetWords() const = 0;
};

struct ICompReaderEx
{
	virtual ~ICompReaderEx() {}
	virtual BOOL Export(LPCSTR pszFileName) = 0;
	virtual BOOL Load(LPCSTR pszCompFile) = 0;
	virtual const COMPHEAD *GetProps() const = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
	COMPBUILDER_API ICompBuilder * ICompBuilder_Create();
	COMPBUILDER_API void ICompBuilder_Destroy(ICompBuilder* pCompBuilder);

	COMPBUILDER_API ICompReaderEx * ICompReader_Create();
	COMPBUILDER_API void ICompReader_Destroy(ICompReaderEx* pCompReader);
#ifdef __cplusplus
}
#endif
