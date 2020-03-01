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

EXTERN_C{
	void REGISTERCORE_API WINAPI  Sinstar_InitW(LPCWSTR pszPath);
	void REGISTERCORE_API WINAPI  Sinstar_InitA(LPCSTR pszPath);
	LPCWSTR REGISTERCORE_API WINAPI  Sinstar_GetErrMsgW();
	LPCSTR REGISTERCORE_API WINAPI  Sinstar_GetErrMsgA();
	BOOL REGISTERCORE_API WINAPI  Sinstar_ShowCaller();
	BOOL REGISTERCORE_API WINAPI  Sinstar_IsRunning();
	BOOL REGISTERCORE_API WINAPI  Sinstar_Update();
	BOOL REGISTERCORE_API WINAPI  Sinstar_Uninstall();
	BOOL REGISTERCORE_API WINAPI  Sinstar_ForceUninstall();
	BOOL REGISTERCORE_API WINAPI  Sinstar_Install();
	BOOL REGISTERCORE_API WINAPI  Sinstar_GetCurrentVer(int wVers[4]);
	BOOL REGISTERCORE_API WINAPI  Sinstar_GetCurrentVer2(int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API WINAPI  Sinstar_PEVersionW(LPCWSTR pszFileName, int wVers[4]);
	BOOL REGISTERCORE_API WINAPI  Sinstar_PEVersionA(LPCSTR pszFileName, int wVers[4]);
	BOOL REGISTERCORE_API WINAPI  Sinstar_PEVersion2W(LPCWSTR pszFileName, int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API WINAPI  Sinstar_PEVersion2A(LPCSTR pszFileName, int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API WINAPI  Sinstar_CheckFiles();
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
