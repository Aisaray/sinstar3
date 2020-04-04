#include "stdafx.h"
#include "skeytreeEx.h"

//////////////////////////////////////////////////////////////
//	CSBufKeyTreeEx Implement
//////////////////////////////////////////////////////////////

CSBufKeyTreeEx::CSBufKeyTreeEx()
{
	m_funReadData=NULL;
	m_funIsLeaf=NULL;
	m_pBuffer=NULL;
	m_dwBufSize=0;
}

CSBufKeyTreeEx::~CSBufKeyTreeEx()
{
	Free();
}

//--------------------------------------------------------------------
//	�������еĹؼ��֣�������Ҷ�ӽڵ㻹�Ƿֲ�ڵ�
//	Ҫ�ж��ǲ���Ҷ�ӽڵ���Ե���IsLeafNode
//--------------------------------------------------------------------
BKT_NODEEX * CSBufKeyTreeEx::IsKeyExist(BYTE *byKey, int nLen)
{
	ASSERT(m_pBuffer);
	BKT_NODEEX root=GetRoot();
	BKT_NODEEX *pRetNode=NULL;
	BKT_NODEEX *pParentNode=&root;
	BKT_NODEEX *pChildNode=(BKT_NODEEX*)(m_pBuffer+2);
	for(int i=0;i<nLen;i+=m_cKeySize)
	{
		if(!(pRetNode=FindKeyNode(pChildNode,pParentNode->wChilds,byKey+i)))
			return NULL;
		else
		{
			pParentNode=pRetNode;
			pChildNode=(BKT_NODEEX*)(m_pBuffer+pRetNode->dwPosChild);
		}
	}
	return pRetNode;
}

BOOL CSBufKeyTreeEx::EnumTreeNode(PBKT_NODEEX pNode,CBENUMTREEEX funEnumTree,LPARAM lParam,int iLevel,BYTE *byKeyHead)
{
	BOOL bContinue=TRUE;
	PBKT_NODEEX pNodeChild=GetChildHead(pNode);
	for(WORD i=0;i<pNode->wChilds;i++)
	{
		if(byKeyHead) memcpy(byKeyHead+iLevel*m_cKeySize,pNode->byKey,m_cKeySize);
		if(IsLeafNode(pNodeChild)) bContinue=funEnumTree(this,pNodeChild,iLevel,byKeyHead,lParam);
		if(bContinue) bContinue=EnumTreeNode(pNodeChild,funEnumTree,lParam,iLevel+1,byKeyHead);
		if(!bContinue) break;
	}
	return bContinue;
}

void CSBufKeyTreeEx::EnumKeyNode(PKEYEQUALEX pKeyEqual,int nSize,CBENUMKEYEX funEnumKey,LPARAM lParam,BOOL bDeepFirst)
{
	ASSERT(nSize>=1);
	BKT_NODEEX root=GetRoot();
	BYTE *pbyKeyArray=new BYTE[m_cKeySize*nSize];
	_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,&root,0,pbyKeyArray,bDeepFirst);
	delete []pbyKeyArray;
}

BOOL CSBufKeyTreeEx::_EnumKeyNode(PKEYEQUALEX pKeyEqual,int nSize,CBENUMKEYEX funEnumKey,LPARAM lParam,PBKT_NODEEX pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst)
{
	ASSERT(nSize>=1 && iLevel<nSize);
	int nLeftSiblings=(int)pParent->wChilds;
	int nCount=pKeyEqual[iLevel].wCount;
	if(nCount==0) nCount=nLeftSiblings;

	BOOL bContinue=FALSE;
	PBKT_NODEEX pNodeChild=GetChildHead(pParent);
	PBKT_NODEEX pNodeDest=NULL;
	for(int i=0;i<nCount && nLeftSiblings>0 ;i++)
	{
		if(pKeyEqual[iLevel].wCount!=0)
			pNodeDest=FindKeyNode(pNodeChild,nLeftSiblings,pKeyEqual[iLevel].pbyKeys+i*m_cKeySize);
		else
			pNodeDest=pNodeChild;
		if(pNodeDest)
		{
			memcpy(pbyKeyArray+iLevel*m_cKeySize,pNodeDest->byKey,m_cKeySize);
			if(bDeepFirst)
			{
				if(iLevel+1<nSize) bContinue=_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
				if(bContinue && IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
			}else
			{
				if(IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
				if(bContinue && iLevel+1<nSize) bContinue=_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
			}
			nLeftSiblings-=NodeSub(pNodeDest,pNodeChild)+1;
			pNodeChild=NextNode(pNodeDest);
		}
		if(!bContinue) break;
	}
	return bContinue;
}


//--------------------------------------------------------------------
//	���ź�����ֵܽ���������ַ����ҹؼ������ڽڵ�
//--------------------------------------------------------------------
PBKT_NODEEX  CSBufKeyTreeEx::FindKeyNode(BKT_NODEEX *pSibling, WORD wSiblings, BYTE *pbyKey)
{
	if(wSiblings==0)
		return NULL;
	if(wSiblings==1)
	{
		if(memcmp(pSibling->byKey,pbyKey,m_cKeySize)==0)
			return pSibling;
		else
			return NULL;
	}else if(wSiblings==2)
	{
		BKT_NODEEX *pNext=NextNode(pSibling);
		if(memcmp(pSibling->byKey,pbyKey,m_cKeySize)==0)
			return pSibling;
		else if(memcmp(pNext->byKey,pbyKey,m_cKeySize)==0)
			return pNext;
		else
			return NULL;
	}else
	{
		WORD wHalf=wSiblings/2;
		BKT_NODEEX *pMiddle=NextNode(pSibling,wHalf);
		int nCmp=memcmp(pMiddle->byKey,pbyKey,m_cKeySize);
		if(nCmp==0)
			return pMiddle;
		else if(nCmp>0)
			return FindKeyNode(pSibling,wHalf,pbyKey);
		else//if(nCmp<0)
			return FindKeyNode(NextNode(pMiddle,1),wSiblings-wHalf-1,pbyKey);
	}
}

//-----------------------------------------------------------------
//	ƥ��̽��
//	BYTE *pbyKeys:Ҫƥ��Ĺؼ��ִ�
//	int nSize:�ؼ��ִ�����
//	HSTREEITEM *pMatchItem:���ƥ��Ľڵ�
//	return:���ƥ������
//-----------------------------------------------------------------
int CSBufKeyTreeEx::MatchTestMax(BYTE *pbyKeys, int nSize ,PBKT_NODEEX *pMatchItem/*=NULL*/)
{
	BKT_NODEEX root=GetRoot();
	BKT_NODEEX * hsParent=&root;
	BKT_NODEEX * hsNode=NULL,*hsGet=NULL;
	int nDeep=0;
	for(int i=0;i<nSize;i+=m_cKeySize)
	{
		hsNode=FindKeyNode(hsNode,hsParent->wChilds,pbyKeys+i);
		if(!hsNode) break;
		hsParent=hsNode;
		hsGet=hsNode;
		nDeep++;
	}
	if(pMatchItem) *pMatchItem=hsGet;
	return nDeep;
}

//-------------------------------------------------------------------
//	������һ��ʵ��ƥ���Ҷ�ӽڵ�
//-------------------------------------------------------------------
PBKT_NODEEX CSBufKeyTreeEx::FindNextLeaf(BKT_NODEEX * hsParent,BYTE *pbyKeys, int nSize,int *pDeep)
{
	ASSERT(m_funIsLeaf);
	BKT_NODEEX * hsNode=hsParent;
	int i=0;
	while(i<nSize)
	{
		hsNode=FindKeyNode(hsNode,hsNode->wChilds,pbyKeys+i);
		if(!hsNode) return NULL;
		if(IsLeafNode(hsNode))
		{
			if(pDeep) *pDeep=i/m_cKeySize;
			return hsNode;
		}
		i+=m_cKeySize;
	}
	return NULL;
}


//---------------------------------------------------------------
//	��������
//---------------------------------------------------------------
BOOL CSBufKeyTreeEx::LoadData(FILE *f, BYTE byFlag)
{
	DWORD dwSize=0;
	fread(&dwSize,sizeof(DWORD),1,f);
	BYTE byFlagGet=0;
	fread(&byFlagGet,1,1,f);
	if(byFlag!=byFlagGet) return FALSE;
	fread(&m_cKeySize,1,1,f);
	m_pBuffer=(BYTE *)HeapAlloc(GetProcessHeap(),0,dwSize);
	if(!m_pBuffer) return FALSE;
	DWORD dwReaded=fread(m_pBuffer,1,dwSize,f);
	if(dwReaded!=dwSize)
	{
		Free();
		return FALSE;
	}
	m_dwBufSize=dwSize;
	return TRUE;
}


//---------------------------------------------------------------
//	��������
//---------------------------------------------------------------
BOOL CSBufKeyTreeEx::WriteData(FILE *f, BYTE byFlag)
{
	fwrite(&m_dwBufSize,sizeof(DWORD),1,f);
	fwrite(&byFlag,1,1,f);
	fwrite(&m_cKeySize,1,1,f);
	fwrite(m_pBuffer,1,m_dwBufSize,f);
	return TRUE;
}

//--------------------------------------------------------
//	�ͷ��ڴ�
//--------------------------------------------------------
void CSBufKeyTreeEx::Free()
{
	if(m_pBuffer)
	{
		HeapFree(GetProcessHeap(),0,m_pBuffer);
		m_pBuffer=NULL;
		m_dwBufSize=0;
	}
}

//////////////////////////////////////////////////////////////
//	CSKeyTreeEx Implement
//////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CSKeyTreeEx::Dump(HSTREEITEM hsNode,int level)
{
	char tab[6]="     ";
	if(hsNode!=STVI_ROOT)
	{
		KEYDATAEX *pKEYDATAEX=GetItem(hsNode);
		for(int i=0;i<level;i++)	TRACE(tab);
		for(int j=0;j<m_cKeySize;j++)
			TRACE("%02x",pKEYDATAEX->byKey[j]);
		TRACE(":%08x\r\n",pKEYDATAEX->data);
	}
	HSTREEITEM hsChild=GetChildItem(hsNode);
	if(hsChild){
		Dump(hsChild,++level);
		--level;
	}
	if(hsNode==STVI_ROOT) return;
	HSTREEITEM hsSibling=GetNextSiblingItem(hsNode);
	if(hsSibling) Dump(hsSibling,level);
}
#endif

//--------------------------------------------------------------------
//	���һ���ؼ���
//	data:�ؼ��ֶ�Ӧ������
//	key:�ؼ���
//	len:�ؼ��ֳ���
//	return:���һ������Ľڵ�
//--------------------------------------------------------------------
HSTREEITEM CSKeyTreeEx::AddKey(DWORD data,const BYTE *key,int len)
{
	if(len==-1) len=strlen((char *)key);
	HSTREEITEM hParent=STVI_ROOT;
	int nCmp=0;
	for(int i=0;i<len;i+=m_cKeySize)
	{
		HSTREEITEM hDest=NULL;
		HSTREEITEM hsNode=GetChildItem(hParent);
		KEYDATAEX *pKEYDATAEX=NULL;
		//find a node which value must large key[i]
		while(hsNode)
		{
			pKEYDATAEX=GetItem(hsNode);
			nCmp=memcmp(pKEYDATAEX->byKey,key+i,m_cKeySize);
			if(nCmp>=0)
			{
				hDest=hsNode;
				break;
			}
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(hDest!=NULL)
		{
			if(nCmp==0)
			{
				if(i==len-m_cKeySize)
				{
					if(pKEYDATAEX->data==m_dwInvalidKey)
					{
						pKEYDATAEX->data=data;
						return hDest;
					}else
					{
						return NULL;
					}
				}else//i!=len-m_cKeySize
				{
					hParent=hDest;
				}
			}else//key[i]<pKEYDATAEX->c,insert the right key to tree
			{
				KEYDATAEX *pKEYDATAEX=NewItem(key+i,m_dwInvalidKey);
				if(i==len-m_cKeySize) pKEYDATAEX->data=data;
				HSTREEITEM   hInsertAfter=GetPrevSiblingItem(hDest);
				if(hInsertAfter==NULL) hInsertAfter=STVI_FIRST;
				hParent=InsertItem(pKEYDATAEX,hParent,hInsertAfter);
			}
		}else{//hDest==NULL,find no item 's value large key[i],inert to last
			KEYDATAEX *pKEYDATAEX=NewItem(key+i,m_dwInvalidKey);
			if(i==len-m_cKeySize) pKEYDATAEX->data=data;
			hParent=InsertItem(pKEYDATAEX,hParent,STVI_LAST);
		}
	}
	return hParent;
}

//--------------------------------------------------------------------
//	�жϹؼ����Ƿ����
//	key:�ؼ���
//	len:�ؼ��ֳ���
//	pdata:�������عؼ��ֶ�Ӧ���ݵ�ָ��
//	return:�鵽�Ĺؼ��ֵĽڵ㡣
//--------------------------------------------------------------------
HSTREEITEM CSKeyTreeEx::IsKeyExist(const BYTE *key,int len,DWORD *pdata)
{
	HSTREEITEM hParent=STVI_ROOT;
	HSTREEITEM hsNode;
	if(m_cKeySize!=1 && len%m_cKeySize ) return NULL;

	for(int i=0;i<len;i+=m_cKeySize)
	{
		hsNode=GetChildItem(hParent);
		BOOL bFind=FALSE;
		KEYDATAEX  *pKEYDATAEX;
		while(hsNode)
		{
			pKEYDATAEX=GetItem(hsNode);
			int nCmp=memcmp(pKEYDATAEX->byKey,key+i,m_cKeySize);
			if(nCmp==0)
			{
				hParent=hsNode;
				bFind=TRUE;
				break;
			}else if(nCmp>0)
				break;
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(!bFind) return NULL;
		if(i==len-m_cKeySize && pdata) *pdata=pKEYDATAEX->data;
	}
	return hsNode;
}

//-------------------------------------------------------------------
//	ö�����нڵ�
//-------------------------------------------------------------------
BOOL CSKeyTreeEx::EnumTreeNode(HSTREEITEM hParent,CBENUMCHILD cbEnumChild,LPARAM lParam,int iLevel,BYTE *byKeyHead,BOOL bOnlyLeaf)
{
	BOOL bContinue=TRUE;
	HSTREEITEM hSibling=GetChildItem(hParent);
	while(hSibling)
	{
		if(byKeyHead) memcpy(byKeyHead+iLevel*m_cKeySize,GetItemChar(hSibling),m_cKeySize);
		if(!bOnlyLeaf || GetItemData(hSibling)!=m_dwInvalidKey)
		{
			bContinue=cbEnumChild(this,hSibling,byKeyHead,iLevel,lParam);
		}
		if(bContinue && GetChildItem(hSibling))
		{
			bContinue=EnumTreeNode(hSibling,cbEnumChild,lParam,iLevel+1,byKeyHead);
		}
		if(!bContinue) break;
		hSibling=GetNextSiblingItem(hSibling);
	}
	return bContinue;
}

//--------------------------------------------------------------------
//	���ؼ�������Ϣд���ļ�����
//--------------------------------------------------------------------
void CSKeyTreeEx::WriteData(FILE *f)
{
	CSTree<FILETREEDATA> treeTool;
	MakeFileTree(STVI_ROOT,&treeTool,STVI_ROOT);
	//����������ڵ���Ϣ(��һ��ڵ�����
	WORD	wChilds=0;
	HSTREEITEM hChild=(HSTREEITEM)m_hRootFirst;
	while(hChild)
	{
		wChilds++;
		hChild=GetNextSiblingItem(hChild);
	}

	DWORD dwCurOffset=ftell(f);
	DWORD dwSize=0;
	fwrite(&dwSize,sizeof(DWORD),1,f);
	fwrite(&m_byFlag,1,1,f);			//�����������ͱ�־
	fwrite(&m_cKeySize,1,1,f);			//����ؼ��ֳ���

	//�����ӽڵ���Ϣ
	fwrite(&wChilds,sizeof(WORD),1,f);	//��һ��ڵ���
	WriteItem(&treeTool,treeTool.GetRootItem(),dwCurOffset+sizeof(DWORD)+2,f);
	//д�����ݳ���
	fseek(f,0,SEEK_END);
	dwSize=ftell(f)-dwCurOffset-sizeof(DWORD)-2;
	fseek(f,dwCurOffset,SEEK_SET);
	fwrite(&dwSize,sizeof(DWORD),1,f);
	//���¶�λ���ļ�β
	fseek(f,0,SEEK_END);
}

//--------------------------------------------------------------------
//	CSTree<FILETREEDATA> &toolTree:��ʱ������
//	HSTREEITEM hsNode:������Ľڵ�
//	DWORD	dwRoot:���ڵ�ƫ��
//	FILE *f:�ļ�ָ��
//---------------------------------------------------------------------
void CSKeyTreeEx::WriteItem(CSTree<FILETREEDATA> *pTreeTool,
						  HSTREEITEM hsItem,
						  DWORD  dwRoot,
						  FILE *f)
{
	PBKT_NODEEX pbktNode=(PBKT_NODEEX)malloc(sizeof(BKT_NODEEX)-1+m_cKeySize);//��д�սڵ�ʱ��Ҫ�ؼ�����
	//����ڵ�ͷ
	HSTREEITEM hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		ftd->dwPosSelf=ftell(f);
		fwrite(pbktNode,sizeof(BKT_NODEEX)-1+m_cKeySize,1,f);//д�ս�㣬û����ʵ����
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
	//����ڵ�����
	hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		ftd->dwPosData=ftell(f);
		//��������
		if(m_funWriteData) 
			m_funWriteData(ftd->pKEYDATAEX->data,f);
		else
			fwrite(&ftd->pKEYDATAEX->data,sizeof(DWORD),1,f);
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
	//����ÿһ���ڵ����ʵ���ݼ��ӽڵ�����
	hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		HSTREEITEM hChild=pTreeTool->GetChildItem(hSibling);
		DWORD dwEnd=ftell(f);

		pbktNode->dwPosData=ftd->dwPosData-dwRoot;
		pbktNode->dwPosChild=hChild?(dwEnd-dwRoot):-1;
		pbktNode->wChilds=ftd->wChilds;
		fseek(f,ftd->dwPosSelf,SEEK_SET);
		fwrite(pbktNode,sizeof(BKT_NODEEX)-1,1,f);
		fwrite(ftd->pKEYDATAEX->byKey,1,m_cKeySize,f);
		fseek(f,dwEnd,SEEK_SET);
		
		if(hChild) WriteItem(pTreeTool,hChild,dwRoot,f);
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
	free(pbktNode);
}

//----------------------------------------------------------------------
// ���õݹ鷽ʽ����һ���м���
//	HSTREEITEM hItemSour:Դ�ڵ�
//	CSTree<FILETREEDATA> *pTreeTool:�м���
//	HSTREEITEM hItemDest:Ŀ��ڵ�
//----------------------------------------------------------------------
void CSKeyTreeEx::MakeFileTree(HSTREEITEM hItemSour, CSTree<FILETREEDATA> *pTreeTool, HSTREEITEM hItemDest)
{
	HSTREEITEM hsSibSrc=GetChildItem(hItemSour);
	HSTREEITEM hsSibDest=STVI_FIRST;
	while(hsSibSrc)
	{
		KEYDATAEX *pkd=GetItem(hsSibSrc);
		FILETREEDATA ftd={-1,-1,0,pkd};
		//�ۼ��ӽڵ����
		HSTREEITEM hChild=GetChildItem(hsSibSrc);
		while(hChild)
		{
			ftd.wChilds++;
			hChild=GetNextSiblingItem(hChild);
		}
		hsSibDest=pTreeTool->InsertItem(ftd,hItemDest,hsSibDest);
		if(pTreeTool->GetChildItem(hsSibSrc))
			MakeFileTree(hsSibSrc,pTreeTool,hsSibDest);
		hsSibSrc=GetNextSiblingItem(hsSibSrc);
	}
}

//------------------------------------------------
//	����һ����֦
//	HSTREEITEM hParent:��֦Parent
//	PBKT_NODEEX pNode:���������ĸ����
//	FILE *f:�ļ�ָ��
//	DWORD dwRoot:���ڵ�λ�ã���Ϊ�ļ����ڵ㱣���λ�����ݶ������λ��
//------------------------------------------------
void CSKeyTreeEx::LoadItem(HSTREEITEM hParent, PBKT_NODEEX pNode, FILE *f,DWORD dwRoot)
{
	HSTREEITEM hInsertAfter=STVI_LAST;
	BYTE byKey[20];
	for(WORD i=0;i<pNode->wChilds;i++)
	{
		fseek(f,dwRoot+pNode->dwPosChild+i*(sizeof(BKT_NODEEX)-1+m_cKeySize),SEEK_SET);
		BKT_NODEEX node;
		fread(&node,sizeof(BKT_NODEEX)-1,1,f);
		fread(byKey,1,m_cKeySize,f);
		KEYDATAEX *pkd=NewItem(byKey,0);
		//load data
		fseek(f,dwRoot+node.dwPosData,SEEK_SET);
		if(m_funLoadData)
			m_funLoadData(pkd->data,f);
		else
			fread(&pkd->data,sizeof(DWORD),1,f);
		hInsertAfter=InsertItem(pkd,hParent,hInsertAfter);
		if(node.wChilds!=0) LoadItem(hInsertAfter,&node,f,dwRoot);
	}
}

//--------------------------------------------------
//	���ļ�����������������
//--------------------------------------------------
BOOL CSKeyTreeEx::LoadData(FILE *f)
{
	DeleteAllItems();

	DWORD dwOffset=ftell(f);
	DWORD dwSize=0;
	fread(&dwSize,sizeof(DWORD),1,f);
	BYTE byFlag=0;
	fread(&byFlag,1,1,f);
	if(byFlag!=m_byFlag) return FALSE;
	fread(&m_cKeySize,1,1,f);

	BKT_NODEEX bktRoot={0};
	bktRoot.dwPosChild=2;
	fread(&bktRoot.wChilds,sizeof(WORD),1,f);
	LoadItem(STVI_ROOT,&bktRoot,f,dwOffset+sizeof(DWORD)+2);

	fseek(f,dwOffset+sizeof(DWORD)+2+dwSize,SEEK_SET);//���ļ�ָ�붨λ������β�������ⲿ���ýӿڼ�����ȡ��������
	return TRUE;
}

//-----------------------------------------------------------------
//	ƥ��̽��
//	BYTE *pbyKeys:Ҫƥ��Ĺؼ��ִ�
//	int nSize:�ؼ��ִ�����
//	HSTREEITEM *pMatchItem:���ƥ��Ľڵ�
//	return:���ƥ������
//-----------------------------------------------------------------
int CSKeyTreeEx::MatchTest(BYTE *pbyKeys, int nSize ,HSTREEITEM *pMatchItem/*=NULL*/)
{
	HSTREEITEM hParent=STVI_ROOT;
	HSTREEITEM hsNode=NULL,hsGet=NULL;
	int nDeep=0;
	for(int i=0;i<nSize;i+=m_cKeySize)
	{
		hsNode=GetChildItem(hParent);
		BOOL bFind=FALSE;
		KEYDATAEX  *pKEYDATAEX;
		while(hsNode)
		{
			pKEYDATAEX=GetItem(hsNode);
			int nCmp=memcmp(pKEYDATAEX->byKey,pbyKeys+i*m_cKeySize,m_cKeySize);
			if(nCmp==0)
			{
				hParent=hsNode;
				bFind=TRUE;
				hsGet=hsNode;
				break;
			}else if(nCmp>0)
				break;
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(!bFind) break;
		nDeep++;
	}
	if(pMatchItem) *pMatchItem=hsGet;
	return nDeep;
}
