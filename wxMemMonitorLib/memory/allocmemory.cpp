
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "../Control/Global.h"
#include "SharedMemoryMng.h"


namespace memmonitor
{
	void*	AllocateLocal(const std::string &name, size_t size);
	bool		DeAllocateLocal(void *ptr);

}


using namespace memmonitor;

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void* memmonitor::Allocate(const std::string &name, size_t size)
{
	void *ptr = NULL;
	switch (GetExecuteType())
	{
	case INNER_PROCESS: ptr = AllocateLocal(name, size); break;
	case OUTER_PROCESS: ptr = sharedmemory::Allocate(name, size); break;
	}
	return ptr;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool memmonitor::DeAllocateMem(void *ptr)
{
	bool result = false;
	switch (GetExecuteType())
	{
	case INNER_PROCESS: result = DeAllocateLocal(ptr); break;
	case OUTER_PROCESS: result = sharedmemory::DeAllocate(ptr); break;
	}
	return result;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void* memmonitor::AllocateLocal(const std::string &name, size_t size)
{
	RETV(IsClear(), NULL);

	auto it = GetMemoryMap().find(name);
	if (GetMemoryMap().end() != it)
		return NULL; // already exist

	void *ptr = ::malloc(size);
	if (!ptr) // Error!!
		return NULL;

	GetMemoryMap().insert( MapType::value_type(name, SMemInfo(name.c_str(),ptr,0)) );
	return ptr;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool memmonitor::DeAllocateLocal(void *ptr)
{
	RETV(IsClear(), true);

	BOOST_FOREACH(auto &it, GetMemoryMap())
	{
		if (it.second.ptr == ptr)
		{
			::free(ptr);
			GetMemoryMap().erase(it.first);
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------
// enumerate memory info
//------------------------------------------------------------------------
void	memmonitor::EnumerateMemoryInfo(OUT MemoryList &memList)
{
	switch (GetExecuteType())
	{
	case INNER_PROCESS:
		{
			BOOST_FOREACH(auto &it, GetMemoryMap())
			{
				memList.push_back( SMemInfo(it.first.c_str(), it.second.ptr, 0) );
			}
		}
		break;

	case OUTER_PROCESS:
		{
			sharedmemory::MemoryList mems;
			sharedmemory::EnumerateMemoryInfo(mems);
			BOOST_FOREACH(auto &it, mems)
			{
				memList.push_back(SMemInfo(it.name.c_str(), it.ptr, it.size));
			}
		}
		break;
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	memmonitor::FindMemoryInfo(const std::string &name, OUT SMemInfo &info)
{	
	switch (GetExecuteType())
	{
	case INNER_PROCESS:
		{
			auto it = GetMemoryMap().find(name);
			if (GetMemoryMap().end() == it)
				return false;
			info = SMemInfo(name.c_str(), it->second.ptr, 0);
		}
		break;

	case OUTER_PROCESS:
		{
			sharedmemory::SMemoryInfo meminfo;
			if (!sharedmemory::FindMemoryInfo(name, meminfo))
				return false;
			info = SMemInfo(meminfo.name.c_str(), meminfo.ptr, meminfo.size);
		}
		break;
	}

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void* memmonitor::MemoryMapping(void *srcPtr )
{
	switch (GetExecuteType())
	{
	case INNER_PROCESS: return srcPtr;
	case OUTER_PROCESS: return sharedmemory::MemoryMapping(srcPtr);
	}
	return NULL;
}


//------------------------------------------------------------------------
// TypeName%Count 형태로 되어있는 타입이름을 TypeName만 리턴하게 한다.
//------------------------------------------------------------------------
std::string memmonitor::ParseObjectName(const std::string &objectName)
{
	const int offset = objectName.find('#');
	if (std::string::npos == offset)
		return objectName;
	else
		return objectName.substr(0,offset);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	memmonitor::CheckValidStringAddress(const void *ptr )
{
	if (!ptr) return false;

	switch (GetExecuteType())
	{
	case INNER_PROCESS: 
		return !IsBadStringPtrA((char*)ptr, 128);
		//return (0xCDCDCDCD == (DWORD)ptr)? false : true;
	case OUTER_PROCESS: return sharedmemory::CheckValidAddress(ptr);
	}
	return false;
}
