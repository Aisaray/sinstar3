#include "stdafx.h"
#include "skeytree.h"

//////////////////////////////////////////////////////////////
//	CSBufKeyTree Implement
//////////////////////////////////////////////////////////////

CSBufKeyTree::CSBufKeyTree()
{
	m_funReadData=NULL;
	m_funIsLeaf=NULL;
	m_pBuffer=NULL;
	m_dwBufSize=0;
}

CSBufKeyTree::~CSBufKeyTree()
{
	Free();
}

//--------------------------------------------------------------------
//	�������еĹؼ��֣�������Ҷ�ӽڵ㻹�Ƿֲ�ڵ�
//	Ҫ�ж��ǲ���Ҷ�ӽڵ���Ե���IsLeafNode
//--------------------------------------------------------------------
BKT_NODE * CSBufKeyTree::IsKeyExist(BYTE *byKey, int nLen)
{
	ASSERT(m_pBuffer);
	BKT_NODE root=GetRoot();
	BKT_NODE *pRetNode=NULL;
	BKT_NODE *pParentNode=&root;
	BKT_NODE *pChildNode=(BKT_NODE*)(m_pBuffer+2);
	for(int i=0;i<nLen;i++)
	{
		if(!(pRetNode=FindKeyNode(pChildNode,pParentNode->wChilds,byKey[i])))
			return NULL;
		else
		{
			pParentNode=pRetNode;
			pChildNode=(BKT_NODE*)(m_pBuffer+pRetNode->dwPosChild);
		}
	}
	return pRetNode;
}

BOOL CSBufKeyTree::EnumTreeNode(PBKT_NODE pNode,CBENUMTREE funEnumTree,LPARAM lParam,int iLevel,BYTE *byKeyHead)
{
	BOOL bContinue=TRUE;
	PBKT_NODE pNodeChild=GetChildHead(pNode);
	for(WORD i=0;i<pNode->wChilds;i++)
	{
		if(byKeyHead) byKeyHead[iLevel]=pNode->byKey;
		if(IsLeafNode(pNodeChild)) bContinue=funEnumTree(this,pNodeChild,iLevel,byKeyHead,lParam);
		if(bContinue) bContinue=EnumTreeNode(pNodeChild,funEnumTree,lParam,iLevel+1,byKeyHead);
		if(!bContinue) break;
	}
	return bContinue;
}

void CSBufKeyTree::EnumKeyNode(BYTE *pbyKeys,int nSize,BYTE byWild,CBENUMKEY funEnumKey,LPARAM lParam,BOOL bDeepFirst)
{
	ASSERT(nSize>=1);
	BKT_NODE root=GetRoot();
	BYTE *pbyKeyArray=new BYTE[nSize];
	_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,&root,0,pbyKeyArray,bDeepFirst);
	delete []pbyKeyArray;
}

BOOL CSBufKeyTree::_EnumKeyNode(BYTE *pbyKeys,int nSize,BYTE byWild,CBENUMKEY funEnumKey,LPARAM lParam,PBKT_NODE pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst)
{
	ASSERT(nSize>=1 && iLevel<nSize);
	BOOL bContinue=TRUE;
	PBKT_NODE pNodeChild=GetChildHead(pParent);
	PBKT_NODE pNodeDest=NULL;
	if(pbyKeys[iLevel]==byWild)
	{
		pNodeDest=pNodeChild;
		for(WORD i=0;i<pParent->wChilds;i++)
		{
			pbyKeyArray[iLevel]=pNodeDest->byKey;
			if(bDeepFirst)
			{
				if(iLevel+1<nSize) bContinue=_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
				if(bContinue && IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
			}else
			{
				if(IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
				if(bContinue && iLevel+1<nSize) bContinue=_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
			}
			if(!bContinue) break;
			pNodeDest++;
		}
	}else
	{
		pNodeDest=FindKeyNode(pNodeChild,pParent->wChilds,pbyKeys[iLevel]);
		if(pNodeDest)
		{
			pbyKeyArray[iLevel]=pNodeDest->byKey;
			if(bDeepFirst)
			{
				if(iLevel+1<nSize) bContinue=_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
				if(bContinue && IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
			}else
			{
				if(IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
				if(bContinue && iLevel+1<nSize) bContinue=_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
			}
		}
	}
	return bContinue;
}

void CSBufKeyTree::EnumKeyNode(PKEYEQUAL pKeyEqual,int nSize,CBENUMKEY funEnumKey,LPARAM lParam,BOOL bDeepFirst)
{
	ASSERT(nSize>=1);
	BKT_NODE root=GetRoot();
	BYTE *pbyKeyArray=new BYTE[nSize];
	_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,&root,0,pbyKeyArray,bDeepFirst);
	delete []pbyKeyArray;
}

BOOL CSBufKeyTree::_EnumKeyNode(PKEYEQUAL pKeyEqual,int nSize,CBENUMKEY funEnumKey,LPARAM lParam,PBKT_NODE pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst)
{
	ASSERT(nSize>=1 && iLevel<nSize);
	short sLeftSiblings=(short)pParent->wChilds;
	short sCount=pKeyEqual[iLevel].sCount;
	if(sCount==0) sCount=sLeftSiblings;

	BOOL bContinue=TRUE;
	PBKT_NODE pNodeChild=GetChildHead(pParent);
	PBKT_NODE pNodeDest=NULL;
	for(short i=0;i<sCount && sLeftSiblings>0 ;i++)
	{
		if(pKeyEqual[iLevel].sCount!=0)
			pNodeDest=FindKeyNode(pNodeChild,sLeftSiblings,pKeyEqual[iLevel].pbyKeys[i]);
		else
			pNodeDest=pNodeChild;
		if(pNodeDest)
		{
			pbyKeyArray[iLevel]=pNodeDest->byKey;
			if(bDeepFirst)
			{
				if(iLevel+1<nSize) bContinue=_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
				if(bContinue && IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
			}else
			{
				if(IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
				if(bContinue && iLevel+1<nSize) bContinue=_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
			}
			sLeftSiblings-=(pNodeDest-pNodeChild)+1;
			pNodeChild=pNodeDest+1;
		}
		if(!bContinue) break;
	}
	return bContinue;
}

//--------------------------------------------------------------------
//	���ź�����ֵܽ���������ַ����ҹؼ������ڽڵ�
//--------------------------------------------------------------------
BKT_NODE * CSBufKeyTree::FindKeyNode(BKT_NODE *pSibling, WORD wSiblings, BYTE byKey)
{
	if(wSiblings==0)
		return NULL;
	if(wSiblings==1)
	{
		if(pSibling->byKey!=byKey)
			return NULL;
		else
			return pSibling;
	}else if(wSiblings==2)
	{
		if(pSibling->byKey==byKey)
			return pSibling;
		else if((pSibling+1)->byKey==byKey)
			return pSibling+1;
		else
			return NULL;
	}else
	{
		WORD wHalf=wSiblings/2;
		BKT_NODE *pMiddle=pSibling+wHalf;
		if(pMiddle->byKey==byKey)
			return pMiddle;
		else if(pMiddle->byKey>byKey)
			return FindKeyNode(pSibling,wHalf,byKey);
		else
			return FindKeyNode(pMiddle+1,wSiblings-wHalf-1,byKey);
	}
}

//---------------------------------------------------------------
//	��������
//---------------------------------------------------------------
BOOL CSBufKeyTree::LoadData(FILE *f, BYTE byFlag)
{
	DWORD dwSize=0;
	fread(&dwSize,sizeof(DWORD),1,f);
	BYTE byFlagGet=0;
	fread(&byFlagGet,1,1,f);
	if(byFlag!=byFlagGet) return FALSE;
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

//--------------------------------------------------------
//	�ͷ��ڴ�
//--------------------------------------------------------
void CSBufKeyTree::Free()
{
	if(m_pBuffer)
	{
		HeapFree(GetProcessHeap(),0,m_pBuffer);
		m_pBuffer=NULL;
	}
}

//////////////////////////////////////////////////////////////
//	CSKeyTree Implement
//////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CSKeyTree::Dump(HSTREEITEM hsNode,int level)
{
	char tab[6]="     ";
	if(hsNode!=STVI_ROOT)
	{
		KEYDATA *pkeydata=GetItemPt(hsNode);
		for(int i=0;i<level;i++)	TRACE(tab);
		TRACE("%02x:%08x\r\n",pkeydata->c,pkeydata->data);
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
//--------------------------------------------------------------------
BOOL CSKeyTree::AddKey(DWORD data,const BYTE *key,int len)
{
	if(len==-1) len=strlen((char *)key);
	HSTREEITEM hParent=STVI_ROOT;
	for(int i=0;i<len;i++)
	{
		HSTREEITEM hDest=NULL;
		HSTREEITEM hsNode=GetChildItem(hParent);
		KEYDATA *pkeydata=NULL;
		//find a node which value must large key[i]
		while(hsNode)
		{
			pkeydata=GetItemPt(hsNode);
			if(pkeydata->c>=key[i]){
				hDest=hsNode;
				break;
			}
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(hDest!=NULL){
			if(pkeydata->c==key[i]){
				if(i==len-1){
					if(pkeydata->data==m_dwInvalidKey)
					{
						pkeydata->data=data;
						return TRUE;
					}else
						return FALSE;
				}else//i!=len-1
					hParent=hDest;
			}else//key[i]<pkeydata->c,insert the right key to tree
			{
				ASSERT(key[i]<pkeydata->c);
				KEYDATA keydata={key[i],m_dwInvalidKey};
				if(i==len-1) keydata.data=data;
				HSTREEITEM   hInsertAfter=GetPrevSiblingItem(hDest);
				if(hInsertAfter==NULL) hInsertAfter=STVI_FIRST;
				hParent=InsertItem(keydata,hParent,hInsertAfter);
			}
		}else{//hDest==NULL,find no item 's value large key[i],inert to last
			KEYDATA keydata={key[i],m_dwInvalidKey};
			if(i==len-1) keydata.data=data;
			hParent=InsertItem(keydata,hParent,STVI_LAST);
		}
	}
	return TRUE;
}

//--------------------------------------------------------------------
//	�жϹؼ����Ƿ����
//	key:�ؼ���
//	len:�ؼ��ֳ���
//	pdata:�������عؼ��ֶ�Ӧ���ݵ�ָ��
//	return:�鵽�Ĺؼ��ֵĽڵ㡣
//--------------------------------------------------------------------
HSTREEITEM CSKeyTree::IsKeyExist(const BYTE *key,int len,DWORD *pdata)
{
	HSTREEITEM hParent=STVI_ROOT;
	HSTREEITEM hsNode;
	for(int i=0;i<len;i++)
	{
		hsNode=GetChildItem(hParent);
		BOOL bFind=FALSE;
		KEYDATA  *pkeydata;
		while(hsNode)
		{
			pkeydata=GetItemPt(hsNode);
			if(pkeydata->c==key[i])
			{
				hParent=hsNode;
				bFind=TRUE;
				break;
			}else if(pkeydata->c>key[i])
				break;
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(!bFind) return NULL;
		if(i==len-1&&pdata) *pdata=pkeydata->data;
	}
	return hsNode;
}

//--------------------------------------------------------------------
//	���ؼ�������Ϣд���ļ�����
//--------------------------------------------------------------------
void CSKeyTree::WriteData(FILE *f)
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
	fwrite(&m_byFlag,1,1,f);//�����������ͱ�־

	fwrite(&wChilds,sizeof(WORD),1,f);//��һ��ڵ���
	//�����ӽڵ���Ϣ
	WriteItem(&treeTool,treeTool.GetRootItem(),dwCurOffset+sizeof(DWORD)+1,f);
	//д�����ݳ���
	fseek(f,0,SEEK_END);
	dwSize=ftell(f)-dwCurOffset-sizeof(DWORD)-1;
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
void CSKeyTree::WriteItem(CSTree<FILETREEDATA> *pTreeTool,
						  HSTREEITEM hsItem,
						  DWORD  dwRoot,
						  FILE *f)
{
	BKT_NODE bktNode={0};
	//����ڵ�ͷ
	HSTREEITEM hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		ftd->dwPosSelf=ftell(f);
		fwrite(&bktNode,sizeof(BKT_NODE),1,f);//д�ս�㣬û����ʵ����
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
			m_funWriteData(ftd->keyData.data,f);
		else
			fwrite(&ftd->keyData.data,sizeof(DWORD),1,f);
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
	//����ÿһ���ڵ����ʵ���ݼ��ӽڵ�����
	hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		HSTREEITEM hChild=pTreeTool->GetChildItem(hSibling);
		DWORD dwEnd=ftell(f);

		bktNode.dwPosData=ftd->dwPosData-dwRoot;
		bktNode.dwPosChild=hChild?(dwEnd-dwRoot):-1;
		bktNode.wChilds=ftd->wChilds;
		bktNode.byKey=ftd->keyData.c;
		fseek(f,ftd->dwPosSelf,SEEK_SET);
		fwrite(&bktNode,sizeof(BKT_NODE),1,f);
		fseek(f,dwEnd,SEEK_SET);
		
		if(hChild) WriteItem(pTreeTool,hChild,dwRoot,f);
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
}

//----------------------------------------------------------------------
// ���õݹ鷽ʽ����һ���м���
//	HSTREEITEM hItemSour:Դ�ڵ�
//	CSTree<FILETREEDATA> *pTreeTool:�м���
//	HSTREEITEM hItemDest:Ŀ��ڵ�
//----------------------------------------------------------------------
void CSKeyTree::MakeFileTree(HSTREEITEM hItemSour, CSTree<FILETREEDATA> *pTreeTool, HSTREEITEM hItemDest)
{
	HSTREEITEM hsSibSrc=GetChildItem(hItemSour);
	HSTREEITEM hsSibDest=STVI_FIRST;
	while(hsSibSrc)
	{
		KEYDATA *pkd=GetItemPt(hsSibSrc);
		FILETREEDATA ftd={-1,-1,0,{pkd->c,pkd->data}};
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
//	PBKT_NODE pNode:���������ĸ����
//	FILE *f:�ļ�ָ��
//	DWORD dwRoot:���ڵ�λ�ã���Ϊ�ļ����ڵ㱣���λ�����ݶ������λ��
//------------------------------------------------
void CSKeyTree::LoadItem(HSTREEITEM hParent, PBKT_NODE pNode, FILE *f,DWORD dwRoot)
{
	HSTREEITEM hInsertAfter=STVI_LAST;
	for(WORD i=0;i<pNode->wChilds;i++)
	{
		fseek(f,dwRoot+pNode->dwPosChild+i*sizeof(BKT_NODE),SEEK_SET);
		BKT_NODE node;
		fread(&node,sizeof(BKT_NODE),1,f);
		KEYDATA kd={node.byKey};
		//load data
		fseek(f,dwRoot+node.dwPosData,SEEK_SET);
		if(m_funLoadData)
			m_funLoadData(kd.data,f);
		else
			fread(&kd.data,sizeof(DWORD),1,f);
		hInsertAfter=InsertItem(kd,hParent,hInsertAfter);
		if(node.wChilds!=0) LoadItem(hInsertAfter,&node,f,dwRoot);
	}
}

//--------------------------------------------------
//	���ļ�����������������
//--------------------------------------------------
BOOL CSKeyTree::LoadData(FILE *f)
{
	DeleteAllItems();

	DWORD dwOffset=ftell(f);
	DWORD dwSize=0;
	fread(&dwSize,sizeof(DWORD),1,f);
	BYTE byFlag=0;
	fread(&byFlag,1,1,f);
	if(byFlag!=m_byFlag) return FALSE;

	BKT_NODE bktRoot={0};
	bktRoot.dwPosChild=2;
	fread(&bktRoot.wChilds,sizeof(WORD),1,f);
	LoadItem(STVI_ROOT,&bktRoot,f,dwOffset+sizeof(DWORD)+1);

	fseek(f,dwOffset+sizeof(DWORD)+1+dwSize,SEEK_SET);//���ļ�ָ�붨λ������β�������ⲿ���ýӿڼ�����ȡ��������
	return TRUE;
}
