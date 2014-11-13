////////////////////////////////////////////////
// File:	DDOS_Score.h
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
//		Class that tracks DDOS attempts on LLP Servers. 
//		Uses a Timer to calculate Request Score [rScore] 
//		and Connection Score [cScore] as a unit of Score / Second. 
//		Pointer stored by Connection class and Server Listener DDOS_MAP.
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef _DDOS_SCORE_H_
#define _DDOS_SCORE_H_

#include "Timer.h"

#include <vector>

namespace LLP
{
	class DDOS_Score
	{
	private:

		std::vector<std::pair<bool,int>> SCORE;
		Timer* TIMER;
		int nIterator;
		
		void Reset();
		
	public:

		DDOS_Score();
		DDOS_Score(int nTimespan);
		DDOS_Score(const DDOS_Score& score);
		DDOS_Score& operator=(const DDOS_Score& score);
		~DDOS_Score();		
		
		int Score();		
		DDOS_Score & operator++(int);


		///////////////////////////////////////////////////////////////////////////////
		//Accessors
		///////////////////////////////////////////////////////////////////////////////
		const std::vector<std::pair<bool,int>>	GetScores()		const	{	return this->SCORE;		}
		const int								GetIterator()	const	{	return this->nIterator;	}
		const Timer*							GetTimer()		const	{	return this->TIMER;		}

		///////////////////////////////////////////////////////////////////////////////
		//Mutators
		///////////////////////////////////////////////////////////////////////////////

	};

}

#endif //_DDOS_SCORE_H_