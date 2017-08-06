#include "StdAfx.h"
#include "SouiEnv.h"


//��PE�ļ����أ�ע����ļ�����·��λ��
#define RES_TYPE 1
// #define RES_TYPE 0   //���ļ��м�����Դ������ʧ���ٴ�PE����
// #define RES_TYPE 1   //��PE��Դ�м���UI��Դ
// #define RES_TYPE 2   //��zip���м�����Դ

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif


//����Ψһ��һ��R,UIRES����,ROBJ_IN_CPP��resource.h�ж���ĺꡣ
ROBJ_IN_CPP

template<>
CSouiEnv* SSingleton<CSouiEnv>::ms_Singleton = NULL;

CSouiEnv::CSouiEnv(HINSTANCE hInst)
{
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	int nRet = 0;

	m_pComMgr = new SComMgr;

	//�����������·���޸ĵ���Ŀ����Ŀ¼���ڵ�Ŀ¼
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
	//_tcscpy(lpInsertPos + 1, _T("..\\SouiWizard1"));
	SetCurrentDirectory(szCurrentDir);

	BOOL bLoaded=FALSE;
	CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
	CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
	bLoaded = m_pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
	SASSERT_FMT(bLoaded,_T("load interface [render] failed!"));
	bLoaded=m_pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));

	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
	SApplication *theApp = new SApplication(pRenderFactory, hInst);
	//��DLL����ϵͳ��Դ
	HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
	if (hModSysResource)
	{
		CAutoRefPtr<IResProvider> sysResProvider;
		CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
		sysResProvider->Init((WPARAM)hModSysResource, 0);
		theApp->LoadSystemNamedResource(sysResProvider);
		FreeLibrary(hModSysResource);
	}else
	{
		SASSERT(0);
	}

	CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
	CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
	if (!pResProvider->Init((LPARAM)_T("uires"), 0))
	{
		SASSERT(0);
		return 1;
	}
#else 
	CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
	pResProvider->Init((WPARAM)hInst, 0);
#endif

	theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);
	theApp->AddResProvider(pResProvider);

}

CSouiEnv::~CSouiEnv(void)
{
	delete m_theApp;
	delete m_pComMgr;
	OleUninitialize();
}
