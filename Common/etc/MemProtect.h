//------------------------------------------------------------------------
// Name:    MemProtect.h
// Author:  jjuiddong
// Date:    1/6/2013
// 
// 출처
// http://stackoverflow.com/questions/1261198/automatic-break-when-contents-of-a-memory-location-changes-or-is-read
// VirtualAlloc으로 할당한 메모리에 접근 제한을 걸고 싶을 때 쓰는 객체다.
//------------------------------------------------------------------------
#pragma once

namespace common
{
	struct protect_mem_t 
	{
		protect_mem_t( void* addr, size_t size ) : addr(addr), size(size), is_protected(FALSE) 
		{ 
			protect(); 
		}
		~protect_mem_t() { release(); }
		BOOL protect() 
		{ 
			if ( !is_protected ) 
			{
				// To catch only read access you should change PAGE_NOACCESS to PAGE_READONLY
				is_protected = VirtualProtect( addr, size, PAGE_NOACCESS, &old_protect );
			}
			return is_protected;
		}
		BOOL release() 
		{ 
			if ( is_protected ) 
				is_protected = !VirtualProtect( addr, size, old_protect, &old_protect );
			return !is_protected;
		}

	protected:
		void*   addr;
		size_t  size;
		BOOL    is_protected;
		DWORD   old_protect;
	};

}
