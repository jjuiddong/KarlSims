
#ifndef __BOX_H__
#define __BOX_H__

//------------------------------------------------------------------------
// matrix class 에 있던 box 클래스를 분리시켰다.
//
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------

// Box 클래스
// d3d::CreateBox() 함수로 Bounding Box를 생성한다.
class Box
{
public:
	Box() {}

public:
	Vector3 m_Min, m_Max; // Box의 최대 최소값
	Vector3 m_Box[ 8];
	Matrix44 m_matWorld;

public:
	void Render();
	void Render_Hierarchy();
	void Update();

	void SetBox( Vector3 *pMin, Vector3 *pMax );
	void GetMinMax( Vector3 *pMin, Vector3 *pMax, BOOL bOwn=TRUE, BOOL bUpdate=FALSE );
	float GetSize();	// length( m_Max - m_Min )
	BOOL Collision( Box *pBox );
	void SetWorldTM( Matrix44 *mat );
	void MultiplyWorldTM( Matrix44 *mat );
	BOOL Pick(const Vector2 &pickPos);

};

#endif
