
#include "stdafx.h"
#include "EventHandler.h"

using namespace common;


bool CEventHandler::SEventEntry::operator==(const SEventEntry &rhs)
{
	if (rhs.type == EVT_NULL)
		return (handler == rhs.handler);
	else
		return ((type == rhs.type) && (handler == rhs.handler));
}

/**
 @brief Add Event Table
 */
//bool CEventHandler::EventConnect(EventType type, EventFunction fn)
//{
//	SEventEntry entry;
//	entry.handler = NULL;
//	entry.type = type;
//	entry.fn = fn;
//	m_EventTable.push_back(entry);
//	return true;
//}


/**
 @brief Add Event Table
 */
bool CEventHandler::EventConnect(CEventHandler *handler, EventType type, EventFunction fn)
{
	SEventEntry entry;
	entry.handler = handler;
	entry.type = type;
	entry.fn = fn;
	m_EventTable.push_back(entry);
	return true;
}


/**
 @brief Remove Event Handler
 @pram type : if NULL remove all handler emelent
 */
bool CEventHandler::EventDisconnect(CEventHandler *handler, EventType type)
{
	if (EVT_NULL == type)
	{
		for (int i=m_EventTable.size()-1; i >= 0; --i)
		{
			if (m_EventTable[ i].handler == handler)
			{
				common::removevector(m_EventTable, m_EventTable[ i]);
			}
		}
	}
	else
	{
		SEventEntry item;
		item.handler = handler;
		item.type =  type;
		common::removevector(m_EventTable, item);
	}
	return true;
}


/**
 @brief call event handler
 */
bool CEventHandler::SearchEventTable( CEvent &event )
{
	BOOST_FOREACH(auto &entry, m_EventTable)
	{
		if (event.GetType() == entry.type)
		{
			if (entry.handler)
			{
				CEventHandler *handler = entry.handler;
				(handler->*entry.fn)( event );
			}
			else
			{
				// not yet
				//EvtHandler_Fn fn = (EvtHandler_Fn)entry.fn;
				//(*fn)(event);
			}
		}
	}

	return true;
}
