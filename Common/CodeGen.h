//------------------------------------------------------------------------
// Name:    CodeGen.h
// Author:  jjuiddong
// Date:    12/28/2012
// 
// 코드를 생성하는 매크로와 템플릿 모음 파일
//------------------------------------------------------------------------
#pragma once

namespace common
{
	template<class T>
	static bool IsSameId(T *p, int id)
	{
		if (!p) return false;
		return p->GetId() == id;
	} 

	template<class T>
	static bool IsSameHandle(T *p, HANDLE handle)
	{
		if (!p) return false;
		return p->GetHandle() == handle;
	} 

}


/*
 치열한 고민의 현장이다. =,.=;;

template<class Key, class Type>
struct FindFuncObj
{	
	Key operator() (const Type &t)
	{
		return t.GetId();
	}	
};

template<class Key, class Type>
struct FindFuncPtr
{	
	Key operator() (const Type &t)
	{
		return t->GetId();
	}	
};


template<class Key, class Type, class FindFunc=FindFuncObj<Key,Type> >
struct MapOperator
{
	typename typedef std::map<Key,Type> MapType;
	typename typedef MapType::iterator MapItor;
	MapType m_Map;
	//const MapType& GetMap() { return m_Map; }	
	bool Add(Key key, Type value)
	{
		MapItor it = m_Map.find(key);
		if (m_Map.end() != it)
			return false;
		m_Map.insert( MapType::value_type(key,value) );
		return true;
	}
	bool Add(const Type &value)
	{
 		MapItor it = m_Map.find( value->GetId() );
 		if (m_Map.end() != it)
 			return false;
 		m_Map.insert( MapType::value_type( value->GetId(), value ) );
		return true;
	}
	bool Remove(Key key)
	{
		MapItor it = m_Map.find(key);
		if (m_Map.end() == it)
			return false;
		//delete it->second;
		m_Map.erase(it);
		return true;
	}
	const Type& Get(Key key)
	{
		MapItor it = m_Map.find(key);
		if (m_Map.end() == it)
			return Type();
		return it->second;
	}
};

/**/
