//------------------------------------------------------------------------
// Name:    SharedMemoryString.h
// Author:  jjuiddong
// Date:    1/23/2013
// 
// 공유메모리에 저장되는 스트링 클래스
//------------------------------------------------------------------------
#ifndef __SHAREDMEMORYSTRING_H__
#define __SHAREDMEMORYSTRING_H__

#include "SharedMemoryContainer.h"
#include <boost/interprocess/containers/string.hpp>	// basic_string

namespace sharedmemory
{
	using namespace boost::interprocess;

	class shm_string : public basic_string<char, std::char_traits<char>, char_allocator>
	{
		typedef  basic_string<char, std::char_traits<char>, char_allocator> shm_basic_string;

	public:
		shm_string();
		shm_string(const char *str);
		shm_string(const shm_string &rhs);
		virtual ~shm_string() {}
		shm_string& operator = (const char *str);
		shm_string& operator = (const shm_string &rhs);
	};

}

#endif // __SHAREDMEMORYSTRING_H__
