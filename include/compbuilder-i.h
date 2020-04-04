#pragma once

#ifdef ISCORE_EXPORTS
#define COMPBUILDER_API __declspec(dllexport)
#else
#define COMPBUILDER_API __declspec(dllimport)
#endif

#include "global.h"
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
	virtual BOOL ParseRule(LPCWSTR pszRule) = 0;
	virtual BOOL ParseRules(char cCodeMax, BYTE byRules, LPCWSTR pszBuf) = 0;

	//***************************************************
	//	���ɹ��ʹ����ַ���
	//***************************************************
	virtual BOOL MakeRule(int iRule, WCHAR *pszRule) = 0;

	//************************************************
	virtual void WriteEx(FILE *f) = 0;
	//*******************************************************
	//	�Ӵ��̶�ȡ
	//******************************************************
	virtual void Load(FILE *f) = 0;
	virtual void LoadEx(FILE *f) = 0;

};

struct ICompBuilder
{
	virtual ~ICompBuilder() {}

	virtual ICodingRule * CreateCodingRule() = 0;
	virtual void DestroyCodingRule(ICodingRule * pRule) = 0;

	virtual BOOL Make(LPCTSTR pszOutput,	//����ļ���
		COMPHEAD headInfo,	//������Ϣ
		ICodingRule *pRule,	//��ʹ���
		LPCTSTR pszIconFile,//ͼ���ļ�
		COLORREF crKey,		//ͼ��͸��ɫ
		LPCTSTR pszKeyMap	//�ָ����ļ�
	) = 0;

	virtual BOOL AddWord(WCHAR wWord, LPCWSTR pszComp, char cCompLen = -1) = 0;
	virtual BOOL AddMakePhraseCode(WCHAR wWord, LPCWSTR pszMakePhraseCode, char cCodeLen = -1) = 0;
	virtual DWORD GetWords() const = 0;
};

struct ICompReaderEx
{
	virtual ~ICompReaderEx() {}
	virtual BOOL Export(LPCTSTR pszFileName) = 0;
	virtual BOOL Load(LPCTSTR pszCompFile) = 0;
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
