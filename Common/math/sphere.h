
#ifndef __SPHERE_H__
#define __SPHERE_H__

//------------------------------------------------------------------------
// matrix class 에 있던 sphere 클래스를 분리시켰다.
//
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------

class Sphere
{
public:
	Sphere() {}
	Sphere( float r, Vector3 p ) : m_R(r), m_vCenter(p) {}

public:
	float m_R;			// 반지름
	Vector3 m_vCenter;	// 위치

public:
	void SetSphere( float r, Vector3 center );
	void SetRadius( float r ) { m_R = r; }
	BOOL Collision( Sphere *pSphere );
	void SetCenter( Vector3 *pPos ) { m_vCenter = *pPos; }
	void Render();

};

#endif
