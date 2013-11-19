
#ifndef __SPHERE_H__
#define __SPHERE_H__

//------------------------------------------------------------------------
// matrix class �� �ִ� sphere Ŭ������ �и����״�.
//
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------

class Sphere
{
public:
	Sphere() {}
	Sphere( float r, Vector3 p ) : m_R(r), m_vCenter(p) {}

public:
	float m_R;			// ������
	Vector3 m_vCenter;	// ��ġ

public:
	void SetSphere( float r, Vector3 center );
	void SetRadius( float r ) { m_R = r; }
	BOOL Collision( Sphere *pSphere );
	void SetCenter( Vector3 *pPos ) { m_vCenter = *pPos; }
	void Render();

};

#endif
