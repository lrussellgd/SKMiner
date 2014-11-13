////////////////////////////////////////////////
// File:	DDOS_Score.cpp
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "DDOS_Score.h"
#include "types.h"

namespace LLP
{
	DDOS_Score::DDOS_Score()
	{
	}


	DDOS_Score::DDOS_Score(int nTimespan)
	{
		for(int i = 0; i < nTimespan; i++)
			this->SCORE.push_back(std::make_pair(true, 0));
				
		this->TIMER = new Timer();
		this->TIMER->Start();
		this->nIterator = 0;
	}

	DDOS_Score::DDOS_Score(const DDOS_Score& score)
	{
		this->TIMER = new Timer(*score.GetTimer());
		this->nIterator = score.GetIterator();

		const std::vector<std::pair<bool,int>> otherSCORE = score.GetScores();
		for(int index = 0; index < otherSCORE.size(); ++index)
		{
			this->SCORE.push_back(std::make_pair(otherSCORE[index].first, otherSCORE[index].second));
		}
	}

	DDOS_Score& DDOS_Score::operator=(const DDOS_Score& score)
	{
		this->TIMER = new Timer(*score.GetTimer());
		this->nIterator = score.GetIterator();

		const std::vector<std::pair<bool,int>> otherSCORE = score.GetScores();
		for(int index = 0; index < otherSCORE.size(); ++index)
		{
			this->SCORE.push_back(std::make_pair(otherSCORE[index].first, otherSCORE[index].second));
		}

		return *this;
	}
	
	DDOS_Score::~DDOS_Score()
	{
		if(TIMER)
		{
			TIMER->Stop();
			delete(TIMER);
		}
	}

	int DDOS_Score::Score()
	{
		int nMovingAverage = 0;
		for(int i = 0; i < SCORE.size(); i++)
			nMovingAverage += SCORE[i].second;
			
		return nMovingAverage / ((int)SCORE.size());
	}

	DDOS_Score & DDOS_Score::operator++(int)
	{
		int nTime = TIMER->Elapsed();
		if(nTime >= SCORE.size())
		{
			Reset();
			nTime -= (int)(SCORE.size());
				
			printf("reset\n");
		}
				
			
		for(int i = nIterator; i <= nTime; i++)
		{
			if(!SCORE[i].first)
			{
				SCORE[i].first  = true;
				SCORE[i].second = 0;
			}
		}
					
		SCORE[nTime].second++;
		nIterator = nTime;

		return *this;
	}



	void DDOS_Score::Reset()
	{
		for(int i = 0; i < SCORE.size(); i++)
			SCORE[i].first = false;
				
		TIMER->Reset();
		nIterator = 0;
	}
}