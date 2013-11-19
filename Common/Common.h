
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

	// ������� �������� �ְ� �޴� �޼��� ����
	typedef struct _SExternalMsg
	{
		int rcvTaskId;			// �޼����� ���� Task ID (0=tread, -1=�ܺο��� �޴� �޼���)
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
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
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

#define RET(exp)		{if((exp)) return; }			// exp�� true�̸� ����
#define RETV(exp,val)	{if((exp)) return val; }
#define ASSERT_RET(exp)		{assert(exp); RET(!(exp) ); }
#define ASSERT_RETV(exp,val)		{assert(exp); RETV(!(exp),val ); }


// WinDef.h �� ���� �Ǿ� �ִ�.
//#define IN		// �Լ� ������ �Է����� ���� �Ķ���� (������)
//#define OUT		// �Լ� ���ڷ� ���� ���� �����Ǿ� ���ϵǴ� �Ķ����



// ���� ��������� ���� ���߿� �����Ѵ�.
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
#include "CodeGen.h"
#include "container/VectorMap.h"
#include "container/VectorSet.h"
#include "container/StableVectorMap.h"

#include "event/Event.h"
#include "event/EventHandler.h"



typedef common::ReferencePtr<common::CThread> ThreadPtr;


#endif // __COMMON_H__
