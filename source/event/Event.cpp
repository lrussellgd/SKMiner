////////////////////////////////////////////////
// File:	Event.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "Event.h"

#include <cstdarg>


///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
Event::Event()
{

}

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
Event::Event(const EVENT_TYPE& enmEventType, void* pA, void* pB, void* pC, void* pD, bool bDeallocateCustom)
{
	this->m_enmEventType = enmEventType;
		
	m_pParam[0] = pA;
	m_pParam[1] = pB;
	m_pParam[2] = pC;
	m_pParam[3] = pD;
	m_bDeallocateCustom = bDeallocateCustom;

	// Initialize all custom parameters to NULL.
	for (unsigned int x = 0; x < 4; ++x)
		m_pCustomParam[x] = NULL;
}


///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
Event::Event(const Event& rEvent) 
	: m_enmEventType(rEvent.GetEventType()), m_bDeallocateCustom(rEvent.GetDeallocateCustom())
{
	// Copy the parameters.
	m_pParam[0] = rEvent.m_pParam[0];
	m_pParam[1] = rEvent.m_pParam[1];
	m_pParam[2] = rEvent.m_pParam[2];
	m_pParam[3] = rEvent.m_pParam[3];

	// Since they hold the data... don't deallocate.
	m_bDeallocateCustom = false;

	// Copy the custom paramters.
	for (unsigned int x = 0; x < 4; ++x)
		m_pCustomParam[x] = rEvent.m_pCustomParam[x];

}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
Event& Event::operator = (const Event& rEvent)
{
	this->m_enmEventType = rEvent.GetEventType();


	// Copy the parameters.
	m_pParam[0] = rEvent.m_pParam[0];
	m_pParam[1] = rEvent.m_pParam[1];
	m_pParam[2] = rEvent.m_pParam[2];
	m_pParam[3] = rEvent.m_pParam[3];

	// Since they hold the data... don't deallocate.
	m_bDeallocateCustom = false;

	// Copy the custom paramters.
	for (unsigned int x = 0; x < 4; ++x)
	{
		m_pCustomParam[x] = rEvent.m_pCustomParam[x];
	}

	return *this;
}

bool Event::operator == (const Event& rEvent)
{
	// Check the ID.
	if (m_enmEventType != rEvent.GetEventType())
	{
		return false;
	}

	// Check the parameters.
	for (unsigned int x = 0; x < 4; ++x)
	{
		if (m_pParam[x] != rEvent.m_pParam[x])
		{
			return false;
		}
	}	
}

Event::~Event()
{
	ClearCustomParameters();
}

void Event::ClearCustomParameters()
{
	// Loop through each custom parameter.
	for (unsigned int x = 0; x < 4; ++x)
	{
		// Is there data there?
		if (m_pCustomParam[x] != NULL)
		{
			// Delete it.
			delete m_pCustomParam[x];

			// Reset it.
			m_pCustomParam[x] = NULL;
		}
	}
}