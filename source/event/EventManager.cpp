////////////////////////////////////////////////
// File:	EventManager.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "EventManager.h"
#include "Event.h"
#include "IListener.h"


EventManager* EventManager::sm_pInstance;

EventManager::EventManager()
{
	m_bShutdown = false;
	m_bIsInitialized = false;
	sm_pInstance = NULL;
}

EventManager* EventManager::GetInstance()
{
	if (!sm_pInstance)
	{
		sm_pInstance = new EventManager();
	}

	return sm_pInstance;
}

void EventManager::Initialize()
{
	if (m_bIsInitialized)
	{
		return;
	}


	m_eventLock = new boost::mutex();

	m_bIsInitialized = true;
}

void EventManager::Start()
{
	if (!m_bIsInitialized)
	{
		return;
	}

	m_pEventSeparator = new boost::thread(&EventManager::SeparateEvents, this);
	m_thTHREAD = boost::thread(&EventManager::HandleEvents, this);
}

void EventManager::Shutdown()
{
	m_bShutdown = true;

	m_thTHREAD.detach();
		
	if (m_pEventSeparator)
	{
		m_pEventSeparator->detach();
		delete(m_pEventSeparator);
		m_pEventSeparator = NULL;
	}

	if (m_eventLock)
	{
		m_eventLock->unlock();
		delete(m_eventLock);
		m_eventLock = NULL;
	}
		
	Clear();
	m_bIsInitialized = false;
}

void EventManager::AddEvent(Event* pEvent)
{
	if (!pEvent)
	{
		return;
	}

	m_lstEvents.push_back(pEvent);
}

void EventManager::AddEvent(Event::EVENT_TYPE enmEventType, void* pA, void* pB, void* pC, void* pD)
{
	m_lstEvents.push_back(new Event(enmEventType, pA, pB, pC, pD));
}

void EventManager::Register(Event::EVENT_TYPE enmEventType, IListener* pListener)
{
	if (!pListener)
	{
		return;
	}

	for (std::multimap<Event::EVENT_TYPE, IListener*>::iterator iter = m_mapListeners.begin(); iter != m_mapListeners.end(); ++iter)
	{
		if ((*iter).first == enmEventType && (*iter).second == pListener)
		{
			return;
		}
	}

	m_mapListeners.insert(std::make_pair(enmEventType, pListener));
}

void EventManager::Deregister(IListener* pListener)
{
	std::multimap<Event::EVENT_TYPE, IListener*>::iterator iter = m_mapListeners.begin();
	while (iter != m_mapListeners.end())
	{
		if ((*iter).second == pListener)
		{
			delete(iter->second);
			iter->second = NULL;

			iter = m_mapListeners.erase(iter);
			continue;
		}
		++iter;
	}
}

void EventManager::Clear()
{
	ClearEvents();
	ClearListeners();
}

void EventManager::ClearEvents()
{
	while (!m_lstEvents.empty())
	{
		delete(m_lstEvents.front());
		m_lstEvents.pop_front();
	}

	while (!m_vecEvents.empty())
	{
		delete(m_vecEvents.front());
		m_vecEvents.erase(m_vecEvents.begin());
	}

	m_lstEvents.clear();
	m_vecEvents.clear();
}

void EventManager::ClearListeners()
{
	for (std::multimap<Event::EVENT_TYPE, IListener*>::iterator iter = m_mapListeners.begin(); iter != m_mapListeners.end(); ++iter)
	{
		IListener* pListener = (*iter).second;

		if (pListener)
		{
			delete(pListener);
			pListener = NULL;
		}
	}

	m_mapListeners.clear();
}


void EventManager::SeparateEvents()
{
	while (!m_bShutdown)
	{
		Sleep(10);

		while (!m_lstEvents.empty())
		{
			m_vecEvents.push_back(m_lstEvents.front());
			m_lstEvents.pop_front();
		}
	}
}

void EventManager::HandleEvents()
{
	while (!m_bShutdown)
	{
		//Run this thread at 1 Cycle per 10 Milliseconds.
		Sleep(10);

		while (!m_vecEvents.empty())
		{				
			Event* pEvent = m_vecEvents.front();

			std::pair<std::multimap<Event::EVENT_TYPE, IListener*>::iterator, std::multimap<Event::EVENT_TYPE, IListener*>::iterator> listenerRange = m_mapListeners.equal_range(pEvent->GetEventType());
			for (std::multimap<Event::EVENT_TYPE, IListener*>::iterator iter = listenerRange.first; iter != listenerRange.second; ++iter)
			{
				IListener* pListener = iter->second;
				pListener->HandleEvent(pEvent);
			}

			delete m_vecEvents.front();
			m_vecEvents.erase(m_vecEvents.begin());
		}
	}
}