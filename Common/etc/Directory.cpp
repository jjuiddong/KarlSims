
#include "stdafx.h"
#include "Directory.h"

using namespace common;
using namespace std;


//------------------------------------------------------------------------
// findFilePath : 찾고자 하는 파일명
//				  예: *.*  -> 모든 파일을 리스트에 담는다.
//					  *.txt -> 텍스트 파일을 리스트에 담는다.
// findFilePath에 해당하는 파일을 리스트에 담아서 리턴한다.
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
	// 현재 경로에 있는 파일들을 검색한다.
	BOOL bResult = TRUE;
	while (bResult) 
	{
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  // 폴더가 아닐때만 추가
		{
			if (GetFileExt(wfd.cFileName) == ext)
				fileList.push_back( string(drive)+dir+wfd.cFileName);
		}
		bResult = FindNextFileA(hSrch,&wfd);
	}
	FindClose(hSrch);

	//-----------------------------------------------------------------------------------
	// 현재 경로에 있는 폴더를 검사한다.
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
// 현재 디렉토리 경로를 적용해서 FindFileList() 함수를 호출한다.
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
