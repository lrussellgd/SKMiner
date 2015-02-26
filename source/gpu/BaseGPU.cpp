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
	this->m_enmEntityType = ENTITY_TYPE::BASE_GPU;
	this->m_bIsManaged = false;
	this->m_nGPUID = 0;
	this->m_pTwin = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
BaseGPU::BaseGPU(const BaseGPU& baseGPU) : Entity(baseGPU)
{
	this->m_bIsManaged = baseGPU.GetIsManaged();
	this->m_nGPUID = baseGPU.GetGPUID();
	this->m_pTwin = baseGPU.GetTwin();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
BaseGPU& BaseGPU::operator=(const BaseGPU& baseGPU)
{
	this->m_enmEntityType = baseGPU.GetEntityType();
	this->m_bIsManaged = baseGPU.GetIsManaged();
	this->m_nGPUID = baseGPU.GetGPUID();
	this->m_pTwin = baseGPU.GetTwin();

	return *this;
}

BaseGPU* BaseGPU::Clone()
{
	BaseGPU* pGPU = new BaseGPU();

	pGPU->SetGPUID(this->m_nGPUID);
	pGPU->SetIsManaged(this->m_bIsManaged);
	pGPU->SetTwin(this->m_pTwin);

	return pGPU;
}

BaseGPU* BaseGPU::DeepCopy()
{
	BaseGPU* pGPU = new BaseGPU();

	pGPU->SetGPUID(this->m_nGPUID);
	pGPU->SetIsManaged(this->m_bIsManaged);
	pGPU->SetTwin(this->m_pTwin->DeepCopy());

	return pGPU;
}