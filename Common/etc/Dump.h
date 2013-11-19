//------------------------------------------------------------------------
// Name:    Dump.h
// Author:  jjuiddong
// Date:    1/6/2013
// 
// 프로그램이 비정상 종료시 미니덤프를 남긴다.
//------------------------------------------------------------------------
#pragma once

namespace common { namespace dump {

	void InstallSelfMiniDump(bool IsFullDump=false);
	std::string GenerateDumpFileName();

}}
