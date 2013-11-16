//------------------------------------------------------------------------
// Name:    String.h
// Author:  jjuiddong
// Date:    2012-12-05
// 
// �����ڵ�, ��Ƽ����Ʈ���� �� ������ �� �ִ� ��Ʈ�� Ŭ������ �����Ѵ�.
//
// �ѱ��� �� ��ȯ�Ǳ� ���ؼ� �ݵ�� ȣ�����־�� �Ѵ�. (�Ʒ��� �ִ� �Լ�)
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

