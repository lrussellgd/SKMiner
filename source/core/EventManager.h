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

#include "types.h"

class IListener;

namespace Events
{
	class Event;


	class EventManager
	{

	private:

		static EventManager*						sm_pInstance;

		bool m_bIsInitialized;
		std::multimap<std::string, IListener*>		m_mapListeners;
		std::vector<Event*>							m_vecEvents;

		LLP::Thread_t m_thTHREAD;

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

	public:

		static EventManager* GetInstance();

		void Initialize();
		void Start();

		void Shutdown();

		void AddEvent(Event* pEvent);
		void Register(std::string szEventName, IListener* pListener);
		void Deregister(IListener* pListener);

		void Clear();
		void ClearEvents();
		void ClearListeners();

		void HandleEvents();
	};
}


#endif //_EVENTMANAGER_H_