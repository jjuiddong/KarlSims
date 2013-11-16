//------------------------------------------------------------------------
// Name:    Time.h
// Author:  jjuiddong
// Date:    12/23/2012
// 
// �ð��� ��Ʈ������ �����Ѵ�.
//------------------------------------------------------------------------
#pragma once

namespace memmonitor
{
	// xx��xx��xx��xx��xx��xx��
	static std::string GetTimeString()
	{
		SYSTEMTIME t;
		GetLocalTime(&t);
		return format("(%d-%d-%d) %d��%d��%d��", t.wYear, t.wMonth, t.wDay, 
			t.wHour, t.wMinute, t.wSecond);
	}
}
