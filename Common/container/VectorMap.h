//------------------------------------------------------------------------
// Name:    VectorMap.h
// Author:  jjuiddong
// Date:    2/28/2013
// 
// Map 과 Vector를 합쳐놓은 자료구조다.
// 랜덤 액세스 때는 Map을 이용하고, 전체를 탐색할 때는
// Vector를 이용한다.
// 
// VectorType 의 Type은 유니크한 데이터여야 한다. 같은 Type이
// 있는 경우 Vector에서 제대로 제거하지 못하게 된다.
//------------------------------------------------------------------------
#pragma once

#include "VectorHelper.h"

namespace common
{
	template <class KeyType, class Type>
	class VectorMap
	{
	public:
		typedef std::map<KeyType, Type> MapType;
		typedef std::vector<Type> VectorType;
		typedef typename MapType::iterator iterator;
		typedef typename MapType::value_type value_type;
		typedef typename VectorType::iterator viterator;

		VectorMap() {}
		VectorMap(int reserved) : m_Seq(reserved) {}

		bool insert(const value_type &vt)
		{
			// insert map
			MapType::iterator it = m_RandomAccess.find(vt.first);
			if (m_RandomAccess.end() != it)
				return false; // 이미 존재 함
			m_RandomAccess.insert( vt );

			// insert vector
			putvector(m_Seq, m_RandomAccess.size() - 1, vt.second);
			return true;
		}

		bool remove(const KeyType &key)
		{
			MapType::iterator it = m_RandomAccess.find(key);
			if (m_RandomAccess.end() == it)
				return false; // 없다면 리턴

			//removevector(it->second);
			removevector(m_Seq, it->second);
			m_RandomAccess.erase(it);
			return true;
		}

		// Type 이 유니크할 때만 사용할 수 있는 함수다.
		bool removebytype(const Type &ty)
		{
			MapType::iterator it = m_RandomAccess.begin();
			while (m_RandomAccess.end() != it)
			{
				if (ty == it->second)
				{
					//removevector(it->second);
					removevector(m_Seq, it->second);
					m_RandomAccess.erase(it);
					return true;
				}
			}
			return false; // 못찾음
		}

		bool empty()
		{
			return m_RandomAccess.empty();
		}

		void clear()
		{
			m_RandomAccess.clear();
			m_Seq.clear();
		}

		size_t size()
		{
			return m_RandomAccess.size();
		}

		void reserve(unsigned int size)
		{
			m_Seq.reserve(size);
		}

		iterator find(const KeyType &key) { return m_RandomAccess.find(key); }
		iterator begin() { return m_RandomAccess.begin(); }
		iterator end() { return m_RandomAccess.end(); }

	public:
		std::map<KeyType, Type>	m_RandomAccess;
		std::vector<Type>				m_Seq;

	};

}
