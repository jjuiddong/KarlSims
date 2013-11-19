//------------------------------------------------------------------------
// Name:    Reference.h
// Author:  jjuiddong
// Date:    12/22/2012
// 
// ���ڷ� �Ѿ�� Ÿ���� �����͸� ��������� ������.
// 
// ����Ʈ �����Ͱ� �ƴϴ�.
//
// �ٸ� ������ �Ҵ�� �޸� �����͸� ������ �� ����ϴ� Ŭ������.
// �Ǽ��� �޸𸮸� �����ϴ� ���� �������� ���������.
//
// delete �����ڷ� ReferencePtr�� ������ �� �ִ� ������ �ִ�. 
// T* operator�� �����ؾ���
//------------------------------------------------------------------------
#pragma once

namespace common
{
	template <class T>
	class ReferencePtr
	{
	public:
		ReferencePtr():m_p(NULL) {}
		ReferencePtr(T *p) : m_p(p) {}
		ReferencePtr( const ReferencePtr<T> &rhs );
		template<class T1> ReferencePtr( const ReferencePtr<T1> &rhs );

		T* operator->() const { return m_p; }
		bool operator!() const { return (m_p)? false : true; }
		T* operator=(T *p) { m_p = p; return m_p; }
		bool operator==(T *p) const { return m_p == p; }
		bool operator!=(T *p) const { return m_p != p; }
		bool operator==(const ReferencePtr<T> &rhs) const { return m_p == rhs.m_p; }
		bool operator!=(const ReferencePtr<T> &rhs) const { return m_p != rhs.m_p; }
		operator T*() { return m_p; } // casting operator
		T* Get() const { return m_p; }

		//typedef T* this_type::*unspecified_bool_type;
		// 		operator unspecified_bool_type() const 
		// 		{ 
		// 			return (m_p == NULL)?  0 : &this_type::m_p;
		// 		} // casting operator

	private:
		//typedef ReferencePtr<T> this_type;
		T *m_p;
	};

	template<class T> 
	inline ReferencePtr<T>::ReferencePtr( const ReferencePtr<T> &rhs ) {
		m_p = rhs.m_p;
	}
	
	template<class T> template<class T1> 
	inline ReferencePtr<T>::ReferencePtr( const ReferencePtr<T1> &rhs ) {
		m_p = dynamic_cast<T*>(rhs.Get());
	}

}
