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

	Short2 operator - ( const Short2& v ) const {
		return Short2(x-v.x, y-v.y);
	}

	Short2 operator + ( const Short2& v ) const {
		return Short2(x+v.x, y+v.y);
	}

	Short2 operator * ( const Short2& v ) const {
		return Short2(x*v.x, y*v.y);
	}

	Short2 operator / ( const Short2& v ) const {
		return Short2(x/v.x, y/v.y);
	}

	Short2 operator * ( const u_short& v ) const {
		return Short2(x*v, y*v);
	}

	Short2 operator / ( const u_short& v ) const {
		return Short2(x/v, y/v);
	}

	Short2& operator -= ( const Short2& v ) {
		if (this != &v)
		{
			x-=v.x;
			y-=v.y;
		}
		return *this;
	}

	Short2& operator += ( const Short2& v ) {
		if (this != &v)
		{
			x+=v.x;
			y+=v.y;
		}
		return *this;
	}

	Short2& operator *= ( const Short2& v ) {
		if (this != &v)
		{
			x*=v.x;
			y*=v.y;
		}
		return *this;
	}

	Short2& operator /= ( const Short2& v ) {
		if (this != &v)
		{
			x/=v.x;
			y/=v.y;
		}
		return *this;
	}

	Short2& operator *= ( const u_short& v ) {
		x*=v;
		y*=v;
		return *this;
	}

	Short2& operator /= ( const u_short& v ) {
		x/=v;
		y/=v;
		return *this;
	}

	bool operator==(const Short2 &v) {
		return (x == v.x) && (y == v.y);
	}

	bool operator!=(const Short2 &v) {
		return (x != v.x) && (y != v.y);
	}

	u_short x, y;
};


struct Short3
{
	Short3() {}
	Short3(short _x, short _y, short _z) : x(_x), y(_y), z(_z) {}
	u_short x, y, z;
};



struct Int2
{
	Int2() {}
	Int2(int _x, int _y) : x(_x), y(_y) {}

	Int2 operator - ( const Int2& v ) const {
		return Int2(x-v.x, y-v.y);
	}

	Int2 operator + ( const Int2& v ) const {
		return Int2(x+v.x, y+v.y);
	}

	Int2 operator * ( const Int2& v ) const {
		return Int2(x*v.x, y*v.y);
	}

	Int2 operator / ( const Int2& v ) const {
		return Int2(x/v.x, y/v.y);
	}

	Int2 operator * ( const int& v ) const {
		return Int2(x*v, y*v);
	}

	Int2 operator / ( const int& v ) const {
		return Int2(x/v, y/v);
	}

	Int2& operator -= ( const Int2& v ) {
		if (this != &v)
		{
			x-=v.x;
			y-=v.y;
		}
		return *this;
	}

	Int2& operator += ( const Int2& v ) {
		if (this != &v)
		{
			x+=v.x;
			y+=v.y;
		}
		return *this;
	}

	Int2& operator *= ( const Int2& v ) {
		if (this != &v)
		{
			x*=v.x;
			y*=v.y;
		}
		return *this;
	}

	Int2& operator /= ( const Int2& v ) {
		if (this != &v)
		{
			x/=v.x;
			y/=v.y;
		}
		return *this;
	}

	Int2& operator *= ( const int& v ) {
		x*=v;
		y*=v;
		return *this;
	}

	Int2& operator /= ( const int& v ) {
		x/=v;
		y/=v;
		return *this;
	}

	bool operator==(const Int2 &v) const {
		return (x == v.x) && (y == v.y);
	}

	bool operator!=(const Int2 &v) const {
		return (x != v.x) && (y != v.y);
	}

	int x, y;
};
