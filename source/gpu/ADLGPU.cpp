////////////////////////////////////////////////
// File:	ADLGPU.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#include "ADLGPU.h"

#include "../config/GPUSetting.h"

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
ADLGPU::ADLGPU() : BaseGPU()
{
	m_nLPAdapterID = 0;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
ADLGPU::ADLGPU(const ADLGPU& adlGPU) : BaseGPU(adlGPU)
{
	this->m_lpTemperature = adlGPU.GetADLTemperature();
	this->m_lpActivity = adlGPU.GetADLPMActivity();
	this->m_lpOdParameters = adlGPU.GetADLODParameters();

	int level = this->m_lpOdParameters.iNumberOfPerformanceLevels - 1;
	size_t perfLevelSize = sizeof(ADLODPerformanceLevels)+level * sizeof(ADLODPerformanceLevel);
	this->m_pDefPerfLev = (ADLODPerformanceLevels *)malloc(perfLevelSize);
	this->m_pDefPerfLev->iSize = (int)perfLevelSize;
	memcpy(this->m_pDefPerfLev, adlGPU.GetADLODPerformanceLevels(), perfLevelSize);

	this->m_lpFanSpeedInfo = adlGPU.GetADLFanSpeedInfo();
	this->m_lpFanSpeedValue = adlGPU.GetADLFanSpeedValue();
	this->m_lpDefFanSpeedValue = adlGPU.GetADLDefFanSpeedValue();
	this->m_nLPAdapterID = adlGPU.GetLPAdapterID();
}


ADLGPU& ADLGPU::operator=(const ADLGPU& adlGPU)
{
	this->m_lpTemperature = adlGPU.GetADLTemperature();
	this->m_lpActivity = adlGPU.GetADLPMActivity();
	this->m_lpOdParameters = adlGPU.GetADLODParameters();
	if(this->m_pDefPerfLev != NULL)
	{
		free(this->m_pDefPerfLev);
		this->m_pDefPerfLev = NULL;
	}

	int level = this->m_lpOdParameters.iNumberOfPerformanceLevels - 1;
	size_t perfLevelSize = sizeof(ADLODPerformanceLevels) + level * sizeof(ADLODPerformanceLevel);
	this->m_pDefPerfLev = (ADLODPerformanceLevels *)malloc(perfLevelSize);
	this->m_pDefPerfLev->iSize = (int)perfLevelSize;
	memcpy(this->m_pDefPerfLev, adlGPU.GetADLODPerformanceLevels(), perfLevelSize);

	this->m_lpFanSpeedInfo = adlGPU.GetADLFanSpeedInfo();
	this->m_lpFanSpeedValue = adlGPU.GetADLFanSpeedValue();
	this->m_lpDefFanSpeedValue = adlGPU.GetADLDefFanSpeedValue();
	this->m_nLPAdapterID = adlGPU.GetLPAdapterID();

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
ADLGPU::~ADLGPU()
{
	if (this->m_pGPUSettings)
	{
		delete(this->m_pGPUSettings);
		this->m_pGPUSettings = NULL;
	}

	if(this->m_pDefPerfLev != NULL)
	{
		free(this->m_pDefPerfLev);
		this->m_pDefPerfLev = NULL;
	}
}


ADLGPU* ADLGPU::Clone()
{
	ADLGPU* pADLGPU = new ADLGPU();


	pADLGPU->SetAdapterIndex(this->m_nAdapterIndex);
	pADLGPU->SetAdapterName(this->m_szAdapterName);
	pADLGPU->SetBusNumber(this->m_nBusNumber);
	pADLGPU->SetGPUID(this->m_nGPUID);
	pADLGPU->SetGPUSetting(this->m_pGPUSettings);
	pADLGPU->SetIsDefFanValid(this->m_bIsDefFanValid);
	pADLGPU->SetIsFanValid(this->m_bIsFanValid);
	pADLGPU->SetIsManaged(this->m_bIsManaged);
	pADLGPU->SetLastEngine(this->m_nLastEngine);
	pADLGPU->SetLastTemp(this->m_nLastTemp);
	pADLGPU->SetTwin(this->m_pTwin);

	pADLGPU->SetADLTemperature(this->m_lpTemperature);
	pADLGPU->SetADLPMActivity(this->m_lpActivity);
	pADLGPU->SetADLODParameters(this->m_lpOdParameters);
	pADLGPU->SetADLODPerformanceLevels(this->m_pDefPerfLev);
	pADLGPU->SetADLFanSpeedInfo(this->m_lpFanSpeedInfo);
	pADLGPU->SetADLFanSpeedValue(this->m_lpFanSpeedValue);
	pADLGPU->SetADLDefFanSpeedValue(this->m_lpDefFanSpeedValue);
	pADLGPU->SetLPAdapterID(this->m_nLPAdapterID);

	return pADLGPU;
}

ADLGPU* ADLGPU::DeepCopy()
{
	ADLGPU* pADLGPU = new ADLGPU();

	pADLGPU->SetAdapterIndex(this->m_nAdapterIndex);
	pADLGPU->SetAdapterName(this->m_szAdapterName);
	pADLGPU->SetBusNumber(this->m_nBusNumber);
	pADLGPU->SetGPUID(this->m_nGPUID);
	pADLGPU->SetGPUSetting(this->m_pGPUSettings->DeepCopy());
	pADLGPU->SetIsDefFanValid(this->m_bIsDefFanValid);
	pADLGPU->SetIsFanValid(this->m_bIsFanValid);
	pADLGPU->SetIsManaged(this->m_bIsManaged);
	pADLGPU->SetLastEngine(this->m_nLastEngine);
	pADLGPU->SetLastTemp(this->m_nLastTemp);

	if (this->m_pTwin)
	{
		pADLGPU->SetTwin(this->m_pTwin);
	}
	
	pADLGPU->SetADLTemperature(this->m_lpTemperature);
	pADLGPU->SetADLPMActivity(this->m_lpActivity);
	pADLGPU->SetADLODParameters(this->m_lpOdParameters);

	int level = this->m_lpOdParameters.iNumberOfPerformanceLevels - 1;
	size_t perfLevelSize = sizeof(ADLODPerformanceLevels)+level * sizeof(ADLODPerformanceLevel);

	ADLODPerformanceLevels* pDefPerfLev = (ADLODPerformanceLevels *)malloc(perfLevelSize);
	pDefPerfLev->iSize = (int)perfLevelSize;
	memcpy(pDefPerfLev, this->m_pDefPerfLev, perfLevelSize);

	pADLGPU->SetADLODPerformanceLevels(pDefPerfLev);

	pADLGPU->SetADLFanSpeedInfo(this->m_lpFanSpeedInfo);
	pADLGPU->SetADLFanSpeedValue(this->m_lpFanSpeedValue);
	pADLGPU->SetADLDefFanSpeedValue(this->m_lpDefFanSpeedValue);
	pADLGPU->SetLPAdapterID(this->m_nLPAdapterID);

	return pADLGPU;
}