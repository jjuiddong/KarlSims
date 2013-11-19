/**
Name:   StableVectorMap.h
Author:  jjuiddong
Date:    4/28/2013

	VectorMap에 GarbageCollector, Remove Safe 기능을 추가했다.

	- 아직 GarbageCollector 는 구현하지 못했다.
*/
#pragma once

#include "VectorMap.h"

namespace common
{
	template <class KeyType, class Type>
	class StableVectorMap : public VectorMap<KeyType, Type>
	{
	public:
		typedef std::set<KeyType> RemoveKeys;

		StableVectorMap() {}
		StableVectorMap(int reserved) : VectorMap(reserved) {}


		/**
		 @brief insert 전에 제거 목록을 먼저 검사한다.
		 */
		bool insert(const value_type &vt)
		{
			apply_removes();
			return VectorMap::insert(vt);
		}


		/**
		 @brief 제거할 값을 등록한다.
		 */
		bool remove(const KeyType &key)
		{
			m_RmKeys.insert(key);
			return true;
		}


		/**
		 @brief 제거 목록에 들어있는 데이타를 적용한다.
		 */
		bool apply_removes()
		{
			if (m_RmKeys.empty())
				return true;

			BOOST_FOREACH(KeyType key, m_RmKeys)
			{
				VectorMap::remove(key);
			}
			m_RmKeys.clear();
			return true;
		}

	public:
		RemoveKeys m_RmKeys;
	};

}
