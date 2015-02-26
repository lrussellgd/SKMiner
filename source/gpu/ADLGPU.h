////////////////////////////////////////////////
// File:	ADLGPU.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#ifndef _ADLGPU_H_
#define _ADLGPU_H_

#include "../ADL_SDK/adl_sdk.h"
#include "BaseGPU.h"

#include <string>

class ADLGPU : public BaseGPU
{
private:

	//AdapterInfo					m_adlAdapterInfo;

	/*ADLThermalControllerInfo	m_lpThermalControllerInfo;
	ADLPowerControlInfo			m_lpPowerControlInfo;
	ADLTemperature				m_lpTemperature;
	ADLPMActivity				m_lpActivity;
	ADLODParameters				m_lpOdParameters;
	ADLODPerformanceLevels*		m_pDefPerfLev;
	ADLODPerformanceLevels*		m_pCurrPerfLev;
	ADLFanSpeedInfo				m_lpFanSpeedInfo;
	ADLFanSpeedValue			m_lpFanSpeedValue;
	ADLFanSpeedValue			m_lpDefFanSpeedValue;
	ADLBiosInfo					m_lpBiosInfo;

	ADLOD6ThermalControllerCaps m_lpThermalControllerCaps_6;
	ADLOD6Capabilities			m_lpCapabilities_6;
	ADLOD6CurrentStatus			m_lpCurrentStatus_6;
	ADLOD6PowerControlInfo		m_lpPowerControlInfo_6;
	ADLOD6FanSpeedValue			m_lpFanSpeedValue_6;
	ADLOD6FanSpeedValue			m_lpDefFanSpeedValue_6;
	ADLOD6FanSpeedInfo			m_lpFanSpeedInfo_6;	
	ADLOD6VoltageControlInfo	m_lpVoltageControlInfo_6;
	ADLOD6StateInfo*			m_pDefaultStateInfo_6;
	ADLOD6StateInfo*			m_pCustomStateInfo_6;*/

	bool						m_bIsAutoFan;
	bool						m_bHasFanSpeed;

	int							m_nOD6Capabilities;
	int							m_nOverdriveSupported;
	int							m_nOverdriveEnabled;
	int							m_nOverdriveVersion;
	int							m_nPowerControlSupported;
	int							m_nPowerControlCurrent;
	int							m_nPowerControlDefault;
	int							m_nAdapterIndex;
	int							m_nDeviceNumber;
	int							m_nAIExist;
	int							m_nAIFunctionNumber;
	int							m_nAIOSDisplayIndex;
	int							m_nAIPresent;
	int							m_nVendorID;
	int							m_nBusNumber;
	int							m_nTempTarget;
	int							m_nTempOverHeat;
	int							m_nTempCutOff;
	int							m_nThermalDomain;
	int							m_nThermalDomainIndex;
	int							m_nThermalFlags;
	int							m_nThermalControllerIndex;
	int							m_nThermalControllerCapabilities;
	int							m_nActivityReportingSupported;
	int							m_nDiscretePerformanceLevels;
	int							m_nNumberOfPerformanceLevels;
	int							m_nEngineClockMax;
	int							m_nEngineClockMin;
	int							m_nEngineClockStep;
	int							m_nMemoryClockMax;
	int							m_nMemoryClockMin;
	int							m_nMemoryClockStep;
	int							m_nVDDCMax;
	int							m_nVDDCMin;
	int							m_nVDDCStep;
	int							m_nPowertuneMax;
	int							m_nPowertuneMin;
	int							m_nPowertuneStep;

	int							m_nDefaultFanSpeed;
	int							m_nDefaultEngineClock;
	int							m_nDefaultMemoryClock;
	int							m_nDefaultPowertune;

	int							m_nCurrentActivity;
	int							m_nCurrentFanSpeed;
	int							m_nCurrentEngineClock;
	int							m_nCurrentMemoryClock;
	int							m_nCurrentPowertune;

	int							m_nFanSpeedMethod;
	int							m_nFanMaxPercent;
	int							m_nFanMaxRPM;
	int							m_nFanMinPercent;
	int							m_nFanMinRPM;
	int							m_nFanSpeedFlags;

	float						m_fCurrentTemperature;
	float						m_fDefaultVoltage;
	float						m_fCurrentVoltage;

	std::string					m_szAdapterName;
	std::string					m_szDisplayName;
	std::string					m_szPNPString;
	std::string					m_szUDID;
	std::string					m_szDriverPath;
	std::string					m_szDriverPathExt;
	std::string					m_szPCIDeviceID;
	std::string					m_szSubSystemID;
	std::string					m_szSubSystemVendorID;
	std::string					m_szBiosDate;
	std::string					m_szBiosPartNumber;
	std::string					m_szBiosVersion;
	
public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	ADLGPU();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	ADLGPU(const ADLGPU& adlGPU);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	ADLGPU& operator=(const ADLGPU& adlGPU);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~ADLGPU();

	ADLGPU* Clone();
	ADLGPU* DeepCopy();

	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const bool						    GetIsAutoFan()						const								{	return this->m_bIsAutoFan;													}
	const bool							GetHasFanSpeed()					const								{	return this->m_bHasFanSpeed;												}
	
	const int							GetOD6Capabilities()				const								{	return this->m_nOD6Capabilities;											}
	const int							GetOverdriveSupported()				const								{	return this->m_nOverdriveSupported;											}
	const int							GetOverdriveEnabled()				const								{	return this->m_nOverdriveEnabled;											}
	const int							GetOverdriveVersion()				const								{	return this->m_nOverdriveVersion;											}
	const int							GetPowerControlSupported()			const								{	return this->m_nPowerControlSupported;										}
	const int							GetPowerControlCurrent()			const								{	return this->m_nPowerControlCurrent;										}
	const int							GetPowerControlDefault()			const								{	return this->m_nPowerControlDefault;										}
	const int							GetAdapterIndex()					const								{	return this->m_nAdapterIndex;												}
	const int							GetDeviceNumber()					const								{	return this->m_nDeviceNumber;												}
	const int							GetAIExist()						const								{	return this->m_nAIExist;													}
	const int							GetAIFunctionNumber()				const								{	return this->m_nAIFunctionNumber;											}
	const int							GetAIOSDisplayIndex()				const								{	return this->m_nAIOSDisplayIndex;											}
	const int							GetAIPresent()						const								{	return this->m_nAIPresent;													}
	const int							GetVendorID()						const								{	return this->m_nVendorID;													}	
	const int							GetBusNumber()						const								{	return this->m_nBusNumber;													}
	const int							GetTempTarget()						const								{	return this->m_nTempTarget;													}
	const int							GetTempOverHeat()					const								{	return this->m_nTempOverHeat;												}
	const int							GetTempCutOff()						const								{	return this->m_nTempCutOff;													}	
	const int							GetThermalDomain()					const								{	return this->m_nThermalDomain;												}
	const int							GetThermalDomainIndex()				const								{	return this->m_nThermalDomainIndex; 										}
	const int							GetThermalFlags()					const								{	return this->m_nThermalFlags;												}	
	const int							GetThermalControllerIndex()			const								{	return this->m_nThermalControllerIndex;										}
	const int							GetThermalControllerCapabilities()	const								{	return this->m_nThermalControllerCapabilities;								}
	const int							GetActivityReportingSupported()		const								{	return this->m_nActivityReportingSupported;									}
	const int							GetDiscretePerformanceLevels()		const								{	return this->m_nDiscretePerformanceLevels;									}
	const int							GetNumberOfPerformanceLevels()		const								{	return this->m_nNumberOfPerformanceLevels;									}
	const int							GetEngineClockMax()					const								{	return this->m_nEngineClockMax;												}
	const int							GetEngineClockMin()					const								{	return this->m_nEngineClockMin;												}
	const int							GetEngineClockStep()				const								{	return this->m_nEngineClockStep;											}
	const int							GetMemoryClockMax()					const								{	return this->m_nMemoryClockMax;												}
	const int							GetMemoryClockMin()					const								{	return this->m_nMemoryClockMin;												}
	const int							GetMemoryClockStep()				const								{	return this->m_nMemoryClockStep;											}
	const int							GetVDDCMax()						const								{	return this->m_nVDDCMax;													}
	const int							GetVDDCMin()						const								{	return this->m_nVDDCMin;													}
	const int							GetVDDCStep()						const								{	return this->m_nVDDCStep;													}	
	const int							GetPowertuneMax()					const								{	return this->m_nPowertuneMax;												}
	const int							GetPowertuneMin()					const								{	return this->m_nPowertuneMin;												}
	const int							GetPowertuneStep()					const								{	return this->m_nPowertuneStep;												}
	const int							GetDefaultFanSpeed()				const								{	return this->m_nDefaultFanSpeed;											}
	const int							GetDefaultEngineClock()				const								{	return this->m_nDefaultEngineClock;											}
	const int							GetDefaultMemoryClock()				const								{	return this->m_nDefaultMemoryClock;											}
	const int							GetDefaultPowertune()				const								{	return this->m_nDefaultPowertune;											}
	const int							GetCurrentActivity()				const								{	return this->m_nCurrentActivity;											}
	const int							GetCurrentFanSpeed()				const								{	return this->m_nCurrentFanSpeed;											}
	const int							GetCurrentEngineClock()				const								{	return this->m_nCurrentEngineClock;											}
	const int							GetCurrentMemoryClock()				const								{	return this->m_nCurrentMemoryClock;											}
	const int							GetCurrentPowertune()				const								{	return this->m_nCurrentPowertune;											}
	const int							GetFanSpeedMethod()					const								{	return this->m_nFanSpeedMethod;												}
	const int							GetFanMaxPercent()					const								{	return this->m_nFanMaxPercent;												}
	const int							GetFanMaxRPM()						const								{	return this->m_nFanMaxRPM;													}
	const int							GetFanMinPercent()					const								{	return this->m_nFanMinPercent;												}
	const int							GetFanMinRPM()						const								{	return this->m_nFanMinRPM;													}
	const int							GetFanSpeedFlags()					const								{	return this->m_nFanSpeedFlags;												}

	const float							GetCurrentTemperature()				const								{	return this->m_fCurrentTemperature;											}
	const float							GetDefaultVoltage()					const								{	return this->m_fDefaultVoltage;												}	
	const float							GetCurrentVoltage()					const								{	return this->m_fCurrentVoltage;												}
	
	const std::string&					GetAdapterName()					const								{	return this->m_szAdapterName;												}
	const std::string&					GetDisplayName()					const								{	return this->m_szDisplayName;												}
	const std::string&					GetPNPString()						const								{	return this->m_szPNPString;													}
	const std::string&					GetUDID()							const								{	return this->m_szUDID;														}
	const std::string&					GetDriverPath()						const								{	return this->m_szDriverPath;												}
	const std::string&					GetDriverPathExt()					const								{	return this->m_szDriverPathExt;												}
	const std::string&					GetPCIDeviceID()					const								{	return this->m_szPCIDeviceID;												}
	const std::string&					GetSubSystemID()					const								{	return this->m_szSubSystemID;												}
	const std::string&					GetSubSystemVendorID()				const								{	return this->m_szSubSystemVendorID;											}
	const std::string&					GetBiosDate()						const								{	return this->m_szBiosDate;													}
	const std::string&					GetBiosPartNumber()					const								{	return this->m_szBiosPartNumber;											}
	const std::string&					GetBiosVersion()					const								{	return this->m_szBiosVersion;												}


	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void SetIsAutoFan(const bool bIsAutoFan)																	{	this->m_bIsAutoFan = bIsAutoFan;											}	
	void SetHasFanSpeed(const bool bHasFanSpeed)																{	this->m_bHasFanSpeed = bHasFanSpeed;										}
	
	void SetOD6Capabilities(const int nOD6Capabilities)															{	this->m_nOD6Capabilities = nOD6Capabilities;								}
	void SetOverdriveSupported(const int nOverdriveSupported)													{	this->m_nOverdriveSupported = nOverdriveSupported;							}
	void SetOverdriveEnabled(const int nOverdriveEnabled)														{	this->m_nOverdriveEnabled = nOverdriveEnabled;								}
	void SetOverdriveVersion(const int nOverdriveVersion)														{	this->m_nOverdriveVersion = nOverdriveVersion;								}	
	void SetPowerControlSupported(const int nPowerControlSupported)												{	this->m_nPowerControlSupported = nPowerControlSupported;					}
	void SetPowerControlCurrent(const int nPowerControlCurrent)													{	this->m_nPowerControlCurrent = nPowerControlCurrent;						}
	void SetPowerControlDefault(const int nPowerControlDefault)													{	this->m_nPowerControlDefault = nPowerControlDefault;						}
	void SetAdapterIndex(const int nAdapterIndex)																{	this->m_nAdapterIndex = nAdapterIndex;										}
	void SetDeviceNumber(const int nDeviceNumber)																{	this->m_nDeviceNumber = nDeviceNumber;										}
	void SetAIExist(const int nAIExist)																			{	this->m_nAIExist = nAIExist;												}
	void SetAIFunctionNumber(const int nAIFunctionNumber)														{	this->m_nAIFunctionNumber = nAIFunctionNumber;								}
	void SetAIOSDisplayIndex(const int nAIOSDisplayIndex)														{	this->m_nAIOSDisplayIndex = nAIOSDisplayIndex;								}
	void SetAIPresent(const int nAIPresent)																		{	this->m_nAIPresent = nAIPresent;											}
	void SetVendorID(const int nVendorID)																		{	this->m_nVendorID = nVendorID;												}	
	void SetBusNumber(const int nBusNumber)																		{	this->m_nBusNumber = nBusNumber;											}
	void SetTempTarget(const int nTempTarget)																	{	this->m_nTempTarget = nTempTarget;											}
	void SetTempOverHeat(const int nTempOverHeat)																{	this->m_nTempOverHeat = nTempOverHeat;										}
	void SetTempCutOff(const int nTempCutOff)																	{	this->m_nTempCutOff = nTempCutOff;											}	
	void SetThermalDomain(const int nThermalDomain)																{	this->m_nThermalDomain = nThermalDomain;									}
	void SetThermalDomainIndex(const int nThermalDomainIndex)													{	this->m_nThermalDomainIndex = nThermalDomainIndex;							}
	void SetThermalFlags(const int nThermalFlags)																{	this->m_nThermalFlags = nThermalFlags;										}	
	void SetThermalControllerIndex(const int nThermalControllerIndex)											{	this->m_nThermalControllerIndex = nThermalControllerIndex;					}
	void SetThermalControllerCapabilities(const int nThermalControllerCapabilities)								{	this->m_nThermalControllerCapabilities = nThermalControllerCapabilities;	}
	void SetActivityReportingSupported(const int nActivityReportingSupported)									{	this->m_nActivityReportingSupported = nActivityReportingSupported;			}
	void SetDiscretePerformanceLevels(const int nDiscretePerformanceLevels)										{	this->m_nDiscretePerformanceLevels = nDiscretePerformanceLevels;			}
	void SetNumberOfPerformanceLevels(const int nNumberOfPerformanceLevels)										{	this->m_nNumberOfPerformanceLevels = nNumberOfPerformanceLevels;			}
	void SetEngineClockMax(const int nEngineClockMax)															{	this->m_nEngineClockMax = nEngineClockMax;									}
	void SetEngineClockMin(const int nEngineClockMin)															{	this->m_nEngineClockMin = nEngineClockMin;									}
	void SetEngineClockStep(const int nEngineClockStep)															{	this->m_nEngineClockStep = nEngineClockStep;								}
	void SetMemoryClockMax(const int nMemoryClockMax)															{	this->m_nMemoryClockMax = nMemoryClockMax;									}
	void SetMemoryClockMin(const int nMemoryClockMin)															{	this->m_nMemoryClockMin = nMemoryClockMin;									}
	void SetMemoryClockStep(const int nMemoryClockStep)															{	this->m_nMemoryClockStep = nMemoryClockStep;								}
	void SetVDDCMax(const int nVDDCMax)																			{	this->m_nVDDCMax = nVDDCMax;												}
	void SetVDDCMin(const int nVDDCMin)																			{	this->m_nVDDCMin = nVDDCMin;												}
	void SetVDDCStep(const int nVDDCStep)																		{	this->m_nVDDCStep = nVDDCStep;												}
	void SetPowertuneMax(const int nPowertuneMax)																{	this->m_nPowertuneMax = nPowertuneMax;										}
	void SetPowertuneMin(const int nPowertuneMin)																{	this->m_nPowertuneMin = nPowertuneMin;										}
	void SetPowertuneStep(const int nPowertuneStep)																{	this->m_nPowertuneStep = nPowertuneStep;									}
	void SetDefaultFanSpeed(const int nDefaultFanSpeed)															{	this->m_nDefaultFanSpeed = nDefaultFanSpeed;								}
	void SetDefaultEngineClock(const int nDefaultEngineClock)													{	this->m_nDefaultEngineClock = nDefaultEngineClock;							}
	void SetDefaultMemoryClock(const int nDefaultMemoryClock)													{	this->m_nDefaultMemoryClock = nDefaultMemoryClock;							}
	void SetDefaultPowertune(const int nDefaultPowertune)														{	this->m_nDefaultPowertune = nDefaultPowertune;								}
	void SetCurrentActivity(const int nCurrentActivity)															{	this->m_nCurrentActivity = nCurrentActivity;								}
	void SetCurrentFanSpeed(const int nCurrentFanSpeed)															{	this->m_nCurrentFanSpeed = nCurrentFanSpeed;								}
	void SetCurrentEngineClock(const int nCurrentEngineClock)													{	this->m_nCurrentEngineClock = nCurrentEngineClock;							}
	void SetCurrentMemoryClock(const int nCurrentMemoryClock)													{	this->m_nCurrentMemoryClock = nCurrentMemoryClock;							}
	void SetCurrentPowertune(const int nCurrentPowertune)														{	this->m_nCurrentPowertune = nCurrentPowertune;								}
	void SetFanSpeedMethod(const int nFanSpeedMethod)															{	this->m_nFanSpeedMethod = nFanSpeedMethod;									}
	void SetFanMaxPercent(const int nFanMaxPercent)																{	this->m_nFanMaxPercent = nFanMaxPercent;									}
	void SetFanMaxRPM(const int nFanMaxRPM)																		{	this->m_nFanMaxRPM = nFanMaxRPM;											}
	void SetFanMinPercent(const int nFanMinPercent)																{	this->m_nFanMinPercent = nFanMinPercent;									}
	void SetFanMinRPM(const int nFanMinRPM)																		{	this->m_nFanMinRPM = nFanMinRPM;											}
	void SetFanSpeedFlags(const int nFanSpeedFlags)																{	this->m_nFanSpeedFlags = nFanSpeedFlags;									}

	void SetCurrentTemperature(const float fCurrentTemperature)													{	this->m_fCurrentTemperature = fCurrentTemperature;							}
	void SetDefaultVoltage(const float fDefaultVoltage)															{	this->m_fDefaultVoltage = fDefaultVoltage;									}
	void SetCurrentVoltage(const float fCurrentVoltage)															{	this->m_fCurrentVoltage = fCurrentVoltage;									}
	
	void SetAdapterName(const std::string& szAdapterName)														{	this->m_szAdapterName = szAdapterName;										}
	void SetDisplayName(const std::string& szDisplayName)														{	this->m_szDisplayName = szDisplayName;										}	
	void SetPNPString(const std::string& szPNPString)															{	this->m_szPNPString = szPNPString;											}
	void SetUDID(const std::string& szUDID)																		{	this->m_szUDID = szUDID;													}
	void SetDriverPath(const std::string& szDriverPath)															{	this->m_szDriverPath = szDriverPath;										}
	void SetDriverPathExt(const std::string& szDriverPathExt)													{	this->m_szDriverPathExt = szDriverPathExt;									}
	void SetPCIDeviceID(const std::string& szPCIDeviceID)														{	this->m_szPCIDeviceID = szPCIDeviceID;										}
	void SetSubSystemID(const std::string& szSubSystemID)														{	this->m_szSubSystemVendorID = szSubSystemID;								}
	void SetSubSystemVendorID(const std::string& szSubSystemVendorID)											{	this->m_szSubSystemVendorID = szSubSystemVendorID;							}
	void SetBiosDate(const std::string& szBiosDate)																{	this->m_szBiosDate = szBiosDate;											}
	void SetBiosPartNumber(const std::string& szBiosPartNumber)													{	this->m_szBiosPartNumber = szBiosPartNumber;								}
	void SetBiosVersion(const std::string& szBiosVersion)														{	this->m_szBiosVersion = szBiosVersion;										}
};


#endif //_ADLGPU_H_