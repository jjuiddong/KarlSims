
// Include 
	#include "stdafx.h"
	#include "Math.h"
// ~Include

//-------------------------------
// Constructor
//-------------------------------
Vector2::Vector2()
:	x ( 0.0F ),
	y ( 0.0F )
{
} //Vector2::Vector2

Vector2::Vector2( float fX, float fY )
:	x ( fX ),
	y ( fY )
{
} //Vector2::Vector2

//-------------------------------
// operator +
//-------------------------------
Vector2	Vector2::operator + () const
{
	return Vector2( x, y );
} //operator + () const

//-------------------------------
// operator -
//-------------------------------
Vector2	Vector2::operator - () const
{
	return Vector2( -x, -y );
} //operator - () const

//-------------------------------
// operator +
//-------------------------------
Vector2 Vector2::operator + ( const Vector2& v ) const
{
	return Vector2( x + v.x, y + v.y );
} //operator + 

//-------------------------------
// operator -
//-------------------------------
Vector2 Vector2::operator - ( const Vector2& v ) const
{
	return Vector2( x - v.x, y - v.y );
} //operator -

//-------------------------------
// operator *
//-------------------------------
Vector2 Vector2::operator * ( const Vector2& v ) const
{
	return Vector2( x * v.x, y * v.y );
} //operator

//-------------------------------
// operator /
//-------------------------------
Vector2 Vector2::operator / ( const Vector2& v ) const
{
	return Vector2( x / v.x, y / v.y );
} //operator /

//-------------------------------
// operator +
//-------------------------------
Vector2 Vector2::operator + ( const float s ) const
{
	return Vector2( x + s, y + s );
} //operator +

//-------------------------------
// operator -
//-------------------------------
Vector2 Vector2::operator - ( const float s ) const
{
	return Vector2( x - s, y - s );
} //operator -

//-------------------------------
// operator *
//-------------------------------
Vector2 Vector2::operator * ( const float s ) const
{
	return Vector2( x * s, y * s );
} //operator *

//-------------------------------
// operator /
//-------------------------------
Vector2 Vector2::operator / ( const float s ) const
{
	return Vector2( x / s, y / s );
} //operator /

//-------------------------------
// operator +
//-------------------------------
Vector2& Vector2::operator += ( const Vector2& v )
{
	x += v.x;
	y += v.y;
	return *this;
} //operator +=

//-------------------------------
// operator -
//-------------------------------
Vector2& Vector2::operator -= ( const Vector2& v )
{
	x -= v.x;
	y -= v.y;
	return *this;
} //operator -=

//-------------------------------
// operator *
//-------------------------------
Vector2& Vector2::operator *= ( const Vector2& v )
{
	x *= v.x;
	y *= v.y;

	return *this;
} //operator *=

//-------------------------------
// operator /
//-------------------------------
Vector2& Vector2::operator /= ( const Vector2& v )
{
	x /= v.x;
	y /= v.y;

	return *this;
} //operator /=

//-------------------------------
// operator +
//-------------------------------
Vector2& Vector2::operator += ( const float s )
{
	x += s;
	y += s;

	return *this;
} //operator +=

//-------------------------------
// operator -
//-------------------------------
Vector2& Vector2::operator -= ( const float s )
{
	x -= s;
	y -= s;

	return *this;
} //operator -=

//-------------------------------
// operator *
//-------------------------------
Vector2& Vector2::operator *= ( const float s )
{
	x *= s;
	y *= s;

	return *this;
} //operator *=

//-------------------------------
// operator /
//-------------------------------
Vector2& Vector2::operator /= ( const float s )
{
	x /= s;
	y /= s;

	return *this;
} //operator /=

//-------------------------------
// operator ==
//-------------------------------
BOOL Vector2::operator == ( const Vector2& v ) const
{
	return ( x == v.x ) && ( y == v.y );
} //operator ==

//-------------------------------
// operator !=
//-------------------------------
BOOL Vector2::operator != ( const Vector2& v ) const
{
	return ( x != v.x ) || ( y != v.y );
} //operator !=

//-------------------------------
//
//-------------------------------
float Vector2::Length() const
{
	return sqrtf( x * x + y * y );
} //Vector2::Length

//-------------------------------
//
//-------------------------------
Vector2 Vector2::Normal() const
{
	return *this / sqrtf( x * x + y * y );
} //Vector2::Normal

//-------------------------------
//
//-------------------------------
void Vector2::Normalize()
{
	*this /= sqrtf( x * x + y * y );
} //Vector2::Normalize

//-------------------------------
//
//-------------------------------
float Vector2::DotProduct( const Vector2& v ) const
{
	return x * v.x + y * v.y;
} //Vector2::DotProduct

//-------------------------------
//
//-------------------------------
float Vector2::CrossProduct( const Vector2& v ) const
{
	return x * v.y - y * v.x;
} //Vector2::CrossProduct

//-------------------------------
//
//-------------------------------
Vector2 Vector2::Interpolate( Vector2& v, float f ) const
{	
	return *this;
} //Vector2::Interpolate


float Vector2::Distance(const Vector2 &v) const
{
	float tx = this->x - v.x;
	float ty = this->y - v.y;
	return sqrtf(tx*tx + ty*ty);
}
