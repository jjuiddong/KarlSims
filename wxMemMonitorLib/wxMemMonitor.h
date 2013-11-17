//------------------------------------------------------------------------
// Name:    wxMemMonitor.h
// Author:  jjuiddong
// Date:    3/5/2013
// 
// memory monitor library header file
//------------------------------------------------------------------------
#pragma once

#define _AFX_SECURE_NO_WARNINGS 
#define _ATL_SECURE_NO_WARNINGS

#pragma warning (disable: 4996)	// strcpy ��� ����

#include <list>
#include <string>
#include <sstream>
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


// using memory monitor inner process
#define MEMORYMONITOR_INNER_PROCESS()			\
	DECLARE_APP(memmonitor::CApp)							\
	IMPLEMENT_APP_NO_MAIN(memmonitor::CApp)	


// generate memory monitor execute file
#define MEMORYMONITOR_OUTER_PROCESS()		\
	DECLARE_APP(memmonitor::CApp)						\
	IMPLEMENT_APP(memmonitor::CApp)


namespace memmonitor
{
	enum EXECUTE_TYPE
	{
		INNER_PROCESS,		// using inner process memory
		OUTER_PROCESS,		// using shared memory
	};

	bool Init(EXECUTE_TYPE type, HINSTANCE hInst, const std::string configFileName);
	bool ShowToggle();
	void Cleanup();
	
	// Error Report
	const std::string& GetLastError();




	
	

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	// MEMORY FUNCTION
	
	/// Memory Info
	struct SMemInfo
	{
		std::string name;
		void *ptr;
		size_t size;
		SMemInfo() {}
		SMemInfo(const char *n, void *p, size_t s):name(n), ptr(p), size(s) {}
	};
	typedef std::list<SMemInfo> MemoryList;


	void*	Allocate(const std::string &name, size_t size);
	bool		DeAllocateMem(void *ptr);
	void		EnumerateMemoryInfo(OUT MemoryList &memList);	
	bool		FindMemoryInfo(const std::string &name, OUT SMemInfo &info);
	void*	MemoryMapping(void *srcPtr );
	std::string ParseObjectName(const std::string &objectName);
	bool		CheckValidAddress(const void *ptr );

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	// operator new, delete overloading template class
	template<class T, class typeName> // typeName = DECLARE_TYPE_NAME() ��ũ�θ� ���� �������
	class Monitor
	{
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
			DeAllocateMem(ptr);
		}
		void operator delete (void *ptr, char* lpszFileName, int nLine)
		{
			DeAllocateMem(ptr);
		}
		void operator delete (void *ptr, char *attachTypeName)
		{
			DeAllocateMem(ptr);
		}
		void operator delete[] (void *ptr)
		{
			DeAllocateMem(ptr);
		}

	private:
		static void* Allocate(const size_t size, char *attachTypeName=NULL)
		{
			std::stringstream ss;
			ss << typeName::typeName() << "#";
			if (attachTypeName)
				ss << attachTypeName;
			ss << ++m_Count;
			return memmonitor::Allocate(ss.str(), size);
		}

		static int m_Count;
	};

	template<class T, class typeName>
	int Monitor<T,typeName>::m_Count=0;



	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	// Sub System

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	// App
	class CApp : public wxApp
	{
	public:
		CApp();
		virtual bool OnInit();
		virtual void ExitMainLoop();
		void OnTerminate(wxThreadEvent& event);
	};

}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// MACRO

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

