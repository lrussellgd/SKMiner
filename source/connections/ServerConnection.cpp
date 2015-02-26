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
	this->m_enmEntityType = ENTITY_TYPE::SERVER_CONNECTION;
	this->m_nThreads = this->m_nTimeout = this->m_nBestHeight = this->m_nNewBlocks = 0;
	this->m_bIsShutDown = this->m_bDidShutDown = false;
	this->m_bIsEnabled = true;
}

ServerConnection::ServerConnection(const ServerConnection& serverConnection) : Entity(serverConnection)
{
	this->m_bIsEnabled = serverConnection.GetIsEnabled();
	this->m_nThreads = serverConnection.GetThreads();
	this->m_nTimeout = serverConnection.GetTimeout();
	this->m_bIsShutDown - serverConnection.GetIsShuttingDown();
	this->m_bDidShutDown = serverConnection.GetDidShutDown();
	this->m_pCLIENT = serverConnection.GetClient();
	this->m_vecTHREADS = serverConnection.GetMiningThreads();
	this->m_tTIMER = serverConnection.GetTimer();
	this->m_totalTIMER = serverConnection.GetTotalTimer();
	this->m_szIP = serverConnection.GetIP();
	this->m_szPORT = serverConnection.GetPort();
	this->m_nBestHeight = serverConnection.GetBestHeight();
	this->m_nNewBlocks = this->GetNewBlocks();
}

ServerConnection& ServerConnection::operator=(const ServerConnection& serverConnection)
{
	this->m_enmEntityType = serverConnection.GetEntityType();
	this->m_bIsEnabled = serverConnection.GetIsEnabled();
	this->m_nThreads = serverConnection.GetThreads();
	this->m_nTimeout = serverConnection.GetTimeout();
	this->m_bIsShutDown - serverConnection.GetIsShuttingDown();
	this->m_bDidShutDown = serverConnection.GetDidShutDown();
	this->m_pCLIENT = serverConnection.GetClient();
	this->m_vecTHREADS = serverConnection.GetMiningThreads();
	this->m_tTIMER = serverConnection.GetTimer();
	this->m_totalTIMER = serverConnection.GetTotalTimer();
	this->m_szIP = serverConnection.GetIP(); 
	this->m_szPORT = serverConnection.GetPort();
	this->m_nBestHeight = serverConnection.GetBestHeight();
	this->m_nNewBlocks = this->GetNewBlocks();

	return *this;
}		

ServerConnection::~ServerConnection()
{
	m_tTIMER.Stop();
	m_totalTIMER.Stop();

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

	double loclSecs = ((double)unElapsed) / 1000.0;

	double dHashes = 0;

	for (int nIndex = 0; nIndex < m_vecTHREADS.size(); nIndex++)
	{
		double KHASH = ((double)m_vecTHREADS[nIndex]->GetHashes()) / ((double)unElapsed);
		m_vecTHREADS[nIndex]->SetLastHashRate(KHASH);

		dHashes += KHASH;
		m_vecTHREADS[nIndex]->SetHashes(0);
	}

	m_dCurrentHashses = dHashes;
	return dHashes;
}

int ServerConnection::GetTotalAccepted()
{
	int ttlAccepted = 0;
	for (int nIndex = 0; nIndex < m_vecTHREADS.size(); nIndex++)
	{
		ttlAccepted += m_vecTHREADS[nIndex]->GetBlocksFound();
	}

	return ttlAccepted;
}

int ServerConnection::GetTotalRejected()
{
	int ttlRejected = 0;
	for (int nIndex = 0; nIndex < m_vecTHREADS.size(); nIndex++)
	{
		ttlRejected += m_vecTHREADS[nIndex]->GetRejected();
	}

	return ttlRejected;
}

int ServerConnection::GetTotalHardwareErrors()
{
	int ttlHW = 0;
	for (int nIndex = 0; nIndex < m_vecTHREADS.size(); nIndex++)
	{
		ttlHW += m_vecTHREADS[nIndex]->GetMinerData()->GetGPUData()->GetHardwareErrors();
	}

	return ttlHW;
}

void ServerConnection::DecayTime(double fadd, double fsecs, double& fRet)
{
	double ftotal, fprop;

	fprop = 1.0 - 1 / (exp(fsecs / 10.0));
	ftotal = 1.0 + fprop;
	fRet += (fadd * fprop);
	fRet /= ftotal;
}