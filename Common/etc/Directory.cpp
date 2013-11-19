
#include "stdafx.h"
#include "Directory.h"

using namespace common;
using namespace std;


//------------------------------------------------------------------------
// findFilePath : ã���� �ϴ� ���ϸ�
//				  ��: *.*  -> ��� ������ ����Ʈ�� ��´�.
//					  *.txt -> �ؽ�Ʈ ������ ����Ʈ�� ��´�.
// findFilePath�� �ش��ϴ� ������ ����Ʈ�� ��Ƽ� �����Ѵ�.
//------------------------------------------------------------------------
list<string> common::FindFileList( const string &findFilePath )
{
	list<string> fileList;

	WIN32_FIND_DATAA wfd;
	HANDLE hSrch = FindFirstFileA(findFilePath.c_str(), &wfd );
	if (hSrch == INVALID_HANDLE_VALUE)
		return fileList;

	char drive[_MAX_DRIVE], dir[MAX_PATH], name[MAX_PATH], ext[16];
	_splitpath_s(findFilePath.c_str(), drive, dir, name, ext);

	//-----------------------------------------------------------------------------------
	// ���� ��ο� �ִ� ���ϵ��� �˻��Ѵ�.
	BOOL bResult = TRUE;
	while (bResult) 
	{
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  // ������ �ƴҶ��� �߰�
		{
			if (GetFileExt(wfd.cFileName) == ext)
				fileList.push_back( string(drive)+dir+wfd.cFileName);
		}
		bResult = FindNextFileA(hSrch,&wfd);
	}
	FindClose(hSrch);

	//-----------------------------------------------------------------------------------
	// ���� ��ο� �ִ� ������ �˻��Ѵ�.
	string newDirPath = string(drive) + dir + "*.*";
	hSrch = FindFirstFileA(newDirPath.c_str(), &wfd );
	if (hSrch == INVALID_HANDLE_VALUE)
		return fileList;

	bResult = TRUE;
	while (bResult) 
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			if (strcmp(wfd.cFileName,".") && strcmp(wfd.cFileName,"..")) 
			{
				string newPath = string(drive) + dir + wfd.cFileName + "\\" + name + ext;
				list<string> tmp = FindFileList(newPath);
				std::copy(tmp.begin(), tmp.end(), std::back_inserter(fileList) );
			}
		} 
		bResult = FindNextFileA(hSrch,&wfd);
	}
	FindClose(hSrch);

	return fileList;
}


//------------------------------------------------------------------------
// ���� ���丮 ��θ� �����ؼ� FindFileList() �Լ��� ȣ���Ѵ�.
//------------------------------------------------------------------------
std::list<std::string> common::FindFileList_ApplyCurrentDirector( const std::string &findFilePath )
{
	char curDir[ MAX_PATH];
	GetCurrentDirectoryA(sizeof(curDir), curDir);
	std::string searchDir = curDir;
	searchDir += "\\";
	searchDir += findFilePath;
	return FindFileList(searchDir);
}
