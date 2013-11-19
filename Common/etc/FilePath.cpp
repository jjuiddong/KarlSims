
#include "stdafx.h"
#include "FilePath.h"
#include <shlwapi.h> // �� ��� ���Ͽ� FilePath�� ���õ� �Լ����� ����. �� �̿��غ���.
#pragma comment(lib, "shlwapi")

using namespace common;

//	_splitpath(findFilePath.c_str(), drive, dir, NULL,NULL); �Լ��� ���ϸ��� �и��� �� �ִ�.


/**
 @brief fileName�� ���丮 ��θ� ������ �����̸��� Ȯ���ڸ� �����Ѵ�.
 */
std::string common::GetFileName(const std::string &fileName)
{
	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, fileName.c_str() );
	char *name = PathFindFileNameA(srcFileName);
	return name;
}


/**
 @brief  fileName���� Ȯ���ڸ� �����Ѵ�.
 */
std::string common::GetFileExt(const std::string &fileName)
{
	char *ext = PathFindExtensionA(fileName.c_str());
	return ext;
}


//------------------------------------------------------------------------
// fileName�� ��ο� Ȯ���ڸ� ������ �����̸����� �����Ѵ�.
//------------------------------------------------------------------------
std::string common::GetFileNameExceptExt(const std::string &fileName)
{
	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, fileName.c_str() );
	char *name = PathFindFileNameA(srcFileName);
	PathRemoveExtensionA(name);
	return name;
}


//------------------------------------------------------------------------
// fileName���� �����̸��� Ȯ���ڸ� ������ ������ ��θ� �����Ѵ�.
// �������� '\' ���ڴ� ����.
//------------------------------------------------------------------------
std::string common::GetFilePathExceptFileName(const std::string &fileName)
{
	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, fileName.c_str() );
	PathRemoveFileSpecA( srcFileName );
	return srcFileName;
}


/**
 @brief 
 */
std::string common::GetCurrentProcessName()
{
	HMODULE hMod = GetModuleHandle(NULL);
	char moduleName[ MAX_PATH] = "";  
	GetModuleFileNameA(hMod, moduleName, MAX_PATH);
	char *name = PathFindFileNameA(moduleName);
	return name;
}


/**
 @brief 
 */
std::string common::GetCurrentProcessPath()
{
	HMODULE hMod = GetModuleHandle(NULL);
	char moduleName[ MAX_PATH] = "";  
	GetModuleFileNameA(hMod, moduleName, MAX_PATH);
	return GetFilePathExceptFileName(moduleName);
}
