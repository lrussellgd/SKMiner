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
	this->m_enmEntityType = ENTITY_TYPE::ADL_GPU;

	this->m_bIsAutoFan = false;
	this->m_bHasFanSpeed = false;

	this->m_nOverdriveSupported = 0;
	this->m_nOverdriveEnabled = 0;
	this->m_nOverdriveVersion = 0;

	this->m_nFanSpeedMethod = -1;
	this->m_nPowerControlSupported = -1;
	this->m_nPowerControlCurrent = -1;
	this->m_nPowerControlDefault = -1;
	this->m_nTempTarget = 0;
	this->m_nTempOverHeat = 0;
	this->m_nTempCutOff = 0;

	this->m_nDefaultFanSpeed = 0;
	this->m_nDefaultEngineClock = 0;
	this->m_nDefaultMemoryClock = 0;
	this->m_nDefaultPowertune = 0;
	this->m_nThermalControllerIndex = 0;
	this->m_fDefaultVoltage = 0.0f;

}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
ADLGPU::ADLGPU(const ADLGPU& adlGPU) : BaseGPU(adlGPU)
{
	this->m_nOverdriveSupported = adlGPU.GetOverdriveSupported();
	this->m_nOverdriveEnabled = adlGPU.GetOverdriveEnabled();
	this->m_nOverdriveVersion = adlGPU.GetOverdriveVersion();
	this->m_nFanSpeedMethod = adlGPU.GetFanSpeedMethod();
	this->m_nPowerControlSupported = adlGPU.GetPowerControlSupported();
	this->m_nPowerControlCurrent = adlGPU.GetPowerControlCurrent();
	this->m_nPowerControlDefault = adlGPU.GetPowerControlDefault();

	this->m_bIsAutoFan = adlGPU.GetIsAutoFan();
	this->m_bHasFanSpeed = adlGPU.GetHasFanSpeed();
	this->m_nBusNumber = adlGPU.GetBusNumber();
	this->m_nTempTarget = adlGPU.GetTempTarget();
	this->m_nTempOverHeat = adlGPU.GetTempOverHeat();
	this->m_nTempCutOff = adlGPU.GetTempCutOff();
	this->m_nDefaultFanSpeed = adlGPU.GetDefaultFanSpeed();
	this->m_nDefaultEngineClock = adlGPU.GetDefaultEngineClock();
	this->m_nDefaultMemoryClock = adlGPU.GetDefaultMemoryClock();
	this->m_nDefaultPowertune = adlGPU.GetDefaultPowertune();
	this->m_nThermalControllerIndex = adlGPU.GetThermalControllerIndex();
	this->m_fDefaultVoltage = adlGPU.GetDefaultVoltage();

	this->m_szAdapterName = adlGPU.GetAdapterName();
	this->m_szDisplayName = adlGPU.GetDisplayName();
	this->m_szPNPString = adlGPU.GetPNPString();
	this->m_szUDID = adlGPU.GetUDID();
	this->m_szPCIDeviceID = adlGPU.GetPCIDeviceID();
	this->m_szSubSystemID = adlGPU.GetSubSystemID();
	this->m_szSubSystemVendorID = adlGPU.GetSubSystemVendorID();
}


ADLGPU& ADLGPU::operator=(const ADLGPU& adlGPU)
{
	this->m_enmEntityType = adlGPU.GetEntityType();
	this->m_nOverdriveSupported = adlGPU.GetOverdriveSupported();
	this->m_nOverdriveEnabled = adlGPU.GetOverdriveEnabled();
	this->m_nOverdriveVersion = adlGPU.GetOverdriveVersion();
	this->m_nFanSpeedMethod = adlGPU.GetFanSpeedMethod();
	this->m_nPowerControlSupported = adlGPU.GetPowerControlSupported();
	this->m_nPowerControlCurrent = adlGPU.GetPowerControlCurrent();
	this->m_nPowerControlDefault = adlGPU.GetPowerControlDefault();

	this->m_bIsAutoFan = adlGPU.GetIsAutoFan();
	this->m_bHasFanSpeed = adlGPU.GetHasFanSpeed();
	this->m_nBusNumber = adlGPU.GetBusNumber();
	this->m_nTempTarget = adlGPU.GetTempTarget();
	this->m_nTempOverHeat = adlGPU.GetTempOverHeat();
	this->m_nTempCutOff = adlGPU.GetTempCutOff();
	this->m_nDefaultFanSpeed = adlGPU.GetDefaultFanSpeed();
	this->m_nDefaultEngineClock = adlGPU.GetDefaultEngineClock();
	this->m_nDefaultMemoryClock = adlGPU.GetDefaultMemoryClock();
	this->m_nDefaultPowertune = adlGPU.GetDefaultPowertune();
	this->m_nThermalControllerIndex = adlGPU.GetThermalControllerIndex();
	this->m_fDefaultVoltage = adlGPU.GetDefaultVoltage();

	this->m_szAdapterName = adlGPU.GetAdapterName();
	this->m_szDisplayName = adlGPU.GetDisplayName();
	this->m_szPNPString = adlGPU.GetPNPString();
	this->m_szUDID = adlGPU.GetUDID();
	this->m_szPCIDeviceID = adlGPU.GetPCIDeviceID();
	this->m_szSubSystemID = adlGPU.GetSubSystemID();
	this->m_szSubSystemVendorID = adlGPU.GetSubSystemVendorID();

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
ADLGPU::~ADLGPU()
{
}


ADLGPU* ADLGPU::Clone()
{
	ADLGPU* pADLGPU = new ADLGPU();
	
	pADLGPU->SetGPUID(this->m_nGPUID);
	pADLGPU->SetIsManaged(this->m_bIsManaged);
	pADLGPU->SetTwin(this->m_pTwin);


	pADLGPU->SetIsAutoFan(this->m_bIsAutoFan);
	pADLGPU->SetHasFanSpeed(this->m_bHasFanSpeed);
	pADLGPU->SetOverdriveSupported(this->m_nOverdriveSupported);
	pADLGPU->SetOverdriveEnabled(this->m_nOverdriveEnabled);
	pADLGPU->SetOverdriveVersion(this->m_nOverdriveVersion);
	pADLGPU->SetFanSpeedMethod(this->m_nFanSpeedMethod);
	pADLGPU->SetPowerControlSupported(this->m_nPowerControlSupported);
	pADLGPU->SetPowerControlCurrent(this->m_nPowerControlCurrent);
	pADLGPU->SetPowerControlDefault(this->m_nPowerControlDefault);

	pADLGPU->SetBusNumber(this->m_nBusNumber);
	pADLGPU->SetTempTarget(this->m_nTempTarget);
	pADLGPU->SetTempOverHeat(this->m_nTempOverHeat);
	pADLGPU->SetTempCutOff(this->m_nTempCutOff);
	pADLGPU->SetDefaultFanSpeed(this->m_nDefaultFanSpeed);
	pADLGPU->SetDefaultEngineClock(this->m_nDefaultEngineClock);
	pADLGPU->SetDefaultMemoryClock(this->m_nDefaultMemoryClock);
	pADLGPU->SetDefaultPowertune(this->m_nDefaultPowertune);
	pADLGPU->SetThermalControllerIndex(this->m_nThermalControllerIndex);
	pADLGPU->SetDefaultVoltage(this->m_fDefaultVoltage);

	pADLGPU->SetAdapterName(this->m_szAdapterName);
	pADLGPU->SetDisplayName(this->m_szDisplayName);
	pADLGPU->SetPNPString(this->m_szPNPString);
	pADLGPU->SetUDID(this->m_szUDID);
	pADLGPU->SetPCIDeviceID(this->m_szPCIDeviceID);
	pADLGPU->SetSubSystemID(this->m_szSubSystemID);
	pADLGPU->SetSubSystemVendorID(this->m_szSubSystemVendorID);

	return pADLGPU;
}

ADLGPU* ADLGPU::DeepCopy()
{
	ADLGPU* pADLGPU = new ADLGPU();

	pADLGPU->SetGPUID(this->m_nGPUID);
	pADLGPU->SetIsManaged(this->m_bIsManaged);
	pADLGPU->SetTwin(this->m_pTwin);

	pADLGPU->SetIsAutoFan(this->m_bIsAutoFan);
	pADLGPU->SetHasFanSpeed(this->m_bHasFanSpeed);
	pADLGPU->SetFanSpeedMethod(this->m_nFanSpeedMethod);
	pADLGPU->SetOverdriveSupported(this->m_nOverdriveSupported);
	pADLGPU->SetOverdriveEnabled(this->m_nOverdriveEnabled);
	pADLGPU->SetOverdriveVersion(this->m_nOverdriveVersion);
	pADLGPU->SetPowerControlSupported(this->m_nPowerControlSupported);
	pADLGPU->SetPowerControlCurrent(this->m_nPowerControlCurrent);
	pADLGPU->SetPowerControlDefault(this->m_nPowerControlDefault);

	pADLGPU->SetBusNumber(this->m_nBusNumber);
	pADLGPU->SetTempTarget(this->m_nTempTarget);
	pADLGPU->SetTempOverHeat(this->m_nTempOverHeat);
	pADLGPU->SetTempCutOff(this->m_nTempCutOff);
	pADLGPU->SetDefaultFanSpeed(this->m_nDefaultFanSpeed);
	pADLGPU->SetDefaultEngineClock(this->m_nDefaultEngineClock);
	pADLGPU->SetDefaultMemoryClock(this->m_nDefaultMemoryClock);
	pADLGPU->SetDefaultPowertune(this->m_nDefaultPowertune);
	pADLGPU->SetThermalControllerIndex(this->m_nThermalControllerIndex);
	pADLGPU->SetDefaultVoltage(this->m_fDefaultVoltage);

	pADLGPU->SetAdapterName(this->m_szAdapterName);
	pADLGPU->SetDisplayName(this->m_szDisplayName);
	pADLGPU->SetPNPString(this->m_szPNPString);
	pADLGPU->SetUDID(this->m_szUDID);
	pADLGPU->SetPCIDeviceID(this->m_szPCIDeviceID);
	pADLGPU->SetSubSystemID(this->m_szSubSystemID);
	pADLGPU->SetSubSystemVendorID(this->m_szSubSystemVendorID);

	return pADLGPU;
}