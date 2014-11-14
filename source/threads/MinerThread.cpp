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
}

MinerThread::MinerThread(MinerData* pData)
{
	m_pMinerData = pData;
	m_bBlockFound =false; 
	m_unHashes = 0;
	m_pTHREAD = boost::thread(&MinerThread::Miner, this);
	total_mhashes_done = 0;
}

MinerThread::MinerThread(const MinerThread& miner)
{
	m_pMinerData = miner.GetMinerData();
	m_bBlockFound = miner.GetIsBlockFound(); 
	m_bNewBlock = miner.GetIsNewBlock();
	m_bReady = miner.GetIsReady();
	m_unHashes = miner.GetHashes();
	m_pTHREAD = boost::thread(&MinerThread::Miner, this);
}

MinerThread& MinerThread::operator=(const MinerThread& miner)
{
	m_pMinerData = miner.GetMinerData();
	m_bBlockFound = miner.GetIsBlockFound(); 
	m_bNewBlock = miner.GetIsNewBlock();
	m_bReady = miner.GetIsReady();
	m_unHashes = miner.GetHashes();
	m_pTHREAD = boost::thread(&MinerThread::Miner, this);

	return *this;
}

MinerThread::~MinerThread()
{
	if (m_pMinerData)
	{
		delete(m_pMinerData);
	}

}