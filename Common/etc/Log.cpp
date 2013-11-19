
#include "stdafx.h"
#include "log.h"
#include "time.h"
#include "FilePath.h"


namespace common { namespace log {

	void Log_char( const char *label, const char *msg );
	void ErrorLog_char( const char *label, const char *msg );
	void Output_char( const char *label, const char *msg );

	std::string GetLogFileName();
	std::string GetErrorLogFileName();

	std::string g_LogFileName;
	std::string g_ErrorLogFileName;
}}

using namespace common;
using namespace log;
using namespace std;


/**
 @brief cut the Too many string of same value
 */
bool IsCutMessage(const char *msg)
{
	static int sameMsgCnt = 0;
	static int oldT = GetTickCount();
	static string oldMsg = msg;
	const int curT = GetTickCount();
	if ((curT - oldT < 10) &&  (oldMsg == msg) && (sameMsgCnt > 3))
	{
		++sameMsgCnt;
		return true;
	}

	sameMsgCnt = 0;
	oldMsg = msg;
	return false;
}


/**
 @brief �α����� �̸� ����
 */
string log::GetLogFileName()
{
	string path = GetCurrentProcessPath() + "/Log/";
	path += GetDateString();
	path += "-";
	path += GetCurrentProcessName();
	path += ".Log.log";
	return path;
}


/**
 @brief  �����α����� �̸� ���� 
 */
string log::GetErrorLogFileName()
{
	//string processName = GetCurrentProcessName();
	//processName += ".Log.log";
	//return processName;
	string path = GetCurrentProcessPath() + "/Log/";
	path += GetDateString();
	path += "-";
	path += GetCurrentProcessName();
	path += ".Log.log";
	return path;
}


/**
 @brief 
 */
void log::Log_char( const char *label, const char *msg )
{
	if (IsCutMessage(msg))
		return;

	if (g_LogFileName.empty())
		g_LogFileName = GetLogFileName();

	ofstream fs(g_LogFileName, ios_base::app);
	if (!fs.is_open()) return;
	fs << "[" << common::GetTimeString() << "]  " << label  << msg << endl;
}


/**
 @brief 
 */
void log::ErrorLog_char( const char *label, const char *msg )
{
	if (IsCutMessage(msg))
		return;

	if (g_ErrorLogFileName.empty())
		g_ErrorLogFileName = GetErrorLogFileName();

	ofstream fs(g_ErrorLogFileName, ios_base::app);
	if (!fs.is_open()) return;
	fs << "[" << common::GetTimeString() << "]  " << label << msg << endl;
}


/**
 @brief 
 */
void log::Output_char( const char *label, const char *msg )
{
	string str = string(label) + " " + msg;
	::OutputDebugStringA( str.c_str() );
	::OutputDebugStringA( "\n" );
}



//------------------------------------------------------------------------
// Log.log ���Ͽ� �α׸� �����.
// ��¥/�ð� + �޼���
//------------------------------------------------------------------------
void log::Log(const std::string &str)
{
	if (g_LogFileName.empty())
		g_LogFileName = GetLogFileName();

	ofstream fs(g_LogFileName, ios_base::out);
	if (!fs.is_open()) return;

	fs << "[" << common::GetTimeString() << "]	" << str << endl;
}


//------------------------------------------------------------------------
// �������� 
//------------------------------------------------------------------------
void log::Log( const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );

	if (g_LogFileName.empty())
		g_LogFileName = GetLogFileName();

	ofstream fs(g_LogFileName, ios_base::out);
	if (!fs.is_open()) return;
	fs << "[" << common::GetTimeString() << "]	" << textString << endl;
}


//------------------------------------------------------------------------
// ���� �޼���â ���
//------------------------------------------------------------------------
void log::ErrorMsg(const std::string &str)
{
	::MessageBoxA(NULL, str.c_str(), "Error", MB_OK );
}


//------------------------------------------------------------------------
// ErrorLog.log ���Ͽ� �α׸� �����.
// ��¥/�ð� + �����޼���
//------------------------------------------------------------------------
void log::ErrorLog(const std::string &str)
{
	if (g_ErrorLogFileName.empty())
		g_ErrorLogFileName = GetErrorLogFileName();

	std::ofstream fs(g_ErrorLogFileName, std::ios_base::out);
	if (!fs.is_open()) return;

	fs << "[" << common::GetTimeString() << "]	" << str;
}


//------------------------------------------------------------------------
// �޼���â�� ����Ѵ�.
//------------------------------------------------------------------------
void log::Msg(const std::string &str)
{
	::MessageBoxA(NULL, str.c_str(), "Msg", MB_OK );
}


/**
 @brief print log
 */
void log::Log( LOG_TYPE level, const std::string &str )
{
	switch (level)
	{
	case LOG_FILE: Log_char("", str.c_str()); break;
	case LOG_OUTPUTWINDOW: Output_char("", str.c_str()); break;
	case LOG_FILE_N_OUTPUTWINDOW: 
		Log_char("", str.c_str());
		Output_char("", str.c_str());
		break;
	}
}


/**
 @brief print log
 */
void log::Log( LOG_TYPE level, const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );

	switch (level)
	{
	case LOG_FILE:  Log_char("", textString); break;
	case LOG_OUTPUTWINDOW: Output_char("", textString); break;
	case LOG_FILE_N_OUTPUTWINDOW:
		Log_char("", textString);
		Output_char("", textString);
		break;
	}
}


/**
 @brief log print
 */
void log::Log( LOG_TYPE type, LOG_LEVEL level, int subLevel, const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );

	stringstream ss;
	ss << "{" << level << "," << subLevel << "} ";

	switch (type)
	{
	case LOG_FILE:  Log_char(ss.str().c_str(), textString); break;
	case LOG_OUTPUTWINDOW: Output_char(ss.str().c_str(), textString); break;
	case LOG_FILE_N_OUTPUTWINDOW:
		Log_char(ss.str().c_str(), textString);
		Output_char(ss.str().c_str(), textString);
		break;
	}
}


/**
 @brief log print
 */
void log::Log( LOG_TYPE type, LOG_LEVEL level, int subLevel, const std::string &str )
{
	stringstream ss;
	ss << "{" << level << "," << subLevel << "} ";

	switch (type)
	{
	case LOG_FILE: Log_char(ss.str().c_str(), str.c_str()); break;
	case LOG_OUTPUTWINDOW: Output_char(ss.str().c_str(), str.c_str()); break;
	case LOG_FILE_N_OUTPUTWINDOW: 
		Log_char(ss.str().c_str(), str.c_str());
		Output_char(ss.str().c_str(), str.c_str());
		break;
	}
}


/**
 @brief print error
 */
void log::Error( ERROR_LEVEL level, const std::string &str )
{
	switch (level)
	{
	case ERROR_CRITICAL: 
		ErrorLog_char( "Critical ", str.c_str() );
		Output_char( "Critical ", str.c_str() );
		break;

	case ERROR_PROBLEM:
		ErrorLog_char( "Problem ", str.c_str() );
		Output_char( "Problem ", str.c_str() );
		break;

	case ERROR_WARNING:
		Output_char( "Warning ", str.c_str() );
		break;
	}
}


/**
 @brief print error
 */
void log::Error( ERROR_LEVEL level, const char* fmt, ... )
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );

	switch (level)
	{
	case ERROR_CRITICAL: 
		ErrorLog_char( "Critical ",textString );
		Output_char( "Critical ",textString );
		break;

	case ERROR_PROBLEM:
		ErrorLog_char( "Problem ", textString );
		Output_char( "Problem ",textString );
		break;

	case ERROR_WARNING:
		Output_char( "Warning ", textString);
		break;
	}
}


/**
 @brief  error print
 */
void log::Error( ERROR_LEVEL level, int subLevel, const char* fmt, ... )
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );

	char *name[] = {"Critical ", "Problem ",  "Warning " };

	stringstream ss;
	if (level >= 0 && level < 3)
		ss << name[ level];
	ss << " {" << level << "," << subLevel << "} ";

	switch (level)
	{
	case ERROR_CRITICAL: 
		ErrorLog_char( ss.str().c_str() ,textString );
		Output_char( ss.str().c_str(),textString );
		break;

	case ERROR_PROBLEM:
		ErrorLog_char( ss.str().c_str(), textString );
		Output_char( ss.str().c_str(),textString );
		break;

	case ERROR_WARNING:
		Output_char( ss.str().c_str(), textString);
		break;
	}
}


/**
 @brief error print
 */
void log::Error( ERROR_LEVEL level, int subLevel, const std::string &str )
{
	char *name[] = {"Critical ", "Problem ",  "Warning " };
	stringstream ss;
	if (level >= 0 && level < 3)
		ss << name[ level];
	ss << " {" << level << "," << subLevel << "} ";

	switch (level)
	{
	case ERROR_CRITICAL: 
		ErrorLog_char( ss.str().c_str() , str.c_str() );
		Output_char( ss.str().c_str(),str.c_str() );
		break;

	case ERROR_PROBLEM:
		ErrorLog_char( ss.str().c_str(), str.c_str() );
		Output_char( ss.str().c_str(), str.c_str() );
		break;

	case ERROR_WARNING:
		Output_char( ss.str().c_str(), str.c_str());
		break;
	}
}

