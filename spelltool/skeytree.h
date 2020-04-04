//************************************************************
//	�ؼ�����
//	�ƽ��� 2003-10-22
//************************************************************
#ifndef SKEYTREE_H
#define SKEYTREE_H

#include "stree.hpp"

#pragma pack(push,1)
typedef struct tagBKT_NODE{
	DWORD	dwPosChild;			//�ӽڵ��ڻ������е�λ��
	DWORD	dwPosData;			//�����ڻ������е�λ��
	WORD	wChilds;			//�ӽڵ�����,��������256���ӽڵ�,��˱���ռ��2���ֽ�
	BYTE	byKey;				//�ؼ���
}BKT_NODE,*PBKT_NODE;
#pragma pack(pop)

typedef struct tagKEYEQUAL
{
	short sCount;	//�ȼ۹ؼ��ָ���,0�������з�֦��ʱpbyKeys==NULL
	BYTE *pbyKeys;	//�ȼ۹ؼ��ִ�
}KEYEQUAL,*PKEYEQUAL;

class CSBufKeyTree ;

//�ؼ���ö�ٻص��ӿ�
//CSBufKeyTree *pTree:������
//PBKT_NODE pNode:Ŀ��ڵ�
//int nSize:		�ܼ�����
//int iLevel:		��ǰ����
//BYTE *pbyKeys:	�ؼ��ִ�
//LPARAM lParam:	�û�����
//return: FALSE-ö�ٽ���,TRUE-����ö��
typedef BOOL (*CBENUMKEY)(CSBufKeyTree *pTree,PBKT_NODE pNode,int nSize,int iLevel,BYTE *pbyKeys,LPARAM lParam);

//�����ӽڵ�ö�ٻص��ӿ�
//CSBufKeyTree *pTree:������
//PBKT_NODE pNode:Ŀ��ڵ�
//int iLevel:		��ǰ����
//BYTE *pbyKeys:	�ؼ��ִ�
//LPARAM lParam:	�û�����
//return: FALSE-ö�ٽ���,TRUE-����ö��
typedef BOOL (*CBENUMTREE)(CSBufKeyTree *pTree,PBKT_NODE pNode,int iLevel,BYTE *pbyKeys,LPARAM lParam);

class CSBufKeyTree  
{
public:
	void Free();
	BOOL LoadData(FILE *f,BYTE byFlag);

	BOOL EnumTreeNode(PBKT_NODE pNode,CBENUMTREE funEnumTree,LPARAM lParam,int iLevel=0,BYTE *byKeyHead=NULL);
	void EnumKeyNode(BYTE *pbyKeys,int nSize,BYTE byWild,CBENUMKEY funEnumKey,LPARAM lParam,BOOL bDeepFirst);	
	void EnumKeyNode(PKEYEQUAL pKeyEqual,int nSize,CBENUMKEY funEnumKey,LPARAM lParam,BOOL bDeepFirst);
	BKT_NODE * IsKeyExist(BYTE *byKey,int nLen);
	BKT_NODE * GetChildHead(BKT_NODE * pNode)
	{
		if(pNode->wChilds==0) return NULL;
		return (BKT_NODE *)(m_pBuffer+pNode->dwPosChild);
	}

	CSBufKeyTree();
	virtual ~CSBufKeyTree();


	BOOL IsLeafNode(PBKT_NODE pNode)
	{
		return m_funIsLeaf(m_pBuffer+pNode->dwPosData);
	}
	
	void GetItemData(PBKT_NODE pNode,void *pData)
	{
		m_funReadData(m_pBuffer+pNode->dwPosData,pData);
	}
	LPBYTE GetItemDataPtr(PBKT_NODE pNode){return m_pBuffer+pNode->dwPosData;}

	BKT_NODE GetRoot()
	{
		BKT_NODE root;
		root.dwPosData=0xffffffff;	//û������
		root.byKey=0;				//���Բ�����ʹ�ø����ݣ���û������
		root.dwPosChild=2;
		memcpy(&root.wChilds,m_pBuffer,2);
		return root;
	}
	inline BYTE *GetBuffer()const
	{
		return m_pBuffer;
	}
	//************************************************************
	//	���ڴ��ж�ȡ����
	//************************************************************
	void (*m_funReadData)(BYTE *pBuf,void *pData);
	BOOL (*m_funIsLeaf)(BYTE *pBuf);
	BKT_NODE * FindKeyNode(BKT_NODE *pSibling,WORD wSiblings,BYTE byKey);
protected:
	BOOL _EnumKeyNode(PKEYEQUAL pKeyEqual,int nSize,CBENUMKEY funEnumKey,LPARAM lParam,PBKT_NODE pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst);
	BOOL _EnumKeyNode(BYTE *pbyKeys,int nSize,BYTE byWild,CBENUMKEY funEnumKey,LPARAM lParam,PBKT_NODE pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst);

	BYTE	 * m_pBuffer;
	DWORD		m_dwBufSize;
};

typedef struct tagKEYDATA{
	BYTE	c;		//�ַ�
	DWORD	data;	//����
}KEYDATA;

typedef void (*LOADDATAFUN)(DWORD &data, FILE *f);
typedef void (*WRITEDATAFUN)(DWORD &data, FILE *f);

class CSKeyTree:public CSTree<KEYDATA>
{
	//�ڴ���ʱ���ڹ�����ʱ������
	typedef struct _FILETREEDATA{
		DWORD	dwPosSelf;		//�Լ���λ��
		DWORD	dwPosData;		//����λ��
		WORD	wChilds;		//�ӽڵ����
		KEYDATA	keyData;
	}FILETREEDATA;

public:
	BOOL LoadData(FILE *f);
	void WriteData(FILE *f);
#ifdef _DEBUG
	//**********************************************************
	//	������������ı���ʽ��ӡ���ķ�֧�����ڵ���ʱ�鿴���ṹ
	//**********************************************************
	void Dump(HSTREEITEM hsNode=STVI_ROOT,int level=0);
#endif
	CSKeyTree(BYTE byFlag=0,DWORD dwInvalidKey=0){
		m_byFlag=byFlag;
		m_dwInvalidKey=dwInvalidKey;
		m_funLoadData=m_funWriteData=NULL;
	}
	
	virtual ~CSKeyTree(){
		DeleteAllItems();
	}
	
	//****************************************************
	//	����ӳ�����ݷǷ�ֵ��һ���ؼ��ִ�һ��Ҫ��Ӧһ������
	//	���һ��������ϵ�ӳ�������ǷǷ�ֵ���ʾ�ýڵ�����;���
	//****************************************************
	void SetInvalidKey(DWORD dwInvalidKey){
		m_dwInvalidKey=dwInvalidKey;
	}
	
	//**********************************************************
	//	�õ�һ����ַ�
	//**********************************************************
	BYTE GetItemChar(HSTREEITEM hsNode){
		return GetItemPt(hsNode)->c;
	}

	//*********************************************************
	//	�õ�һ�������
	//*********************************************************
	DWORD GetItemData(HSTREEITEM hsNode){
		return GetItemPt(hsNode)->data;
	}

	//********************************************************
	//	set key word data
	//*********************************************************
	void SetItemData(HSTREEITEM hsNode, DWORD data)
	{
		GetItemPt(hsNode)->data=data;
	}

	//*****************************************************
	//	���һ���ؼ���
	//	data:�ؼ��ֵĸ�������
	//	key:�ؼ����ַ���
	//	len:�ؼ����ַ������ȣ���1��ʾΪ�ַ���ȫ��
	//*****************************************************
	BOOL AddKey(DWORD data,const BYTE *key,int len=-1);

	//***************************************************
	//	���ؼ����Ƿ����
	//	key:����ؼ���
	//	len:����ؼ��ֳ���
	//	pdata:�ؼ��ֵĸ������ݣ�����ؼ��ִ�����
	//		  �ùؼ��ֵĸ������ݸ��Ƶ���ָ��ָ��ĵ�ַ��
	//***************************************************
	HSTREEITEM IsKeyExist(const BYTE *key,int len,DWORD *pdata=NULL);

	LOADDATAFUN		m_funLoadData;
	WRITEDATAFUN	m_funWriteData;
private:
	void LoadItem(HSTREEITEM hItem,PBKT_NODE pNode,FILE *f,DWORD dwRoot);
	void MakeFileTree(HSTREEITEM hItemSour,CSTree<FILETREEDATA> *pTreeTool,HSTREEITEM hItemDest);
	void WriteItem(CSTree<FILETREEDATA> *pTreeTool,HSTREEITEM hsItem,DWORD  dwRootOffset,FILE *f);
	DWORD	m_dwInvalidKey;
	BYTE	m_byFlag;		//���ݱ�־
};
#endif//#ifndef SKEYTREE_H
