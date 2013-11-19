
#ifndef __BOX_H__
#define __BOX_H__

//------------------------------------------------------------------------
// matrix class �� �ִ� box Ŭ������ �и����״�.
//
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------

// Box Ŭ����
// d3d::CreateBox() �Լ��� Bounding Box�� �����Ѵ�.
class Box
{
public:
	Box() {}

public:
	Vector3 m_Min, m_Max; // Box�� �ִ� �ּҰ�
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
