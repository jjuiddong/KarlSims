//------------------------------------------------------------------------
// Name:    VectorHelp.h
// Author:  jjuiddong
// Date:    3/11/2013
// 
// std::vector helper class
//------------------------------------------------------------------------
#pragma once

namespace common
{	
	/**
	 @brief this function only avaible to unique ty value
	 remove ty and then rotate vector to sequence elements
	 */
	template <class Seq>
	bool removevector(Seq &seq, const typename Seq::value_type &ty)
	{
		for (size_t i=0; i < seq.size(); ++i)
		{
			if (seq[ i] == ty)
			{
				if ((seq.size()-1) > i) // elements를 회전해서 제거한다.
					std::rotate( seq.begin()+i, seq.begin()+i+1, seq.end() );
				seq.pop_back();
				return true;
			}
		}
		return false;
	}

	template <class T>
	void putvector( std::vector<T> &seq, size_t putIdx, const T &ty)
	{
		if (seq.size() > putIdx)
			seq[ putIdx] = ty;
		else
			seq.push_back(ty);
	}

	// void inservector(const std::vector<T> &seq, u_int idx, const T &ty);

	//template<class T>
	//class sizevector : public std::vector<T>
	//{
	//public:
	//	sizevector() : m_Size(0) {}
	//	sizevector(size_t s) : vector<T>(s), m_Size(0) {} // process like reserve()  function call

	//	bool remove(const T &ty) {
	//		const bool r = removevector(*this, ty);
	//		if (r) --m_Size;
	//		return r;
	//	}
	//	void push_back(const T &ty) {
	//		vector<T>::push_back(ty);
	//		++m_Size;
	//	}
	//	void pop_back() {
	//		std::vector<T>::pop_back();
	//		--m_Size;
	//	}
	//	void putback(const T &ty) {
	//		putvector(*this, m_Size, ty);
	//		++m_Size;
	//	}
	//	void popback() {
	//		--m_Size;
	//	}
	//	size_t size() const {
	//		return m_Size;
	//	}
	//	void clear() {
	//		std::vector<T>::clear();
	//		m_Size = 0;
	//	}
	//protected:
	//	size_t m_Size;
	//};

}

