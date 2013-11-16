//------------------------------------------------------------------------
// Name:    Singleton.h
// Author:  jjuiddong
// Date:    2012-12-05
// 
// ΩÃ±€≈Ê ≈¨∑°Ω∫
//------------------------------------------------------------------------
#pragma once

namespace memmonitor
{
	template <class T>
	class CSingleton
	{
	public:
		static T* m_pInstance;
		static T* Get()
		{
			if (!m_pInstance)
				m_pInstance = new T;
			return m_pInstance;
		}
		static void Release()
		{
			if (m_pInstance)
			{
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}
	};
	template <class T>
	T* CSingleton<T>::m_pInstance = NULL;
}
