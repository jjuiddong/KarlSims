
#include "stdafx.h"
#include "dump.h"
#include <DbgHelp.h>
#include <stdio.h>

#pragma comment(lib,"DbgHelp.Lib")

namespace common { namespace dump {

	bool n_IsFullDump = false; // 전체덤프일 때 true로 설정한다.
}}

using namespace common;
using namespace dump;


//------------------------------------------------------------------------
// 예외상황 처리 핸들러
//------------------------------------------------------------------------
LONG WINAPI ExceptionFilter(__in PEXCEPTION_POINTERS pExceptionPointer)
{
	MINIDUMP_EXCEPTION_INFORMATION MinidumpExceptionInformation;
	MinidumpExceptionInformation.ThreadId = ::GetCurrentThreadId();
	MinidumpExceptionInformation.ExceptionPointers = pExceptionPointer;
	MinidumpExceptionInformation.ClientPointers = FALSE;

	std::string fileName = dump::GenerateDumpFileName();
	if (fileName.empty() == true)
	{
		//::TerminateProcess(::GetCurrentProcess(), 0);
		return 0;
	}

	HANDLE hDumpFile = ::CreateFileA(fileName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		hDumpFile,
		dump::n_IsFullDump? MiniDumpWithFullMemory : MiniDumpNormal,	// 덤프 타입 설정
		&MinidumpExceptionInformation,
		NULL,
		NULL);
	//::TerminateProcess(::GetCurrentProcess(), 0);

	return 0;
}


//------------------------------------------------------------------------
// 덤프파일 이름 생성해서 리턴
//------------------------------------------------------------------------
std::string dump::GenerateDumpFileName()
{
	char ModulePath[ MAX_PATH];
	if (::GetModuleFileNameA(0, ModulePath, sizeof(ModulePath)) == 0)
		return "";

	std::string modulePath = ModulePath;
	std::string fileName = modulePath.substr( 0, modulePath.rfind("\\") + 1);
	fileName += GetTimeString();
	fileName +=  ".dmp";
	return fileName;
}


//------------------------------------------------------------------------
// 예외상황 발생시 덤프함수 등록
// 전체덤프라면 IsFullDump를 true로 준다.
//------------------------------------------------------------------------
void dump::InstallSelfMiniDump(bool IsFullDump) // IsFullDump = false;
{
	n_IsFullDump = IsFullDump;
	SetUnhandledExceptionFilter(ExceptionFilter);
}
