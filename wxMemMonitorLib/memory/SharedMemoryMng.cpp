
#include "stdafx.h"
#include "SharedMemoryMng.h"
#include "SharedMemoryContainer.h"
#include <boost/interprocess/containers/string.hpp>	// basic_string
#include <boost/unordered_map.hpp>     //boost::unordered_map
#include <functional>									//std::equal_to
#include <boost/functional/hash.hpp>		//boost::hash


namespace sharedmemory
{
//	using namespace boost::interprocess;

	typedef basic_string<char, std::char_traits<char>, char_allocator>		shm_basic_string;

	typedef struct _complex_data
	{
		// �����޸� �ּҴ� ���μ����� ���� �޶����Ƿ� handle�� �����ؾ� �Ѵ�.
		managed_shared_memory::handle_t handle;
		void *srcPtr;	// �޸𸮸� ������ ���μ��������� �ּ� 
								// (�ٸ� ���μ������� �˻��� �� ���δ�.)
		bool isArray;	// �迭�϶� true�� �ȴ�. �迭�� ������ �����ʹ� ù��°
								// 4byte�� �迭���̰� ����Ǿ� �ִ�. (���� ����ȵ�)
		size_t size;

		_complex_data() {}
		_complex_data(size_t s, managed_shared_memory::handle_t h, void *ptr) : size(s),handle(h),srcPtr(ptr) {}
	} complex_data;

	typedef std::pair<const shm_basic_string, complex_data>					shm_map_value_type;
	typedef allocator<shm_map_value_type, segment_manager_t>			shm_map_type_allocator;
	typedef boost::unordered_map< shm_basic_string, complex_data,
		boost::hash<shm_basic_string>, std::equal_to<shm_basic_string>,
		shm_map_type_allocator>																	shm_complex_map;

	// variables
	managed_shared_memory *n_pSegment = NULL;
	shm_complex_map *n_pMap = NULL;
	std::string n_Name;
	SHARED_TYPE n_Type;
	int	n_AnonymousCount=0;

	// functions
	bool InitSharedMemory_Server(const std::string &name, const size_t size);
	bool InitSharedMemory_Client(const std::string &name);
}


using namespace sharedmemory;

//------------------------------------------------------------------------
// �����޸� �ʱ�ȭ
//------------------------------------------------------------------------
bool sharedmemory::Init( const std::string &name, SHARED_TYPE type, const size_t size ) // size=65536
{
	n_Name = name;
	n_Type = type;
	bool result = false;

	try 
	{
		switch (type)
		{
		case SHARED_SERVER: result = InitSharedMemory_Server(name, size); break;
		case SHARED_CLIENT: result = InitSharedMemory_Client(name); break;
		}
	}
	catch (interprocess_exception &e)
	{
		std::string errorStr = e.what(); // debug��
		return false;
	}

	return result;
}


//------------------------------------------------------------------------
// ���� �޸� ����
//------------------------------------------------------------------------
void sharedmemory::Release()
{
	if (SHARED_SERVER == n_Type) // ServerType�϶��� ������ִ�.
	{
		shared_memory_object::remove(n_Name.c_str());
	}
	SAFE_DELETE(n_pSegment);
}


//------------------------------------------------------------------------
// �����޸� Ŭ���̾�Ʈ�� �ʱ�ȭ
//------------------------------------------------------------------------
bool sharedmemory::InitSharedMemory_Client(const std::string &name)
{
	n_pSegment = new managed_shared_memory(open_only, name.c_str());
	n_pMap = n_pSegment->find<shm_complex_map>("StringKeyPointerValueMap").first;
	return n_pMap? true : false;
}


//------------------------------------------------------------------------
// �����޸� ������ �ʱ�ȭ
//------------------------------------------------------------------------
bool sharedmemory::InitSharedMemory_Server(const std::string &name, const size_t size)
{
	shared_memory_object::remove(name.c_str());
	n_pSegment = new managed_shared_memory(create_only, name.c_str(),  size);
	n_pMap = n_pSegment->construct<shm_complex_map>("StringKeyPointerValueMap")
		(10, boost::hash<shm_basic_string>(), std::equal_to<shm_basic_string>(), 
		n_pSegment->get_allocator<shm_map_value_type>() );

	return true;
}


//------------------------------------------------------------------------
// �޸𸮸� �Ҵ��ؼ� �����Ѵ�.
// map�� �Ҵ���� ��ü�� �̸��� �ּҸ� �����Ѵ�.
// name : �ߺ����� ���� ����ũ�� �̸��̾�� �Ѵ�. �ߺ��� �̸��� �����ϸ� ����
//------------------------------------------------------------------------
void* sharedmemory::Allocate(const std::string &name, size_t size)
{
	RETV(!n_pSegment, NULL);
	RETV(!n_pMap, NULL);

	// ���� Map�� ����Ÿ�� ã������ �����޸𸮸� �̿��ϴ� ��� �ۿ� ����.
	shm_basic_string str( n_pSegment->get_allocator<shm_basic_string>() );
	str = name.c_str();
	shm_complex_map::iterator it = n_pMap->find( str );
	if (n_pMap->end() != it)
		return NULL; // �̹� �����Ѵٸ� ����

	void *ptr = n_pSegment->allocate(size, std::nothrow_t());
	RETV(!ptr, NULL);

	// �����޸� �ּҴ� ���μ����� ���� �޶����Ƿ� handle�� �����ؾ� �Ѵ�.
	managed_shared_memory::handle_t handle = n_pSegment->get_handle_from_address(ptr);

	n_pMap->insert( shm_map_value_type(str,complex_data(size,handle,ptr)) );
	return ptr;
}


//------------------------------------------------------------------------
// CSharedMem<T>�� ��ġ�� �ʰ�, �������� �Ҵ��Ҷ� ���̴� �Լ���.
// �����ڴ� ȣ������ �ʰ�, �޸𸮸� ���ϵȴ�.
//------------------------------------------------------------------------
void* sharedmemory::AllocateAnonymous(const std::string &typeName, size_t size)
{
	++n_AnonymousCount;
	std::stringstream ss;
	ss << "Anonymous@" << typeName << "#" << size;
	return Allocate(ss.str(), size);
}


//------------------------------------------------------------------------
// �޸𸮸� �ݳ��Ѵ�.
//------------------------------------------------------------------------
bool sharedmemory::DeAllocate(void *ptr)
{
	RETV(!n_pSegment, false);
	RETV(!n_pMap, false);

	const managed_shared_memory::handle_t handle = n_pSegment->get_handle_from_address(ptr);

	shm_complex_map::iterator it = n_pMap->begin();
	while (n_pMap->end() != it)
	{
		if (it->second.handle == handle)
		{
			n_pSegment->deallocate(ptr);
			n_pMap->erase(it);
			break;
		}
		++it;
	}
	return true;
}


//------------------------------------------------------------------------
// n_pMap�� ����� ������ �����Ѵ�.
//------------------------------------------------------------------------
void sharedmemory::EnumerateMemoryInfo(OUT MemoryList &memList)
{
	RET(!n_pSegment);
	RET(!n_pMap);

	shm_complex_map::iterator it = n_pMap->begin();
	while (n_pMap->end() != it)
	{
		void *ptr = n_pSegment->get_address_from_handle(it->second.handle);
		memList.push_back( SMemoryInfo(it->first.c_str(), ptr, it->second.size) );
		++it;
	}
}


//------------------------------------------------------------------------
// name�� �ش��ϴ� ������ �����޸𸮿��� ã�Ƽ� �����Ѵ�.
//------------------------------------------------------------------------
bool sharedmemory::FindMemoryInfo(const std::string &name, OUT SMemoryInfo &info)
{
	RETV(!n_pSegment, false);
	RETV(!n_pMap, false);

	// ���� Map�� ����Ÿ�� ã������ �����޸𸮸� �̿��ϴ� ����ۿ� ����.
	shm_basic_string str( n_pSegment->get_allocator<shm_basic_string>() );
	str = name.c_str();
	shm_complex_map::iterator it = n_pMap->find( str );
	if (n_pMap->end() == it)
		return false; // ���ٸ� ����
	
	void *ptr = n_pSegment->get_address_from_handle(it->second.handle);
	info = SMemoryInfo(it->first.c_str(), ptr, it->second.size);
	return true;
}


//------------------------------------------------------------------------
// �޸𸮸� ������ ���μ��� �ּ� srcPtr�� Ÿ�� ���μ�����
// �޸� �ּҷ� ���ν��� �����Ѵ�.
//------------------------------------------------------------------------
void* sharedmemory::MemoryMapping(const void *srcPtr )
{
	RETV(!n_pSegment, NULL);
	RETV(!n_pMap, NULL);

	shm_complex_map::iterator it = n_pMap->begin();
	while (n_pMap->end() != it)
	{
		if (it->second.srcPtr == srcPtr)
		{
			void *ptr = n_pSegment->get_address_from_handle(it->second.handle);
			return ptr;
		}
		++it;
	}
	return NULL;
}


//------------------------------------------------------------------------
// ptr �� �����޸𸮸� ����Ų�ٸ� true�� �����Ѵ�.
//------------------------------------------------------------------------
bool	sharedmemory::CheckValidAddress(const void *ptr )
{
	RETV(!n_pSegment, false);
	RETV(!n_pMap, false);

	void *shm_ptr = n_pSegment->get_address();
	const size_t size = n_pSegment->get_size();
	return (shm_ptr <= ptr) && (((char*)shm_ptr + size) > ptr);
}

