// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� REGISTERCORE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// REGISTERCORE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef REGISTERCORE_EXPORTS
#define REGISTERCORE_API __declspec(dllexport) 
#else
#define REGISTERCORE_API __declspec(dllimport) 
#endif
//#define RC_API __stdcall
#define RC_API 

EXTERN_C{
	void REGISTERCORE_API RC_API  Sinstar_InitW(LPCWSTR pszPath);
	void REGISTERCORE_API RC_API  Sinstar_InitA(LPCSTR pszPath);
	void REGISTERCORE_API RC_API  Sinstar_QuitServer();
	BOOL REGISTERCORE_API RC_API  Sinstar_GetInstallDir(LPWSTR  pszPath,int nSize);
	LPCWSTR REGISTERCORE_API RC_API  Sinstar_GetErrMsgW();
	LPCSTR REGISTERCORE_API RC_API  Sinstar_GetErrMsgA();
	BOOL REGISTERCORE_API RC_API  Sinstar_ShowCaller(BOOL bOrg);
	BOOL REGISTERCORE_API RC_API  Sinstar_IsRunning();
	BOOL REGISTERCORE_API RC_API  Sinstar_Update();
	BOOL REGISTERCORE_API RC_API  Sinstar_Uninstall();
	BOOL REGISTERCORE_API RC_API  Sinstar_ForceUninstall();
	BOOL REGISTERCORE_API RC_API  Sinstar_Install();
	BOOL REGISTERCORE_API RC_API  Sinstar_GetCurrentVer(int wVers[4]);
	BOOL REGISTERCORE_API RC_API  Sinstar_GetCurrentVer2(int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API RC_API  Sinstar_PEVersionW(LPCWSTR pszFileName, int wVers[4]);
	BOOL REGISTERCORE_API RC_API  Sinstar_PEVersionA(LPCSTR pszFileName, int wVers[4]);
	BOOL REGISTERCORE_API RC_API  Sinstar_PEVersion2W(LPCWSTR pszFileName, int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API RC_API  Sinstar_PEVersion2A(LPCSTR pszFileName, int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API RC_API  Sinstar_CheckFiles();
};

#ifdef _UNICODE
#define Sinstar_PEVersion Sinstar_PEVersionW
#define Sinstar_Init	  Sinstar_InitW
#define Sinstar_GetErrMsg Sinstar_GetErrMsgW
#else 
#define Sinstar_PEVersion Sinstar_PEVersionA
#define Sinstar_Init	  Sinstar_InitA
#define Sinstar_GetErrMsg Sinstar_GetErrMsgA
#endif
