////////////////////////////////////////////////
// File:	BaseGPU.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#include "BaseGPU.h"

#include "../config/GPUSetting.h"

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
BaseGPU::BaseGPU()
{
	this->m_bIsDefFanValid = this->m_bIsFanValid = this->m_bIsManaged = false;
	this->m_nGPUID = this->m_nAdapterIndex = this->m_nBusNumber = 0;
	this->m_nLastEngine = this->m_nLastTemp = 0;
	this->m_pTwin = NULL;
	this->m_pGPUSettings = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
BaseGPU::BaseGPU(const BaseGPU& baseGPU)
{
	this->m_bIsDefFanValid = baseGPU.GetIsDefFanValid();
	this->m_bIsFanValid = baseGPU.GetIsFanValid();
	this->m_bIsManaged = baseGPU.GetIsManaged();
	this->m_nAdapterIndex = baseGPU.GetAdapterIndex();
	this->m_nBusNumber = baseGPU.GetBusNumber();
	this->m_nLastEngine = baseGPU.GetLastEngine();
	this->m_nLastTemp = baseGPU.GetLastTemp();
	this->m_nGPUID = baseGPU.GetGPUID();
	this->m_pGPUSettings = baseGPU.GetGPUSetting();
	this->m_pTwin = baseGPU.GetTwin();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
BaseGPU& BaseGPU::operator=(const BaseGPU& baseGPU)
{
	this->m_bIsDefFanValid = baseGPU.GetIsDefFanValid();
	this->m_bIsFanValid = baseGPU.GetIsFanValid();
	this->m_bIsManaged = baseGPU.GetIsManaged();
	this->m_nAdapterIndex = baseGPU.GetAdapterIndex();
	this->m_nBusNumber = baseGPU.GetBusNumber();
	this->m_nLastEngine = baseGPU.GetLastEngine();
	this->m_nLastTemp = baseGPU.GetLastTemp();
	this->m_nGPUID = baseGPU.GetGPUID();
	this->m_pGPUSettings = baseGPU.GetGPUSetting();
	this->m_pTwin = baseGPU.GetTwin();

	return *this;
}

BaseGPU* BaseGPU::Clone()
{
	BaseGPU* pGPU = new BaseGPU();
	
	pGPU->SetAdapterIndex(this->m_nAdapterIndex);
	pGPU->SetBusNumber(this->m_nBusNumber);
	pGPU->SetGPUID(this->m_nGPUID);
	pGPU->SetGPUSetting(this->m_pGPUSettings);
	pGPU->SetIsDefFanValid(this->m_bIsDefFanValid);	
	pGPU->SetIsFanValid(this->m_bIsFanValid);
	pGPU->SetIsManaged(this->m_bIsManaged);
	pGPU->SetLastEngine(this->m_nLastEngine);
	pGPU->SetLastTemp(this->m_nLastTemp);
	pGPU->SetTwin(this->m_pTwin);

	return pGPU;
}

BaseGPU* BaseGPU::DeepCopy()
{
	BaseGPU* pGPU = new BaseGPU();

	pGPU->SetAdapterIndex(this->m_nAdapterIndex);
	pGPU->SetBusNumber(this->m_nBusNumber);
	pGPU->SetGPUID(this->m_nGPUID);
	pGPU->SetGPUSetting(this->m_pGPUSettings->DeepCopy());
	pGPU->SetIsDefFanValid(this->m_bIsDefFanValid);
	pGPU->SetIsFanValid(this->m_bIsFanValid);
	pGPU->SetIsManaged(this->m_bIsManaged);
	pGPU->SetLastEngine(this->m_nLastEngine);
	pGPU->SetLastTemp(this->m_nLastTemp);
	pGPU->SetTwin(this->m_pTwin->DeepCopy());

	return pGPU;
}