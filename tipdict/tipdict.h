// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� TIPDICT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// TIPDICT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef TIPDICT_EXPORTS
#define TIPDICT_API __declspec(dllexport)
#else
#define TIPDICT_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"{
#endif
TIPDICT_API int TipDict(LPCWSTR pszKey,int nKeyLen,LPWSTR pszBuf,int nBufLen);

#ifdef __cplusplus
}
#endif