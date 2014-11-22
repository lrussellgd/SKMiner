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
	this->m_pMinerData = NULL;
	this->m_bShutown = this->m_bDidShutDown = false;
	this->m_clLock = new boost::mutex();
}

MinerThread::MinerThread(MinerData* pData)
{
	m_pMinerData = pData;
	m_bBlockFound =false; 
	m_unHashes = 0;
	m_pTHREAD = new boost::thread(&MinerThread::Miner, this);
	total_mhashes_done = 0;
	this->m_bShutown = this->m_bDidShutDown = false;
	this->m_clLock = new boost::mutex();
}

MinerThread::MinerThread(const MinerThread& miner)
{
	m_pMinerData = miner.GetMinerData();
	m_bBlockFound = miner.GetIsBlockFound(); 
	m_bNewBlock = miner.GetIsNewBlock();
	m_bReady = miner.GetIsReady();
	m_unHashes = miner.GetHashes();
	m_pTHREAD = new boost::thread(&MinerThread::Miner, this);
	this->m_bShutown = miner.GetIsShuttingDown();
	this->m_bDidShutDown = miner.GetDidShutDown();
	this->m_clLock = new boost::mutex();
}

MinerThread& MinerThread::operator=(const MinerThread& miner)
{
	m_pMinerData = miner.GetMinerData();
	m_bBlockFound = miner.GetIsBlockFound(); 
	m_bNewBlock = miner.GetIsNewBlock();
	m_bReady = miner.GetIsReady();
	m_unHashes = miner.GetHashes();
	m_pTHREAD = new boost::thread(&MinerThread::Miner, this);
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