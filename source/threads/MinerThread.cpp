////////////////////////////////////////////////
// File:	MinerThread.cpp
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "MinerThread.h"
#include "../kernel/KernelFuncs.h"
#include "../gpu/BaseGPU.h"
#include "../core/util.h"
#include "../data/MinerData.h"


MinerThread::MinerThread()
{
	this->m_bBlockFound = this->m_bNewBlock = this->m_bReady = false;
	this->m_unHashes = 0;
	this->total_mhashes_done = 0;
	this->m_dLastHashRate = 0.0;
	this->m_stBlocksFound = this->m_stRejected = 0;
	this->m_pMinerData = NULL;
	this->m_bShutown = this->m_bDidShutDown = false;
	this->m_clLock = new boost::mutex();
}

MinerThread::MinerThread(MinerData* pData)
{
	this->m_pMinerData = pData;
	this->m_bBlockFound = false;
	this->m_unHashes = 0;
	this->m_pTHREAD = new boost::thread(&MinerThread::Miner, this);
	total_mhashes_done = 0;
	this->m_dLastHashRate = 0.0;
	this->m_stBlocksFound = this->m_stRejected = 0;
	this->m_bShutown = this->m_bDidShutDown = false;
	this->m_clLock = new boost::mutex();
}

MinerThread::MinerThread(const MinerThread& miner)
{
	this->m_pMinerData = miner.GetMinerData();
	this->m_bBlockFound = miner.GetIsBlockFound();
	this->m_bNewBlock = miner.GetIsNewBlock();
	this->m_bReady = miner.GetIsReady();
	this->m_unHashes = miner.GetHashes();
	this->m_stBlocksFound = miner.GetBlocksFound();
	this->m_stRejected = miner.GetRejected();
	this->m_dLastHashRate = miner.GetLastHashRate();
	this->m_pTHREAD = new boost::thread(&MinerThread::Miner, this);
	this->m_bShutown = miner.GetIsShuttingDown();
	this->m_bDidShutDown = miner.GetDidShutDown();
	this->m_clLock = new boost::mutex();
}

MinerThread& MinerThread::operator=(const MinerThread& miner)
{
	this->m_pMinerData = miner.GetMinerData();
	this->m_bBlockFound = miner.GetIsBlockFound();
	this->m_bNewBlock = miner.GetIsNewBlock();
	this->m_bReady = miner.GetIsReady();
	this->m_unHashes = miner.GetHashes();
	this->m_stBlocksFound = miner.GetBlocksFound();
	this->m_stRejected = miner.GetRejected();
	this->m_dLastHashRate = miner.GetLastHashRate();
	this->m_pTHREAD = new boost::thread(&MinerThread::Miner, this);
	this->m_bShutown = miner.GetIsShuttingDown();
	this->m_bDidShutDown = miner.GetDidShutDown();
	this->m_clLock = new boost::mutex();

	return *this;
}

MinerThread::~MinerThread()
{
	if (m_clLock)
	{
		m_clLock->destroy();
		delete(this->m_clLock);
		m_clLock = NULL;
	}
	
	if (m_pTHREAD)
	{
		//m_pTHREAD->detach();
		delete(m_pTHREAD);
	}
	

	if (m_pMinerData)
	{
		delete(m_pMinerData);
		m_pMinerData = NULL;
	}

}