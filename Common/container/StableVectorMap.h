/**
Name:   StableVectorMap.h
Author:  jjuiddong
Date:    4/28/2013

	VectorMap�� GarbageCollector, Remove Safe ����� �߰��ߴ�.

	- ���� GarbageCollector �� �������� ���ߴ�.
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
		 @brief insert ���� ���� ����� ���� �˻��Ѵ�.
		 */
		bool insert(const value_type &vt)
		{
			apply_removes();
			return VectorMap::insert(vt);
		}


		/**
		 @brief ������ ���� ����Ѵ�.
		 */
		bool remove(const KeyType &key)
		{
			m_RmKeys.insert(key);
			return true;
		}


		/**
		 @brief ���� ��Ͽ� ����ִ� ����Ÿ�� �����Ѵ�.
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
