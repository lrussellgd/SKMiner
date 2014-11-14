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

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
GPUData::GPUData()
{
	this->m_bIsActive = false;
	this->m_bIsDisabled = false;
	this->m_nID = -1;
	this->m_nDeviceID = -1;
	this->m_unWorkSize = 0;
	this->m_nGPUEngine = 0;
	this->m_nMinEngine = 0;
	this->m_nGPUFan = 0;
	this->m_nMinFan = 0;
	this->m_nGPUMemclock = 0;
	this->m_nGPUMemDiff = 0;
	this->m_nGPUPowertune = 0;
	this->m_nGPUEngineExit = 0;
	this->m_nGPUMemclockExit = 0;
	this->m_unHashes = 0;
	this->m_unMaxHashes = 0;
	this->m_fGPUVDDC = 0.0f;
	this->m_fGPUTemp = 0.0f;
	this->m_unWorkSize = 0;
	this->m_pGPU = NULL;
	this->m_pDevice = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
GPUData::GPUData(const GPUData& gpuData)
{
	this->m_bIsActive = gpuData.GetIsActive();
	this->m_bIsDisabled = gpuData.GetIsDisabled();
	this->m_nID = gpuData.GetID();
	this->m_nDeviceID = gpuData.GetDeviceID();
	this->m_unWorkSize = gpuData.GetWorkSize();
	this->m_nGPUEngine = gpuData.GetGPUEngine();
	this->m_nMinEngine = gpuData.GetMinEngine();
	this->m_nGPUFan = gpuData.GetGPUFan();
	this->m_nMinFan = gpuData.GetMinFan();
	this->m_nGPUMemclock = gpuData.GetGPUMemclock();
	this->m_nGPUMemDiff = gpuData.GetGPUMemDiff();
	this->m_nGPUPowertune = gpuData.GetGPUPowerTune();
	this->m_fGPUVDDC = gpuData.GetGPUVDDC();
	this->m_szName = gpuData.GetName();
	this->m_szDevicePath = gpuData.GetDevicePath();	
	this->m_pGPU =  gpuData.GetGPU();
	this->m_pDevice = gpuData.GetDevice();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
GPUData& GPUData::operator=(const GPUData& gpuData)
{
	this->m_bIsActive = gpuData.GetIsActive();
	this->m_bIsDisabled = gpuData.GetIsDisabled();
	this->m_nID = gpuData.GetID();
	this->m_nDeviceID = gpuData.GetDeviceID();
	this->m_unWorkSize = gpuData.GetWorkSize();
	this->m_nGPUEngine = gpuData.GetGPUEngine();
	this->m_nMinEngine = gpuData.GetMinEngine();
	this->m_nGPUFan = gpuData.GetGPUFan();
	this->m_nMinFan = gpuData.GetMinFan();
	this->m_nGPUMemclock = gpuData.GetGPUMemclock();
	this->m_nGPUMemDiff = gpuData.GetGPUMemDiff();
	this->m_nGPUPowertune = gpuData.GetGPUPowerTune();
	this->m_fGPUVDDC = gpuData.GetGPUVDDC();
	this->m_szName = gpuData.GetName();
	this->m_szDevicePath = gpuData.GetDevicePath();	
	this->m_pGPU = gpuData.GetGPU();
	this->m_pDevice = gpuData.GetDevice();

	return *this;
}



///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
GPUData::~GPUData()
{
	if(m_pGPU)
	{
		delete(m_pGPU);
	}

	if(m_pDevice)
	{
		delete(m_pDevice);
	}
}

GPUData* GPUData::Clone()
{
	GPUData* pGPUData = new GPUData();

	pGPUData->SetIsActive(this->m_bIsActive);
	pGPUData->SetIsDisabled(this->m_bIsDisabled);

	pGPUData->SetID(this->m_nID);
	pGPUData->SetDeviceID(this->m_nDeviceID);
	pGPUData->SetGPUEngine(this->m_nGPUEngine);
	pGPUData->SetMinEngine(this->m_nMinEngine);
	pGPUData->SetGPUFan(this->m_nGPUFan);
	pGPUData->SetMinFan(this->m_nMinFan);
	pGPUData->SetGPUMemclock(this->m_nGPUMemclock);
	pGPUData->SetGPUMemDiff(this->m_nGPUMemDiff);
	pGPUData->SetGPUPowerTune(this->m_nGPUPowertune);
	pGPUData->SetGPUEngineExit(this->m_nGPUEngineExit);
	pGPUData->SetGPUMemclockExit(this->m_nGPUMemclockExit);
	pGPUData->SetHashes(this->m_unHashes);
	pGPUData->SetMaxHashes(this->m_unMaxHashes);
	pGPUData->SetGPUVDDC(this->m_fGPUVDDC);
	pGPUData->SetGPUTemp(this->m_fGPUTemp);
	pGPUData->SetWorksize(this->m_unWorkSize);
	pGPUData->SetName(this->m_szName);
	pGPUData->SetDevicePath(this->m_szDevicePath);
	pGPUData->SetGPU(this->m_pGPU);
	pGPUData->SetDevice(this->m_pDevice);

	return pGPUData;

}

GPUData* GPUData::DeepCopy()
{
	GPUData* pGPUData = new GPUData();

	pGPUData->SetIsActive(this->m_bIsActive);
	pGPUData->SetIsDisabled(this->m_bIsDisabled);

	pGPUData->SetID(this->m_nID);
	pGPUData->SetDeviceID(this->m_nDeviceID);
	pGPUData->SetGPUEngine(this->m_nGPUEngine);
	pGPUData->SetMinEngine(this->m_nMinEngine);
	pGPUData->SetGPUFan(this->m_nGPUFan);
	pGPUData->SetMinFan(this->m_nMinFan);
	pGPUData->SetGPUMemclock(this->m_nGPUMemclock);
	pGPUData->SetGPUMemDiff(this->m_nGPUMemDiff);
	pGPUData->SetGPUPowerTune(this->m_nGPUPowertune);
	pGPUData->SetGPUEngineExit(this->m_nGPUEngineExit);
	pGPUData->SetGPUMemclockExit(this->m_nGPUMemclockExit);
	pGPUData->SetHashes(this->m_unHashes);
	pGPUData->SetMaxHashes(this->m_unMaxHashes);
	pGPUData->SetGPUVDDC(this->m_fGPUVDDC);
	pGPUData->SetGPUTemp(this->m_fGPUTemp);
	pGPUData->SetWorksize(this->m_unWorkSize);
	pGPUData->SetName(this->m_szName);
	pGPUData->SetDevicePath(this->m_szDevicePath);
	pGPUData->SetGPU(this->m_pGPU->DeepCopy());
	pGPUData->SetDevice(this->m_pDevice->DeepCopy());

	return pGPUData;
}
