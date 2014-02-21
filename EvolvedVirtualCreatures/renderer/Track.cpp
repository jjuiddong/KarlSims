
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
// ���ϸ��̼� ������ �ε��Ѵ�.
// bContinue : TRUE�̸� �� �����ӿ��� ��� ���ϸ��̼� �ȴ�.
// bSmooth: ���ϸ��̼��� ���� �ε��Ҷ� �̹� ���ϸ��̼� ���̶�� �����ϸ��̼ǰ�
//			�������� ���θ� ����Ŵ
// nSmoothTime : �����ð�
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

		// �������� 0���� �ʱ�ȭ �ȴ�. 
		// ������ �ٽ� �����Ǹ� ������ �߻��Ҽ� �ֱ⶧���� �ʱ�ȭ �ȴ�.
		m_nCurFrame = 0;
		m_pKeys = pLoader;
	}
	else
	{
		m_pKeys = pLoader;
		if( !bContinue )
			InitAnimate(); // ���ϸ��̼��� ó�����·� ���´�.
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���ϸ��̼��� ó�� ��ġ�� ������. (����ȭ������ �������)
// �̷��� ���������� �ŷ������� frame�� ���� Node�� ã�ƾ��Ѵ�.
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
	// �������� ���� ��带 ����Ű�� ���� ���ϸ��̼����� �Ѿ��.
	if( frame > m_pKeyPos[ 1]->frame )
	{
		// ���ϸ��̼��� �������̸� ������ Ű���� �����Ѵ�.
		if( m_pKeys->pos.size-1 <= m_nKeyPos )
		{
			*pVec = m_pKeyPos[ 1]->p;
			return TRUE;
		}

		// ���� ���ϸ��̼����� �Ѿ��.
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
	// �������� ���� ��带 ����Ű�� ���� ���ϸ��̼����� �Ѿ��.
	if( frame > m_pKeyRot[ 1]->frame )
	{
		// ���ϸ��̼��� �������̸� ������ Ű���� �����Ѵ�.
		if( m_pKeys->rot.size-1 <= m_nKeyRot )
		{
			*pQuat = m_pKeyRot[ 1]->q;
			return TRUE;
		}

		// ���� ���ϸ��̼����� �Ѿ��.
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
	// �������� ���� ��带 ����Ű�� ���� ���ϸ��̼����� �Ѿ��.
	if( frame > m_pKeyScale[ 1]->frame )
	{
		// ���ϸ��̼��� �������̸� ������ Ű���� �����Ѵ�.
		if( m_pKeys->scale.size-1 <= m_nKeyScale )
		{
			*pVec = m_pKeyScale[ 1]->s;
			return TRUE;
		}

		// ���� ���ϸ��̼����� �Ѿ��.
		++m_nKeyScale;
		m_pKeyScale[ 0] = m_pKeyScale[ 1];
		m_pKeyScale[ 1] = &m_pKeys->scale.s[ m_nKeyScale];
	}

	float fAlpha = GetAlpha( m_pKeyScale[ 0]->frame, m_pKeyScale[ 1]->frame, (float)frame );
	*pVec = m_pKeyScale[ 0]->s.Interpolate( m_pKeyScale[ 1]->s, fAlpha );
	return TRUE;
}

