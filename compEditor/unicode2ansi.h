#ifndef UNICODE2ANSI_H
#define UNICODE2ANSI_H

#define TMPANSI_FILE "c:\\uni2ansi.tmp"

#define U2A_OK		0	//ת���ɹ�
#define U2A_CANCEL	1	//����UNICODE���룬δת��
#define U2A_FAIL	2	//ת��ʧ��

int Unicode2Ansi(BYTE *pBuffer,DWORD len);
BOOL CheckFileUnicode(LPCTSTR pszFileName);

#endif//UNICODE2ANSI_H