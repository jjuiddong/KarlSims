//------------------------------------------------------------------------
// Name:    ShareMemoryContainer.h
// Author:  jjuiddong
// Date:    1/22/2013
// 
// �ν�Ʈ �����޸� ���̺귯���� ����ϴ� Ŭ��������
// �ʿ��� ���� ������� ��Ƴ���
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

