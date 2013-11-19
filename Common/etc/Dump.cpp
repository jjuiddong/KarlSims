
#include "stdafx.h"
#include "dump.h"
#include <DbgHelp.h>
#include <stdio.h>

#pragma comment(lib,"DbgHelp.Lib")

namespace common { namespace dump {

	bool n_IsFullDump = false; // ��ü������ �� true�� �����Ѵ�.
}}

using namespace common;
using namespace dump;


//------------------------------------------------------------------------
// ���ܻ�Ȳ ó�� �ڵ鷯
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
		dump::n_IsFullDump? MiniDumpWithFullMemory : MiniDumpNormal,	// ���� Ÿ�� ����
		&MinidumpExceptionInformation,
		NULL,
		NULL);
	//::TerminateProcess(::GetCurrentProcess(), 0);

	return 0;
}


//------------------------------------------------------------------------
// �������� �̸� �����ؼ� ����
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
// ���ܻ�Ȳ �߻��� �����Լ� ���
// ��ü������� IsFullDump�� true�� �ش�.
//------------------------------------------------------------------------
void dump::InstallSelfMiniDump(bool IsFullDump) // IsFullDump = false;
{
	n_IsFullDump = IsFullDump;
	SetUnhandledExceptionFilter(ExceptionFilter);
}
