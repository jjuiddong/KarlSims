//------------------------------------------------------------------------
// Name:    SyncNode.h
// Author:  jjuiddong
// Date:    2012-12-03
// 
// 멀티쓰레드 시스템을 위해 만들어졌다.
// 서로 다른 쓰레드간에 쓰이는 객체일경우 메모리 제거를 일반화하기 위해 
// 만들어졌다.
//------------------------------------------------------------------------

#pragma once


namespace common
{
	class CInstance
	{
	public:
		CInstance() {}
		virtual ~CInstance() {}

	public:
		// Overriding
		virtual void	Release() {}
	};
}
