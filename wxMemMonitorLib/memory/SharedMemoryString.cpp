
#include "stdafx.h"
#include "SharedMemoryString.h"
#include "SharedMemoryMng.h"

using namespace sharedmemory;

//------------------------------------------------------------------------
// �⺻ ������
//------------------------------------------------------------------------
shm_string::shm_string() : 
	shm_basic_string( GetAllocator<shm_string>() )
{

}


//------------------------------------------------------------------------
// �⺻ ������4
//------------------------------------------------------------------------
shm_string::shm_string(const char *str) :
	shm_basic_string( GetAllocator<shm_string>() )
{
	operator=(str);
}


//------------------------------------------------------------------------
// ���� ������
//------------------------------------------------------------------------
shm_string::shm_string(const shm_string &rhs) :
	shm_basic_string( GetAllocator<shm_string>() )
{
	operator=(rhs);
}


//------------------------------------------------------------------------
// �Ҵ� ������
//------------------------------------------------------------------------
shm_string& shm_string::operator = (const char *str)
{
	shm_basic_string::operator =(str);
	return *this;
}


//------------------------------------------------------------------------
// �Ҵ� ������
//------------------------------------------------------------------------
shm_string& shm_string::operator = (const shm_string &rhs)
{
	shm_basic_string::operator =(rhs);
	return *this;
}
