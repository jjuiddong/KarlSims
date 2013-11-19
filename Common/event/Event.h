/**
Name:   Event.h
Author:  jjuiddong
Date:    4/4/2013

*/
#pragma once

namespace common
{

	enum EventType
	{
		EVT_NULL, // default

		// common
		EVT_TIMER,

		// Server/Client
		EVT_CONNECT,
		EVT_DISCONNECT,

		// Server
		EVT_LISTEN,
		EVT_CLIENT_JOIN,
		EVT_CLIENT_LEAVE,

		// p2p
		EVT_P2P_CONNECT,
		EVT_P2P_DISCONNECT,
		EVT_MEMBER_JOIN,
		EVT_MEMBER_LEAVE,
	};


	/// Event Class
	class CEvent
	{
	public:
		CEvent();
		CEvent(EventType type);
		CEvent(EventType type, int param);
		virtual ~CEvent() {}
		void			 SetType(EventType type);
		EventType GetType() const;
		int			GetParam() const;
		void			Skip();
		bool			IsSkip() const;

	private:
		EventType	m_Type;
		int				m_Param;
		bool				m_IsSkip;
	};

	inline CEvent::CEvent() { m_Type = EVT_NULL; m_IsSkip = false; }
	inline CEvent::CEvent(EventType type) : m_Type(type), m_IsSkip(false) { }
	inline CEvent::CEvent(EventType type, int param) : m_Type(type), m_Param(param), m_IsSkip(false) { }
	inline void	CEvent::SetType(EventType type) { m_Type = type; }
	inline EventType CEvent::GetType() const { return m_Type; }
	inline int CEvent::GetParam() const { return m_Param; }
	inline void	CEvent::Skip() { m_IsSkip = true; }
	inline bool CEvent::IsSkip() const { return m_IsSkip; }

}
