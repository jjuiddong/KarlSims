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
//  ������ ����
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
// ������ ����
//------------------------------------------------------------------------
void CThread::Terminate(const int milliSeconds) //milliSeconds = -1
{
	m_State = END;
	DWORD timeOutTime = (milliSeconds>=0)? milliSeconds : INFINITE;
	WaitForSingleObject(m_hThread, timeOutTime); // �����尡 ����� ������ ��ٸ���.
	//TerminateThread(m_hThread, 0);
	//_endthreadex(m_hThread);
	//WaitForSingleObject(m_hThread, 0); // �����尡 ����� ������ ��ٸ���.
	CloseHandle(m_hThread);
	m_hThread = NULL;
}


//------------------------------------------------------------------------
// �����忡�� �޼����� ������.
// rcvTaskId : ���� �½�ũ ���̵� ('0' �̶�� �����尡 �޴´�.)
//			   -1 : �ܺη� ���� �޼����� ����
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
// �����尡 ���� �޼����� �����Ѵ�.
// �޼����� ���ٸ� false�� �����Ѵ�.
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
// �����忡�� �ܺη� ���ϴ� �޼����� �����Ѵ�.
// �޼����� ���ٸ� false�� �����Ѵ�.
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
		return false; // �̹� �����Ѵٸ� ����

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
		return false; // ���ٸ� ����

	m_Tasks.remove_if( IsTask(pTask->GetId()) );
	return true;
}


/**
 @brief 
 */
int CThread::GetTaskCount()
{
	AutoCSLock cs(m_TaskCS);
	return m_Tasks.size();
}


/**
@brief  taskId�� �ش��ϴ� task�� ����.
*/
CTask*	CThread::GetTask(const int taskId)
{
	//AutoCSLock cs(m_TaskCS); ����ȭ ���� �ʴ´�.
	TaskItor it = find_if(m_Tasks.begin(), m_Tasks.end(), IsTask(taskId));
	if (m_Tasks.end() == it)
		return NULL; // ���ٸ� ����
	return *it;
}


//------------------------------------------------------------------------
// �������� ������ Ŭ���� ����
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
// ������ ����
// Task�� �����Ų��.
//------------------------------------------------------------------------
void CThread::Run()
{
	while (RUN == m_State)
	{
		if (m_Tasks.empty()) // break no task
			break;

		//1. �½�ũ ó��
		{
			AutoCSLock cs(m_TaskCS);
			TaskItor it = m_Tasks.begin();
			while (m_Tasks.end() != it)
			{
				CTask *pTask = *it;
				if (CTask::RR_END == pTask->Run())
				{
					// �½�ũ ����
					it = m_Tasks.erase(it);
					delete pTask;
				}
				else
				{
					++it;
				}
			}
		}

		//2. �޼��� ó��
		DispatchMessage();

		Sleep(1);
	}

	// ���������� �� �޼����� ���������� ó���Ѵ�.
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
// ����� �޼������� �½�ũ�� ������.
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
					boost::bind( &IsSameId<CTask>, _1, it->wParam) );
				if (m_Tasks.end() != t)
				{
					(*t)->MessageProc((threadmsg::MSG)it->msg, it->wParam, it->lParam, it->added);
				}
				else
				{
					// �������� ���� �޼��� error
				}
			}
		}
		else // Thread���� �� �޼���
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
