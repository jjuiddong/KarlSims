/**
Name:   AutoLock.h
Author:  jjuiddong
Date:    3/25/2013

*/
#pragma once

namespace common
{
	/// Auto Lock, Unlock
	template<class T>
	class AutoLock
	{
	public:
		AutoLock(T& t) : m_t(t) { m_t.Lock(); }
		~AutoLock() { m_t.Unlock(); }
	protected:
		T &m_t;
	};


	/// Critical Section auto initial and delete
	class CriticalSection
	{
	public:
		CriticalSection();
		~CriticalSection();
		void Lock();
		void Unlock();
	protected:
		CRITICAL_SECTION m_cs;
	};

	inline CriticalSection::CriticalSection() { 
		InitializeCriticalSection(&m_cs); 
	}
	inline CriticalSection::~CriticalSection() { 
		DeleteCriticalSection(&m_cs); 
	}
	inline void CriticalSection::Lock() {
		EnterCriticalSection(&m_cs);
	}
	inline void CriticalSection::Unlock() {
		LeaveCriticalSection(&m_cs);
	}

	
	/// auto critical section lock, unlock
	class AutoCSLock : protected AutoLock<CriticalSection>
	{
	public:
		AutoCSLock(CriticalSection &cs);
	};

	inline AutoCSLock::AutoCSLock(CriticalSection &cs) : AutoLock(cs) { }


}
