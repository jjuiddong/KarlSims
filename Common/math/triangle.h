//-----------------------------------------------------------------------------//
// 2009-08-07  programer: jaejung ¦®(¢Á¬Õ¢Á)¦°
// 
//-----------------------------------------------------------------------------//

#if !defined (__TRIANGLE_H__)
#define __TRIANGLE_H__


struct Triangle
{
public:
	Triangle();
	Triangle( const Vector3& vA, const Vector3& vB, const Vector3& vC );

public:
	Vector3	a, b, c;
	void Init( const Vector3& vA, const Vector3& vB, const Vector3& vC );
	BOOL Intersect(	const Vector3& vOrig, const Vector3& vDir,
					float* pfT = NULL, float* pfU = NULL, float* pfV = NULL );
	float Distance( const Vector3& vPos );

};

BOOL Intersection( const Triangle& tri1, const Triangle& tri2 );

#endif // __TRIANGLE_H__
