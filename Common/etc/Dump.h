//------------------------------------------------------------------------
// Name:    Dump.h
// Author:  jjuiddong
// Date:    1/6/2013
// 
// ���α׷��� ������ ����� �̴ϴ����� �����.
//------------------------------------------------------------------------
#pragma once

namespace common { namespace dump {

	void InstallSelfMiniDump(bool IsFullDump=false);
	std::string GenerateDumpFileName();

}}
