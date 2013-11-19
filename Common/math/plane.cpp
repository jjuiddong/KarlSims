
#include "stdafx.h"
//#include "../Common.h"
#include "math.h"

Vector3 v0;
Vector3 v1;


//-----------------------
//
//-----------------------
Plane::Plane()
{
} //Plane

//-----------------------
//
//-----------------------
Plane::Plane( float a, float b, float c, float d )
{
	v0.x = a;
	v0.y = b;
	v0.z = c;

	N = v0.Normal();
	D = d;
} //Plane

//-----------------------
//
//-----------------------
Plane::Plane( Vector3& vN, float d )
{
	N = vN.Normal();
	D = d;
} //Plane::Plane

//-----------------------
//
//-----------------------
Plane::Plane( Vector3& vA, Vector3& vB, Vector3& vC )
{
	v0 = vC - vA;
	v1 = vC - vB;

	N = v0.CrossProduct( v1 );
	N.Normalize();

	D = -N.DotProduct( vA );
} //Plane::Plane

//
void Plane::Init( Vector3& vA, Vector3& vB, Vector3& vC )
{
	v0 = vC - vA;
	v1 = vC - vB;

	N = v0.CrossProduct( v1 );
	N.Normalize();

	D = -N.DotProduct( vA );
} //Plane::Init

//-----------------------
//
//-----------------------
float Plane::Distance( Vector3& vP ) const
{
	float fDot = N.DotProduct( vP );
	return fDot + (D);
} //Plane::Distance

//-----------------------
//
//-----------------------
Vector3	Plane::Pick( Vector3& vOrig, Vector3& vDir ) const
{
	return vOrig + vDir * Distance( vOrig ) / vDir.DotProduct( -N );
} //Plane::Pick

//-----------------------
//
//-----------------------
float Plane::GetX( float fY, float fZ ) const
{
	return -( N.y * fY + N.z * fZ + D ) / N.x;
} //Plane::GetX

//-----------------------
//
//-----------------------
float Plane::GetY( float fX, float fZ ) const
{
	return -( N.x * fX + N.z * fZ + D ) / N.y;
} //Plane::GetY

//-----------------------
//
//-----------------------
float Plane::GetZ( float fX, float fY ) const
{
	//Assert( ABS( N.z ) > MATH_EPSILON );
	return -( N.x * fX + N.y * fY + D ) / N.z;
} //Plane::GetZ

//-----------------------
// Get Collision
//-----------------------
float Plane::Collision( Vector3& vP )
{
	return N.x * vP.x + N.y * vP.y + N.z * vP.z + D;
} //Plane::Collision

//-----------------------
// Get Collision
//-----------------------
BOOL Plane::CollisionSphere( Vector3& vP, const float fRadius )
{
	/*
		평면의 방정식 : N * P + D = 0
		구의 중심점과 평면과의 거리 : | N * C + D | / | N |
	*/

	return TRUE;
} //Plane::CollisionSphere

//-----------------------
//	LineCross
//-----------------------
int Plane::LineCross( Vector3& v0, Vector3& v1, Vector3* pvOut ) const
{
	float fSign[2]; 
 
	fSign[0] = N.DotProduct( v0 ) + D;
	fSign[1] = N.DotProduct( v1 ) + D;

	if( fSign[0] * fSign[1] > 0.0F ) 
	{
		return 0; 
	} //if

	if( fSign[0] == 0.0F )
	{
		if( pvOut )
		{
			*pvOut = v0;
		} //if

		if( fSign[1] == 0.0F )
		{
			return 3;
		} //if

		return 2;
	} //if

	if( fSign[1] == 0.0F )
	{
		if( pvOut )
		{
			*pvOut = v1;
		} //if
		return 2;
	} //if

	if( pvOut )
	{
		float aDot = v0.x * N.x + v0.y * N.y + v0.z * N.z;
		float bDot = v1.x * N.x + v1.y * N.y + v1.z * N.z;
		float scale = ( -D - aDot) / ( bDot - aDot );
		*pvOut = v0 + ( (v1 - v0)*scale );
	} //if

	return 1;
} //Plane::LineCross
