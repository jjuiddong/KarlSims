
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
// 데이타 로드
// nObjId = Bone의 오브젝트 아이디 = Model ID와 동일하다.
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
// 에니메이션 데이타 설정
// nAniTime : 에니메이션될 시간, 0이면 총 에니메이션 시간이다.
// bContinue : TRUE이면 전 프레임에서 계속 에니메이션 된다.
// bLoop : 반복해서 에니메이션 한다면 TRUE
// bSmooth : 에니메이션간에 보간을 한다면 TRUE
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

	// 에니메이션 중일때만 보간된다.
	// bContinue일때는 스킵된다.
	// 전 에니메이션이 Quick상태일때 보간은 스킵된다.
	BOOL interpol = bSmooth && m_pTrack && (!bContinue) && (!m_bQuick);
	if( interpol )
	{
		m_AniEnd = SMOOTH_TIME;
		m_nTempEndTime = aniend;
	}

	m_bQuick = bQuick;
	m_bSmooth = interpol;

	// 전 에니메이션에서 연속해서 에니메이션 된다면
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
// 충돌박스 설정
//-----------------------------------------------------------------------------//
void CBoneNode::SetBox( Box *pbox )
{
	if( !m_pBox ) m_pBox = new Box;
	*m_pBox = *pbox;
}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CBoneNode::Render()
{
	if(m_pBox) m_pBox->Render();
}


//-----------------------------------------------------------------------------//
// 에니메이션
//-----------------------------------------------------------------------------//
BOOL CBoneNode::Animate( int nDelta )
{
	if( !m_bAni ) return TRUE;
	if( !m_pTrack ) return TRUE;

	m_AniFrame += nDelta;
	m_AniIncFrame += nDelta;
	BOOL ani_loop_end = (m_AniFrame > m_AniEnd);	// 에니메이션 끝까지 갔다면 TRUE
	BOOL ani_end	  = (!m_bLoop) && (m_AniIncFrame > m_AniTime);	// 총에니메이션 시간이 지났다면 TRUE

	if( ani_loop_end || ani_end )
 	{
		// 보간중이였다면 원래 에니메이션으로 되돌린다.
		if( m_bSmooth )
		{
			m_AniFrame = m_AniStart;
			m_AniIncFrame = m_AniStart;		// 보간이 끝났다면 0으로 초기화해서 총 에니메이션 시간을 저장한다.
			m_AniEnd = m_nTempEndTime;
			m_bSmooth = FALSE;
			m_pTrack->InitAnimate();
		}
		else
		{
			// 보간에니메이션이 종료된후 반복 에니메이션이라면
			// 보간없이 에니메이션을 처음으로 돌린다.
			if( m_bLoop )
			{
				m_AniFrame = m_AniStart;
				m_pTrack->InitAnimate();
			}
			else
			{
				// 반복 에니메이션이 아니라면 
				// 총 에니메이션 시간이 지나면 에니메이션을 종료하고 FALSE를 리턴한다.
				// 그렇지 않다면 에니메이션을 처음으로 돌린다.				
				if( ani_loop_end )
				{
					m_AniFrame = m_AniStart;

					// 총 에니메이션이 끝나지 않았다면 에니메이션 정보를 처음으로 되돌린다.
					// 총 에니메이션이 끝났다면 정보를 되돌리지 않고 마지막 프레임을 향하게 내버려둔다.
					// 다음 에니메이션에서 보간되기 위해서 마지막 프레임으로 두어야 한다.
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

	// 만약 pos키값이 없으면 local TM의 좌표를 사용한다
	if( m_matAni._41 == 0.0f && m_matAni._42 == 0.0f && m_matAni._43 == 0.0f )
	{
		m_matAccTM._41 = m_matLocal._41;
		m_matAccTM._42 = m_matLocal._42;
		m_matAccTM._43 = m_matLocal._43;
	}
	else	// pos키값을 좌표값으로 적용한다(이렇게 하지 않으면 TM의 pos성분이 두번적용된다)
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
// Bone 업데이트
// 새 BoneNode를 생성해서 트리를 만든다.
// bCollision = TRUE : 충돌박스를 생성한다.
// 충돌박스를 생성하기 위해서 SMeshGroupLoader 데이타가 필요하다.
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

	// 충돌박스를 생성한다.
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

		// 충돌박스를 본에 설정한다.
		if( bCollision )
		{
			//if( !strcmp(g_CollisionNodeName, pLoader->pBone[ i].szName) )
			//	pbone->SetBox( &box );
		}
		// 본의중점을 설정한다.
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
// 새로운 Animation 설정
// 각 BoneNode에 Key정보를 설정하며, 각 BoneNode는 자신의 Track정보를 업데이트한다.
//-----------------------------------------------------------------------------//
void CBone_::SetAnimation( SKeyGroupLoader *pLoader, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	SetAnimationRec( m_pRoot, pLoader, nAniTime, bContinue, bLoop, bQuick );
}
//-----------------------------------------------------------------------------//
// Tree를 따라가며 에니메이션을 설정한다. 만약 Bone Tree에 한노드라도 에니메이션이
// 설정되어 있지않다면 문제가 생길수 있다. 
// Node->id 는 저장된 에니메이션의 배열index값을 가르키는 동시에 자신의 ID이기도 하다
// Tree의 모든 노드가 에니메이션 데이타가 있을때만 가능한 방식이다.
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
// Bone의 충돌박스를 출력한다.
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
// 현재 에니메이션 프레임을 리턴한다.
//-----------------------------------------------------------------------------//
int  CBone_::GetCurrentFrame()
{
	if( !m_pRoot ) return 0;
	return m_pRoot->m_AniFrame;
}


//-----------------------------------------------------------------------------//
// BoneNode 찾기
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
// 충돌박스를 생성한다.
// 충돌박스를 생성하기 위해서 SMeshGroupLoader 데이타가 필요하다.
// 몸통 충돌박스만 생성된다.
//-----------------------------------------------------------------------------//
BOOL CBone_::CreateCollisionBox( SBoneGroupLoader *pLoader, SMeshGroupLoader *pMeshLoader, Box *pReval )
{
	// 몸통 노드들 { bip01 spine1, Bip01 Spine, Bip01 Pelvis, Bip01 L Clavicle, Bip01 R Clavicle, Bip01 Neck }
	// 각 본에 연결된 버텍스값을 얻어와서 충돌박스를 생성한다.
	char *nodetable[] = 
	{ "bip01 spine1", "Bip01 Spine", "Bip01 Pelvis", "Bip01 L Clavicle", "Bip01 R Clavicle", "Bip01 Neck" };
	const int tabsize = sizeof(nodetable) / sizeof(char*);

	// physiq정보를 얻어온다.
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

		// 몸통에 관련된 본노드 일경우 physiq를 검색해서 본에 연결된 버텍스 찾는다.
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

	// Vertex의 최대최소를 구한다.
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
// CollisionBox가 월드좌표가 되도록한다.
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
// BoneNode에 저장된 CollisionBox를 리스트에 저장한다.
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
// pickPos : Screen 좌표
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL CBone_::Pick( const Vector2 &pickPos )
{
	return PickRec( m_pRoot, pickPos);
}


//------------------------------------------------------------------------
// pickPos : Screen 좌표
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
