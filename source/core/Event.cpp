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

namespace Events
{

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	Event::Event()
	{

	}

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	Event::Event(const Event& rEvent)
	{
		this->m_szEventName = rEvent.GetName();

		std::map<std::string, void*> paramMap = rEvent.GetParameters();
		for (auto iter = paramMap.begin(); iter != paramMap.end(); ++iter)
		{
			this->m_mapParams[iter->first] = iter->second;
		}

	}

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	Event& Event::operator = (const Event& rEvent)
	{
		this->m_szEventName = rEvent.GetName();

		std::map<std::string, void*> paramMap = rEvent.GetParameters();
		for (auto iter = paramMap.begin(); iter != paramMap.end(); ++iter)
		{
			this->m_mapParams[iter->first] = iter->second;
		}

		return *this;
	}

	Event::~Event()
	{
		for (auto iter = this->m_mapParams.begin(); iter != this->m_mapParams.end(); ++iter)
		{
			void* pParam = iter->second;
			if (pParam)
			{
				delete(pParam);
				pParam = NULL;
			}
		}
	}
}