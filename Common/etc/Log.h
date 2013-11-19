/**
Name:   Log.h
Author:  jjuiddong
Date:    1/5/2013

�α� ���
*/
#pragma once

/// Write Log File, "Log.log" filename
namespace common { namespace log {

	enum ERROR_LEVEL {
		ERROR_CRITICAL,		/// �ɰ��� ����, ���α׷��� ���°� ���� �� �ִ�.
											/// output string : "critical : ~~ "
											/// print output window
											/// log "log.log"
											/// log "errorlog.log"

		ERROR_PROBLEM,		/// �� �����Ǿ�� �� �κп��� ������ �߻� ��. �ٸ� ������ �Ļ��� ����� �ִ� ����.
											/// ���� �Ҹ��� ��������, ���α׷��� ���� �� ������ �ƴϴ�.
											/// output string : "problem : ~~~"
											/// print output window
											/// log "Log.log" 

		ERROR_WARNING,		/// ���, �ǵ�ġ ���� �������� ���α׷��� �����ϴµ��� ���� ����.
											/// output string : "warning : ~~~ ", 
											/// print output window
	};

	enum LOG_TYPE {
		LOG_FILE,
		LOG_OUTPUTWINDOW,
		LOG_FILE_N_OUTPUTWINDOW,
		LOG_F = LOG_FILE,
		LOG_O = LOG_OUTPUTWINDOW,
		LOG_F_N_O = LOG_FILE_N_OUTPUTWINDOW,
	};

	enum LOG_LEVEL {
		LOG_PACKET=100,
		LOG_EVENT,
		LOG_MESSAGE,

	};

	/// log print
	void Log( LOG_TYPE level, const std::string &str );
	
	/// log print
	void Log( LOG_TYPE level, const char* fmt, ...);

	/// log print
	void Log( LOG_TYPE type, LOG_LEVEL level, int subLevel, const char* fmt, ...);

	/// log print
	void Log( LOG_TYPE type, LOG_LEVEL level, int subLevel, const std::string &str );


	/// error print
	void Error( ERROR_LEVEL level, const std::string &str );

	/// error print
	void Error( ERROR_LEVEL level, const char* fmt, ... );

	/// error print
	void Error( ERROR_LEVEL level, int subLevel, const char* fmt, ... );

	/// error print
	void Error( ERROR_LEVEL level, int subLevel, const std::string &str );


	/// write message to "Log.log" fie
	void Log(const std::string &str);
	/// write message to "Log.log" file
	void Log( const char* fmt, ...);
	/// write message to "ErrorLog.log" file
	void ErrorLog(const std::string &str);
	/// show message box
	void Msg(const std::string &str);
	/// show message box
	void ErrorMsg(const std::string &str);

}}
