//------------------------------------------------------------------------
// Name:    SharedMemoryList.h
// Author:  jjuiddong
// Date:    1/23/2013
// 
// 공유메모리에 사용하는 리스트
//------------------------------------------------------------------------
#ifndef __SHAREDMEMORYLIST_H__
#define __SHAREDMEMORYLIST_H__

#include "SharedMemoryContainer.h"
#include "SharedMemoryMng.h"
#include <boost/interprocess/containers/list.hpp>

namespace sharedmemory
{
	using namespace boost::interprocess;

	template <class T>
	class shm_list : public list<T, typename shm_allocator<T>::type >
	{
		typedef list<T, typename shm_allocator<T>::type > basic_list;
	public:
		shm_list<T>() : basic_list( GetAllocator<T>() ) {}
		shm_list<T>(const shm_list<T> &rhs)  : basic_list( GetAllocator<T>() ) {
			basic_list::operator =(rhs);
		}
		virtual ~shm_list<T>()  {}
		shm_list<T>& operator = (const shm_list<T> &rhs) {
			basic_list::operator =(rhs);
		}
	};

}

#endif // __SHAREDMEMORYLIST_H__
