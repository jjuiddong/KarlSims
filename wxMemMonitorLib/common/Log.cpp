
#include "stdafx.h"
#include "log.h"

using namespace memmonitor;
using namespace std;


//------------------------------------------------------------------------
// Log.log 파일에 로그를 남긴다.
// 날짜/시간 + 메세지
//------------------------------------------------------------------------
void log::Log(const std::string &str)
{
	ofstream fs("Log.log", ios_base::out);
	if (!fs.is_open()) return;

	fs << "[" << GetTimeString() << "]	" << str << endl;
}


//------------------------------------------------------------------------
// 가변인자 
//------------------------------------------------------------------------
void log::Log( const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );

	ofstream fs("Log.log", ios_base::out);
	if (!fs.is_open()) return;
	fs << "[" << GetTimeString() << "]	" << textString << endl;
}
