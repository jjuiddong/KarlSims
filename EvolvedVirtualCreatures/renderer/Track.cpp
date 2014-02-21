
#include "stdafx.h"
#include "track.h"


CTrack::CTrack() : m_pKeys(NULL), 
m_nCurFrame(0), m_nKeyPos(-1), m_nKeyRot(-1), m_nKeyScale(-1)
{
	ZeroMemory( m_pKeyPos, sizeof(m_pKeyPos) );
	ZeroMemory( m_pKeyRot, sizeof(m_pKeyRot) );
	ZeroMemory( m_pKeyScale, sizeof(m_pKeyScale) );

}


CTrack::~CTrack() 
{

}


//-----------------------------------------------------------------------------//
// 에니메이션 정보를 로드한다.
// bContinue : TRUE이면 전 프레임에서 계속 에니메이션 된다.
// bSmooth: 에니메이션을 새로 로드할때 이미 에니메이션 중이라면 새에니메이션과
//			보간할지 여부를 가리킴
// nSmoothTime : 보간시간
// bSmooth=FALSE, nSmoothTime=0
//-----------------------------------------------------------------------------//
BOOL CTrack::Load( SKeyLoader *pLoader, BOOL bContinue, BOOL bSmooth, int nSmoothTime )
{
	if( bSmooth && m_pKeys )
	{
		SKeyPos pos[ 2];
		SKeyRot rot[ 2];
		SKeyScale scale[ 2];

		rot[ 0].frame = 0;
		rot[ 0].q = Quaternion(0,0,0,1);
		GetRotKey( m_nCurFrame, &rot[ 0].q );
		pos[ 0].frame = 0;
		pos[ 0].p = Vector3( 0, 0, 0 );
		GetPosKey( m_nCurFrame, &pos[ 0].p );
		scale[ 0].frame = 0;
		scale[ 0].s = Vector3( 1, 1, 1 );
		GetScaleKey( m_nCurFrame, &scale[ 0].s );

		m_nKeyPos = 1;
		m_nKeyRot = 1;
		m_nKeyScale = 1;

		pos[ 1] = pLoader->pos.p[ 0];
		rot[ 1] = pLoader->rot.r[ 0];
		scale[ 1] = pLoader->scale.s[ 0];
		pos[ 1].frame = (float)nSmoothTime;
		rot[ 1].frame = (float)nSmoothTime;
		scale[ 1].frame = (float)nSmoothTime;

		memcpy( m_TempPos, pos, sizeof(pos) );
		memcpy( m_TempScale, scale, sizeof(scale) );
		memcpy( m_TempRot, rot, sizeof(rot) );

		m_pKeyPos[ 0] = &m_TempPos[ 0];
		m_pKeyRot[ 0] = &m_TempRot[ 0];
		m_pKeyScale[ 0] = &m_TempScale[ 0];
		m_pKeyPos[ 1] = &m_TempPos[ 1];
		m_pKeyRot[ 1] = &m_TempRot[ 1];
		m_pKeyScale[ 1] = &m_TempScale[ 1];

		// 보간된후 0으로 초기화 된다. 
		// 보간중 다시 보간되면 문제가 발생할수 있기때문에 초기화 된다.
		m_nCurFrame = 0;
		m_pKeys = pLoader;
	}
	else
	{
		m_pKeys = pLoader;
		if( !bContinue )
			InitAnimate(); // 에니메이션을 처음상태로 놓는다.
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 에니메이션을 처음 위치로 돌린다. (최적화때문에 만들어짐)
// 이렇게 하지않으면 매루프마다 frame에 따른 Node를 찾아야한다.
//-----------------------------------------------------------------------------//
void CTrack::InitAnimate()
{
	if( !m_pKeys ) return;

	m_nCurFrame = 0;
	ZeroMemory( m_pKeyPos, sizeof(m_pKeyPos) );
	ZeroMemory( m_pKeyRot, sizeof(m_pKeyRot) );
	ZeroMemory( m_pKeyScale, sizeof(m_pKeyScale) );

	if( 1 <= m_pKeys->pos.size )
	{
		m_nKeyPos = 1;
		m_pKeyPos[ 0] = &m_pKeys->pos.p[ 0];
		if( 2 <= m_pKeys->pos.size )
			m_pKeyPos[ 1] = &m_pKeys->pos.p[ 1];
	}
	if( 1 <= m_pKeys->rot.size )
	{
		m_nKeyRot = 1;
		m_pKeyRot[ 0] = &m_pKeys->rot.r[ 0];
		if( 2 <= m_pKeys->rot.size )
			m_pKeyRot[ 1] = &m_pKeys->rot.r[ 1];
	}
	if( 1 <= m_pKeys->scale.size )
	{
		m_nKeyScale = 1;
		m_pKeyScale[ 0] = &m_pKeys->scale.s[ 0];
		if( 2 <= m_pKeys->scale.size )
			m_pKeyScale[ 1] = &m_pKeys->scale.s[ 1];
	}

}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CTrack::Animate( int frame, Matrix44 *pMat )
{
	if( !m_pKeys ) return;

	m_nCurFrame = frame;

	Quaternion q;
	if( GetRotKey(frame, &q) )
		*pMat = q.GetMatrix();

	Vector3 p(0,0,0);
	if( GetPosKey(frame, &p) )
	{
		pMat->_41 = p.x;
		pMat->_42 = p.y;
		pMat->_43 = p.z;
	}

	Vector3 s(1,1,1);
	if( GetScaleKey(frame, &s) )
	{
		pMat->_11 *= s.x;
		pMat->_22 *= s.y;
		pMat->_33 *= s.z;
	}

}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CTrack::GetPosKey( int frame, Vector3 *pVec )
{
	if( !m_pKeyPos[ 0] ) return FALSE;

	if( !m_pKeyPos[ 1] )
	{
		*pVec = m_pKeyPos[ 0]->p;
		return TRUE;
	}
	// 프레임이 다음 노드를 가리키면 다음 에니메이션으로 넘어간다.
	if( frame > m_pKeyPos[ 1]->frame )
	{
		// 에니메이션의 마지막이면 마지막 키값을 리턴한다.
		if( m_pKeys->pos.size-1 <= m_nKeyPos )
		{
			*pVec = m_pKeyPos[ 1]->p;
			return TRUE;
		}

		// 다음 에니메이션으로 넘어간다.
		++m_nKeyPos;
		m_pKeyPos[ 0] = m_pKeyPos[ 1];
		m_pKeyPos[ 1] = &m_pKeys->pos.p[ m_nKeyPos];
	}

	float fAlpha = GetAlpha( m_pKeyPos[ 0]->frame, m_pKeyPos[ 1]->frame, (float)frame );
	*pVec = m_pKeyPos[ 0]->p.Interpolate( m_pKeyPos[ 1]->p, fAlpha );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CTrack::GetRotKey( int frame, Quaternion *pQuat )
{
	if( !m_pKeyRot[ 0] ) return FALSE;

	if( !m_pKeyRot[ 1] )
	{
		*pQuat = m_pKeyRot[ 0]->q;
		return TRUE;
	}
	// 프레임이 다음 노드를 가리키면 다음 에니메이션으로 넘어간다.
	if( frame > m_pKeyRot[ 1]->frame )
	{
		// 에니메이션의 마지막이면 마지막 키값을 리턴한다.
		if( m_pKeys->rot.size-1 <= m_nKeyRot )
		{
			*pQuat = m_pKeyRot[ 1]->q;
			return TRUE;
		}

		// 다음 에니메이션으로 넘어간다.
		++m_nKeyRot;
		m_pKeyRot[ 0] = m_pKeyRot[ 1];
		m_pKeyRot[ 1] = &m_pKeys->rot.r[ m_nKeyRot];
	}

	float fAlpha = GetAlpha( m_pKeyRot[ 0]->frame, m_pKeyRot[ 1]->frame, (float)frame );
	*pQuat = m_pKeyRot[ 0]->q.Interpolate( m_pKeyRot[ 1]->q, fAlpha );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CTrack::GetScaleKey( int frame, Vector3 *pVec )
{
	if( !m_pKeyScale[ 0] ) return FALSE;

	if( !m_pKeyScale[ 1] )
	{
		*pVec = m_pKeyScale[ 0]->s;
		return TRUE;
	}
	// 프레임이 다음 노드를 가리키면 다음 에니메이션으로 넘어간다.
	if( frame > m_pKeyScale[ 1]->frame )
	{
		// 에니메이션의 마지막이면 마지막 키값을 리턴한다.
		if( m_pKeys->scale.size-1 <= m_nKeyScale )
		{
			*pVec = m_pKeyScale[ 1]->s;
			return TRUE;
		}

		// 다음 에니메이션으로 넘어간다.
		++m_nKeyScale;
		m_pKeyScale[ 0] = m_pKeyScale[ 1];
		m_pKeyScale[ 1] = &m_pKeys->scale.s[ m_nKeyScale];
	}

	float fAlpha = GetAlpha( m_pKeyScale[ 0]->frame, m_pKeyScale[ 1]->frame, (float)frame );
	*pVec = m_pKeyScale[ 0]->s.Interpolate( m_pKeyScale[ 1]->s, fAlpha );
	return TRUE;
}

