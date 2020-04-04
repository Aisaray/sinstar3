//************************************************************
//	�ؼ�������չ
//	�ƽ��� 2005-7-4
//************************************************************
#ifndef SKEYTREEEX_H
#define SKEYTREEEX_H

#include "stree.hpp"

#pragma pack(push,1)
typedef struct tagBKT_NODEEX{
	DWORD	dwPosChild;			//�ӽڵ��ڻ������е�λ��
	DWORD	dwPosData;			//�����ڻ������е�λ��
	WORD	wChilds;			//�ӽڵ�����,�ֵܽڵ㲻�ܳ���65534
	BYTE	byKey[1];			//�ؼ��ִ�
}BKT_NODEEX,*PBKT_NODEEX;
#pragma pack(pop)

typedef struct tagKEYEQUALEX
{
	WORD wCount;	//�ȼ۹ؼ��ָ��� 0~65534 ,0�������з�֦��ʱpbyKeys==NULL
	BYTE *pbyKeys;	//�ȼ۹ؼ��ִ�
}KEYEQUALEX,*PKEYEQUALEX;


class CSBufKeyTreeEx ;

//�ؼ���ö�ٻص��ӿ�
//CSBufKeyTreeEx *pTree:������
//PBKT_NODEEX pNode:Ŀ��ڵ�
//int nSize:		�ܼ�����
//int iLevel:		��ǰ����
//BYTE *pbyKeys:	�ؼ��ִ�
//LPARAM lParam:	�û�����
//return: FALSE-ö�ٽ���,TRUE-����ö��
typedef BOOL (*CBENUMKEYEX)(CSBufKeyTreeEx *pTree,PBKT_NODEEX pNode,int nSize,int iLevel,BYTE *pbyKeys,LPARAM lParam);

//�����ӽڵ�ö�ٻص��ӿ�
//CSBufKeyTreeEx *pTree:������
//PBKT_NODEEX pNode:Ŀ��ڵ�
//int iLevel:		��ǰ����
//BYTE *pbyKeys:	�ؼ��ִ�
//LPARAM lParam:	�û�����
//return: FALSE-ö�ٽ���,TRUE-����ö��
typedef BOOL (*CBENUMTREEEX)(CSBufKeyTreeEx *pTree,PBKT_NODEEX pNode,int iLevel,BYTE *pbyKeys,LPARAM lParam);

class CSBufKeyTreeEx  
{
public:
	void Free();
	BOOL LoadData(FILE *f,BYTE byFlag);
	BOOL WriteData(FILE *f,BYTE byFlag);
	
	BOOL EnumTreeNode(PBKT_NODEEX pNode,CBENUMTREEEX funEnumTree,LPARAM lParam,int iLevel=0,BYTE *byKeyHead=NULL);
	void EnumKeyNode(PKEYEQUALEX pKeyEqual,int nSize,CBENUMKEYEX funEnumKey,LPARAM lParam,BOOL bDeepFirst=FALSE);
	BKT_NODEEX * IsKeyExist(BYTE *byKey,int nLen);
	BKT_NODEEX * GetChildHead(BKT_NODEEX * pNode)
	{
		if(pNode->wChilds==0) return NULL;
		return (BKT_NODEEX *)(m_pBuffer+pNode->dwPosChild);
	}

	CSBufKeyTreeEx();
	virtual ~CSBufKeyTreeEx();


	BOOL IsLeafNode(PBKT_NODEEX pNode)
	{
		ASSERT(m_funIsLeaf);
		return m_funIsLeaf(m_pBuffer+pNode->dwPosData);
	}
	
	BOOL GetItemData(PBKT_NODEEX pNode,void *pData)
	{
		ASSERT(m_funReadData);
		return m_funReadData(m_pBuffer+pNode->dwPosData,pData);
	}
	LPBYTE GetItemDataPtr(PBKT_NODEEX pNode){return m_pBuffer+pNode->dwPosData;}
	char GetKeySize(){ return m_cKeySize;}

	BKT_NODEEX GetRoot()
	{
		BKT_NODEEX root;
		root.dwPosData=0xffffffff;	//û������
		root.byKey[0]=0;			//���Բ�����ʹ�ø����ݣ���û������
		root.dwPosChild=2;
		memcpy(&root.wChilds,m_pBuffer,2);
		return root;
	}
	inline BYTE *GetBuffer()const
	{
		return m_pBuffer;
	}
	inline int NODESIZE(){ return sizeof(BKT_NODEEX)-1+m_cKeySize;}

	inline BKT_NODEEX * NextNode(BKT_NODEEX *pNode,int nOffset=1)
	{
		return (BKT_NODEEX *)((BYTE*)pNode+NODESIZE()*nOffset);
	}
	
	//���������ֵܽڵ��ľ���
	inline int NodeSub(PBKT_NODEEX pNode1,PBKT_NODEEX pNode2)
	{
		if(pNode1==pNode2) return 0;
		BYTE *p1=(BYTE*)pNode1;
		BYTE *p2=(BYTE*)pNode2;
		return (p1-p2)/NODESIZE();
	}
	//************************************************************
	//	���ڴ��ж�ȡ����
	//************************************************************
	BOOL (*m_funReadData)(BYTE *pBuf,void *pData);
	BOOL (*m_funIsLeaf)(BYTE *pBuf);

	int MatchTestMax(BYTE *pbyKeys, int nSize ,PBKT_NODEEX *pMatchItem=NULL);
	PBKT_NODEEX FindNextLeaf(BKT_NODEEX * hsParent,BYTE *pbyKeys, int nSize,int *pDeep=NULL);
	PBKT_NODEEX FindKeyNode(BKT_NODEEX *pSibling,WORD wSiblings,BYTE *pbyKey);
protected:
	BOOL _EnumKeyNode(PKEYEQUALEX pKeyEqual,int nSize,CBENUMKEYEX funHandle,LPARAM lParam,PBKT_NODEEX pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst=FALSE);

	char	   m_cKeySize;	//�ؼ��ֳ���
	BYTE	 * m_pBuffer;
	DWORD		m_dwBufSize;
};

typedef struct tagKEYDATAEX{
	DWORD	data;		//����
	BYTE	byKey[1];	//�ؼ��ִ�
}KEYDATAEX,*PKEYDATAEX;

typedef void (*LOADDATAFUN)(DWORD &data, FILE *f);
typedef void (*WRITEDATAFUN)(DWORD &data, FILE *f);

class CSKeyTreeEx;
//�ؼ��������ؼ��ֳ��ȿɱ�,�ӽڵ㰴�ؼ��ִ�С�������򣬱ȽϺ���memcmp
typedef BOOL (*CBENUMCHILD)(CSKeyTreeEx *pKeyTree,HSTREEITEM hsItem,BYTE *pbyKeys,int iLevel,LPARAM lParam);
class CSKeyTreeEx:public CSTree<KEYDATAEX *>
{
	//�ڴ���ʱ���ڹ�����ʱ������
	typedef struct _FILETREEDATA{
		DWORD	dwPosSelf;		//�Լ���λ��
		DWORD	dwPosData;		//����λ��
		WORD	wChilds;		//�ӽڵ����
		KEYDATAEX *pKEYDATAEX;		//�ؼ�������
	}FILETREEDATA;

public:
	int MatchTest(BYTE *pbyKeys,int nSize,HSTREEITEM *pMatchItem=NULL);
	BOOL LoadData(FILE *f);
	void WriteData(FILE *f);
#ifdef _DEBUG
	//--------------------------------------------------------------------
	//	������������ı���ʽ��ӡ���ķ�֧�����ڵ���ʱ�鿴���ṹ
	//--------------------------------------------------------------------
	void Dump(HSTREEITEM hsNode=STVI_ROOT,int level=0);
#endif
	CSKeyTreeEx(BYTE byFlag=0,DWORD dwInvalidKey=0){
		m_byFlag=byFlag;
		m_dwInvalidKey=dwInvalidKey;
		m_funLoadData=m_funWriteData=NULL;
		m_funFreeData=FreeItem;
		m_cKeySize=1;
	}
	
	virtual ~CSKeyTreeEx(){
		DeleteAllItems();
	}
	
	//--------------------------------------------------------------------
	//	����ӳ�����ݷǷ�ֵ��һ���ؼ��ִ�һ��Ҫ��Ӧһ������
	//	���һ��������ϵ�ӳ�������ǷǷ�ֵ���ʾ�ýڵ�����;���
	//--------------------------------------------------------------------
	void SetInvalidKey(DWORD dwInvalidKey){
		m_dwInvalidKey=dwInvalidKey;
	}
	
	void SetFlag(BYTE byFlag)
	{
		m_byFlag=byFlag;
	}
	void SetKeySize(char cKeySize){ m_cKeySize=cKeySize;}
	//--------------------------------------------------------------------
	//	�õ�һ����ַ�
	//--------------------------------------------------------------------
	BYTE * GetItemChar(HSTREEITEM hsNode){
		return GetItem(hsNode)->byKey;
	}

	//--------------------------------------------------------------------
	//	�õ�һ�������
	//--------------------------------------------------------------------
	DWORD GetItemData(HSTREEITEM hsNode){
		return GetItem(hsNode)->data;
	}

	//--------------------------------------------------------------------
	//	set key word data
	//--------------------------------------------------------------------
	void SetItemData(HSTREEITEM hsNode, DWORD data)
	{
		GetItem(hsNode)->data=data;
	}

	//--------------------------------------------------------------------
	//	���һ���ؼ���
	//	data:�ؼ��ֵĸ�������
	//	key:�ؼ����ַ���
	//	len:�ؼ����ַ������ȣ���1��ʾΪ�ַ���ȫ��
	//	return:���һ������Ľڵ�
	//--------------------------------------------------------------------
	HSTREEITEM AddKey(DWORD data,const BYTE *key,int len=-1);

	//-------------------------------------------------------------------
	//	ö�����нڵ�
	//	CBENUMCHILD cbEnumChild:ö�ٻص�
	//	BYTE *pKeyBuf:���ڵ����ݴ�
	//-------------------------------------------------------------------
	BOOL EnumTreeNode(HSTREEITEM hParent,CBENUMCHILD cbEnumChild,LPARAM lParam,int iLevel=0,BYTE *byKeyHead=NULL,BOOL bOnlyLeaf=TRUE);


	//--------------------------------------------------------------------
	//	���ؼ����Ƿ����
	//	key:����ؼ���
	//	len:����ؼ��ֳ���
	//	pdata:�ؼ��ֵĸ������ݣ�����ؼ��ִ�����
	//		  �ùؼ��ֵĸ������ݸ��Ƶ���ָ��ָ��ĵ�ַ��
	//--------------------------------------------------------------------
	HSTREEITEM IsKeyExist(const BYTE *key,int len,DWORD *pdata=NULL);


	KEYDATAEX * NewItem(const BYTE *byKey,DWORD dwData)
	{
		KEYDATAEX *pRet= (KEYDATAEX *)malloc(sizeof(DWORD)+m_cKeySize);
		pRet->data=dwData;
		if(byKey) memcpy(pRet->byKey,byKey,m_cKeySize);
		return pRet;
	}

	static void FreeItem(const PKEYDATAEX & pData)
	{
		free(pData);
	}


	LOADDATAFUN		m_funLoadData;
	WRITEDATAFUN	m_funWriteData;
private:
	void LoadItem(HSTREEITEM hItem,PBKT_NODEEX pNode,FILE *f,DWORD dwRoot);
	void MakeFileTree(HSTREEITEM hItemSour,CSTree<FILETREEDATA> *pTreeTool,HSTREEITEM hItemDest);
	void WriteItem(CSTree<FILETREEDATA> *pTreeTool,HSTREEITEM hsItem,DWORD  dwRootOffset,FILE *f);

	DWORD	m_dwInvalidKey;
	BYTE	m_byFlag;		//���ݱ�־
	char	m_cKeySize;		//�ؼ��ֳ��ȣ�һ�������Ҫ����4
};
#endif//#ifndef SKEYTREEEX_H
