////////////////////////////////////////////////
// File:	EventManager.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _EVENTMANAGER_H_
#define _EVENTMANAGER_H_

#include <string>
#include <map>
#include <vector>

#include "Event.h"
#include "../core/types.h"

class IListener;

class EventManager
{

private:

	static EventManager*							sm_pInstance;
	bool m_bShutdown;
	bool m_bIsInitialized;
	std::multimap<Event::EVENT_TYPE, IListener*>	m_mapListeners;
	std::list<Event*>								m_lstEvents;
	std::vector<Event*>								m_vecEvents;

	LLP::Thread_t m_thTHREAD;
	LLP::Thread_t* m_pEventSeparator;
	boost::mutex* m_eventLock;

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	EventManager();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	EventManager(const EventManager& rEvent) {}

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	EventManager& operator=(const EventManager& rEvent) { return *this; }

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~EventManager();

	void ProcessEvents();
	void SeparateEvents();

public:

	static EventManager* GetInstance();

	void Initialize();
	void Start();

	void Shutdown();

	void AddEvent(Event::EVENT_TYPE enmEventType, void* pA = NULL, void* pB = NULL, void* pC = NULL, void* pD = NULL);
	void AddEvent(Event* pEvent);

	void Register(Event::EVENT_TYPE enmEventType, IListener* pListener);
	void Deregister(IListener* pListener);

	void Clear();
	void ClearEvents();
	void ClearListeners();

	void HandleEvents();
};

#endif //_EVENTMANAGER_H_