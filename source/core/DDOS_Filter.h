////////////////////////////////////////////////
// File:	DDOS_Filter.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _DDOS_FILTER_H_
#define _DDOS_FILTER_H_


#include "DDOS_Score.h"

namespace LLP
{
	class Timer;

	class DDOS_Filter
	{

	private:

		DDOS_Score* rSCORE;
		DDOS_Score* cSCORE;
		Timer* TIMER;
		unsigned int BANTIME, TOTALBANS;
		
	public:
		
		DDOS_Filter();
		DDOS_Filter(unsigned int nTimespan);
		DDOS_Filter(const DDOS_Filter& filter);
		DDOS_Filter& operator=(const DDOS_Filter& filter);
		~DDOS_Filter();		


			
		void Ban();
		
		///////////////////////////////////////////////////////////////////////////////
		//Accessors
		///////////////////////////////////////////////////////////////////////////////
		const bool			GetIsBanned()	const	{	return (TIMER->Elapsed() < BANTIME);	}
		const unsigned int	GetBanTime()	const	{	return this->BANTIME;					}
		const unsigned int	GetTotalBans()	const	{	return this->TOTALBANS;					}
		const DDOS_Score*	GetRScore()		const	{	return this->rSCORE;					}
		const DDOS_Score*	GetCScore()		const	{	return this->cSCORE;					}
		const Timer*		GetTimer()		const	{	return this->TIMER;						}

		///////////////////////////////////////////////////////////////////////////////
		//Mutators
		///////////////////////////////////////////////////////////////////////////////
		void SetRScore(DDOS_Score* score)			{	this->rSCORE = score;				}
		void SetCScore(DDOS_Score* score)			{	this->rSCORE = score;				}
	};
}

#endif //_DDOS_FILTER_H_