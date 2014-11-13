////////////////////////////////////////////////
// File:	ServerConnection.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _SERVERCONNECTION_H_
#define _SERVERCONNECTION_H_

#include "../core/types.h"
#include "../core/Miner.h"
#include "../threads/MinerThread.h"

class GPUData;

class ServerConnection
{
protected:

	LLP::Miner* CLIENT;
	int nThreads, nTimeout;
	std::vector<MinerThread*> THREADS;
	LLP::Thread_t THREAD;
	LLP::Timer    TIMER;
	std::string   IP, PORT;
public:

	ServerConnection();
	ServerConnection& operator=(const ServerConnection& serverConnection);
	~ServerConnection();

	void ResetThreads();
	unsigned int Hashes();
    
	virtual void ServerThread() = 0;
};

#endif //_SERVERCONNECTION_H_