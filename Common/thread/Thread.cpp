#include "stdafx.h"
#include "Thread.h"
#include "Task.h"
#include <process.h>
#include <boost/bind.hpp>

using namespace common;

namespace common
{
	unsigned __stdcall ThreadProcess(void *pThreadPtr )
	{
		CThread *pThread = (CThread*)pThreadPtr;
		pThread->Run();
		pThread->Exit();
		return 0;
	}

}


CThread::CThread(const std::string &name) :
	m_State(WAIT)
,	m_hThread(NULL)
//,	m_Name(name)
{
	strcpy_s(m_Name, name.c_str());

}

CThread::~CThread()
{
	Clear();

}


//------------------------------------------------------------------------
//  쓰레드 실행
//------------------------------------------------------------------------
void CThread::Start()
{
	if (RUN != m_State)
	{
		m_State = RUN;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProcess, this, 0, NULL);
	}
}


//------------------------------------------------------------------------
// 쓰레드 종료
//------------------------------------------------------------------------
void CThread::Terminate(const int milliSeconds) //milliSeconds = -1
{
	m_State = END;
	DWORD timeOutTime = (milliSeconds>=0)? milliSeconds : INFINITE;
	WaitForSingleObject(m_hThread, timeOutTime); // 쓰레드가 종료될 때까지 기다린다.
	//TerminateThread(m_hThread, 0);
	//_endthreadex(m_hThread);
	//WaitForSingleObject(m_hThread, 0); // 쓰레드가 종료될 때까지 기다린다.
	CloseHandle(m_hThread);
	m_hThread = NULL;
}


//------------------------------------------------------------------------
// 쓰레드에게 메세지를 보낸다.
// rcvTaskId : 받을 태스크 아이디 ('0' 이라면 쓰레드가 받는다.)
//			   -1 : 외부로 가는 메세지를 뜻함
//------------------------------------------------------------------------
void CThread::Send2ThreadMessage( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added)
{
	AutoCSLock cs(m_MsgCS);
	m_ThreadMsgs.push_back( SExternalMsg(-1, (int)msg, wParam, lParam, added) );
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CThread::Send2ExternalMessage( int msg, WPARAM wParam, LPARAM lParam, LPARAM added )
{
	AutoCSLock cs(m_MsgCS);
	m_ExternalMsgs.push_back( SExternalMsg(-1, msg, wParam, lParam, added) );
}


//------------------------------------------------------------------------
// 쓰레드가 받은 메세지를 리턴한다.
// 메세지가 없다면 false를 리턴한다.
//------------------------------------------------------------------------
bool CThread::GetThreadMsg( OUT SExternalMsg *pMsg, MSG_OPT opt ) // opt = MSG_REMOVE
{
	if (!pMsg) 
		return false;

	bool reval;
	{
		AutoCSLock cs(m_MsgCS);
		if (m_ThreadMsgs.empty())
		{
			reval = false;
		}
		else
		{
			*pMsg = m_ThreadMsgs.front();
			if (MSG_REMOVE == opt)
				m_ThreadMsgs.pop_front();
			reval = true;
		}
	}
	return reval;
}


//------------------------------------------------------------------------
// 쓰레드에서 외부로 향하는 메세지를 리턴한다.
// 메세지가 없다면 false를 리턴한다.
//------------------------------------------------------------------------
bool CThread::GetExternalMsg( OUT SExternalMsg *pMsg, MSG_OPT opt ) // opt = MSG_REMOVE
{
	if (!pMsg) 
		return false;

	bool reval;
	{
		AutoCSLock cs(m_MsgCS);
		if (m_ExternalMsgs.empty())
		{
			reval = false;
		}
		else
		{
			*pMsg = m_ExternalMsgs.front();
			if (MSG_REMOVE == opt)
				m_ExternalMsgs.pop_front();
			reval = true;
		}
	}
	return reval;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CThread::AddTask(CTask *pTask)
{
	if (!pTask)
		return false;

	AutoCSLock cs(m_TaskCS);
	TaskItor it = find_if(m_Tasks.begin(), m_Tasks.end(), IsTask(pTask->GetId()));
	if (m_Tasks.end() != it)
		return false; // 이미 존재한다면 실패

	pTask->SetThread(this);
	m_Tasks.push_back( pTask );

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CThread::RemoveTask(CTask *pTask)
{
	if (!pTask)
		return false;
	
	AutoCSLock cs(m_TaskCS);
	TaskItor it = find_if(m_Tasks.begin(), m_Tasks.end(), IsTask(pTask->GetId()));
	if (m_Tasks.end() == it)
		return false; // 없다면 실패

	m_Tasks.remove_if( IsTask(pTask->GetId()) );
	return true;
}


/**
 @brief 
 */
int CThread::GetTaskCount()
{
	AutoCSLock cs(m_TaskCS);
	return (int)m_Tasks.size();
}


/**
@brief  taskId에 해당하는 task를 리턴.
*/
CTask*	CThread::GetTask(const int taskId)
{
	//AutoCSLock cs(m_TaskCS); 동기화 하지 않는다.
	TaskItor it = find_if(m_Tasks.begin(), m_Tasks.end(), IsTask(taskId));
	if (m_Tasks.end() == it)
		return NULL; // 없다면 실패
	return *it;
}


//------------------------------------------------------------------------
// 동적으로 생성된 클래스 제거
//------------------------------------------------------------------------
void CThread::Clear()
{
	AutoCSLock cs(m_TaskCS);
	TaskItor it = m_Tasks.begin();
	while (m_Tasks.end() != it)
	{
		CTask *p = *it++;
		delete p;
	}
	m_Tasks.clear();

	AutoCSLock cs2(m_MsgCS);
	m_ThreadMsgs.clear();
	m_ExternalMsgs.clear();

	CloseHandle(m_hThread);
}


//------------------------------------------------------------------------
// 쓰레드 실행
// Task를 실행시킨다.
//------------------------------------------------------------------------
void CThread::Run()
{
	while (RUN == m_State)
	{
		if (m_Tasks.empty()) // break no task
			break;

		//1. 태스크 처리
		{
			AutoCSLock cs(m_TaskCS);
			TaskItor it = m_Tasks.begin();
			while (m_Tasks.end() != it)
			{
				CTask *pTask = *it;
				if (CTask::RR_END == pTask->Run())
				{
					// 태스크 제거
					it = m_Tasks.erase(it);
					delete pTask;
				}
				else
				{
					++it;
				}
			}
		}

		//2. 메세지 처리
		DispatchMessage();

		Sleep(1);
	}

	// 남았을지도 모를 메세지를 마지막으로 처리한다.
	DispatchMessage();
}


//------------------------------------------------------------------------
// call exit thread
//------------------------------------------------------------------------
void	CThread::Exit()
{
	m_State = END;
}


//------------------------------------------------------------------------
// 저장된 메세지들을 태스크로 보낸다.
//------------------------------------------------------------------------
void CThread::DispatchMessage()
{
	AutoCSLock cs(m_MsgCS);
	ExternalMsgItor it = m_ThreadMsgs.begin();
	while (m_ThreadMsgs.end() != it)
	{
		if (threadmsg::TASK_MSG == it->msg) // task message
		{
			{
				AutoCSLock cs(m_TaskCS);
				TaskItor t = find_if(m_Tasks.begin(), m_Tasks.end(), 
					boost::bind( &IsSameId<CTask>, _1, (int)it->wParam) );
				if (m_Tasks.end() != t)
				{
					(*t)->MessageProc((threadmsg::MSG)it->msg, it->wParam, it->lParam, it->added);
				}
				else
				{
					// 목적지가 없는 메세지 error
				}
			}
		}
		else // Thread에게 온 메세지
		{
			MessageProc((threadmsg::MSG)it->msg, it->wParam, it->lParam, it->added);
		}
		++it;
	}
	m_ThreadMsgs.clear();
}


//------------------------------------------------------------------------
// Message Process
//------------------------------------------------------------------------
void	CThread::MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added )
{
	switch (msg)
	{
	case threadmsg::TERMINATE_TASK:
		{
			// terminate task of id wParam
			{
				AutoCSLock cs(m_TaskCS);
				auto it = std::find_if( m_Tasks.begin(), m_Tasks.end(), 
					bind( &IsSameId<common::CTask>, _1, (int)wParam) );
				if (m_Tasks.end() != it)
				{
					delete *it;
					m_Tasks.erase(it);
				}
			}
		}
		break;
	}
}
