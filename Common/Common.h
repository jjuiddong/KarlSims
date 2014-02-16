
#ifndef __COMMON_H__
#define __COMMON_H__

#include <Windows.h>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <boost/foreach.hpp>
#include <comdef.h> // for _variant_t

#define  BOOST_DATE_TIME_NO_LIB // boost/interprocess
#define  BOOST_SPIRIT_THREADSAFE // boost spirits thread safe mode
#pragma warning(disable: 4355) // disable warning, using this pointer in constructor 


namespace common
{
	class CTask;
	typedef std::list<CTask*> TaskList;
	typedef TaskList::iterator TaskItor;

	// 쓰레드와 로직간의 주고 받는 메세지 정의
	typedef struct _SExternalMsg
	{
		int rcvTaskId;			// 메세지를 받을 Task ID (0=tread, -1=외부에서 받는 메세지)
		int msg;
		WPARAM wParam;
		LPARAM lParam;
		LPARAM added;

		_SExternalMsg() {}
		_SExternalMsg( int rcvtaskId, int msgtype, WPARAM wparam, LPARAM lparam, LPARAM _added) :
			rcvTaskId(rcvtaskId), msg(msgtype), wParam(wparam), lParam(lparam), added(_added)
			{
			}

	} SExternalMsg;

	typedef std::list<SExternalMsg> ExternalMsgList;
	typedef ExternalMsgList::iterator ExternalMsgItor;
}

typedef unsigned short u_short;


#ifdef UNICODE
	typedef std::wstring	tstring;
#else
	typedef std::string	tstring;
#endif


#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) {if (p) { delete p; p=NULL;} }
#endif
#ifndef SAFE_DELETEA
	#define SAFE_DELETEA(p) {if (p) { delete[] p; p=NULL;} }
#endif
#ifndef DX_SAFE_RELEASE
	#define DX_SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
#endif

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

#ifndef INOUT
	#define INOUT
#endif

#define RET(exp)		{if((exp)) return; }			// exp가 true이면 리턴
#define RETV(exp,val)	{if((exp)) return val; }
#define ASSERT_RET(exp)		{assert(exp); RET(!(exp) ); }
#define ASSERT_RETV(exp,val)		{assert(exp); RETV(!(exp),val ); }


// WinDef.h 에 정의 되어 있다.
//#define IN		// 함수 인자의 입력으로 들어가는 파라메터 (생략함)
//#define OUT		// 함수 인자로 들어가서 값이 설정되어 리턴되는 파라메터



// 공용 헤더파일은 가장 나중에 선언한다.
#include "math/Math.h"
#include "etc/AutoLock.h"
#include "thread/Thread.h"
#include "thread/Task.h"
#include "thread/Instance.h"
#include "etc/Singleton.h"
#include "etc/String.h"
#include "etc/FastMemLoader.h"
#include "etc/Reference.h"
#include "etc/RandNoDuplicate.h"
#include "etc/time.h"
#include "etc/FilePath.h"
#include "etc/Directory.h"
#include "etc/Dbg.h"
#include "etc/Log.h"
#include "etc/Dump.h"
#include "etc/random.h"
#include "CodeGen.h"
#include "container/VectorMap.h"
#include "container/VectorSet.h"
#include "container/StableVectorMap.h"

#include "event/Event.h"
#include "event/EventHandler.h"



typedef common::ReferencePtr<common::CThread> ThreadPtr;


#endif // __COMMON_H__
