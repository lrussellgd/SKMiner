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
	this->m_nThreads = this->m_nTimeout = 0;
	this->m_bIsShutDown = this->m_bDidShutDown = false;
}

ServerConnection::ServerConnection(const ServerConnection& serverConnection)
{
	this->m_nThreads = serverConnection.GetThreads();
	this->m_nTimeout = serverConnection.GetTimeout();
	this->m_bIsShutDown - serverConnection.GetIsShuttingDown();
	this->m_bDidShutDown = serverConnection.GetDidShutDown();
	this->m_pCLIENT = serverConnection.GetClient();
	this->m_vecTHREADS = serverConnection.GetMiningThreads();
	this->m_tTIMER = serverConnection.GetTimer();
	this->m_szIP = serverConnection.GetIP();
	this->m_szPORT = serverConnection.GetPort();
}

ServerConnection& ServerConnection::operator=(const ServerConnection& serverConnection)
{
	this->m_nThreads = serverConnection.GetThreads();
	this->m_nTimeout = serverConnection.GetTimeout();
	this->m_bIsShutDown - serverConnection.GetIsShuttingDown();
	this->m_bDidShutDown = serverConnection.GetDidShutDown();
	this->m_pCLIENT = serverConnection.GetClient();
	this->m_vecTHREADS = serverConnection.GetMiningThreads();
	this->m_tTIMER = serverConnection.GetTimer();
	this->m_szIP = serverConnection.GetIP(); 
	this->m_szPORT = serverConnection.GetPort();

	return *this;
}		

ServerConnection::~ServerConnection()
{
	for (int index = 0; index < m_vecTHREADS.size(); ++index)
	{
		MinerThread* pMinerThread = m_vecTHREADS[index];
		if (pMinerThread)
		{
			pMinerThread->SetIsShuttingDown(true);

			while (!pMinerThread->GetDidShutDown()){}

			delete(pMinerThread);
			pMinerThread = NULL;
		}
	}

	m_vecTHREADS.clear();

	m_thTHREAD.detach();

	if (m_pCLIENT->Connected())
	{
		m_pCLIENT->Disconnect();
	}

	if (m_pCLIENT)
	{
		delete(m_pCLIENT);
		m_pCLIENT = NULL;
	}
}

//Reset the block on each of the Threads.
void ServerConnection::ResetThreads()
{
	/** Reset each individual flag to tell threads to stop mining. **/
	for (int nIndex = 0; nIndex < m_vecTHREADS.size(); nIndex++)
	{
		m_vecTHREADS[nIndex]->SetIsBlockFound(false);
		m_vecTHREADS[nIndex]->SetIsNewBlock(true);
	}
}

//Get the total Hashes from Each Mining Thread.
//Then reset their counter.
double ServerConnection::Hashes()
{	
	unsigned int unElapsed = m_tTIMER.ElapsedMilliseconds();

	double dHashes = 0;

	for (int nIndex = 0; nIndex < m_vecTHREADS.size(); nIndex++)
	{
		double KHASH = (double)m_vecTHREADS[nIndex]->GetHashes() / unElapsed;
		m_vecTHREADS[nIndex]->SetLastHashRate(KHASH);

		dHashes += KHASH;
		m_vecTHREADS[nIndex]->SetHashes(0);
	}
			
	return dHashes;
}