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
	void REGISTERCORE_API Sinstar_Init(LPCTSTR pszPath);
	LPCTSTR REGISTERCORE_API Sinstar_GetErrMsg();
	BOOL REGISTERCORE_API Sinstar_ShowCaller();
	BOOL REGISTERCORE_API Sinstar_IsRunning();
	BOOL REGISTERCORE_API Sinstar_Update();
	BOOL REGISTERCORE_API Sinstar_Uninstall();
	BOOL REGISTERCORE_API Sinstar_ForceUninstall();
	BOOL REGISTERCORE_API Sinstar_Install();
	BOOL REGISTERCORE_API Sinstar_GetCurrentVer(WORD wVers[4]);
	BOOL REGISTERCORE_API Sinstar_PEVersion(LPCTSTR pszFileName, WORD wVers[4]);
	BOOL REGISTERCORE_API Sinstar_CheckFiles();
};
