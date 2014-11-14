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
	this->m_pBLOCK = NULL;
	this->m_pGPUData = NULL;

}

MinerData::MinerData(const MinerData& minerData)
{
	this->m_pBLOCK = minerData.GetBlock();
	this->m_pGPUData = minerData.GetGPUData();
}

MinerData& MinerData::operator=(const MinerData& minerData)
{
	this->m_pBLOCK = minerData.GetBlock();
	this->m_pGPUData = minerData.GetGPUData();

	return *this;
}

MinerData::~MinerData()
{
	if (m_pBLOCK)
	{
		delete(m_pBLOCK);
	}

	if (m_pGPUData)
	{
		delete(m_pGPUData);
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