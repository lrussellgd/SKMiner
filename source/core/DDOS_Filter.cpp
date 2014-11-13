////////////////////////////////////////////////
// File:	DDOS_Filter.cpp
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "DDOS_Filter.h"
#include "Timer.h"
#include "types.h"

namespace LLP
{
	DDOS_Filter::DDOS_Filter()
	{
		this->BANTIME = 0;
		this->TOTALBANS = 0;
		TIMER = new Timer();
	}

	DDOS_Filter::DDOS_Filter(unsigned int nTimespan)
	{
		rSCORE = new DDOS_Score(nTimespan);
		cSCORE = new DDOS_Score(nTimespan);
		BANTIME = 0;
		TOTALBANS = 0;
	}

	DDOS_Filter::DDOS_Filter(const DDOS_Filter& filter)
	{
		rSCORE = new DDOS_Score(*filter.GetRScore());
		cSCORE = new DDOS_Score(*filter.GetCScore());
		TIMER = new Timer(*filter.GetTimer());
		BANTIME = filter.GetBanTime();
		TOTALBANS = filter.GetTotalBans();
	}

	DDOS_Filter& DDOS_Filter::operator=(const DDOS_Filter& filter)
	{
		rSCORE = new DDOS_Score(*filter.GetRScore());
		cSCORE = new DDOS_Score(*filter.GetCScore());
		TIMER = new Timer(*filter.GetTimer());
		BANTIME = filter.GetBanTime();
		TOTALBANS = filter.GetTotalBans();

		return *this;
	}

	DDOS_Filter::~DDOS_Filter()
	{
		if(TIMER)
		{
			TIMER->Stop();
			delete(TIMER);
		}

		if(rSCORE)
		{
			delete(rSCORE);
		}

		if(cSCORE)
		{
			delete(cSCORE);
		}
	}

	void DDOS_Filter::Ban()
	{
		TIMER->Start();
		TOTALBANS++;
			
		BANTIME = std::max((unsigned int)(TOTALBANS * rSCORE->Score() * cSCORE->Score()), (unsigned int)60);
	}		
}