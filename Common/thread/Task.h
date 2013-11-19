//------------------------------------------------------------------------
// Name:    Task.h
// Author:  jjuiddong
// Date:    2012-12-02
// 
// 쓰레드에서 실행되는 태스크를 정의한다.
//------------------------------------------------------------------------
#pragma once

namespace common
{
	class CThread;
	class CTask
	{
	public:
		enum RUN_RESULT
		{
			RR_END,			// 태스크 종료
			RR_CONTINUE,	// 태스크 계속실행
		};

		CTask(int id, const std::string &name="");
		virtual ~CTask() {}
		int GetId() const;
		const std::string& GetName() const;
		void	SetThread(CThread *p);
		CThread* GetThread();

		// overriding
		virtual RUN_RESULT	Run() { return RR_END; }
		virtual void MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added ) {}

	protected:
		int				m_Id;
		CThread		*m_pThread;
		std::string	m_Name;
	};

	inline CTask::CTask(int id, const std::string &name) : m_Id(id), m_pThread(NULL), m_Name(name) { }
	inline int CTask::GetId() const { return m_Id; }
	inline const std::string& CTask::GetName() const { return m_Name; }
	inline void	 CTask::SetThread(CThread *p) { m_pThread = p; }
	inline CThread* CTask::GetThread() { return m_pThread; }


	// list<CTask*>에서 CTask를 찾는 객체
	class IsTask : public std::unary_function<CTask*, bool>
	{
	public:
		IsTask(int taskId):m_id(taskId) {}
		int m_id;
		bool operator ()(CTask *t) const
			{ return (t->GetId() == m_id); }
	};

}
