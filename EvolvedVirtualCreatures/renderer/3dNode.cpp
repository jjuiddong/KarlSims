
#include "stdafx.h"
#include "3dnode.h"


//-----------------------------------------------------------------------------//
// ������
//-----------------------------------------------------------------------------//
C3dNode::C3dNode( int id ): 
m_nId(id), m_pNext(NULL), m_pChild(NULL), m_pPrev(NULL), m_pParent(NULL) 
{

}


//-----------------------------------------------------------------------------//
// Node�� ���峡�� �߰��ȴ�.
//-----------------------------------------------------------------------------//
void InsertChildTree( C3dNode *pParent, C3dNode *pInst )
{
	if( !pParent ) return;

	if( !pParent->m_pChild )
	{
		pParent->m_pChild = pInst;
		pInst->m_pParent = pParent;
	}
	else
	{
		C3dNode *pNext = pParent->m_pChild;
		C3dNode *pPrev = pParent->m_pChild;
		while( pNext )
		{
			pPrev = pNext;
			pNext = pNext->m_pNext;
		}
		pPrev->m_pNext = pInst;
		pInst->m_pPrev = pPrev;
		pInst->m_pParent = pParent;
	}
}


//-----------------------------------------------------------------------------//
// pCurNode Tree ���� nId ���� Tree�� ã�´�.
//-----------------------------------------------------------------------------//
C3dNode* FindTree( C3dNode *pCurNode, int nId )
{
	if( !pCurNode ) return NULL;
	if( pCurNode->GetId() == nId ) return pCurNode;

	C3dNode *pNode = NULL;
	pNode = FindTree( pCurNode->m_pNext, nId );
	if( pNode ) return pNode;
	pNode = FindTree( pCurNode->m_pChild, nId );
	if( pNode ) return pNode;

	return NULL;
}


//-----------------------------------------------------------------------------//
// Tree ���� (�޸𸮴� ���������ʴ´�.)
//-----------------------------------------------------------------------------//
BOOL DelTree( C3dNode *pCurTree, int nId )
{
	C3dNode *pNode = FindTree( pCurTree, nId );
	if( !pNode ) return FALSE;

	C3dNode *pPrevNode = pNode->m_pPrev;
	C3dNode *pNextNode = pNode->m_pNext;
	C3dNode *pParentNode = pNode->m_pParent;

	if( pPrevNode )
		pPrevNode->m_pNext = pNextNode;
	if( pNextNode )
		pNextNode->m_pPrev = pPrevNode;

	if( !pPrevNode ) // root �϶�
	{
		if( pParentNode )
		{
			pParentNode->m_pChild = pNextNode;
			if( pNextNode )
				pNextNode->m_pPrev = NULL;
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Tree �޸�����
//-----------------------------------------------------------------------------//
void ReleaseTree( C3dNode *pTree )
{
	if( !pTree ) return;
	
	ReleaseTree( pTree->m_pChild );
	ReleaseTree( pTree->m_pNext );
	delete pTree;
}

