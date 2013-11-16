//------------------------------------------------------------------------
// Name:    SharedMemoryMng.h
// Author:  jjuiddong
// Date:    1/12/2013
// 
// �����޸𸮸� �����ϴ� Ŭ������.
//------------------------------------------------------------------------
#ifndef __SHAREDMEMORYMNG_H__
#define __SHAREDMEMORYMNG_H__

#include "SharedMemoryContainer.h"

namespace sharedmemory
{
	enum SHARED_TYPE
	{ 
		SHARED_SERVER,	// ���� �޸𸮸� �����ؼ� ����ϴ� ���μ���
		SHARED_CLIENT	// ���� �޸𸮸� �˻��ϴ� ���μ���
	};

	template<class T>
	typename shm_allocator<T>::type GetAllocator()
	{
		assert(n_pSegment);
		return shm_allocator<T>::type( n_pSegment->get_segment_manager() );
	}

	bool		Init( const std::string &name, SHARED_TYPE type, const size_t size=65536 );
	void		Release();
	void*	Allocate(const std::string &name, size_t size);
	bool		DeAllocate(void *ptr);
	void*	AllocateAnonymous(const std::string &typeName, size_t size);


	// Memory Info
	struct SMemoryInfo
	{
		std::string name;
		void *ptr;
		size_t size;
		SMemoryInfo() {}
		SMemoryInfo(const char *n, void *p, size_t s):name(n), ptr(p), size(s) {}
	};
	typedef std::list<SMemoryInfo> MemoryList;

	void		EnumerateMemoryInfo(OUT MemoryList &memList);
	bool		FindMemoryInfo(const std::string &name, OUT SMemoryInfo &info);
	void*	MemoryMapping(const void *srcPtr );
	bool		CheckValidAddress(const void *ptr );

	extern managed_shared_memory *n_pSegment;
}

#endif // __SHAREDMEMORYMNG_H__