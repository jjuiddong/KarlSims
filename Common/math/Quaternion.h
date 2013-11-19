//-------------------------------------------------------
//	Quaternion.h
//
//	Author	:	Joung Man, Kim
//	Comment	:
//				
//
//	Copyright(C) 2005 by Sunnyzen all right reserved
//-------------------------------------------------------

#ifndef __Math_Quaternion_Header__
#define __Math_Quaternion_Header__

//--------------------------------
//
//--------------------------------
struct Quaternion
{
	float	x, y, z, w;

	//	Constructor And Destructor
		Quaternion();
		Quaternion( const float fX, const float fY, const float fZ, const float fW );
		Quaternion( const Vector3& vAxis, const float fAngle );
		Quaternion( const Vector3& vDir1, const Vector3& vDir2 );
	//	~Constructor And Destructor

	// Operator
		Quaternion&		operator *= ( const Quaternion& q )
		{
			*this = *this * q;
			return *this;
		} //operator *=

		Quaternion		operator * ( const Quaternion& q )
		{
			return Quaternion(
				 x * q.w + y * q.z - z * q.y + w * q.x,
				-x * q.z + y * q.w + z * q.x + w * q.y,
				 x * q.y - y * q.x + z * q.w + w * q.z,
				-x * q.x - y * q.y - z * q.z + w * q.w );
		} //operator *

		Quaternion		operator * ( const Matrix44& m )
		{
			return *this * m.GetQuaternion();
		} //operator *
	// ~Operator
	
	// Function
		const Quaternion&	Interpolate( const Quaternion& qNext, const float fTime ) const;
		const Matrix44&		GetMatrix() const;
		Vector3				GetDirection() const;
	// ~Function

	// Rotate
		void	SetRotationX( const float fRadian );
		void	SetRotationY( const float fRadian );
		void	SetRotationZ( const float fRadian );
		void	SetOrientation( const Vector3& vDir, const Vector3& vUp );
		void	SetRotationArc( const Vector3& vDir, const Vector3& vDir2 );
	// ~Rotate

		void	Normalize();
}; //struct Quaternion


#endif //#ifndef __Math_Quaternion_Header__