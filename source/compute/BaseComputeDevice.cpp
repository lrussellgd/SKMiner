////////////////////////////////////////////////
// File:	BaseComputeDevice.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "BaseComputeDevice.h"

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
BaseComputeDevice::BaseComputeDevice()
{
	m_unVendorID = 0;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
BaseComputeDevice::BaseComputeDevice(const BaseComputeDevice& device)
{
	this->m_szAlgorithm = device.GetAlgorithm();
	this->m_unVendorID = device.GetVendorID();
	this->m_szVendorName = device.GetVendorName();
	this->m_szName = device.GetName();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
BaseComputeDevice& BaseComputeDevice::operator=(const BaseComputeDevice& device)
{
	this->m_szAlgorithm = device.GetAlgorithm();
	this->m_unVendorID = device.GetVendorID();
	this->m_szVendorName = device.GetVendorName();
	this->m_szName = device.GetName();

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
BaseComputeDevice::~BaseComputeDevice()
{
}

BaseComputeDevice* BaseComputeDevice::Clone()
{
	BaseComputeDevice* pBCD = new BaseComputeDevice();

	pBCD->SetVendorID(this->m_unVendorID);
	pBCD->SetVendorName(this->m_szVendorName);
	pBCD->SetName(this->m_szName);
	pBCD->SetAlgorithm(this->m_szAlgorithm);

	return pBCD;
}

BaseComputeDevice* BaseComputeDevice::DeepCopy()
{
	BaseComputeDevice* pBCD = new BaseComputeDevice();

	pBCD->SetVendorID(this->m_unVendorID);
	pBCD->SetVendorName(this->m_szVendorName);
	pBCD->SetName(this->m_szName);
	pBCD->SetAlgorithm(this->m_szAlgorithm);

	return pBCD;
}