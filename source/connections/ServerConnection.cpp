////////////////////////////////////////////////
// File:	ServerConnection.cpp
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "ServerConnection.h"
#include "../data/MinerData.h"

ServerConnection::ServerConnection()
{
	this->nThreads = this->nTimeout = 0;
}

ServerConnection& ServerConnection::operator=(const ServerConnection& serverConnection)
{
	return *this;
}		

ServerConnection::~ServerConnection()
{
	for (int index = 0; index < THREADS.size(); ++index)
	{
		MinerThread* pMinerThread = THREADS[index];
		if (pMinerThread)
		{
			delete(pMinerThread);
		}
	}

	THREADS.clear();
}

//Reset the block on each of the Threads.
void ServerConnection::ResetThreads()
{
	/** Reset each individual flag to tell threads to stop mining. **/
	for(int nIndex = 0; nIndex < THREADS.size(); nIndex++)
	{
		THREADS[nIndex]->SetIsBlockFound(false);
		THREADS[nIndex]->SetIsNewBlock(true);
	}
}

//Get the total Hashes from Each Mining Thread.
//Then reset their counter.
unsigned int ServerConnection::Hashes()
{	
	unsigned int nHashes = 0;
	for(int nIndex = 0; nIndex < THREADS.size(); nIndex++)
	{
		nHashes += THREADS[nIndex]->GetHashes();
		THREADS[nIndex]->SetHashes(0);
	}
			
	return nHashes;
}