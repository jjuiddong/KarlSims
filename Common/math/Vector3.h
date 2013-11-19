//-------------------------------------------------------
//	Vector3.h
//-------------------------------------------------------

#ifndef __Math_Vector3_Header__
#define __Math_Vector3_Header__

//-----------------------------
//	Vector3
//-----------------------------
struct Matrix44;

struct Vector3
{
	float x, y, z;
	
	Vector3();
	Vector3( float fX, float fY, float fZ );

	// Operator 
		Vector3 operator + () const;
		Vector3 operator - () const;
		Vector3 operator + ( const Vector3& v ) const;
		Vector3 operator - ( const Vector3& v ) const;
		Vector3 operator * ( Vector3& v ) const;
		Vector3 operator / ( Vector3& v ) const;
		Vector3 operator + ( float s ) const;
		Vector3 operator - ( float s ) const;
		Vector3 operator * ( float s ) const;
		Vector3 operator / ( float s ) const;

		Vector3 operator * ( const Matrix44& m );

		Vector3& operator += ( Vector3& v );
		Vector3& operator -= ( Vector3& v );
		Vector3& operator *= ( Vector3& v );
		Vector3& operator /= ( Vector3& v );

		Vector3& operator += ( float s );
		Vector3& operator -= ( float s );
		Vector3& operator *= ( float s );
		Vector3& operator /= ( float s );

		Vector3& operator *= ( Matrix44& m );

		BOOL operator == ( const Vector3& v ) const;
		BOOL operator != ( const Vector3& v ) const;
	// ~Operator

	// Function
		Vector3 Normal() const;
		void	Normalize();
		void	Absolute();

		BOOL	EqualEpsilon( const float epsilon, const Vector3 &v ) const;

		float	Length();
		float	LengthRoughly( Vector3& v ) const;
		float	Distance(const Vector3& v);

		float	DotProduct( const Vector3& v ) const;
		Vector3	CrossProduct( const Vector3& v ) const;

		Vector3 Interpolate( Vector3& v, float f ) const;
	// ~Function

	// Rotate
		void	RotateX( float fAngle );
		void	RotateY( float fAngle );
		void	RotateZ( float fAngle );
		void	Rotate( Vector3& vAxis, float fAngle );
	// ~Rotate

	// Quaternion
		Quaternion	GetQuaternion();
		void	GetQuaternion( Quaternion& qRot );
	// ~Quaternion
}; //struct Vector3


//MultipleMatrix
Vector3& MultipleMatrix( const Vector3& vIn, const Matrix44& m, Vector3& vOut );

#endif //#ifndef __Math_Vector3_Header__
