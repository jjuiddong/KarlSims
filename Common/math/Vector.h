//------------------------------------------------------------------------
// Name:    Vector.h
// Author:  jjuiddong
// Date:    2012-12-03
// 
// 다양한 벡터들을 정의한다.
//------------------------------------------------------------------------

#pragma once




struct Short2
{
	Short2() {}
	Short2(short _x, short _y) : x(_x), y(_y) {}
	u_short x, y;
};


struct Short3
{
	Short3() {}
	Short3(short _x, short _y, short _z) : x(_x), y(_y), z(_z) {}
	u_short x, y, z;
};


