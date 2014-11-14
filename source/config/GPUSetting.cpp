////////////////////////////////////////////////
// File:	GPUSetting.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "GPUSetting.h"

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
GPUSetting::GPUSetting()
{
	m_bIsAutoEngine = m_bIsAutoFan = m_bHasFanSpeed = m_bIsEnabled = false;

	m_nEngineClock = m_nMinEngineSpeed = m_nMaxEngineSpeed = m_nMemclock = m_nPowerTune = 0;
	m_nThreads = m_nShaders = m_nTargetFan = m_nTargetTemp = m_nOverheatTemp = m_nCutOffTemp = 0;	
	m_nIntensity = m_nRawintensity = m_nXIntensity = m_nThreadConcurreny = m_nLookupGap = 0;
	m_nWorkSize = m_nVectors = 0;

	m_fVDDC = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
GPUSetting::GPUSetting(const GPUSetting& setting)
{
	this->m_bIsEnabled = setting.GetIsEnabled();
	this->m_bIsAutoEngine = setting.GetIsAutoEngine();
	this->m_bIsAutoFan = setting.GetIsAutoFan();
	this->m_bHasFanSpeed = setting.GetHasFanSpeed();
	this->m_nEngineClock = setting.GetEngineClock();
	this->m_nMinEngineSpeed = setting.GetMinEngineSpeed();
	this->m_nMaxEngineSpeed = setting.GetMaxEngineSpeed();
	this->m_nMemclock = setting.GetMemclock();
	this->m_nPowerTune = setting.GetPowerTune();
	this->m_nThreads = setting.GetThreads();
	this->m_nShaders = setting.GetShaders();
	this->m_nTargetFan = setting.GetTargetFan();
	this->m_nTargetTemp = setting.GetTargetTemp();
	this->m_nOverheatTemp = setting.GetOverHeatTemp();
	this->m_nCutOffTemp = setting.GetCutOffTemp();	
	this->m_nIntensity = setting.GetIntensity();
	this->m_nRawintensity = setting.GetRawIntensity();
	this->m_nXIntensity = setting.GetXIntensity();
	this->m_nThreadConcurreny = setting.GetThreadConcurrency();
	this->m_nLookupGap = setting.GetLookupGap();
	this->m_nWorkSize = setting.GetWorkSize();
	this->m_nVectors = setting.GetVectors();
	this->m_fVDDC = setting.GetVoltage();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
GPUSetting& GPUSetting::operator=(const GPUSetting& setting)
{
	this->m_bIsEnabled = setting.GetIsEnabled();
	this->m_bIsAutoEngine = setting.GetIsAutoEngine();
	this->m_bIsAutoFan = setting.GetIsAutoFan();
	this->m_bHasFanSpeed = setting.GetHasFanSpeed();
	this->m_nEngineClock = setting.GetEngineClock();
	this->m_nMinEngineSpeed = setting.GetMinEngineSpeed();
	this->m_nMaxEngineSpeed = setting.GetMaxEngineSpeed();
	this->m_nMemclock = setting.GetMemclock();
	this->m_nPowerTune = setting.GetPowerTune();
	this->m_nThreads = setting.GetThreads();
	this->m_nShaders = setting.GetShaders();
	this->m_nTargetFan = setting.GetTargetFan();
	this->m_nTargetTemp = setting.GetTargetTemp();
	this->m_nOverheatTemp = setting.GetOverHeatTemp();
	this->m_nCutOffTemp = setting.GetCutOffTemp();	
	this->m_nIntensity = setting.GetIntensity();
	this->m_nRawintensity = setting.GetRawIntensity();
	this->m_nXIntensity = setting.GetXIntensity();
	this->m_nThreadConcurreny = setting.GetThreadConcurrency();
	this->m_nLookupGap = setting.GetLookupGap();
	this->m_nWorkSize = setting.GetWorkSize();
	this->m_nVectors = setting.GetVectors();
	this->m_fVDDC = setting.GetVoltage();

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
GPUSetting::~GPUSetting()
{

}


GPUSetting* GPUSetting::Clone()
{
	GPUSetting* pGPUSetting = new GPUSetting();

	pGPUSetting->SetIsAutoEngine(this->m_bIsAutoEngine);
	pGPUSetting->SetIsAutoFan(this->m_bIsAutoFan);
	pGPUSetting->SetHasFanSpeed(this->m_bHasFanSpeed);
	pGPUSetting->SetIsEnabled(this->m_bIsEnabled);
	pGPUSetting->SetEngineClock(this->m_nEngineClock);
	pGPUSetting->SetMinEngineSpeed(this->m_nMinEngineSpeed);
	pGPUSetting->SetMaxEngineSpeed(this->m_nMaxEngineSpeed);
	pGPUSetting->SetMemclock(this->m_nMemclock);
	pGPUSetting->SetPowerTune(this->m_nPowerTune);
	pGPUSetting->SetThreads(this->m_nThreads);
	pGPUSetting->SetShaders(this->m_nShaders);
	pGPUSetting->SetTargetFan(this->m_nTargetFan);
	pGPUSetting->SetTargetTemp(this->m_nTargetTemp);
	pGPUSetting->SetOverHeatTemp(this->m_nOverheatTemp);
	pGPUSetting->SetCutOffTemp(this->m_nCutOffTemp);
	pGPUSetting->SetIntensity(this->m_nIntensity);
	pGPUSetting->SetRawIntensity(this->m_nRawintensity);
	pGPUSetting->SetXIntensity(this->m_nXIntensity);
	pGPUSetting->SetThreadConcurrency(this->m_nThreadConcurreny);
	pGPUSetting->SetLookupGap(this->m_nLookupGap);
	pGPUSetting->SetWorkSize(this->m_nWorkSize);
	pGPUSetting->SetVectors(this->m_nVectors);
	pGPUSetting->SetVoltage(this->m_fVDDC);

	return pGPUSetting;
}

GPUSetting* GPUSetting::DeepCopy()
{
	GPUSetting* pGPUSetting = new GPUSetting();

	pGPUSetting->SetIsAutoEngine(this->m_bIsAutoEngine);
	pGPUSetting->SetIsAutoFan(this->m_bIsAutoFan);
	pGPUSetting->SetHasFanSpeed(this->m_bHasFanSpeed);
	pGPUSetting->SetIsEnabled(this->m_bIsEnabled);
	pGPUSetting->SetEngineClock(this->m_nEngineClock);
	pGPUSetting->SetMinEngineSpeed(this->m_nMinEngineSpeed);
	pGPUSetting->SetMaxEngineSpeed(this->m_nMaxEngineSpeed);
	pGPUSetting->SetMemclock(this->m_nMemclock);
	pGPUSetting->SetPowerTune(this->m_nPowerTune);
	pGPUSetting->SetThreads(this->m_nThreads);
	pGPUSetting->SetShaders(this->m_nShaders);
	pGPUSetting->SetTargetFan(this->m_nTargetFan);
	pGPUSetting->SetTargetTemp(this->m_nTargetTemp);
	pGPUSetting->SetOverHeatTemp(this->m_nOverheatTemp);
	pGPUSetting->SetCutOffTemp(this->m_nCutOffTemp);
	pGPUSetting->SetIntensity(this->m_nIntensity);
	pGPUSetting->SetRawIntensity(this->m_nRawintensity);
	pGPUSetting->SetXIntensity(this->m_nXIntensity);
	pGPUSetting->SetThreadConcurrency(this->m_nThreadConcurreny);
	pGPUSetting->SetLookupGap(this->m_nLookupGap);
	pGPUSetting->SetWorkSize(this->m_nWorkSize);
	pGPUSetting->SetVectors(this->m_nVectors);
	pGPUSetting->SetVoltage(this->m_fVDDC);

	return pGPUSetting;
}