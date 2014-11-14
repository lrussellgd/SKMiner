////////////////////////////////////////////////
// File:	SKMinerData.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "SKMinerData.h"
#include "../core/CBlock.h"
#include "../core/bignum.h"
#include "../gpu/GPUData.h"

SKMinerData::SKMinerData() : MinerData()
{
	this->m_pTarget = NULL;
}

SKMinerData::SKMinerData(const SKMinerData& minerData) : MinerData(minerData)
{
	this->m_pTarget = minerData.GetTarget();
}

SKMinerData& SKMinerData::operator=(const SKMinerData& minerData)
{
	this->m_pTarget = minerData.GetTarget();
	return *this;
}

SKMinerData::~SKMinerData()
{
	MinerData::~MinerData();

	if (m_pTarget)
	{
		delete(m_pTarget);
	}
}

SKMinerData* SKMinerData::Clone()
{
	SKMinerData* pSKData = new SKMinerData();

	pSKData->SetGPUData(this->m_pGPUData);
	pSKData->SetBlock(this->m_pBLOCK);
	pSKData->SetTarget(this->m_pTarget);

	return pSKData;

}

SKMinerData* SKMinerData::DeepCopy()
{
	SKMinerData* pSKData = new SKMinerData();

	pSKData->SetGPUData(this->m_pGPUData->DeepCopy());
	pSKData->SetBlock(new Core::CBlock(*this->m_pBLOCK));
	pSKData->SetTarget(new CBigNum(*this->m_pTarget));

	return pSKData;
}
