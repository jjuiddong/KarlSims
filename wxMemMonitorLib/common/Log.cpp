
#include "stdafx.h"
#include "log.h"

using namespace memmonitor;
using namespace std;


//------------------------------------------------------------------------
// Log.log ���Ͽ� �α׸� �����.
// ��¥/�ð� + �޼���
//------------------------------------------------------------------------
void log::Log(const std::string &str)
{
	ofstream fs("Log.log", ios_base::out);
	if (!fs.is_open()) return;

	fs << "[" << GetTimeString() << "]	" << str << endl;
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

	ofstream fs("Log.log", ios_base::out);
	if (!fs.is_open()) return;
	fs << "[" << GetTimeString() << "]	" << textString << endl;
}
