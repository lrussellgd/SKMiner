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
	this->m_enmEntityType = ENTITY_TYPE::SK_MINER_DATA;
	this->m_pTarget = NULL;
}

SKMinerData::SKMinerData(const SKMinerData& minerData) : MinerData(minerData)
{
	this->SetTarget(minerData.GetTarget());
}

SKMinerData& SKMinerData::operator=(const SKMinerData& minerData)
{
	this->m_enmEntityType = minerData.GetEntityType();
	this->SetGPUData(minerData.GetGPUData());
	this->SetBlock(minerData.GetBlock());
	this->SetTarget(minerData.GetTarget());

	return *this;
}

SKMinerData::~SKMinerData()
{
	MinerData::~MinerData();
	BN_free(this->m_pTarget);
	if (this->m_pTarget)
	{
		delete(this->m_pTarget);
		this->m_pTarget = NULL;
	}
}

SKMinerData* SKMinerData::Clone()
{
	SKMinerData* pSKData = new SKMinerData();

	pSKData->SetGPUData(this->m_pGPUData);
	pSKData->SetBlock(this->m_pBLOCK);
	pSKData->SetTarget(this->GetTarget());

	return pSKData;

}

SKMinerData* SKMinerData::DeepCopy()
{
	SKMinerData* pSKData = new SKMinerData();

	pSKData->SetGPUData(this->m_pGPUData->DeepCopy());
	pSKData->SetBlock(new Core::CBlock(*this->m_pBLOCK));

	CBigNum* pBigNum = new CBigNum(*this->GetTarget());
	pSKData->SetTarget(pBigNum);

	return pSKData;
}
