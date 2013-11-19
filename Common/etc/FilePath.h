//------------------------------------------------------------------------
// Name:    FilePath.h
// Author:  jjuiddong
// Date:    12/27/2012
// 
// ���� ��ο� ���� ó���� �Ѵ�.
//------------------------------------------------------------------------
#pragma once

namespace common
{
	std::string GetFileName(const std::string &fileName);
	std::string GetFileExt(const std::string &fileName);
	std::string GetFileNameExceptExt(const std::string &fileName);
	std::string GetFilePathExceptFileName(const std::string &fileName);

	std::string GetCurrentProcessName();
	std::string GetCurrentProcessPath();
}
