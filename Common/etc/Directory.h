//------------------------------------------------------------------------
// Name:    Directory.h
// Author:  jjuiddong
// Date:    1/3/2013
// 
// ���� Ž��, ���� ã��� ���丮�� ���õ� �Լ��� ��Ƴ��Ҵ�.
//------------------------------------------------------------------------
#pragma once


namespace common
{
	std::list<std::string> FindFileList( const std::string &findFilePath );
	std::list<std::string> FindFileList_ApplyCurrentDirector( const std::string &findFilePath );

}
