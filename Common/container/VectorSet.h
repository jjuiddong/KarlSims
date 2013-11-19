/**
Name:   VectorSet.h
Date:    9/24/2013

	Set �� Vector�� ���ĳ��� �ڷᱸ����.
	���� �׼��� ���� Vector�� �̿��ϰ�, ��ü�� Ž���� ����
	Set �� �̿��Ѵ�.

	 VectorType �� Type�� ����ũ�� �����Ϳ��� �Ѵ�. ���� Type��
	�ִ� ��� Vector���� ����� �������� ���ϰ� �ȴ�.
*/
#pragma once

#include "VectorHelper.h"

namespace common
{
	template <class Type>
	class VectorSet
	{
	public:
		typedef std::set<Type> SetType;
		typedef std::vector<Type> VectorType;
		typedef typename SetType::iterator iterator;
		typedef typename SetType::value_type value_type;
		typedef typename VectorType::iterator viterator;

		VectorSet() { }
		VectorSet(int reserved) : m_Seq(reserved) { }
		VectorSet(const VectorSet &rhs) { operator=(rhs); }

		bool insert(const value_type &type)
		{
			// insert set
			SetType::iterator it = m_RandomAccess.find(type);
			if (m_RandomAccess.end() != it)
				return false; // �̹� ���� ��
			m_RandomAccess.insert( type );

			// insert vector
			putvector(m_Seq, m_RandomAccess.size() - 1, type);
			return true;
		}

		bool remove(const Type &type)
		{
			SetType::iterator it = m_RandomAccess.find(type);
			if (m_RandomAccess.end() == it)
				return false; // ���ٸ� ����

			removevector(m_Seq, *it);
			m_RandomAccess.erase(it);
			return true;
		}

		bool empty() const
		{
			return m_RandomAccess.empty();
		}

		void reserve(unsigned int size)
		{
			m_Seq.reserve(size);
		}

		void clear()
		{
			m_RandomAccess.clear();
			m_Seq.clear();
		}

		size_t size() const
		{
			return m_RandomAccess.size();
		}

		iterator find(const Type &key) { return m_RandomAccess.find(key); }
		iterator begin() { return m_RandomAccess.begin(); }
		iterator end() { return m_RandomAccess.end(); }


		VectorSet& operator=(const VectorSet &rhs)
		{
			if (this != &rhs)
			{
				m_RandomAccess = rhs.m_RandomAccess;
				m_Seq = rhs.m_Seq;
			}
			return *this;
		}

	public:
		std::set<Type>		m_RandomAccess;
		std::vector<Type>	m_Seq;
	};

}
