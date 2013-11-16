//------------------------------------------------------------------------
// Name:    Time.h
// Author:  jjuiddong
// Date:    12/23/2012
// 
// 시간을 스트링으로 생성한다.
//------------------------------------------------------------------------
#pragma once

namespace memmonitor
{
	// xx년xx월xx일xx시xx분xx초
	static std::string GetTimeString()
	{
		SYSTEMTIME t;
		GetLocalTime(&t);
		return format("(%d-%d-%d) %d시%d분%d초", t.wYear, t.wMonth, t.wDay, 
			t.wHour, t.wMinute, t.wSecond);
	}
}
