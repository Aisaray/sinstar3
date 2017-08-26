#pragma  once


#ifdef ENABLE_LANGUAGEBAR
#include <msctf.h>
#endif//ENABLE_LANGUAGEBAR

//����Composition Stringһ��Range�����ԣ���TSF��������һ��
#define RG_ATTR_INPUT  0 
#define RG_ATTR_TARGET_CONVERTED  1 
#define RG_ATTR_CONVERTED  2 
#define RG_ATTR_TARGET_NOTCONVERTED 3 
#define RG_ATTR_INPUT_ERROR  4 
#define RG_ATTR_FIXEDCONVERTED  5 
#define RG_ATTR_OTHER  -1

#ifndef TF_MOD_ALT
#define TF_MOD_ALT                         0x0001
#define TF_MOD_CONTROL                     0x0002
#define TF_MOD_SHIFT                       0x0004
#define TF_MOD_RALT                        0x0008
#define TF_MOD_RCONTROL                    0x0010
#define TF_MOD_RSHIFT                      0x0020
#define TF_MOD_LALT                        0x0040
#define TF_MOD_LCONTROL                    0x0080
#define TF_MOD_LSHIFT                      0x0100
#define TF_MOD_ON_KEYUP                    0x0200
#define TF_MOD_IGNORE_ALL_MODIFIER         0x0400
#endif//TF_MOD_ALT

//�ȼ����壬ͬTSF��TF_PRESERVEDKEY����
typedef struct _PRESERVEDKEY
{
	UINT uVKey;
	UINT uModifiers;
}PRESERVEDKEY;

// ����ģʽ
enum EInputMethod
{
	HalfAlphanumeric=100,// ���Ӣ������
	HalfNative,			 // ��Ǻ���
	FullNative,			 // ȫ�Ǻ���
};

interface ITextService
{
	virtual BOOL IsCompositing()=NULL;
	virtual void StartComposition(LPVOID lpImeContext)=NULL;

	//ʹ��ָ�������滻��ǰ���봮
	// nLeft,nRight: [-1,-1]:�ڵ�ǰ���λ�ò���
	//				 [0,-1]:�滻ȫ������
	//				 [left,right]:�滻�÷�Χ��������Ϊ��ֵ
	virtual void ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)=NULL;
	virtual void UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)=NULL;
	virtual void EndComposition(LPVOID lpImeContext)=NULL;
	virtual int  MoveCaretPos(LPVOID lpImeContext,int nPos,BOOL bSet)=NULL;
	virtual LPVOID GetImeContext()=NULL;
	virtual BOOL   ReleaseImeContext(LPVOID lpImeContext)=NULL;
	virtual void  SetConversionMode(EInputMethod mode)=NULL;
	virtual EInputMethod GetConversionMode()=NULL;
	virtual BOOL  RegisterIMEHotKey(REFGUID guidHotKey,LPCWSTR pszName,const PRESERVEDKEY *pKey)=NULL;
	virtual BOOL  UnregisterIMEHotKey(REFGUID guidHotKey,const PRESERVEDKEY *pKey)=NULL;
	virtual BOOL SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)=NULL;
	virtual BOOL GetOpenStatus(LPVOID lpImeContext)=NULL;
	virtual LRESULT DoWildRequest(WPARAM wParam,LPARAM lParam)=NULL;
#ifdef ENABLE_LANGUAGEBAR
	virtual BOOL GetLanguageBarItemMgr(ITfLangBarItemMgr **ppLangBarMgr,GUID *pGuidTIP)=NULL;
#endif//ENABLE_LANGUAGEBAR
};