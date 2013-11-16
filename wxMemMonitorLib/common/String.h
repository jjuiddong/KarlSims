//------------------------------------------------------------------------
// Name:    String.h
// Author:  jjuiddong
// Date:    2012-12-05
// 
// 유니코드, 멀티바이트문자 에 대응할 수 있는 스트링 클래스를 정의한다.
//
// 한글이 잘 변환되기 위해서 반드시 호출해주어야 한다. (아래에 있는 함수)
// std::locale::global(std::locale(""));
//------------------------------------------------------------------------

#pragma once


namespace memmonitor
{
	std::string wstr2str(const std::wstring &wstr);
	std::wstring str2wstr(const std::string &str);
	std::string variant2str(const _variant_t &var);
	_variant_t str2variant(const _variant_t &varType, const std::string &value);

	std::string format(const char* fmt, ...);
	std::wstring formatw(const char* fmt, ...);

}

