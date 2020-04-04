#ifndef _INPUTSTARDOC_H_
#define _INPUTSTARDOC_H_

#define ISSAVEID	'SI'

typedef struct tagSISHEAD{
	WORD	wISID;			// Must "IS" ISSAVEID
	WORD	wFileID;		// �ļ����� ID
	WORD	wVersionMajor;	// ���汾��
	WORD	wVersionMinor;	// ���汾��
	DWORD	dwData;			// ��������
}SISHEAD,*PSISHEAD;

BOOL ISDOC_ReadHeader(FILE *f,WORD wFileID,WORD wVerMajor,WORD wVerMinor,DWORD *pdwData=NULL,SISHEAD *pHead=NULL);
void ISDOC_WriteHeader(FILE *f,WORD wFileID,WORD wVerMajor,WORD wVerMinor,DWORD dwData=0,SISHEAD *pHead=NULL);
BOOL ISDOC_ReadHeader2(const void *pBuf,int nLen,WORD wFileID,WORD wVerMajor,WORD wVerMinor,DWORD *pdwData=NULL,SISHEAD *pHead=NULL);

#endif//_INPUTSTARDOC_H_