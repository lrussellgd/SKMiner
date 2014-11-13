////////////////////////////////////////////////
// File:	Timer.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//
//Definition:	
//		Class the tracks the duration of time elapsed in seconds or milliseconds.
//		Used for socket timers to determine time outs.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _TIMER_H_
#define _TIMER_H_

#include "types.h"

namespace LLP
{

class Timer
	{
	private:
		boost::posix_time::ptime TIMER_START, TIMER_END;
		bool m_bStopped;
	
	public:

		Timer();
		Timer(const Timer& timer);
		Timer& operator=(const Timer& timer);
		~Timer();

		void Start();
		void Reset();		
		void Stop();		
		unsigned int Elapsed();		
		unsigned int ElapsedMilliseconds();

		
		///////////////////////////////////////////////////////////////////////////////
		//Accessors
		///////////////////////////////////////////////////////////////////////////////
		const bool						GetIsStopped()		const	{	return this->m_bStopped;	}
		const boost::posix_time::ptime&	GetTimerStart()		const	{	return this->TIMER_START;	}
		const boost::posix_time::ptime&	GetTimerEnd()		const	{	return this->TIMER_END;		}

	};
}

#endif //_TIMER_H_