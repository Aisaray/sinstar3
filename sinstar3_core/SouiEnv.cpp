#include "StdAfx.h"
#include "SouiEnv.h"

//��PE�ļ����أ�ע����ļ�����·��λ��
#define RES_TYPE 0
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

	m_pComMgr = new SComMgr(_T("imgdecoder-png"));

	//�����������·���޸ĵ���Ŀ����Ŀ¼���ڵ�Ŀ¼
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(hInst, szCurrentDir, sizeof(szCurrentDir));
	LPTSTR lpInsertPos = _tcsstr(szCurrentDir, _T("\\data"));
	_tcscpy(lpInsertPos,_T("\\sinstar3_core"));

	BOOL bLoaded=FALSE;
	CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
	CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
	bLoaded = m_pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
	SASSERT_FMT(bLoaded,_T("load interface [render] failed!"));
	bLoaded=m_pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));

	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
	
	m_theApp = new SApplication(pRenderFactory, hInst,_T("Sinstar3_Wnd"));
	m_theApp->SetAppDir(szCurrentDir);

	m_theApp->RegisterWindowClass<SToggle2>();
	m_theApp->RegisterWindowClass<SCandView>();
	m_theApp->RegisterWindowClass<SCaption2>();

	CAutoRefPtr<ILog4zManager> pLogMgr;
	bLoaded = m_pComMgr->CreateLog4z((IObjRef**)&pLogMgr);
	SASSERT_FMT(bLoaded,_T("load ILog4zManager failed!"),_T("log4z"));

	m_theApp->SetLogManager(pLogMgr);

	_tcscpy(lpInsertPos,_T("\\data\\log"));
	pLogMgr->setLoggerPath(0,S_CT2A(szCurrentDir));
	pLogMgr->start();

	//��DLL����ϵͳ��Դ
	HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
	if (hModSysResource)
	{
		CAutoRefPtr<IResProvider> sysResProvider;
		CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
		sysResProvider->Init((WPARAM)hModSysResource, 0);
		m_theApp->LoadSystemNamedResource(sysResProvider);
		FreeLibrary(hModSysResource);
	}else
	{
		SASSERT(0);
	}

	CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
	CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
	SStringT strPath = m_theApp->GetAppDir();
	strPath+=_T("\\uires");
	if (!pResProvider->Init((LPARAM)(LPCTSTR)strPath, 0))
	{
		SASSERT(0);
		return ;
	}
#else 
	CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
	pResProvider->Init((WPARAM)hInst, 0);
#endif

	m_theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);
	m_theApp->AddResProvider(pResProvider);

	new CDataCenter;
	new SNotifyCenter;
}

CSouiEnv::~CSouiEnv(void)
{
	delete SNotifyCenter::getSingletonPtr();
	delete CDataCenter::getSingletonPtr();
 	delete m_theApp;
	delete m_pComMgr;
	OleUninitialize();
}
