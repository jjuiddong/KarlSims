/**
Name:   EventHandler.h
Author:  jjuiddong
Date:    4/4/2013

	Event Handler class
*/
#pragma once

#include "Event.h"

namespace common
{
	class CEventHandler;
	typedef void (CEventHandler::*EventFunction)(common::CEvent &);

	/// Event Handler class
	class CEventHandler
	{
	public:
		CEventHandler() {}
		virtual ~CEventHandler() {}

		bool EventConnect(CEventHandler *handler, EventType type, EventFunction fn);
		bool EventDisconnect(CEventHandler *handler, EventType type);

	protected:
		virtual bool SearchEventTable( CEvent &event );

	private:
		struct SEventEntry
		{
			EventType type;
			CEventHandler *handler;
			EventFunction fn;
			bool operator==(const SEventEntry &rhs);
		};

		std::vector<SEventEntry> m_EventTable;
	};

}
