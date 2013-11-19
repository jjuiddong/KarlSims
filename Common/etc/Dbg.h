//------------------------------------------------------------------------
// Name:    Dbg.h
// Author:  jjuiddong
// Date:    1/5/2013
// 
// 디버그에 관련된 함수를 모아놓았다.
//------------------------------------------------------------------------
#pragma once

namespace common { namespace dbg
{
	void Print( const std::string &str );
	void Print( const char* fmt, ...);

}}
