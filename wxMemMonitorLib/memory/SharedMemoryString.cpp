
#include "stdafx.h"
#include "SharedMemoryString.h"
#include "SharedMemoryMng.h"

using namespace sharedmemory;

//------------------------------------------------------------------------
// 기본 생성자
//------------------------------------------------------------------------
shm_string::shm_string() : 
	shm_basic_string( GetAllocator<shm_string>() )
{

}


//------------------------------------------------------------------------
// 기본 생성자4
//------------------------------------------------------------------------
shm_string::shm_string(const char *str) :
	shm_basic_string( GetAllocator<shm_string>() )
{
	operator=(str);
}


//------------------------------------------------------------------------
// 복사 생성자
//------------------------------------------------------------------------
shm_string::shm_string(const shm_string &rhs) :
	shm_basic_string( GetAllocator<shm_string>() )
{
	operator=(rhs);
}


//------------------------------------------------------------------------
// 할당 연산자
//------------------------------------------------------------------------
shm_string& shm_string::operator = (const char *str)
{
	shm_basic_string::operator =(str);
	return *this;
}


//------------------------------------------------------------------------
// 할당 연산자
//------------------------------------------------------------------------
shm_string& shm_string::operator = (const shm_string &rhs)
{
	shm_basic_string::operator =(rhs);
	return *this;
}
