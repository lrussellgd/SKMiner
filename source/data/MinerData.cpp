////////////////////////////////////////////////
// File:	MinerData.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "MinerData.h"

#include "../core/CBlock.h"
#include "../gpu/GPUData.h"

MinerData::MinerData()
{
	this->m_enmEntityType = ENTITY_TYPE::MINER_DATA;
	this->m_pBLOCK = NULL;
	this->m_pGPUData = NULL;
}

MinerData::MinerData(const MinerData& minerData) : Entity(minerData)
{
	this->m_pBLOCK = minerData.GetBlock();
	this->m_pGPUData = minerData.GetGPUData();
}

MinerData& MinerData::operator=(const MinerData& minerData)
{
	this->m_enmEntityType = minerData.GetEntityType();
	this->m_pBLOCK = minerData.GetBlock();
	this->m_pGPUData = minerData.GetGPUData();

	return *this;
}

MinerData::~MinerData()
{
	if (m_pBLOCK)
	{
		delete(m_pBLOCK);
		m_pBLOCK = NULL;
	}

	if (m_pGPUData)
	{
		delete(m_pGPUData);
		m_pGPUData = NULL;
	}
}

MinerData* MinerData::Clone()
{
	MinerData* minerData = new MinerData();

	minerData->SetBlock(this->m_pBLOCK);
	minerData->SetGPUData(this->m_pGPUData);

	return minerData;
}

MinerData* MinerData::DeepCopy()
{
	MinerData* minerData = new MinerData();

	minerData->SetBlock(new Core::CBlock(*this->m_pBLOCK));
	minerData->SetGPUData(this->m_pGPUData->DeepCopy());

	return minerData;
}