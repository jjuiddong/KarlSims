//-------------------------------------------------------
//	Math.h
//-------------------------------------------------------

#ifndef __Math_Header__
#define __Math_Header__

// Include
//	#include "stdafx.h"
#include "../Common.h"
	#include <d3dx9.h>
	#include <windows.h>
	#include <math.h>
	#include <time.h>
// ~Include

// Const
	const float MATH_PI = 3.14159265358979323846F;
	const float MATH_EPSILON = 0.0005F;//1.0e-5F;
// ~Const

	typedef unsigned short u_short;


// Macro
	#define random( m_posX )		(int)( ( (long)rand() * (m_posX) ) / ( RAND_MAX + 1 ) )
	#define randomize()		srand( (unsigned)time( NULL ) )

	#define ABS( m_posX )	( (m_posX) >= 0 ? (m_posX) : -(m_posX) )

	#define MAX( a, b )	( (a) > (b) ? (a) : (b) )
	#define MIN( a, b )	( (a) < (b) ? (a) : (b) )
	#define GET_MARK( a ) ( (a) < 0 ? 0 : 1 )

	#define ANGLE( m_posX )	( (m_posX) * MATH_PI / 180.0F )	// angle to radian
	#define DEGREE( m_posX ) ( (m_posX) * 180 / MATH_PI )		// radian to angle
	#define ANGLETORAD( m_posX ) (ANGLE( m_posX ))
	#define RADTOANGLE( m_posX ) (DEGREE( m_posX ))

	#define	SQR(m_posX)		( (m_posX) * (m_posX) )

	// limits a value to low and high
	#define LIMIT_RANGE(low, value, high)	{	if (value < low)	value = low;	else if(value > high)	value = high;	}
	#define FLOAT_EQ(m_posX,v)	( ((v) - MATH_EPSILON) < (m_posX) && (m_posX) < ((v) + MATH_EPSILON) )
	__forceinline void SWAP(float &x, float &y) {	float temp;	temp = x;	x = y;	y = temp;	}

// ~Macro

// Inline Function
	int GetRand( int x, int add = 0 );

	// 가까운 2의 승수 구하기.
	unsigned __fastcall CeilPowerOf2(unsigned x);
// ~Inline Function
	
// Structure
	struct Vector2;		// 2 Axis 
	struct Vector3;		// 3 Axis
	struct Matrix44;	// 4 Row
	struct Quaternion;	// 
	struct Triangle;	// 
	struct SBezier;		//
	struct Line2;
	class CFrustum;
// ~Structure

// Include
	#include "Vector.h"	
	#include "Vector2.h"
	#include "Vector3.h"
	#include "Matrix.h"	
	#include "Quaternion.h"
	#include "triangle.h"
	#include "plane.h"	
	#include "box.h"
	#include "sphere.h"

//	#include "Inc/Interpolate.h"
//	#include "Inc/Triangle.h"
//	#include "Inc/Plane.h"
//	#include "Inc/Collision.h"	
//	#include "Inc/Curve.h"
//	#include "Inc/Frustum.h"
//	#include "Inc/Line.h"
// ~Include


#endif //#ifndef __Math_Header__
