
#include "stdafx.h"
//#include "../Common.h"
#include "box.h"


//-----------------------------------------------------------------------------//
// Box ����
//-----------------------------------------------------------------------------//
void Box::SetBox( Vector3 *pMin, Vector3 *pMax )
{
	//       3-----2
	//      /|    /|
	//     1-----0 |
	//	   | 7 - | 6
	//	   |/    |/
	//	   5-----4
	m_Box[ 0] = Vector3( pMax->x, pMax->y, pMin->z );
	m_Box[ 1] = Vector3( pMin->x, pMax->y, pMin->z );
	m_Box[ 2] = Vector3( pMax->x, pMax->y, pMax->z );
	m_Box[ 3] = Vector3( pMin->x, pMax->y, pMax->z );
	m_Box[ 4] = Vector3( pMax->x, pMin->y, pMin->z );
	m_Box[ 5] = Vector3( pMin->x, pMin->y, pMin->z );
	m_Box[ 6] = Vector3( pMax->x, pMin->y, pMax->z );
	m_Box[ 7] = Vector3( pMin->x, pMin->y, pMax->z );

	m_Min = *pMin;
	m_Max = *pMax;
	m_matWorld.SetIdentity();
}


//-----------------------------------------------------------------------------//
// ��������� ������� min, max�� �����Ѵ�.
//-----------------------------------------------------------------------------//
void Box::Update()
{
	Vector3 box[ 8];
	int i=0;
	for( i=0; i < 8; ++i )
		box[ i] = m_Box[ i] * m_matWorld;

	Vector3 _min = box[ 0];
	Vector3 _max = box[ 1];
	for( i=0; i < 8; i++ )
	{
		Vector3 *v = &box[ i];
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
	m_Min = _min;
	m_Max = _max;
}


//-----------------------------------------------------------------------------//
// bOwn = TRUE : ������� m_Min, m_Max �� �����Ѵ�.
// bOwn = FALSE: m_Box�� �ִ��ּҸ� ������ ���ϵȴ�.
// bUpdate = TRUE : ������ min, max���� ��������� ������Ʈ �Ѵ�.
//-----------------------------------------------------------------------------//
void Box::GetMinMax( Vector3 *pMin, Vector3 *pMax, BOOL bOwn, BOOL bUpdate ) // bOwn = TRUE, bUpdate = FALSE
{
	if( bOwn )
	{
		*pMin = m_Min;
		*pMax = m_Max;
	}
	else
	{
		// world ��� ����
		Vector3 _min = m_Min * m_matWorld;
		Vector3 _max = m_Max * m_matWorld;

		*pMin = _min;
		*pMax = _max;

		if( bUpdate )
		{
			m_Min = _min;
			m_Max = _max;
		}
	}
}


//-----------------------------------------------------------------------------//
// �浹�׽�Ʈ �浹�Ǿ��ٸ� ���� TRUE
// AABB �浹�׽�Ʈ
//-----------------------------------------------------------------------------//
BOOL Box::Collision( Box *pBox )
{
	return ((m_Min.x < pBox->m_Max.x) && (m_Min.y < pBox->m_Max.y) && (m_Min.z < pBox->m_Max.z))
		&& ((pBox->m_Min.x < m_Max.x) && (pBox->m_Min.y < m_Max.y) && (pBox->m_Min.z < m_Max.z));
}

void Box::SetWorldTM( Matrix44 *mat ) 
{ 
	m_matWorld = *mat; 
}

void Box::MultiplyWorldTM( Matrix44 *mat )
{
	m_matWorld *= *mat;
}


// length( m_Max - m_Min )
float Box::GetSize()
{
	return sqrt( m_Max.LengthRoughly(m_Min) );
}


//-----------------------------------------------------------------------------//
// ���
//-----------------------------------------------------------------------------//
void Box::Render()
{
// 	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
// 	g_pDevice->SetFVF( D3DFVF_XYZ );
// 	g_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&m_matWorld );
// 	g_pDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 7, m_Box, sizeof(Vector3) );	
// 	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}
//-----------------------------------------------------------------------------//
// ����������½� ���ȴ�.
//-----------------------------------------------------------------------------//
void Box::Render_Hierarchy()
{
// 	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
// 	g_pDevice->SetFVF( D3DFVF_XYZ );
// 	g_pDevice->MultiplyTransform( D3DTS_WORLD, (D3DXMATRIX*)&m_matWorld );
// 	g_pDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 7, m_Box, sizeof(Vector3) );	
// 	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


//------------------------------------------------------------------------
// pickPos : screen ���� ��ǥ
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL Box::Pick(const Vector2 &pickPos)
{
	Vector3 box[ 8];
	for( int i=0; i < 8; ++i )
		box[ i] = m_Box[ i] * m_matWorld;

	//       3-----2
	//      /|    /|
	//     1-----0 |
	//	   | 7 - | 6
	//	   |/    |/
	//	   5-----4
	Triangle triangle[ 12];
	// top
	triangle[ 0].Init(box[0], box[1], box[2]);
	triangle[ 1].Init(box[1], box[3], box[2]);
	// left
	triangle[ 2].Init(box[1], box[5], box[7]);
	triangle[ 3].Init(box[3], box[1], box[7]);
	// right
	triangle[ 4].Init(box[0], box[2], box[6]);
	triangle[ 5].Init(box[0], box[6], box[4]);
	// back
	triangle[ 6].Init(box[2], box[3], box[6]);
	triangle[ 7].Init(box[3], box[7], box[6]);
	// front
	triangle[ 8].Init(box[1], box[0], box[4]);
	triangle[ 9].Init(box[1], box[4], box[5]);
	// bottom
	triangle[ 10].Init(box[4], box[7], box[5]);
	triangle[ 11].Init(box[4], box[6], box[7]);

	Vector3 vOrig, vDir;
//	d3d::GetPickRay( (int)pickPos.x, (int)pickPos.y, &vOrig, &vDir );

	BOOL isPick = FALSE;
	float a, b, c;
	for (int i=0; i < 12; ++i)
	{
		if (triangle[ i].Intersect(vOrig, vDir, &a, &b, &c))
		{
			isPick = TRUE;
			break;
		}
	}

	return isPick;
}
