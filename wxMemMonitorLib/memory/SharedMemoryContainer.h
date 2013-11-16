//------------------------------------------------------------------------
// Name:    ShareMemoryContainer.h
// Author:  jjuiddong
// Date:    1/22/2013
// 
// 부스트 공유메모리 라이브러리를 사용하는 클래스에서
// 필요한 공통 선언들을 모아놓음
//------------------------------------------------------------------------
#ifndef __SHAREDMEMORYCONTAINER_H__
#define __SHAREDMEMORYCONTAINER_H__

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

namespace sharedmemory
{
	using namespace boost::interprocess;

	typedef managed_shared_memory::segment_manager	segment_manager_t;

	template<class T>
	struct shm_allocator {
		typedef typename allocator<T, segment_manager_t> type;
	};

	typedef shm_allocator<char>::type char_allocator;


}

#endif // __SHAREDMEMORYCONTAINER_H__

