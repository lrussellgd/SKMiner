////////////////////////////////////////////////
// File:	SKMinerThread.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _SKMINERTHREAD_H_
#define _SKMINERTHREAD_H_

#include "MinerThread.h"

class SKMinerData;

class SKMinerThread : public MinerThread
{

public:

	SKMinerThread();
	SKMinerThread(SKMinerData* pData);
	SKMinerThread(const SKMinerThread& miner);
	SKMinerThread& operator=(const SKMinerThread& miner);
	~SKMinerThread();

	void Miner();

};

#endif //_SKMINERTHREAD_H_