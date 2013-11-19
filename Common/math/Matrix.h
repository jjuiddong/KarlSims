//-------------------------------------------------------
//	Matrix44.h
//-------------------------------------------------------

#ifndef __Math_Matrix44_Header__
#define __Math_Matrix44_Header__

//-----------------------------
// 4 x 4 Matrix
//-----------------------------
struct Matrix44
{
	// union
		union
		{
			struct
			{
				float	_11, _12, _13, _14;
				float	_21, _22, _23, _24;
				float	_31, _32, _33, _34;
				float	_41, _42, _43, _44;
			}; //struct
			float	m[4][4];
		}; //union
	// ~union

	// Constructor
		Matrix44();
		Matrix44(float	_m11, float _m12, float _m13, float _m14,
				 float	_m21, float _m22, float _m23, float _m24,
				 float	_m31, float _m32, float _m33, float _m34,
				 float	_m41, float _m42, float _m43, float _m44);

	// ~Constructor

	// operator
		Matrix44		operator * ( const Matrix44& m ) const;
		Matrix44		operator * ( const float f );
		Matrix44&	operator *= ( const Matrix44& m );
		const float& operator () ( const int nRow, const int nColumn );
	// ~operator

	// Function
		Matrix44	Inverse() const;
		Quaternion GetQuaternion() const;

		void		SetIdentity();
		void		SetRotationX( const float fRadian );
		void		SetRotationY( const float fRadian );
		void		SetRotationZ( const float fRadian );
		void		SetRotation( const Vector3& vINAxis, const float fAngle );

		void		SetView( const Vector3& vPos, const Vector3& vINDir, const Vector3& vINUp );
		void		SetProjection( const float fFOV, const float fAspect, const float fNearPlane, const float fFarPlane );

		// Set World
			void		SetWorld( const Vector3& vPos );
			void		SetWorld( const Vector3& vPos, const Quaternion& qRot );
			void		SetWorld( const Vector3& vPos, const Vector3& vDir, const Vector3& vUp );
		// ~Set World

		//void		SetPosition( const Vector3& vPos );
		void		Translate( const Vector3& vPos );
		void		SetScale( const Vector3& vScale);

		Vector3		GetPosition();

		Matrix44		GetBillboard() const;
		Matrix44		GetBillboardX() const;
		Matrix44		GetBillboardZ() const;
	// ~Function
}; //struct Matrix44

// Multiply
void Matrix44Multiply( Matrix44& matA, Matrix44& matB, Matrix44& matOut );


#endif //#ifndef __Math_Matrix44_Header__
