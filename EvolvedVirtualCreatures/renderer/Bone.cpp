
#include "stdafx.h"
#include "bone.h"
#include "track.h"



//////////////////////////////////////////////////////////////////////////////////
// CBoneNode
CBoneNode::CBoneNode( int nId ):
	C3dNode( nId ), m_pTrack(NULL), m_AniStart(0), m_AniEnd(0), m_bQuick(FALSE), m_bSmooth(FALSE),m_AniFrame(0),
	m_nTempEndTime(0), m_pBox(NULL), m_bAni(TRUE)
{
	m_matAccTM.SetIdentity();
	m_matTM.SetIdentity();
	m_matAni.SetIdentity();

}

CBoneNode::~CBoneNode()
{
	SAFE_DELETE( m_pTrack );
	SAFE_DELETE( m_pBox );

}


//-----------------------------------------------------------------------------//
// ����Ÿ �ε�
// nObjId = Bone�� ������Ʈ ���̵� = Model ID�� �����ϴ�.
//-----------------------------------------------------------------------------//
BOOL CBoneNode::Load( int ObjId, SBoneLoader *pLoader, Matrix44 *pPalette )
{
	m_ObjId = ObjId;
	m_pPalette = pPalette;
	strcpy_s( m_pName, sizeof(m_pName), pLoader->szName );
	m_matOffset = pLoader->worldtm.Inverse();
	m_matLocal = pLoader->localtm;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���ϸ��̼� ����Ÿ ����
// nAniTime : ���ϸ��̼ǵ� �ð�, 0�̸� �� ���ϸ��̼� �ð��̴�.
// bContinue : TRUE�̸� �� �����ӿ��� ��� ���ϸ��̼� �ȴ�.
// bLoop : �ݺ��ؼ� ���ϸ��̼� �Ѵٸ� TRUE
// bSmooth : ���ϸ��̼ǰ��� ������ �Ѵٸ� TRUE
//-----------------------------------------------------------------------------//
void CBoneNode::SetAnimation( SKeyLoader *pLoader, int nAniTime, 
							  BOOL bContinue, BOOL bLoop, BOOL bQuick, BOOL bSmooth )// bSmooth = FALSE
{
	const int SMOOTH_TIME = 50;

	int aniend = 0;
	if( 0 == nAniTime )
	{
		m_AniTime = (int)pLoader->end;
		aniend = (int)pLoader->end;
	}
	else
	{
		m_AniTime = nAniTime;
//		aniend = (int)((pLoader->end < nAniTime)? pLoader->end : nAniTime);
		aniend = (int)pLoader->end;
	}

	m_AniStart		= 0;
	m_AniEnd		= aniend;
	m_LoopTime		= (int)pLoader->end;
	m_AniIncFrame	= 0;

	m_bLoop			= bLoop;
	m_bAni			= TRUE;

	// ���ϸ��̼� ���϶��� �����ȴ�.
	// bContinue�϶��� ��ŵ�ȴ�.
	// �� ���ϸ��̼��� Quick�����϶� ������ ��ŵ�ȴ�.
	BOOL interpol = bSmooth && m_pTrack && (!bContinue) && (!m_bQuick);
	if( interpol )
	{
		m_AniEnd = SMOOTH_TIME;
		m_nTempEndTime = aniend;
	}

	m_bQuick = bQuick;
	m_bSmooth = interpol;

	// �� ���ϸ��̼ǿ��� �����ؼ� ���ϸ��̼� �ȴٸ�
	if( bContinue )
	{
		m_AniEnd = m_AniFrame + m_AniTime;
		if( m_AniEnd > pLoader->end ) m_AniEnd = (int)pLoader->end;
	}
	else
	{
		m_AniFrame = 0;
	}

	if( !m_pTrack ) m_pTrack = new CTrack;
	m_pTrack->Load( pLoader, bContinue, interpol, SMOOTH_TIME );

}


//-----------------------------------------------------------------------------//
// �浹�ڽ� ����
//-----------------------------------------------------------------------------//
void CBoneNode::SetBox( Box *pbox )
{
	if( !m_pBox ) m_pBox = new Box;
	*m_pBox = *pbox;
}


//-----------------------------------------------------------------------------//
// ���
//-----------------------------------------------------------------------------//
void CBoneNode::Render()
{
	if(m_pBox) m_pBox->Render();
}


//-----------------------------------------------------------------------------//
// ���ϸ��̼�
//-----------------------------------------------------------------------------//
BOOL CBoneNode::Animate( int nDelta )
{
	if( !m_bAni ) return TRUE;
	if( !m_pTrack ) return TRUE;

	m_AniFrame += nDelta;
	m_AniIncFrame += nDelta;
	BOOL ani_loop_end = (m_AniFrame > m_AniEnd);	// ���ϸ��̼� ������ ���ٸ� TRUE
	BOOL ani_end	  = (!m_bLoop) && (m_AniIncFrame > m_AniTime);	// �ѿ��ϸ��̼� �ð��� �����ٸ� TRUE

	if( ani_loop_end || ani_end )
 	{
		// �������̿��ٸ� ���� ���ϸ��̼����� �ǵ�����.
		if( m_bSmooth )
		{
			m_AniFrame = m_AniStart;
			m_AniIncFrame = m_AniStart;		// ������ �����ٸ� 0���� �ʱ�ȭ�ؼ� �� ���ϸ��̼� �ð��� �����Ѵ�.
			m_AniEnd = m_nTempEndTime;
			m_bSmooth = FALSE;
			m_pTrack->InitAnimate();
		}
		else
		{
			// �������ϸ��̼��� ������� �ݺ� ���ϸ��̼��̶��
			// �������� ���ϸ��̼��� ó������ ������.
			if( m_bLoop )
			{
				m_AniFrame = m_AniStart;
				m_pTrack->InitAnimate();
			}
			else
			{
				// �ݺ� ���ϸ��̼��� �ƴ϶�� 
				// �� ���ϸ��̼� �ð��� ������ ���ϸ��̼��� �����ϰ� FALSE�� �����Ѵ�.
				// �׷��� �ʴٸ� ���ϸ��̼��� ó������ ������.				
				if( ani_loop_end )
				{
					m_AniFrame = m_AniStart;

					// �� ���ϸ��̼��� ������ �ʾҴٸ� ���ϸ��̼� ������ ó������ �ǵ�����.
					// �� ���ϸ��̼��� �����ٸ� ������ �ǵ����� �ʰ� ������ �������� ���ϰ� �������д�.
					// ���� ���ϸ��̼ǿ��� �����Ǳ� ���ؼ� ������ ���������� �ξ�� �Ѵ�.
					if( !ani_end )
						m_pTrack->InitAnimate();
				}
				if( ani_end )
				{
					m_bAni = FALSE;
					return FALSE;
				}
			}
		}
	}

//	Matrix44 matAni;
//	matAni.SetIdentity();
	m_matAni.SetIdentity();

	m_pTrack->Animate( m_AniFrame, &m_matAni );

	m_matAccTM = m_matLocal * m_matAni * m_matTM;

	// ���� posŰ���� ������ local TM�� ��ǥ�� ����Ѵ�
	if( m_matAni._41 == 0.0f && m_matAni._42 == 0.0f && m_matAni._43 == 0.0f )
	{
		m_matAccTM._41 = m_matLocal._41;
		m_matAccTM._42 = m_matLocal._42;
		m_matAccTM._43 = m_matLocal._43;
	}
	else	// posŰ���� ��ǥ������ �����Ѵ�(�̷��� ���� ������ TM�� pos������ �ι�����ȴ�)
	{
		m_matAccTM._41 = m_matAni._41;
		m_matAccTM._42 = m_matAni._42;
		m_matAccTM._43 = m_matAni._43;
	}

	if( m_pParent )
		m_matAccTM = m_matAccTM * ((CBoneNode*)m_pParent)->m_matAccTM;

	m_pPalette[ m_nId] = m_matOffset * m_matAccTM;

	if (m_pBox)
		m_pBox->SetWorldTM(&m_pPalette[ m_nId]);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////
// CBone

CBone_::CBone_():
m_pPalette(NULL), m_pRoot(NULL), m_pmatAniTM(NULL)
{
	m_matWorld.SetIdentity();

}


CBone_::~CBone_()
{
	Clear();
}

void CBone_::Clear()
{
	ReleaseTree( m_pRoot );
	SAFE_DELETEA( m_pPalette );
}


//-----------------------------------------------------------------------------//
// Bone ������Ʈ
// �� BoneNode�� �����ؼ� Ʈ���� �����.
// bCollision = TRUE : �浹�ڽ��� �����Ѵ�.
// �浹�ڽ��� �����ϱ� ���ؼ� SMeshGroupLoader ����Ÿ�� �ʿ��ϴ�.
//-----------------------------------------------------------------------------//
BOOL CBone_::Load( int ObjId, SBoneGroupLoader *pLoader, BOOL bCollision, SMeshGroupLoader *pMeshLoader )
{
	if( 0 >= pLoader->size ) return FALSE;

	Clear();

	m_ObjId = ObjId;
	m_pPalette = new Matrix44[ pLoader->size];
	int i=0;
	for( i=0; i < pLoader->size; ++i )
		m_pPalette[ i].SetIdentity();

	// �浹�ڽ��� �����Ѵ�.
	Box box;
	if( bCollision )
		CreateCollisionBox( pLoader, pMeshLoader, &box );

	vector< CBoneNode* > vec;
	vec.resize( pLoader->size );
	for( i=0; i < pLoader->size; ++i )
	{
		SBoneLoader *p = &pLoader->pBone[ i];
		CBoneNode *pbone = new CBoneNode( p->id );
		pbone->Load( m_ObjId, p, m_pPalette );
		if( vec[ p->id] ) delete vec[ p->id];
		vec[ p->id] = pbone;

		// �浹�ڽ��� ���� �����Ѵ�.
		if( bCollision )
		{
			//if( !strcmp(g_CollisionNodeName, pLoader->pBone[ i].szName) )
			//	pbone->SetBox( &box );
		}
		// ���������� �����Ѵ�.
		//if( !strcmp(g_BoneCenterNodeName, pLoader->pBone[ i].szName) )
		//	m_pmatAniTM = pbone->GetAccTM();

		if( -1 == p->parentid ) // root
			m_pRoot = pbone;
		else
			InsertChildTree( vec[ p->parentid], pbone );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���ο� Animation ����
// �� BoneNode�� Key������ �����ϸ�, �� BoneNode�� �ڽ��� Track������ ������Ʈ�Ѵ�.
//-----------------------------------------------------------------------------//
void CBone_::SetAnimation( SKeyGroupLoader *pLoader, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	SetAnimationRec( m_pRoot, pLoader, nAniTime, bContinue, bLoop, bQuick );
}
//-----------------------------------------------------------------------------//
// Tree�� ���󰡸� ���ϸ��̼��� �����Ѵ�. ���� Bone Tree�� �ѳ��� ���ϸ��̼���
// �����Ǿ� �����ʴٸ� ������ ����� �ִ�. 
// Node->id �� ����� ���ϸ��̼��� �迭index���� ����Ű�� ���ÿ� �ڽ��� ID�̱⵵ �ϴ�
// Tree�� ��� ��尡 ���ϸ��̼� ����Ÿ�� �������� ������ ����̴�.
//-----------------------------------------------------------------------------//
void CBone_::SetAnimationRec( CBoneNode *pNode, SKeyGroupLoader *pLoader, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	if( !pNode ) return;
	if( pNode->m_nId >= pLoader->size ) return;

	pNode->SetAnimation( &pLoader->pKey[ pNode->m_nId], nAniTime, bContinue, bLoop, bQuick, TRUE );
	SetAnimationRec( (CBoneNode*)pNode->m_pNext, pLoader, nAniTime, bContinue, bLoop, bQuick );
	SetAnimationRec( (CBoneNode*)pNode->m_pChild, pLoader, nAniTime, bContinue, bLoop, bQuick );
}


//-----------------------------------------------------------------------------//
// Bone�� �浹�ڽ��� ����Ѵ�.
//-----------------------------------------------------------------------------//
void CBone_::Render()
{
	//g_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&m_matWorld );
	RenderRec( m_pRoot );
}
void CBone_::RenderRec( C3dNode *pNode )
{
	if( !pNode ) return;
	pNode->Render();
	RenderRec( pNode->m_pNext );
	RenderRec( pNode->m_pChild );
}


//-----------------------------------------------------------------------------//
// Animation
//-----------------------------------------------------------------------------//
BOOL CBone_::Animate( int nDelta )
{
	return AnimateRec( m_pRoot, nDelta );
}
BOOL CBone_::AnimateRec( C3dNode *pNode, int nDelta )
{
	if( !pNode ) return FALSE;
	BOOL reval = pNode->Animate( nDelta );
	AnimateRec( pNode->m_pNext, nDelta );
	AnimateRec( pNode->m_pChild, nDelta );
	return reval;
}


//-----------------------------------------------------------------------------//
// ���� ���ϸ��̼� �������� �����Ѵ�.
//-----------------------------------------------------------------------------//
int  CBone_::GetCurrentFrame()
{
	if( !m_pRoot ) return 0;
	return m_pRoot->m_AniFrame;
}


//-----------------------------------------------------------------------------//
// BoneNode ã��
//-----------------------------------------------------------------------------//
CBoneNode* CBone_::FindBoneNode( int id )
{
	return (CBoneNode*)FindTree( m_pRoot, id );
}
CBoneNode* CBone_::FindBoneNode( char *pName )
{
	return FindBoneNodeRec( m_pRoot, pName );
}
CBoneNode* CBone_::FindBoneNodeRec( CBoneNode *pCurNode, char *pName )
{
	if( !pCurNode ) return NULL;
	if( !_stricmp(pCurNode->m_pName, pName) ) return pCurNode;

	CBoneNode *pNode = NULL;
	pNode = FindBoneNodeRec( (CBoneNode*)pCurNode->m_pNext, pName );
	if( pNode ) return pNode;
	pNode = FindBoneNodeRec( (CBoneNode*)pCurNode->m_pChild, pName );
	if( pNode ) return pNode;

	return NULL;
}


//-----------------------------------------------------------------------------//
// �浹�ڽ��� �����Ѵ�.
// �浹�ڽ��� �����ϱ� ���ؼ� SMeshGroupLoader ����Ÿ�� �ʿ��ϴ�.
// ���� �浹�ڽ��� �����ȴ�.
//-----------------------------------------------------------------------------//
BOOL CBone_::CreateCollisionBox( SBoneGroupLoader *pLoader, SMeshGroupLoader *pMeshLoader, Box *pReval )
{
	// ���� ���� { bip01 spine1, Bip01 Spine, Bip01 Pelvis, Bip01 L Clavicle, Bip01 R Clavicle, Bip01 Neck }
	// �� ���� ����� ���ؽ����� ���ͼ� �浹�ڽ��� �����Ѵ�.
	char *nodetable[] = 
	{ "bip01 spine1", "Bip01 Spine", "Bip01 Pelvis", "Bip01 L Clavicle", "Bip01 R Clavicle", "Bip01 Neck" };
	const int tabsize = sizeof(nodetable) / sizeof(char*);

	// physiq������ ���´�.
	SPhysiqueLoader *physiq = NULL;
	SMeshLoader *mesh = NULL;
	int i=0;
	for(i=0; i < pMeshLoader->size; ++i )
	{
		if( 0 < pMeshLoader->pMesh[ i].physiq.size )
		{
			mesh = &pMeshLoader->pMesh[ i];
			physiq = &pMeshLoader->pMesh[ i].physiq;
			break;
		}
	}
	if( !physiq ) return FALSE;

	list<int> vtxlist;
	for( i=0; i < pLoader->size; ++i )
	{
		SBoneLoader *p = &pLoader->pBone[ i];
		int k=0;
		for( k=0; k < tabsize; ++k )
		{
			if( !strcmp(p->szName, nodetable[ k]) )
				break;
		}
		if( k >= tabsize ) continue;

		// ���뿡 ���õ� ����� �ϰ�� physiq�� �˻��ؼ� ���� ����� ���ؽ� ã�´�.
		for( int m=0; m < physiq->size; ++m )
		{
			for( int o=0; o < physiq->p[ m].size; ++o )
			{
				if( i == physiq->p[ m].w[ o].bone )
				{
					vtxlist.push_back( m );
					break;
				}
			}
		}
	}

	// Vertex�� �ִ��ּҸ� ���Ѵ�.
	Vector3 _min(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 _max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	list<int>::iterator it = vtxlist.begin();
	while( vtxlist.end() != it )
	{
		int idx = *it++;
		if( mesh->vnt.size <= idx ) continue;
		Vector3 *v = &mesh->vnt.pV[ idx].v;

		if( _min.x > v->x )
			_min.x = v->x;
		else if( _max.x < v->x )
			_max.x = v->x;
		if( _min.y > v->y )
			_min.y = v->y;
		else if( _max.y < v->y )
			_max.y = v->y;
		if( _min.z > v->z )
			_min.z = v->z;
		else if( _max.z < v->z )
			_max.z = v->z;
	}

	pReval->SetBox( &_min, &_max );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// CollisionBox�� ������ǥ�� �ǵ����Ѵ�.
//-----------------------------------------------------------------------------//
void CBone_::UpdateCollisionBox()
{
	UpdateCollisionBoxRec( m_pRoot );
}
void CBone_::UpdateCollisionBoxRec( CBoneNode *pCurNode )
{
	if( !pCurNode ) return;
	if( pCurNode->m_pBox ) 
	{
		pCurNode->m_pBox->SetWorldTM( &(m_pPalette[ pCurNode->m_nId] * m_matWorld));
		pCurNode->m_pBox->Update();
	}
	UpdateCollisionBoxRec( (CBoneNode*)pCurNode->m_pChild );
	UpdateCollisionBoxRec( (CBoneNode*)pCurNode->m_pNext );
}

//-----------------------------------------------------------------------------//
// BoneNode�� ����� CollisionBox�� ����Ʈ�� �����Ѵ�.
//-----------------------------------------------------------------------------//
void CBone_::GetCollisionBox( list<Box*> *pList )
{
	GetCollisionBoxRec( m_pRoot, pList );
}
void CBone_::GetCollisionBoxRec( CBoneNode *pCurNode, list<Box*> *pList )
{
	if( !pCurNode ) return;
	if( pCurNode->m_pBox ) pList->push_back( pCurNode->m_pBox );
	GetCollisionBoxRec( (CBoneNode*)pCurNode->m_pChild, pList );
	GetCollisionBoxRec( (CBoneNode*)pCurNode->m_pNext, pList );
}


Matrix44* CBone_::GetAniTM()
{
	// R = AX
	// A^-1 R = X
	return &m_pPalette[ m_pRoot->m_nId];
}

void CBone_::SetTM( Matrix44 *pTM ) 
{ 
	m_matWorld = *pTM; 
	UpdateCollisionBox();
}

void CBone_::MutliplyTM( Matrix44 *pTM ) 
{ 
	m_matWorld *= *pTM; 
	UpdateCollisionBox();
}


//------------------------------------------------------------------------
// pickPos : Screen ��ǥ
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL CBone_::Pick( const Vector2 &pickPos )
{
	return PickRec( m_pRoot, pickPos);
}


//------------------------------------------------------------------------
// pickPos : Screen ��ǥ
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL CBone_::PickRec( CBoneNode *pCurNode, const Vector2 &pickPos )
{
	if (!pCurNode) return FALSE;
	if (pCurNode->m_pBox) 
	{
		if (pCurNode->m_pBox->Pick(pickPos))
			return TRUE;
	}

	if (PickRec((CBoneNode*)pCurNode->m_pChild, pickPos))
		return TRUE;
	if (PickRec((CBoneNode*)pCurNode->m_pNext, pickPos))
		return TRUE;

	return FALSE;
}
