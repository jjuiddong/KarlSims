#ifndef __Math_Vector2_Header__
#define __Math_Vector2_Header__

//-----------------------------
//	Vector2
//-----------------------------
struct Vector2
{
	float x, y;
	
	// Constructor And Destructor
		Vector2();
		Vector2( float fX, float fY );
	// ~Constructor And Destructor

	// Operator 
		Vector2	operator + () const;
		Vector2	operator - () const;
		Vector2 operator + ( const Vector2& v ) const;
		Vector2 operator - ( const Vector2& v ) const;
		Vector2 operator * ( const Vector2& v ) const;
		Vector2 operator / ( const Vector2& v ) const;
		Vector2 operator + ( const float s ) const;
		Vector2 operator - ( const float s ) const;
		Vector2 operator * ( const float s ) const;
		Vector2 operator / ( const float s ) const;

		Vector2& operator += ( const Vector2& v );
		Vector2& operator -= ( const Vector2& v );
		Vector2& operator *= ( const Vector2& v );
		Vector2& operator /= ( const Vector2& v );

		Vector2& operator += ( const float s );
		Vector2& operator -= ( const float s );
		Vector2& operator *= ( const float s );
		Vector2& operator /= ( const float s );

		BOOL operator == ( const Vector2& v ) const;
		BOOL operator != ( const Vector2& v ) const;
	// ~Operator

	// Function
		float	Length() const;
		Vector2 Normal() const;
		void	Normalize();
		float	DotProduct( const Vector2& v ) const;
		float	CrossProduct( const Vector2& v ) const;
		
		Vector2 Interpolate( Vector2& v, float f ) const;

		float Distance(const Vector2 &v) const;

	// ~Function
}; //struct Vector2

#endif //#ifndef __Math_Vector2_Header__