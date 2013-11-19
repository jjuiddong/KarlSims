//------------------------------------------------------------------------
// Name:    Directory.h
// Author:  jjuiddong
// Date:    1/3/2013
// 
// 폴더 탐색, 파일 찾기등 디렉토리에 관련된 함수를 모아놓았다.
//------------------------------------------------------------------------
#pragma once


namespace common
{
	std::list<std::string> FindFileList( const std::string &findFilePath );
	std::list<std::string> FindFileList_ApplyCurrentDirector( const std::string &findFilePath );

}
