//------------------------------------------------------------------------
// Name:    SharedMemory.h
// Author:  jjuiddong
// Date:    1/12/2013
// 
// CShareMem Ŭ������ ��ӹ޾Ƽ� �����޸� �Ҵ�(new)�� ������,
// �����޸𸮿� ��ü�� �����ȴ�. (CRTP ����)
// Boost shared memory ���̺귯���� �̿���
//
// 1. CSharedMem Ŭ���� ��ӹ޴� ��
//
// DECLARE_TYPE_NAME(CCustomClass);
// class CCustomClass : public sharedmemory::CSharedMem<CCustomClass, TYPE_NAME(CCustomClass)>
// {
// ...
// }
//
//
// 2. namespace �� Ŭ���� ���ο� ����� ��ü�� ��ӹ޴� ��
//
// DECLARE_TYPE_NAME_SCOPE(NameSpace, CCustomClass);
// class CCustomClass : public sharedmemory::CSharedMem<CCustomClass, TYPE_NAME(CCustomClass)>
// {
// ...
// }

//------------------------------------------------------------------------
#ifndef __SHAREDMEMORY_H__
#define __SHAREDMEMORY_H__

#include "SharedMemoryMng.h"


#define DECLARE_TYPE_NAME(className)					\
struct className##typeNameWrap								\
{																						\
	static char* typeName() { return #className; }		\
};

#define DECLARE_TYPE_NAME_SCOPE(scope, className)				\
struct className##typeNameWrap													\
{																											\
	static char* typeName() { return #scope"::"#className; }			\
};

#define TYPE_NAME(className)	className##typeNameWrap


namespace sharedmemory
{
	template<class T, class typeName> // typeName = DECLARE_TYPE_NAME() ��ũ�θ� ���� �������
	class CSharedMem
	{
	protected:
		static int m_Count;

	public:
		void* operator new (size_t size)
		{
			return Allocate(size);
		}
		//attachTypeName : typeName�� �߰������� �ٰԵǴ� �̸�
		// ȣ�� ���: Type *p = new ("attachTypeName") Type();
		void* operator new (size_t size, char *attachTypeName)
		{
			// Name = typeName + # + attachTypeName + m_Count
			return Allocate(size, attachTypeName);
		}
		void* operator new[] (size_t size)
		{
			return Allocate(size);
		}

		// Debug new
		void* operator new (size_t size, char* lpszFileName, int nLine)
		{
			return Allocate(size);
		}
		// Debug new
		void* operator new[] (size_t size, char* lpszFileName, int nLine)
		{
			return Allocate(size);
		}

		void operator delete (void *ptr)
		{
			if (!sharedmemory::DeAllocate(ptr))
				free(ptr);
		}
		void operator delete (void *ptr, char *attachTypeName)
		{
			if (!sharedmemory::DeAllocate(ptr))
				free(ptr);
		}
		void operator delete[] (void *ptr)
		{
			if (!sharedmemory::DeAllocate(ptr))
				free(ptr);
		}

	private:
		static void* Allocate(const size_t size, char *attachTypeName=NULL)
		{
			std::stringstream ss;
			ss << typeName::typeName() << "#";
			if (attachTypeName)
				ss << attachTypeName;
			ss << ++m_Count;
			void *ptr = sharedmemory::Allocate(ss.str(), size);
			if (!ptr)
				ptr = malloc(size);
			return ptr;
		}
	};

	template<class T, class typeName>
	int CSharedMem<T,typeName>::m_Count=0;
}


#endif // __SHAREDMEMORY_H__