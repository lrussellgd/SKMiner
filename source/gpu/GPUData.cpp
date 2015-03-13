////////////////////////////////////////////////
// File:	GPUData.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#include "GPUData.h"
#include "BaseGPU.h"
#include "../compute/BaseComputeDevice.h"
#include "../compute/CLDevice.h"
///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
GPUData::GPUData()
{
	this->m_enmEntityType = ENTITY_TYPE::GPU_DATA;
	this->m_bIsActive = false;
	this->m_bIsDisabled = false;
	this->m_nID = -1;
	this->m_unHashes = 0;
	this->m_unMaxHashes = 0;
	this->m_pDevice = NULL;
	this->m_iAdapterIndex = -1;
	this->m_unHardwareErrors = 0;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
GPUData::GPUData(const GPUData& gpuData) : Entity(gpuData)
{
	this->m_bIsActive = gpuData.GetIsActive();
	this->m_bIsDisabled = gpuData.GetIsDisabled();
	this->m_nID = gpuData.GetID();
<<<<<<< HEAD
	this->m_unHashes = gpuData.GetHashes();
	this->m_unMaxHashes = gpuData.GetMaxHashes();
=======
	this->m_nDeviceID = gpuData.GetDeviceID();
	this->m_unWorkSize = gpuData.GetWorkSize();
	this->m_nGPUEngine = gpuData.GetGPUEngine();
	this->m_nMinEngine = gpuData.GetMinEngine();
	this->m_nGPUFan = gpuData.GetGPUFan();
	this->m_nMinFan = gpuData.GetMinFan();
	this->m_nGPUMemclock = gpuData.GetGPUMemclock();
	this->m_nGPUMemDiff = gpuData.GetGPUMemDiff();
	this->m_nGPUPowertune = gpuData.GetGPUPowerTune();
	this->m_unHashes = gpuData.GetHashes();
	this->m_unMaxHashes = gpuData.GetMaxHashes();
	this->m_fGPUVDDC = gpuData.GetGPUVDDC();
	this->m_fGPUTemp = gpuData.GetGPUTemp();
	this->m_szDevicePath = gpuData.GetDevicePath();	
	this->m_pGPU =  gpuData.GetGPU();
>>>>>>> origin/master
	this->m_pDevice = gpuData.GetDevice();
	this->m_iAdapterIndex = gpuData.GetAdapterIndex();
	this->m_unHardwareErrors = gpuData.GetHardwareErrors();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
GPUData& GPUData::operator=(const GPUData& gpuData)
{
	this->m_enmEntityType = gpuData.GetEntityType();
	this->m_bIsActive = gpuData.GetIsActive();
	this->m_bIsDisabled = gpuData.GetIsDisabled();
	this->m_nID = gpuData.GetID();
<<<<<<< HEAD
	this->m_unHashes = gpuData.GetHashes();
	this->m_unMaxHashes = gpuData.GetMaxHashes();
=======
	this->m_nDeviceID = gpuData.GetDeviceID();
	this->m_unWorkSize = gpuData.GetWorkSize();
	this->m_nGPUEngine = gpuData.GetGPUEngine();
	this->m_nMinEngine = gpuData.GetMinEngine();
	this->m_nGPUFan = gpuData.GetGPUFan();
	this->m_nMinFan = gpuData.GetMinFan();
	this->m_nGPUMemclock = gpuData.GetGPUMemclock();
	this->m_nGPUMemDiff = gpuData.GetGPUMemDiff();
	this->m_nGPUPowertune = gpuData.GetGPUPowerTune();
	this->m_unHashes = gpuData.GetHashes();
	this->m_unMaxHashes = gpuData.GetMaxHashes();
	this->m_fGPUVDDC = gpuData.GetGPUVDDC();
	this->m_fGPUTemp = gpuData.GetGPUTemp();
	this->m_szDevicePath = gpuData.GetDevicePath();	
	this->m_pGPU = gpuData.GetGPU();
>>>>>>> origin/master
	this->m_pDevice = gpuData.GetDevice();
	this->m_iAdapterIndex = gpuData.GetAdapterIndex();
	this->m_unHardwareErrors = gpuData.GetHardwareErrors();

	return *this;
}



///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
GPUData::~GPUData()
{
	if(m_pDevice)
	{
		delete(m_pDevice);
		m_pDevice = NULL;
	}
}

GPUData* GPUData::Clone()
{
	GPUData* pGPUData = new GPUData();

	pGPUData->SetIsActive(this->m_bIsActive);
	pGPUData->SetIsDisabled(this->m_bIsDisabled);

	pGPUData->SetID(this->m_nID);
	pGPUData->SetHashes(this->m_unHashes);
	pGPUData->SetMaxHashes(this->m_unMaxHashes);
<<<<<<< HEAD
=======
	pGPUData->SetGPUVDDC(this->m_fGPUVDDC);
	pGPUData->SetGPUTemp(this->m_fGPUTemp);
	pGPUData->SetWorksize(this->m_unWorkSize);
	pGPUData->SetDevicePath(this->m_szDevicePath);
	pGPUData->SetGPU(this->m_pGPU);
>>>>>>> origin/master
	pGPUData->SetDevice(this->m_pDevice);
	pGPUData->SetAdapterIndex(this->m_iAdapterIndex);
	pGPUData->SetHardwareErrors(this->m_unHardwareErrors);

	return pGPUData;

}

GPUData* GPUData::DeepCopy()
{
	GPUData* pGPUData = new GPUData();

	pGPUData->SetIsActive(this->m_bIsActive);
	pGPUData->SetIsDisabled(this->m_bIsDisabled);

	pGPUData->SetID(this->m_nID);
	pGPUData->SetHashes(this->m_unHashes);
	pGPUData->SetMaxHashes(this->m_unMaxHashes);
<<<<<<< HEAD
=======
	pGPUData->SetGPUVDDC(this->m_fGPUVDDC);
	pGPUData->SetGPUTemp(this->m_fGPUTemp);
	pGPUData->SetWorksize(this->m_unWorkSize);
	pGPUData->SetDevicePath(this->m_szDevicePath);
	pGPUData->SetGPU(this->m_pGPU->DeepCopy());
>>>>>>> origin/master
	pGPUData->SetDevice(this->m_pDevice->DeepCopy());
	pGPUData->SetAdapterIndex(this->m_iAdapterIndex);
	pGPUData->SetHardwareErrors(this->m_unHardwareErrors);

	return pGPUData;
}
