//-------------------------------------------------------
//	Vector3.cpp
//-------------------------------------------------------

// Include 
#include "stdafx.h"
//#include "global.h"
	#include "Math.h"
// ~Include

// Global
//	Vector3		vDummy;
	Vector3		vInit( 0.0F, 0.0F, 0.0F );
	Vector3		vUp( 0.0F, 0.0F, 1.0F );
	Matrix44		matRot;
// ~Global

//-------------------------------
// Constructor
//-------------------------------
Vector3::Vector3()
{
} //Vector3::Vector3

Vector3::Vector3( float fX, float fY, float fZ )
:	x ( fX ),
	y ( fY ),
	z ( fZ )
{
} //Vector3::Vector3

//-------------------------------
// operator +
//-------------------------------
Vector3 Vector3::operator + () const
{
	Vector3	vDummy;
	vDummy = *this;
	return vDummy;
} //operator + () const

//-------------------------------
// operator -
//-------------------------------
Vector3 Vector3::operator - () const
{
	Vector3	vDummy;
	vDummy.x = -x;
	vDummy.y = -y;
	vDummy.z = -z;
	return vDummy;
} //operator - () const

//-------------------------------
// operator +
//-------------------------------
Vector3 Vector3::operator + ( const Vector3& v ) const
{
	Vector3	vDummy;
	vDummy.x = x + v.x;
	vDummy.y = y + v.y;
	vDummy.z = z + v.z;
	return vDummy;
} //operator + 

//-------------------------------
// operator -
//-------------------------------
Vector3 Vector3::operator - ( const Vector3& v ) const
{
	Vector3	vDummy;
	vDummy.x = x - v.x;
	vDummy.y = y - v.y;
	vDummy.z = z - v.z;

	return vDummy;
} //operator -

//-------------------------------
// operator *
//-------------------------------
Vector3 Vector3::operator * ( Vector3& v ) const
{
	Vector3	vDummy;
	vDummy.x = x * v.x;
	vDummy.y = y * v.y;
	vDummy.z = z * v.z;

	return vDummy;
} //operator

//-------------------------------
// operator /
//-------------------------------
Vector3 Vector3::operator / ( Vector3& v ) const
{
	Vector3	vDummy;
	vDummy.x = x / v.x;
	vDummy.y = y / v.y;
	vDummy.z = z / v.z;

	return vDummy;
} //operator /

//-------------------------------
// operator +
//-------------------------------
Vector3 Vector3::operator + ( float s ) const
{
	Vector3	vDummy;
	vDummy.x = x + s;
	vDummy.y = y + s;
	vDummy.z = z + s;

	return vDummy;
} //operator +

//-------------------------------
// operator -
//-------------------------------
Vector3 Vector3::operator - ( float s ) const
{
	Vector3	vDummy;
	vDummy.x = x - s;
	vDummy.y = y - s;
	vDummy.z = z - s;

	return vDummy;
} //operator -

//-------------------------------
// operator *
//-------------------------------
Vector3 Vector3::operator * ( float s ) const
{
	Vector3	vDummy;
	vDummy.x = x * s;
	vDummy.y = y * s;
	vDummy.z = z * s;

	return vDummy;
} //operator *

//-------------------------------
// operator /
//-------------------------------
Vector3 Vector3::operator / ( float s ) const
{
	Vector3	vDummy;
	vDummy.x = x / s;
	vDummy.y = y / s;
	vDummy.z = z / s;

	return vDummy;
} //operator /

//-------------------------------
// operator +
//-------------------------------
Vector3& Vector3::operator += ( const Vector3& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
} //operator +=

//-------------------------------
// operator -
//-------------------------------
Vector3& Vector3::operator -= ( const Vector3& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
} //operator -=

//-------------------------------
// operator *
//-------------------------------
Vector3& Vector3::operator *= ( const Vector3& v )
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
} //operator *=

//-------------------------------
// operator /
//-------------------------------
Vector3& Vector3::operator /= ( const Vector3& v )
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
} //operator /=

//-------------------------------
// operator +
//-------------------------------
Vector3& Vector3::operator += ( float s )
{
	x += s;
	y += s;
	z += s;
	return *this;
} //operator +=

//-------------------------------
// operator -
//-------------------------------
Vector3& Vector3::operator -= ( float s )
{
	x -= s;
	y -= s;
	z -= s;
	return *this;
} //operator -=

//-------------------------------
// operator *
//-------------------------------
Vector3& Vector3::operator *= ( float s )
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
} //operator *=

//-------------------------------
// operator /
//-------------------------------
Vector3& Vector3::operator /= ( float s )
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
} //operator /=

//-------------------------------
// operator ==
//-------------------------------
BOOL Vector3::operator == ( const Vector3& v ) const
{
	return ( x == v.x ) && ( y == v.y ) && ( z == v.y );
} //operator ==


BOOL Vector3::EqualEpsilon( const float epsilon, const Vector3 &v ) const
{
	return	(epsilon > fabs( x-v.x )) &&
			(epsilon > fabs( y-v.y )) &&
			(epsilon > fabs( z-v.z ));
}

//-------------------------------
// operator !=
//-------------------------------
BOOL Vector3::operator != ( const Vector3& v ) const
{
	return ( x != v.x ) || ( y != v.y ) || ( z != v.z );
} //operator !=

//-------------------------------
//
//-------------------------------
float Vector3::Length()
{
	return sqrtf( x * x + y * y + z * z);
} //Vector3::Length


float Vector3::Distance(const Vector3& v)
{
	Vector3 t = v - *this;
	return t.Length();
}


//-------------------------------
//
//-------------------------------
float Vector3::LengthRoughly( Vector3& v ) const
{
	Vector3	vDummy;
	vDummy.x = ( x - v.x );
	vDummy.y = ( y - v.y );
	vDummy.z = ( z - v.z );

	return vDummy.x * vDummy.x + vDummy.y * vDummy.y + vDummy.z * vDummy.z;
} //Vector3::LengthRoughly

//-------------------------------
//
//-------------------------------
Vector3 Vector3::Normal() const
{
	static float fDist = 0.0F;
	fDist = sqrtf( x * x + y * y + z * z );

	if( fDist <= MATH_EPSILON )
	{
		return *this;
	} //if

	return *this / fDist;
} //Vector3::Normal

//-------------------------------
//
//-------------------------------
void Vector3::Normalize()
{
	static float fDist = 0.0F;
	fDist = sqrtf( x * x + y * y + z * z );

	if( fDist <= MATH_EPSILON )
	{
		x = 1.0F;
		y = 0.0F;
		z = 0.0F;
		return ;
	} //if

	*this /= fDist;
} //Vector3::Normalize

//-------------------------------
//
//-------------------------------
float Vector3::DotProduct( const Vector3& v ) const
{
	return x * v.x + y * v.y + z * v.z;
} //Vector3::DotProduct

//-------------------------------
//
//-------------------------------
Vector3	Vector3::CrossProduct( const Vector3& v ) const
{
	Vector3	vDummy;
	vDummy.x = y * v.z - z * v.y;
	vDummy.y = z * v.x - x * v.z;
	vDummy.z = x * v.y - y * v.x;

	return vDummy;
} //CrossProduct

//-------------------------------
//
//-------------------------------
Vector3 Vector3::Interpolate( Vector3& v, float f ) const
{	
	Vector3	vDummy;
	vDummy.x = x + ( f * ( v.x - x ) );
	vDummy.y = y + ( f * ( v.y - y ) );
	vDummy.z = z + ( f * ( v.z - z ) );

	return vDummy;
} //Vector3::Interpolate

//-------------------------------
//
//-------------------------------
Vector3 Vector3::operator * ( const Matrix44 &m )
{
	float	fRHW = 1.0F / ( x * m._14 + y * m._24 + z * m._34 + m._44 );	
	
	Vector3	vDummy;
	vDummy.x = ( x * m._11 + y * m._21 + z * m._31 + m._41 ) * fRHW;
	vDummy.y = ( x * m._12 + y * m._22 + z * m._32 + m._42 ) * fRHW;
	vDummy.z = ( x * m._13 + y * m._23 + z * m._33 + m._43 ) * fRHW;

	return vDummy;
} //Vector3::operator *

//-------------------------------
//
//-------------------------------
Vector3& Vector3::operator *= ( Matrix44& m )
{
	float	fRHW = 1.0F / ( x * m._14 + y * m._24 + z * m._34 + m._44 );	
	
	Vector3	vDummy;
	vDummy.x = ( x * m._11 + y * m._21 + z * m._31 + m._41 ) * fRHW;
	vDummy.y = ( x * m._12 + y * m._22 + z * m._32 + m._42 ) * fRHW;
	vDummy.z = ( x * m._13 + y * m._23 + z * m._33 + m._43 ) * fRHW;
	
	*this = vDummy;

	//*this = *this * m;
	return *this;
} //operator *=

//-------------------------------
//
//-------------------------------
void Vector3::RotateX( float fAngle )
{
	matRot.SetRotationX( fAngle );
	*this *= matRot;
} //Vector3::RotateX

//-------------------------------
//
//-------------------------------
void Vector3::RotateY( float fAngle )
{
	matRot.SetRotationY( fAngle );
	*this *= matRot;
} //Vector3::RotateY

//-------------------------------
//
//-------------------------------
void Vector3::RotateZ( float fAngle )
{
	matRot.SetRotationZ( fAngle );
	*this *= matRot;
} //Vector3::RotateZ

//-------------------------------
//
//-------------------------------
Quaternion Vector3::GetQuaternion()
{
	Vector3	vDummy;
	vDummy.x = x;
	vDummy.y = y;
	vDummy.z = z;

	if( x == 0 && y == 0 )
	{
		Quaternion qRot;
		qRot.SetRotationZ( ANGLE( 0 ) );
		return qRot;
	} //if

	vDummy.Normalize();
	vDummy *= -1;

	Matrix44	matWorld;
	matWorld.SetWorld( vInit, vDummy, vUp );

	return matWorld.GetQuaternion();
} //Vector3::GetQuaternion

//-------------------------------
//
//-------------------------------
void Vector3::GetQuaternion( Quaternion& qRot )
{
	Vector3		vDir;
	vDir.x = x;
	vDir.y = y;
	vDir.z = z;
	vDir.Normalize();
	vDir *= -1;

	Matrix44		matWorld;
	matWorld.SetWorld( Vector3( 0.0F, 0.0F, 0.0F ), vDir, Vector3( 0.0F, 0.0F, 1.0F ) );

	qRot = matWorld.GetQuaternion();	
} //Vector3::GetQuaternion

//-------------------------------
//
//-------------------------------
void Vector3::Absolute()
{
	x = (float)fabs( x );
	y = (float)fabs( y );
	z = (float)fabs( z );
} //Vector3::Absolute

float fX = 0.0F;
float fY = 0.0F;
float fZ = 0.0F;
float fW = 0.0F;

//-------------------------------
// MultipleMatrix
//-------------------------------
Vector3& MultipleMatrix( const Vector3& v, const Matrix44& m, Vector3& vOut )
{
	fX = v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41;
	fY = v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42;
	fZ = v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43;
	fW = v.x * m._14 + v.y * m._24 + v.z * m._34 + m._44;
	float fRHW = 1.0F / fW;	

	vOut.x = fX * fRHW;
	vOut.y = fY * fRHW;
	vOut.z = fZ * fRHW;

	return vOut;
} //MultipleMatrix

