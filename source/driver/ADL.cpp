////////////////////////////////////////////////
// File:	ADL.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper) : Based off of ADL code from Con Kolivas
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "ADL.h"

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <malloc.h>
#include <math.h>
#include "../gpu/BaseGPU.h"
#include "../gpu/GPUData.h"
#include "../gpu/ADLGPU.h"
#include "../config/GPUSetting.h"
#include "../event/EventManager.h"

void ADL::SetupADL()
{
	m_hDLL = LOAD_LIB(ADL_LIB_NAME, ADL_LIB_FLAGS);
	if (m_hDLL == 0)
	{
		m_hDLL = LOAD_LIB(ADL_LIB_NAME_2, ADL_LIB_FLAGS);
		if (m_hDLL == 0)
		{
			std::cout << "Unable to load ATI ADL library." << std::endl;
			return;
		}
	}	

	m_bstADLLock = new boost::mutex();

	ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)GetProcAddress(m_hDLL, "ADL_Main_Control_Create");
	ADL_Main_Control_Refresh = (ADL_MAIN_CONTROL_REFRESH)GetProcAddress(m_hDLL, "ADL_Main_Control_Refresh");
	ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)GetProcAddress(m_hDLL, "ADL_Main_Control_Destroy");
	ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(m_hDLL, "ADL_Adapter_NumberOfAdapters_Get");
	ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET)GetProcAddress(m_hDLL, "ADL_Adapter_AdapterInfo_Get");
	ADL_Display_DisplayInfo_Get = (ADL_DISPLAY_DISPLAYINFO_GET)GetProcAddress(m_hDLL, "ADL_Display_DisplayInfo_Get");
	ADL_Adapter_ID_Get = (ADL_ADAPTER_ID_GET)GetProcAddress(m_hDLL, "ADL_Adapter_ID_Get");
	ADL_Adapter_VideoBiosInfo_Get = (ADL_ADAPTER_VIDEOBIOSINFO_GET)GetProcAddress(m_hDLL, "ADL_Adapter_VideoBiosInfo_Get");
	ADL_Adapter_Active_Get = (ADL_ADAPTER_ACTIVE_GET)GetProcAddress(m_hDLL, "ADL_Adapter_Active_Get");
	ADL_Overdrive_Caps = (ADL_OVERDRIVE_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive_Caps");


	if (NULL == ADL_Main_Control_Create ||
		NULL == ADL_Main_Control_Destroy ||
		NULL == ADL_Adapter_NumberOfAdapters_Get ||
		NULL == ADL_Adapter_AdapterInfo_Get ||
		NULL == ADL_Adapter_Active_Get ||
		NULL == ADL_Overdrive_Caps)
	{
		std::cout << "ADL API Missing!" << std::endl;
	}

	int adlResult = 0;

	m_bstADLLock->lock();
	{
		adlResult = ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1);
	}
	m_bstADLLock->unlock();

	if (ADL_OK != adlResult)
	{
		std::cout << "ADL Initialization Error: " << GetADLError(adlResult) << std::endl;
	}

	m_bstADLLock->lock();
	{
		adlResult = ADL_Main_Control_Refresh();
	}
	m_bstADLLock->unlock();

	if (ADL_OK != adlResult)
	{
		std::cout << "ADL Refresh Error: " << GetADLError(adlResult) << std::endl;
	}

	ADL_Overdrive5_ThermalDevices_Enum = (ADL_OVERDRIVE5_THERMALDEVICES_ENUM)GetProcAddress(m_hDLL, "ADL_Overdrive5_ThermalDevices_Enum");
	ADL_Overdrive5_ODParameters_Get = (ADL_OVERDRIVE5_ODPARAMETERS_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_ODParameters_Get");
	ADL_Overdrive5_Temperature_Get = (ADL_OVERDRIVE5_TEMPERATURE_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_Temperature_Get");
	ADL_Overdrive5_FanSpeed_Get = (ADL_OVERDRIVE5_FANSPEED_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_FanSpeed_Get");
	ADL_Overdrive5_FanSpeedInfo_Get = (ADL_OVERDRIVE5_FANSPEEDINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_FanSpeedInfo_Get");
	ADL_Overdrive5_ODPerformanceLevels_Get = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_ODPerformanceLevels_Get");
	ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_CurrentActivity_Get");
	ADL_Overdrive5_FanSpeed_Set = (ADL_OVERDRIVE5_FANSPEED_SET)GetProcAddress(m_hDLL, "ADL_Overdrive5_FanSpeed_Set");
	ADL_Overdrive5_ODPerformanceLevels_Set = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET)GetProcAddress(m_hDLL, "ADL_Overdrive5_ODPerformanceLevels_Set");
	ADL_Overdrive5_PowerControl_Caps = (ADL_OVERDRIVE5_POWERCONTROL_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive5_PowerControl_Caps");
	ADL_Overdrive5_PowerControlInfo_Get = (ADL_OVERDRIVE5_POWERCONTROLINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_PowerControlInfo_Get");
	ADL_Overdrive5_PowerControl_Get = (ADL_OVERDRIVE5_POWERCONTROL_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_PowerControl_Get");
	ADL_Overdrive5_PowerControl_Set = (ADL_OVERDRIVE5_POWERCONTROL_SET)GetProcAddress(m_hDLL, "ADL_Overdrive5_PowerControl_Set");


	if (NULL == ADL_Overdrive5_ThermalDevices_Enum ||
		NULL == ADL_Overdrive5_Temperature_Get ||
		NULL == ADL_Overdrive5_FanSpeedInfo_Get ||
		NULL == ADL_Overdrive5_ODPerformanceLevels_Get ||
		NULL == ADL_Overdrive5_ODParameters_Get ||
		NULL == ADL_Overdrive5_CurrentActivity_Get ||
		NULL == ADL_Overdrive5_FanSpeed_Set ||
		NULL == ADL_Overdrive5_ODPerformanceLevels_Set ||
		NULL == ADL_Overdrive5_PowerControl_Caps ||
		NULL == ADL_Overdrive5_PowerControlInfo_Get ||
		NULL == ADL_Overdrive5_PowerControl_Get ||
		NULL == ADL_Overdrive5_PowerControl_Set)
	{
		m_bOD5Found = false;

		std::cout << "Overdrive 5 Not Found!" << std::endl;
		std::cout << std::endl;
	}
	else
	{
		m_bOD5Found = true;
	}


	ADL_Overdrive6_FanSpeed_Get = (ADL_OVERDRIVE6_FANSPEED_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_FanSpeed_Get");
	ADL_Overdrive6_ThermalController_Caps = (ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive6_ThermalController_Caps");
	ADL_Overdrive6_Temperature_Get = (ADL_OVERDRIVE6_TEMPERATURE_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_Temperature_Get");
	ADL_Overdrive6_Capabilities_Get = (ADL_OVERDRIVE6_CAPABILITIES_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_Capabilities_Get");
	ADL_Overdrive6_StateInfo_Get = (ADL_OVERDRIVE6_STATEINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_StateInfo_Get");
	ADL_Overdrive6_CurrentStatus_Get = (ADL_OVERDRIVE6_CURRENTSTATUS_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_CurrentStatus_Get");
	ADL_Overdrive6_PowerControl_Caps = (ADL_OVERDRIVE6_POWERCONTROL_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControl_Caps");
	ADL_Overdrive6_PowerControlInfo_Get = (ADL_OVERDRIVE6_POWERCONTROLINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControlInfo_Get");
	ADL_Overdrive6_PowerControl_Get = (ADL_OVERDRIVE6_POWERCONTROL_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControl_Get");
	ADL_Overdrive6_FanSpeed_Set = (ADL_OVERDRIVE6_FANSPEED_SET)GetProcAddress(m_hDLL, "ADL_Overdrive6_FanSpeed_Set");
	ADL_Overdrive6_State_Set = (ADL_OVERDRIVE6_STATE_SET)GetProcAddress(m_hDLL, "ADL_Overdrive6_State_Set");
	ADL_Overdrive6_PowerControl_Set = (ADL_OVERDRIVE6_POWERCONTROL_SET)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControl_Set");
	ADL_Overdrive6_VoltageControlInfo_Get = (ADL_OVERDRIVE6_VOLTAGECONTROLINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_VoltageControlInfo_Get");
	ADL_Overdrive6_VoltageControl_Get = (ADL_OVERDRIVE6_VOLTAGECONTROL_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_VoltageControl_Get");
	ADL_Overdrive6_VoltageControl_Set = (ADL_OVERDRIVE6_VOLTAGECONTROL_SET)GetProcAddress(m_hDLL, "ADL_Overdrive6_VoltageControl_Set");
	ADL_Overdrive6_FanSpeed_Reset = (ADL_OVERDRIVE6_FANSPEED_RESET)GetProcAddress(m_hDLL, "ADL_Overdrive6_FanSpeed_Reset");
	ADL_Overdrive6_State_Reset = (ADL_OVERDRIVE6_STATE_RESET)GetProcAddress(m_hDLL, "ADL_Overdrive6_State_Reset");


	if (NULL == ADL_Overdrive6_FanSpeed_Get ||
		NULL == ADL_Overdrive6_ThermalController_Caps ||
		NULL == ADL_Overdrive6_Temperature_Get ||
		NULL == ADL_Overdrive6_Capabilities_Get ||
		NULL == ADL_Overdrive6_StateInfo_Get ||
		NULL == ADL_Overdrive6_CurrentStatus_Get ||
		NULL == ADL_Overdrive6_PowerControl_Caps ||
		NULL == ADL_Overdrive6_PowerControlInfo_Get ||
		NULL == ADL_Overdrive6_PowerControl_Get ||
		NULL == ADL_Overdrive6_FanSpeed_Set ||
		NULL == ADL_Overdrive6_State_Set ||
		NULL == ADL_Overdrive6_PowerControl_Set ||
		NULL == ADL_Overdrive6_VoltageControlInfo_Get ||
		NULL == ADL_Overdrive6_VoltageControl_Get ||
		NULL == ADL_Overdrive6_VoltageControl_Set ||
		NULL == ADL_Overdrive6_FanSpeed_Reset ||
		NULL == ADL_Overdrive6_State_Reset)
	{
		m_bOD6Found = false;

		std::cout << "Overdrive 6 Not Found!" << std::endl;
		std::cout << std::endl;
	}
	else
	{
		m_bOD6Found = true;
	}


	m_bstADLLock->lock();
	{
		adlResult = ADL_Adapter_NumberOfAdapters_Get(&m_nNumAdapters);
	}
	m_bstADLLock->unlock();
	
	if (ADL_OK != adlResult)
	{
		std::cout << "Cannot get the number of adapters!" << std::endl;
	}


	if (m_nNumAdapters > 0)
	{
		m_lpInfo = (LPAdapterInfo)malloc(sizeof (AdapterInfo) * m_nNumAdapters);
		memset(m_lpInfo, '\0', sizeof (AdapterInfo)* m_nNumAdapters);

		// Get the AdapterInfo structure for all adapters in the system

		m_bstADLLock->lock();
		{
			adlResult = ADL_Adapter_AdapterInfo_Get(m_lpInfo, sizeof(AdapterInfo) * m_nNumAdapters);
		}
		m_bstADLLock->unlock();

		
		if (ADL_OK != adlResult)
		{
			std::cout << "DL_Adapter_AdapterInfo_Get Error! Error: " << adlResult << std::endl;
		}
	}
	else
	{
		std::cout << "No Adapters were found!" << std::endl;
	}

	int nBusID = -1;
	int adapterId = -1;

	m_nNumDevices = 0;
	bool bDevicesMatch = true;

	for (int i = 0; i < m_nNumAdapters; ++i)
	{
		int adapterIndex = m_lpInfo[i].iAdapterIndex;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Adapter_ID_Get(adapterIndex, &adapterId);
		}
		m_bstADLLock->unlock();

		if (adlResult != ADL_OK)
		{
			std::cout << "ADL_Adapter_ID_Get Failed. Error " << adlResult << std::endl;
			if (adlResult == -10)
			{
				std::cout << "Device is not enabled" << adlResult << std::endl;
			}
			continue;
		}

<<<<<<< HEAD
		if (std::find(m_vecValidAdapters.begin(), m_vecValidAdapters.end(), adapterId) != m_vecValidAdapters.end())
		{
			continue;
		}

		if (adapterId == 0)
=======
		if (nLastAdapter == adapterId)
			continue;

		m_nNumDevices++;
		nLastAdapter = adapterId;
		if(adapterId == 0)
>>>>>>> origin/master
		{
			std::cout << "Adapter returns ID 0 meaning not AMD. Card order might be confused" << std::endl;
			continue;
		}

		//int adapterActive = 0;
		AdapterInfo adapterInfo = m_lpInfo[i];

		if (adapterInfo.iVendorID == AMDVENDORID)
		{
			m_vecValidAdapters.push_back(adapterId);
			m_vecAdapterInfo.push_back(adapterInfo);

			int  iOverdriveSupported = 0;
			int  iOverdriveEnabled = 0;
			int  iOverdriveVersion = 0;

			m_bstADLLock->lock();
			{
				adlResult = ADL_Overdrive_Caps(adapterInfo.iAdapterIndex, &iOverdriveSupported, &iOverdriveEnabled, &iOverdriveVersion);
			}
			m_bstADLLock->unlock();
					

			ADLGPU* pADLGPU = new ADLGPU();

			pADLGPU->SetGPUID(i);

			pADLGPU->SetOverdriveEnabled(iOverdriveEnabled);
			pADLGPU->SetOverdriveSupported(iOverdriveSupported);
			pADLGPU->SetOverdriveVersion(iOverdriveVersion);

			pADLGPU->SetAdapterIndex(adapterInfo.iAdapterIndex);
			pADLGPU->SetAdapterName(adapterInfo.strAdapterName);
			pADLGPU->SetDisplayName(adapterInfo.strDisplayName);
			pADLGPU->SetBusNumber(adapterInfo.iBusNumber);
			pADLGPU->SetDeviceNumber(adapterInfo.iDeviceNumber);
			pADLGPU->SetAIExist(adapterInfo.iExist);
			pADLGPU->SetAIFunctionNumber(adapterInfo.iFunctionNumber);
			pADLGPU->SetAIOSDisplayIndex(adapterInfo.iOSDisplayIndex);
			pADLGPU->SetAIPresent(adapterInfo.iPresent);
			pADLGPU->SetVendorID(adapterInfo.iVendorID);
			pADLGPU->SetDriverPath(adapterInfo.strDriverPath);
			pADLGPU->SetDriverPathExt(adapterInfo.strDriverPathExt);
			pADLGPU->SetPNPString(adapterInfo.strPNPString);
			pADLGPU->SetUDID(adapterInfo.strUDID);

			std::string szDeviceID;
			std::string szSubSystemID;
			std::string szSubSystemVendorID;
			std::string szUDID = pADLGPU->GetUDID();

			int devIndex = szUDID.find("&DEV_");
			if (devIndex != std::string::npos)
			{
				szDeviceID = szUDID.substr(devIndex + 5, 4);
			}

			int subVendorIndex = szUDID.find("&SUBSYS_");
			if (subVendorIndex != std::string::npos)
			{
				szSubSystemID = szUDID.substr(subVendorIndex + 8, 4);
				szSubSystemVendorID = szUDID.substr(subVendorIndex + 12, 4);
			}

			pADLGPU->SetPCIDeviceID(szDeviceID);
			pADLGPU->SetSubSystemID(szSubSystemID);
			pADLGPU->SetSubSystemVendorID(szSubSystemVendorID);

			if (adlResult != ADL_OK)
			{
				std::cout << "ADL_Overdrive_Caps failed for Device ID " << szDeviceID << std::endl;
				std::cout << std::endl;
			}

			ADLBiosInfo BiosInfo;
			m_bstADLLock->lock();
			{
				adlResult = ADL_Adapter_VideoBiosInfo_Get(adapterInfo.iAdapterIndex, &BiosInfo);
			}
			m_bstADLLock->unlock();

			pADLGPU->SetBiosDate(BiosInfo.strDate);
			pADLGPU->SetBiosPartNumber(BiosInfo.strPartNumber);
			pADLGPU->SetBiosVersion(BiosInfo.strVersion);

			m_vecDevices.push_back(pADLGPU);
		}
	}

	if (m_vecDevices.size() == 0)
	{
		std::cout << "Cannot find an active AMD adapter!" << std::endl;
	}
}

ADL::ADL()
{
	this->m_enmEntityType = ENTITY_TYPE::ADL_ENTITY;
	m_bOD5Found = false;
	m_bOD6Found = false;
	m_bIsActive = false;

	SetupADL();

	EventManager::GetInstance()->Register(Event::EVENT_TYPE::SET_ADL_GPU_FAN, this);
	EventManager::GetInstance()->Register(Event::EVENT_TYPE::SET_ADL_GPU_ENGINE_CLOCK, this);
	EventManager::GetInstance()->Register(Event::EVENT_TYPE::SET_ADL_GPU_MEMORY_CLOCK, this);
	EventManager::GetInstance()->Register(Event::EVENT_TYPE::SET_ADL_GPU_VOLTAGE, this);
	EventManager::GetInstance()->Register(Event::EVENT_TYPE::SET_ADL_GPU_POWERTUNE, this);
}

const std::vector<std::string> ADL::GetAdapterIDs()
{
	std::vector<std::string> vecAdapterIDs;
	for (size_t i = 0; i < m_vecDevices.size(); ++i)
	{
		vecAdapterIDs.push_back(m_vecDevices[i]->GetPCIDeviceID());
	}

	return vecAdapterIDs;
}


void ADL::SetupOD5(ADLGPU* adlGPU, GPUSetting* gpuSetting)
{
	int adlResult = 0;
	int iAdapterIndex = adlGPU->GetAdapterIndex();

	ADLODParameters adlParams = { 0 };
	adlParams.iSize = sizeof(ADLODParameters);

	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive5_ODParameters_Get(iAdapterIndex, &adlParams);
	}
	m_bstADLLock->unlock();

	if (adlResult != ADL_OK)
	{
		std::cout << "ADL_Overdrive5_ODParameters_Get Failed!" << std::endl;
	}

	adlGPU->SetActivityReportingSupported(adlParams.iActivityReportingSupported);
	adlGPU->SetDiscretePerformanceLevels(adlParams.iDiscretePerformanceLevels);
	adlGPU->SetNumberOfPerformanceLevels(adlParams.iNumberOfPerformanceLevels);
	adlGPU->SetEngineClockMax(adlParams.sEngineClock.iMax);
	adlGPU->SetEngineClockMin(adlParams.sEngineClock.iMin);
	adlGPU->SetEngineClockStep(adlParams.sEngineClock.iStep);
	adlGPU->SetMemoryClockMax(adlParams.sMemoryClock.iMax);
	adlGPU->SetMemoryClockMin(adlParams.sMemoryClock.iMin);
	adlGPU->SetMemoryClockStep(adlParams.sMemoryClock.iStep);
	adlGPU->SetVDDCMax(adlParams.sVddc.iMax);
	adlGPU->SetVDDCMin(adlParams.sVddc.iMin);
	adlGPU->SetVDDCStep(adlParams.sVddc.iStep);


	size_t perfLevelSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (adlGPU->GetNumberOfPerformanceLevels() - 1);

	ADLODPerformanceLevels* pOdPerformanceLevels;
	void* performanceLevelsBuffer = malloc(perfLevelSize);
	memset(performanceLevelsBuffer, 0, perfLevelSize);
	pOdPerformanceLevels = (ADLODPerformanceLevels*)performanceLevelsBuffer;
	pOdPerformanceLevels->iSize = perfLevelSize;

	m_bstADLLock->lock();
	{
		//Defaults
		adlResult = ADL_Overdrive5_ODPerformanceLevels_Get(iAdapterIndex, 1, pOdPerformanceLevels);
	}
	m_bstADLLock->unlock();

<<<<<<< HEAD
	if (adlResult != ADL_OK)
	{
		std::cout << "ADL_Overdrive5_ODPerformanceLevels_Get Failed!" << std::endl;
	}
	
	adlGPU->SetDefaultEngineClock(pOdPerformanceLevels->aLevels[0].iEngineClock / 100);
	adlGPU->SetDefaultMemoryClock(pOdPerformanceLevels->aLevels[0].iMemoryClock / 100);
	adlGPU->SetDefaultVoltage(pOdPerformanceLevels->aLevels[0].iVddc);
	
=======
	
	adapterId = -1;
    for (int i = 0; i < m_nNumDevices; ++i)
    {
		if (!gpus[i]->GetGPU()->GetGPUSetting()->GetIsEnabled())
		{
			gpus[i]->SetGPUEngine(0);
			gpus[i]->SetGPUMemclock(0);
			gpus[i]->SetGPUVDDC(0);
			gpus[i]->SetGPUFan(0);
			gpus[i]->SetGPUPowerTune(0);
			continue;
		}
>>>>>>> origin/master

	memset(performanceLevelsBuffer, 0, perfLevelSize);
	pOdPerformanceLevels->iSize = perfLevelSize;

	m_bstADLLock->lock();
	{
		//Current
		adlResult = ADL_Overdrive5_ODPerformanceLevels_Get(iAdapterIndex, 0, pOdPerformanceLevels);
	}
	m_bstADLLock->unlock();

	if (adlResult != ADL_OK)
	{
		std::cout << "ADL_Overdrive5_ODPerformanceLevels_Get Failed!" << std::endl;
	}

<<<<<<< HEAD
	adlGPU->SetCurrentEngineClock(pOdPerformanceLevels->aLevels[0].iEngineClock / 100);
	adlGPU->SetCurrentMemoryClock(pOdPerformanceLevels->aLevels[0].iMemoryClock / 100);
	adlGPU->SetCurrentVoltage(pOdPerformanceLevels->aLevels[0].iVddc);

	free(performanceLevelsBuffer);

	ADLThermalControllerInfo tcInfo = { 0 };
	tcInfo.iSize = sizeof (ADLThermalControllerInfo);
		
	int thermalIndex = 0;
	for (int iThermalControllerIndex = 0; iThermalControllerIndex < 10; iThermalControllerIndex++)
	{
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_ThermalDevices_Enum(iAdapterIndex, iThermalControllerIndex, &tcInfo);
=======
		//gpus[i]->SetName(m_lpInfo[i].strAdapterName);
		m_bIsActive = true;

		ADLGPU* adlGPU = (ADLGPU*)gpus[i]->GetGPU();
		if(adlGPU == NULL)
		{
			adlGPU = new ADLGPU();
		}
		
		adlGPU->SetGPUID(i);
		adlGPU->SetAdapterIndex(iAdapterIndex);
		adlGPU->SetLPAdapterID(lpAdapterID);
		adlGPU->SetAdapterName(m_lpInfo[i].strAdapterName);
		adlGPU->SetIsDefFanValid(false);

		ADLBiosInfo BiosInfo;
		adlResult = ADL_Adapter_VideoBiosInfo_Get(iAdapterIndex, &BiosInfo);
		if (adlResult != ADL_ERR)
		{
			adlGPU->SetADLBiosInfo(BiosInfo);
>>>>>>> origin/master
		}
		m_bstADLLock->unlock();

		if (tcInfo.iThermalDomain == ADL_DL_THERMAL_DOMAIN_GPU)
		{
			thermalIndex = iThermalControllerIndex;
			break;
		}
	}

	adlGPU->SetThermalDomain(tcInfo.iThermalDomain);
	adlGPU->SetThermalDomainIndex(tcInfo.iDomainIndex);
	adlGPU->SetThermalFlags(tcInfo.iFlags);
	adlGPU->SetThermalControllerIndex(thermalIndex);

	adlGPU->SetCurrentTemperature(GetGPUTemp(adlGPU));

	ADLFanSpeedInfo adlFanSpeedInfo = { 0 };
	adlFanSpeedInfo.iSize = sizeof(ADLFanSpeedInfo);
	
	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive5_FanSpeedInfo_Get(iAdapterIndex, adlGPU->GetThermalControllerIndex(), &adlFanSpeedInfo);
	}
	m_bstADLLock->unlock();

	if (adlResult == ADL_OK)
	{
		adlGPU->SetHasFanSpeed(true);
	}
	else
	{
		std::cout << "Failed to get fan info" << std::endl;
		adlGPU->SetHasFanSpeed(false);
	}

	adlGPU->SetFanMaxPercent(adlFanSpeedInfo.iMaxPercent);
	adlGPU->SetFanMaxRPM(adlFanSpeedInfo.iMaxRPM);
	adlGPU->SetFanMinPercent(adlFanSpeedInfo.iMinPercent);
	adlGPU->SetFanMinRPM(adlFanSpeedInfo.iMinRPM);
	adlGPU->SetFanSpeedFlags(adlFanSpeedInfo.iFlags);	

	int nFanSpeedMethod = ((adlFanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_READ) == ADL_DL_FANCTRL_SUPPORTS_RPM_READ) ? ADL_DL_FANCTRL_SPEED_TYPE_RPM : ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
	adlGPU->SetFanSpeedMethod(nFanSpeedMethod);

	int nFanSpeed = GetFanSpeed(adlGPU);
	adlGPU->SetDefaultFanSpeed(nFanSpeed);
	adlGPU->SetCurrentFanSpeed(nFanSpeed);


	int nPowerControlSupported = 0;

	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive5_PowerControl_Caps(iAdapterIndex, &nPowerControlSupported);
	}
	m_bstADLLock->unlock();

	if (adlResult != ADL_OK)
	{
		std::cout << "Failed to get Power Controls support" << std::endl;
	}

	adlGPU->SetPowerControlSupported(nPowerControlSupported);

	if (nPowerControlSupported)
	{
		ADLPowerControlInfo powerControlInfo = { 0 };

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_PowerControlInfo_Get(iAdapterIndex, &powerControlInfo);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetPowertuneMax(powerControlInfo.iMaxValue);
			adlGPU->SetPowertuneMin(powerControlInfo.iMinValue);
			adlGPU->SetPowertuneStep(powerControlInfo.iStepValue);

			int nCurrent = 0;
			int nDefault = 0;

			m_bstADLLock->lock();
			{
				adlResult = ADL_Overdrive5_PowerControl_Get(iAdapterIndex, &nCurrent, &nDefault);
			}
			m_bstADLLock->unlock();

			if (adlResult == ADL_OK)
			{
				adlGPU->SetPowerControlCurrent(nCurrent);
				adlGPU->SetPowerControlDefault(nDefault);
			}
			else
			{
				std::cout << "Failed to get Power Control current value" << std::endl;
			}				
		}
		else
		{
			std::cout << "Failed to get Power Control Info" << std::endl;
		}			
	}

	int engineClck = gpuSetting->GetEngineClock();
	if (engineClck > 0)
	{
		std::cout << "Setting GPU " << iAdapterIndex << " Engine Clock to " << engineClck << std::endl;
		SetEngineClock(adlGPU, engineClck);
	}

	int memClck = gpuSetting->GetMemclock();
	if (memClck > 0)
	{
		std::cout << "Setting GPU " << iAdapterIndex << " Memory Clock to " << memClck << std::endl;
		SetMemoryClock(adlGPU, memClck);
	}

	float voltage = gpuSetting->GetVoltage();
	if (voltage > 0.0f)
	{
		std::cout << "Setting GPU " << iAdapterIndex << " Voltage to " << voltage << std::endl;
		SetVDDC(adlGPU, voltage);
	}

	int nPowerTune = gpuSetting->GetPowerTune();
	if (nPowerTune >= -50 && nPowerTune <= 50)
	{
		std::cout << "Setting GPU " << iAdapterIndex << " Powertune to " << nPowerTune << std::endl;

		SetPowertune(adlGPU, nPowerTune);
	}
}

void ADL::SetupOD6(ADLGPU* adlGPU, GPUSetting* gpuSetting)
{
	int adlResult = -1;
	int iAdapterIndex = adlGPU->GetAdapterIndex();

	ADLOD6Capabilities adlCapabilities = { 0 };
	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive6_Capabilities_Get(iAdapterIndex, &adlCapabilities);
	}
	m_bstADLLock->unlock();

	if (adlResult == ADL_OK)
	{
		adlGPU->SetOD6Capabilities(adlCapabilities.iCapabilities);
		adlGPU->SetNumberOfPerformanceLevels(adlCapabilities.iNumberOfPerformanceLevels);
		adlGPU->SetEngineClockMin(adlCapabilities.sEngineClockRange.iMin);
		adlGPU->SetEngineClockMax(adlCapabilities.sEngineClockRange.iMax);
		adlGPU->SetEngineClockStep(adlCapabilities.sEngineClockRange.iStep);
		adlGPU->SetMemoryClockMin(adlCapabilities.sMemoryClockRange.iMin);
		adlGPU->SetMemoryClockMax(adlCapabilities.sMemoryClockRange.iMax);
		adlGPU->SetMemoryClockStep(adlCapabilities.sMemoryClockRange.iStep);
	}
	else
	{
		std::cout << "ADL_Overdrive6_Capabilities_Get Failed!" << std::endl;
	}

	if (adlCapabilities.iNumberOfPerformanceLevels != 2)
	{
		std::cout << "Unexpected number of performance levels" << std::endl;
	}

	int size = sizeof (ADLOD6StateInfo)+sizeof (ADLOD6PerformanceLevel);
	ADLOD6StateInfo * defaultStateInfo = (ADLOD6StateInfo*)malloc(size);
	memset((void*)defaultStateInfo, 0, size);
	defaultStateInfo->iNumberOfPerformanceLevels = 2;

	//Getting default effective minimum and maximum values for memory and GPU clocks.
	//The only state supported by Overdrive6 is "Performance".
	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive6_StateInfo_Get(iAdapterIndex, ADL_OD6_GETSTATEINFO_DEFAULT_PERFORMANCE, defaultStateInfo);
	}
	m_bstADLLock->unlock();
		
	if (adlResult == ADL_OK)
	{
		adlGPU->SetDefaultEngineClock(defaultStateInfo->aLevels[0].iEngineClock / 100);
		adlGPU->SetDefaultMemoryClock(defaultStateInfo->aLevels[0].iMemoryClock / 100);
	}
	else
	{
		std::cout << "Failed to get default performance levels info" << std::endl;
	}

	free(defaultStateInfo);

	ADLOD6CurrentStatus currentStatus = { 0 };
	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive6_CurrentStatus_Get(iAdapterIndex, &currentStatus);
	}
	m_bstADLLock->unlock();
 
	if (adlResult == ADL_OK)
	{
		adlGPU->SetCurrentEngineClock(currentStatus.iEngineClock / 100);
		adlGPU->SetCurrentMemoryClock(currentStatus.iMemoryClock / 100);
		adlGPU->SetCurrentActivity(currentStatus.iActivityPercent);
	}
	else
	{
		std::cout << "Failed to get current status" << std::endl;
	}


	bool bHasFanControl = false;
	int fanSpeedMethod = 0;

	ADLOD6ThermalControllerCaps tcCaps = { 0 };
	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive6_ThermalController_Caps(iAdapterIndex, &tcCaps);
	}
	m_bstADLLock->unlock();

	if (adlResult != ADL_OK)
	{
		std::cout << "Failed to get thermal controller capabilities" << std::endl;
	}
	else if (ADL_OD6_TCCAPS_FANSPEED_CONTROL == (tcCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_CONTROL))
	{
		if (ADL_OD6_TCCAPS_FANSPEED_PERCENT_READ == (tcCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_PERCENT_READ) ||
			ADL_OD6_TCCAPS_FANSPEED_RPM_READ == (tcCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_RPM_READ))
		{
			bHasFanControl = true;
			fanSpeedMethod = ((tcCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE) == ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE) ? ADL_OD6_FANSPEED_TYPE_PERCENT : ADL_OD6_FANSPEED_TYPE_RPM;
		}
	}

	adlGPU->SetHasFanSpeed(bHasFanControl);
	adlGPU->SetFanSpeedMethod(fanSpeedMethod);
	adlGPU->SetThermalControllerCapabilities(tcCaps.iCapabilities);
	adlGPU->SetFanMaxPercent(tcCaps.iFanMaxPercent);
	adlGPU->SetFanMinPercent(tcCaps.iFanMinPercent);
	adlGPU->SetFanMaxRPM(tcCaps.iFanMaxRPM);
	adlGPU->SetFanMinRPM(tcCaps.iFanMinRPM);

	int fanSpeed = GetFanSpeed(adlGPU);
	adlGPU->SetDefaultFanSpeed(fanSpeed);
	adlGPU->SetCurrentFanSpeed(fanSpeed);

	int powerControlSupported = 0;

	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive6_PowerControl_Caps(iAdapterIndex, &powerControlSupported);
	}
	m_bstADLLock->unlock();

	if (adlResult == ADL_OK)
	{
		adlGPU->SetPowerControlSupported(powerControlSupported);
	}

	if (powerControlSupported)
	{
		ADLOD6PowerControlInfo pwrCntrlInfo = { 0 };
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_PowerControlInfo_Get(iAdapterIndex, &pwrCntrlInfo);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetPowertuneMax(pwrCntrlInfo.iMaxValue);
			adlGPU->SetPowertuneMin(pwrCntrlInfo.iMinValue);
			adlGPU->SetPowertuneStep(pwrCntrlInfo.iStepValue);

			int powerControlCurrent = 0;
			int powerControlDefault = 0;

			m_bstADLLock->lock();
			{
				ADL_Overdrive6_PowerControl_Get(iAdapterIndex, &powerControlCurrent, &powerControlDefault);
			}
			m_bstADLLock->unlock();

			if (adlResult == ADL_OK)
			{
				adlGPU->SetPowerControlCurrent(powerControlCurrent);
				adlGPU->SetPowerControlDefault(powerControlDefault);
			}
		}
	}

	ADLOD6VoltageControlInfo voltageControlInfo = { 0 };
	m_bstADLLock->lock();
	{
		adlResult = ADL_Overdrive6_VoltageControlInfo_Get(iAdapterIndex, &voltageControlInfo);
	}
	m_bstADLLock->unlock();

	if (adlResult == ADL_OK)
	{
		adlGPU->SetVDDCMax(voltageControlInfo.iMaxValue);
		adlGPU->SetVDDCMin(voltageControlInfo.iMinValue);
		adlGPU->SetVDDCStep(voltageControlInfo.iStepValue);
	}
	else
	{
		std::cout << "Failed to get voltage controller info" << std::endl;
	}

	adlGPU->SetDefaultVoltage(GetVDDC(adlGPU));
	adlGPU->SetCurrentTemperature(GetGPUTemp(adlGPU));

	int engineClck = gpuSetting->GetEngineClock();
	if (engineClck > 0)
	{
		std::cout << "Setting GPU " << iAdapterIndex << " Engine Clock to " << engineClck << std::endl;
		SetEngineClock(adlGPU, engineClck);
	}

	int memClck = gpuSetting->GetMemclock();
	if (memClck > 0)
	{
		std::cout << "Setting GPU " << iAdapterIndex << " Memory Clock to " << memClck << std::endl;
		SetMemoryClock(adlGPU, memClck);
	}

	float voltage = gpuSetting->GetVoltage();
	if (voltage > 0.0f)
	{
		std::cout << "Setting GPU " << iAdapterIndex << " Voltage to " << voltage << std::endl;
		SetVDDC(adlGPU, voltage);
	}

	int nPowerTune = gpuSetting->GetPowerTune();
	if (nPowerTune >= -50 && nPowerTune <= 50)
	{
		std::cout << "Setting GPU " << iAdapterIndex << " Powertune to " << nPowerTune << std::endl;
		SetPowertune(adlGPU, nPowerTune);
	}
}

void ADL::SetupADLDevices(std::vector<GPUData*> gpus)
{
	if (gpus.size() == 0)
	{
		return;
	}

	m_bIsActive = true;

	std::vector<int> usedGPUData;
	for (size_t i = 0; i < m_vecDevices.size(); ++i)
	{
		ADLGPU* pADLGPU = m_vecDevices[i];

		if (!pADLGPU->GetOverdriveSupported())
		{
			std::cout << "Overdrive not supported for GPU ID: " << pADLGPU->GetPCIDeviceID() << std::endl;
			std::cout << std::endl;

			std::cout << "Overdrive Version is " << pADLGPU->GetOverdriveVersion() << " for GPU ID : " << pADLGPU->GetPCIDeviceID() << std::endl;
			std::cout << std::endl;

			continue;
		}

		int iAdapterIndex = pADLGPU->GetAdapterIndex();

		GPUSetting* pGPUSetting = NULL;
		for (size_t settingIndex = 0; settingIndex < gpus.size(); ++settingIndex)
		{
			if (std::find(usedGPUData.begin(), usedGPUData.end(), settingIndex) != usedGPUData.end())
			{
				continue;
			}

			if (pADLGPU->GetPCIDeviceID().compare(gpus[settingIndex]->GetGPUSetting()->GetDeviceID()) == 0)
			{
				gpus[settingIndex]->SetAdapterIndex(iAdapterIndex);
				usedGPUData.push_back(settingIndex);

				pGPUSetting = gpus[settingIndex]->GetGPUSetting();
				break;
			}
		}

		//Couldn't find an actual setting for this device
		//Hope that OpenCL and ADL are using the same device ordering, which is unlikely.
		if (pGPUSetting == NULL)
		{
			std::cout << "Could not find ADL GPU Setting for Device ID: " << pADLGPU->GetPCIDeviceID() << std::endl;
			std::cout << std::endl;

			if ((gpus.size() == 0) || ((gpus.size() - 1) < i))
			{
				pGPUSetting = gpus[0]->GetGPUSetting();
			}
			else
			{
				pGPUSetting = gpus[i]->GetGPUSetting();
			}
		}
		else
		{
			std::cout << "Found ADL GPU Setting for Device ID: " << pADLGPU->GetPCIDeviceID() << std::endl;
			std::cout << std::endl;
		}

		if (pADLGPU->GetOverdriveVersion() == 6)
		{
			std::cout << "AMD Overdrive Version 6 found for Device ID: " << pADLGPU->GetPCIDeviceID() << std::endl;
			std::cout << std::endl;

			SetupOD6(pADLGPU, pGPUSetting);
		}
		else if (pADLGPU->GetOverdriveVersion() == 5)
		{
			std::cout << "AMD Overdrive Version 5 found for Device ID: " << pADLGPU->GetPCIDeviceID() << std::endl;
			std::cout << std::endl;

			SetupOD5(pADLGPU, pGPUSetting);
		}	
		else
		{
			std::cout << "Unsupported AMD Overdrive Version " << pADLGPU->GetOverdriveVersion() << " found for Device ID: " << pADLGPU->GetPCIDeviceID() << std::endl;
			std::cout << std::endl;
		}
	}
}

void ADL::ClearDevices()
{
	for (size_t devIndex = 0; devIndex < m_vecDevices.size(); ++devIndex)
	{
		ADLGPU* adlGPU = m_vecDevices[devIndex];
		if (adlGPU != NULL)
		{
			ResetDevice(adlGPU);
			delete(adlGPU);
			adlGPU = NULL;
		}
	}

	m_vecDevices.clear();
}


///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
ADL::ADL(const ADL& adl)
{
	this->m_enmEntityType = adl.GetEntityType();
}


///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
ADL::~ADL()
{
}

void ADL::Shutdown()
{
	EventManager::GetInstance()->Deregister(this);

	ClearDevices();

	ADL_Main_Memory_Free ((void **)&m_lpInfo);
	ADL_Main_Control_Destroy ();
#if defined (UNIX)
	dlclose(m_hDLL);
#else
	FreeLibrary(m_hDLL);
#endif

	if (m_bstADLLock)
	{
		delete(m_bstADLLock);
		m_bstADLLock = NULL;
	}

	m_vecAdapterInfo.clear();
}


const std::string& ADL::GetADLError(int error)
{
	std::string result;
	switch(error)
	{
		case ADL_ERR:
			result = "Generic error (escape call failed?)";
			break;
		case ADL_ERR_NOT_INIT:
			result = "ADL not initialized";
			break;
		case ADL_ERR_INVALID_PARAM:
			result = "Invalid parameter";
			break;
		case ADL_ERR_INVALID_PARAM_SIZE:
			result = "Invalid parameter size";
			break;
		case ADL_ERR_INVALID_ADL_IDX:
			result = "Invalid ADL index";
			break;
		case ADL_ERR_INVALID_CONTROLLER_IDX:
			result = "Invalid controller index";
			break;
		case ADL_ERR_INVALID_DIPLAY_IDX:
			result = "Invalid display index";
			break;
		case ADL_ERR_NOT_SUPPORTED:
			result = "Function not supported by the driver";
			break;
		case ADL_ERR_NULL_POINTER:
			result = "Null Pointer error";
			break;
		case ADL_ERR_DISABLED_ADAPTER:
			result = "Disabled adapter, can't make call";
			break;
		case ADL_ERR_INVALID_CALLBACK:
			result = "Invalid callback";
			break;
		case ADL_ERR_RESOURCE_CONFLICT:
			result = "Display resource conflict";
			break;
		case ADL_ERR_SET_INCOMPLETE:
			result = "Failed to update some of the values";
			break;
		case ADL_ERR_NO_XDISPLAY:
			result = "No Linux XDisplay in Linux Console environment";
			break;
		default:
			result = "Unhandled error";
			break;
	}
	return result;
}

const bool ADL::SetFanSpeed(ADLGPU* adlGPU, int iFanSpeed)
{
	bool retVal = false;

	if (adlGPU == NULL || !m_bIsActive)
	{
		return retVal;
	}

	int iMin = 0;
	int iMax = 0;
	
	GetFanRange(adlGPU, iMin, iMax);
	if (iFanSpeed < iMin)
	{
		iFanSpeed = iMin;
	}
	else if (iFanSpeed > iMax)
	{
		iFanSpeed = iMax;
	}

	int adlResult = 0;

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		ADLOD6FanSpeedValue adlFSV6 = { 0 };
		adlFSV6.iFanSpeed = iFanSpeed;
		adlFSV6.iSpeedType = adlGPU->GetFanSpeedMethod();

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_FanSpeed_Set(adlGPU->GetAdapterIndex(), &adlFSV6);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetCurrentFanSpeed(iFanSpeed);
			retVal = true;
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		ADLFanSpeedValue fsValue = { 0 };
		fsValue.iSpeedType = adlGPU->GetFanSpeedMethod();
		fsValue.iFanSpeed = iFanSpeed;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_FanSpeed_Set(adlGPU->GetAdapterIndex(), adlGPU->GetThermalControllerIndex(), &fsValue);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetCurrentFanSpeed(iFanSpeed);
			retVal = true;
		}
	}

	adlGPU->SetIsManaged(true);

	return retVal;
}

const bool ADL::SetMemoryClock(ADLGPU* adlGPU, int iMemoryClock)
{	
	bool retVal = false;

	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	int adlResult = 0;

	int iMin = 0; 
	int iMax = 0;
	
	GetMemoryRange(adlGPU, iMin, iMax);

	if (iMemoryClock < iMin)
	{
		iMemoryClock = iMin;
	}
	else if (iMemoryClock > iMax)
	{
		iMemoryClock = iMax;
	}

	int sentMemoryClock = iMemoryClock;
	iMemoryClock *= 100;

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		int size = sizeof (ADLOD6StateInfo)+sizeof (ADLOD6PerformanceLevel);
		ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*)malloc(size);
		memset((void*)customStateInfo, 0, size);
		customStateInfo->iNumberOfPerformanceLevels = 2;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_StateInfo_Get(adlGPU->GetAdapterIndex(), ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo);
		}
		m_bstADLLock->unlock();

		customStateInfo->aLevels[0].iMemoryClock = iMemoryClock;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_State_Set(adlGPU->GetAdapterIndex(), ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo);
		}
		m_bstADLLock->unlock();

		free(customStateInfo);

		if (adlResult == ADL_OK)
		{
			retVal = true;
			adlGPU->SetCurrentMemoryClock(sentMemoryClock);
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		size_t perfLevelSize = sizeof(ADLODPerformanceLevels)+sizeof(ADLODPerformanceLevel)* (adlGPU->GetNumberOfPerformanceLevels() - 1);

		ADLODPerformanceLevels *pOdPerformanceLevels;
		void* performanceLevelsBuffer = malloc(perfLevelSize);
		memset(performanceLevelsBuffer, 0, perfLevelSize);

		pOdPerformanceLevels = (ADLODPerformanceLevels*)performanceLevelsBuffer;
		pOdPerformanceLevels->iSize = (int)perfLevelSize;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels);
		}
		m_bstADLLock->unlock();

		for (int i = 0; i < adlGPU->GetNumberOfPerformanceLevels(); ++i)
		{
			pOdPerformanceLevels->aLevels[i].iMemoryClock = iMemoryClock;
		}

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), pOdPerformanceLevels);
		}
		m_bstADLLock->unlock();

		free(performanceLevelsBuffer);
		performanceLevelsBuffer = NULL;

		if (adlResult == ADL_OK)
		{
			retVal = true;
			adlGPU->SetCurrentMemoryClock(sentMemoryClock);
		}
	}

	adlGPU->SetIsManaged(true);

	return retVal;
}

const bool ADL::GetIsFanSpeedTwin(ADLGPU* adlGPU, ADLGPU* otherGPU)
{
	if (abs(adlGPU->GetBusNumber() - otherGPU->GetBusNumber()) != 1)
	{
		return false;
	}

	if (adlGPU->GetAdapterName().compare(otherGPU->GetAdapterName()) != 0)
	{
		return false;
	}

	if (adlGPU->GetPCIDeviceID().compare(otherGPU->GetPCIDeviceID()) != 0)
	{
		return false;
	}

	if (adlGPU->GetSubSystemVendorID().compare(otherGPU->GetSubSystemVendorID()) != 0)
	{
		return false;
	}	

	return true;
}


const bool ADL::SetEngineClock(ADLGPU* adlGPU, int iEngineClock)
{	
	bool retVal = false;

	if (adlGPU == NULL || !m_bIsActive)
	{	
		std::cout << "Set engineclock not supported!" << std::endl;
		return retVal;
	}

	int adlResult = 0;

	int iMin = 0;
	int iMax = 0;

	GetEngineRange(adlGPU, iMin, iMax);
	if (iEngineClock < iMin)
	{
		iEngineClock = iMin;
	}
	else if (iEngineClock > iMax)
	{
		iEngineClock = iMax;
	}

	int sentEngineClock = iEngineClock;
	iEngineClock *= 100;

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		int size = sizeof (ADLOD6StateInfo)+sizeof (ADLOD6PerformanceLevel);
		ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*)malloc(size);
		memset((void*)customStateInfo, 0, size);
		customStateInfo->iNumberOfPerformanceLevels = 2;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_StateInfo_Get(adlGPU->GetAdapterIndex(), ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo);
		}
		m_bstADLLock->unlock();

		customStateInfo->aLevels[0].iEngineClock = iEngineClock;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_State_Set(adlGPU->GetAdapterIndex(), ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo);
		}
		m_bstADLLock->unlock();

		free(customStateInfo);

		if (adlResult == ADL_OK)
		{
			retVal = true;
			adlGPU->SetCurrentEngineClock(sentEngineClock);
		}		
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		size_t perfLevelSize = sizeof(ADLODPerformanceLevels)+sizeof(ADLODPerformanceLevel)* (adlGPU->GetNumberOfPerformanceLevels() - 1);

		ADLODPerformanceLevels *pOdPerformanceLevels;
		void* performanceLevelsBuffer = malloc(perfLevelSize);
		memset(performanceLevelsBuffer, 0, perfLevelSize);

		pOdPerformanceLevels = (ADLODPerformanceLevels*)performanceLevelsBuffer;
		pOdPerformanceLevels->iSize = (int)perfLevelSize;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels);
		}
		m_bstADLLock->unlock();

		for (int i = 0; i < adlGPU->GetNumberOfPerformanceLevels(); ++i)
		{
			pOdPerformanceLevels->aLevels[i].iEngineClock = iEngineClock;
		}

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), pOdPerformanceLevels);
		}
		m_bstADLLock->unlock();

		free(performanceLevelsBuffer);
		performanceLevelsBuffer = NULL;

		if (adlResult == ADL_OK)
		{
			retVal = true;
			adlGPU->SetCurrentEngineClock(sentEngineClock);
		}	
	}

	adlGPU->SetIsManaged(true);

	return retVal;
}


const bool ADL::SetVDDC(ADLGPU* adlGPU, float fVddc)
{
	bool retVal = false;

	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	int adlResult = 0;	

	float fMin = 0;
	float fMax = 0;
	GetVDDCRange(adlGPU, fMin, fMax);

	if (fVddc < fMin)
	{
		fVddc = fMin;
	}
	else if (fVddc > fMax)
	{
		fVddc = fMax;
	}

	float sentVoltage = fVddc;
	int iVddc = (int)(1000 * fVddc);

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_VoltageControl_Set(adlGPU->GetAdapterIndex(), iVddc);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetCurrentVoltage(sentVoltage);
			retVal = true;
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		size_t perfLevelSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (adlGPU->GetNumberOfPerformanceLevels() - 1);

		ADLODPerformanceLevels *pOdPerformanceLevels;
		void* performanceLevelsBuffer = malloc(perfLevelSize);
		memset(performanceLevelsBuffer, 0, perfLevelSize);

		pOdPerformanceLevels = (ADLODPerformanceLevels*)performanceLevelsBuffer;
		pOdPerformanceLevels->iSize = (int)perfLevelSize;
		
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels);
		}
		m_bstADLLock->unlock();
		
		for (int i = 0; i < adlGPU->GetNumberOfPerformanceLevels(); ++i)
		{
			pOdPerformanceLevels->aLevels[i].iVddc = iVddc;
		}

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), pOdPerformanceLevels);
		}
		m_bstADLLock->unlock();

		free(performanceLevelsBuffer);
		performanceLevelsBuffer = NULL;

		if (adlResult == ADL_OK)
		{
			adlGPU->SetCurrentVoltage(sentVoltage);
			retVal = true;			
		}
	}

	adlGPU->SetIsManaged(true);

	return retVal;
}

const bool ADL::SetPowertune(ADLGPU* adlGPU, int iPercentage)
{
	int nDefault;
	int nCurrent;

	bool retVal = false;

	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	int adlResult = 0;

	int minVal = 0;
	int maxVal = 0;

	GetPowertuneRange(adlGPU, minVal, maxVal);

	if (iPercentage < minVal)
	{
		iPercentage = minVal;
	}
	else if (iPercentage > maxVal)
	{
		iPercentage = maxVal;
	}

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_PowerControl_Set(adlGPU->GetAdapterIndex(), iPercentage);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetCurrentPowertune(iPercentage);
			retVal = true;
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_PowerControl_Set(adlGPU->GetAdapterIndex(), iPercentage);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetCurrentPowertune(iPercentage);
			retVal = true;	
		}
	}

	adlGPU->SetIsManaged(true);

	return retVal;
}

const int ADL::GetFanSpeed(ADLGPU* adlGPU)
{
	int retVal = -1;

	if (adlGPU == NULL || !m_bIsActive)
	{
		return retVal;
	}

	int adlResult = 0;

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		ADLOD6FanSpeedInfo adlFanSpeedInfo = { 0 };
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_FanSpeed_Get(adlGPU->GetAdapterIndex(), &adlFanSpeedInfo);
		}
		m_bstADLLock->unlock();

		if (ADL_OD6_FANSPEED_TYPE_RPM == (adlFanSpeedInfo.iSpeedType & ADL_OD6_FANSPEED_TYPE_RPM))
		{
			adlGPU->SetCurrentFanSpeed(adlFanSpeedInfo.iFanSpeedRPM);
			retVal = adlFanSpeedInfo.iFanSpeedRPM;
		}
		else
		{
			adlGPU->SetCurrentFanSpeed(adlFanSpeedInfo.iFanSpeedPercent);
			retVal = adlFanSpeedInfo.iFanSpeedPercent;
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		ADLFanSpeedValue fsValue = { 0 };
		fsValue.iSpeedType = adlGPU->GetFanSpeedMethod();

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_FanSpeed_Get(adlGPU->GetAdapterIndex(), adlGPU->GetThermalControllerIndex(), &fsValue);
		}
		m_bstADLLock->unlock();
		
		
		adlGPU->SetCurrentFanSpeed(fsValue.iFanSpeed);
		retVal = fsValue.iFanSpeed;
	}

	return retVal;
}

const int ADL::GetEngineClock(ADLGPU* adlGPU)
{
	int retVal = -1;

	if (adlGPU == NULL || !m_bIsActive)
	{
		return retVal;
	}

	int adlResult = 0;

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		ADLOD6CurrentStatus currentStatus = { 0 };
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_CurrentStatus_Get(adlGPU->GetAdapterIndex(), &currentStatus);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			retVal = currentStatus.iEngineClock / 100;
			adlGPU->SetCurrentEngineClock(retVal);
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		ADLPMActivity lpmActivity = { 0 };
		lpmActivity.iSize = sizeof(ADLPMActivity);

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpmActivity);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			retVal = lpmActivity.iEngineClock / 100;
			adlGPU->SetCurrentEngineClock(retVal);			
		}
	}

	return retVal;
}

void ADL::GetMemoryRange(ADLGPU* adlGPU, int &imin, int &imax)
{
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	imin = adlGPU->GetMemoryClockMin() / 100;
	imax = adlGPU->GetMemoryClockMax() / 100;
}

void ADL::GetVDDCRange(ADLGPU* adlGPU, float &imin, float &imax)
{
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	imin = ((float)adlGPU->GetVDDCMin()) * 0.001f;
	imax = ((float)adlGPU->GetVDDCMax()) * 0.001f;
}

void ADL::GetFanRange(ADLGPU* adlGPU, int &imin, int &imax)
{
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		if (adlGPU->GetFanSpeedMethod() == ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE)
		{
			imin = adlGPU->GetFanMinPercent();
			imax = adlGPU->GetFanMaxPercent();
		}
		else
		{
			imin = adlGPU->GetFanMinRPM();
			imax = adlGPU->GetFanMaxRPM();
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		if (adlGPU->GetFanSpeedMethod() == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
		{
			imin = adlGPU->GetFanMinRPM();
			imax = adlGPU->GetFanMaxRPM();
		}
		else
		{
			imin = adlGPU->GetFanMinPercent();
			imax = adlGPU->GetFanMaxPercent();
		}		
	}
}

void ADL::GetEngineRange(ADLGPU* adlGPU, int &imin, int &imax)
{
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	imin = adlGPU->GetEngineClockMin() / 100;
	imax = adlGPU->GetEngineClockMax() / 100;
}

void ADL::GetPowertuneRange(ADLGPU* adlGPU, int &imin, int &imax)
{
	if (adlGPU == NULL || !m_bIsActive)
	{
		return;
	}

	imin = adlGPU->GetPowertuneMin();
	imax = adlGPU->GetPowertuneMax();
}


const bool ADL::FanAutotune(ADLGPU* adlGPU, int temp, int fanpercent, int lasttemp, bool &fan_window)
{
	int tdiff = (int)std::ceil((float)temp - (float)lasttemp);

	if (adlGPU == NULL || !m_bIsActive) 
	{
		return false;
	}

	int newpercent = fanpercent;
	int iMin = 0, iMax = 100;

	GetFanRange(adlGPU, iMin, iMax);
	

	if (temp > adlGPU->GetTempOverHeat() && fanpercent < iMax) 
	{
		std::cout << "Overheat detected on GPU " << adlGPU->GetDeviceNumber() << " increasing fan to 100%%" << std::endl;
		newpercent = iMax;
	} 
	else if (temp > adlGPU->GetTempTarget() && fanpercent < iMax && tdiff >= 0)
	{
		std::cout << "Temperature over target, increasing fanspeed" << std::endl;

		if (temp > adlGPU->GetTempTarget() + 3)
		{
			newpercent = fanpercent + 10;
		}
		else
		{
			newpercent = fanpercent + 5;
		}

		if (newpercent > iMax)
		{
			newpercent = iMax;
		}
	} 
	else if (fanpercent > iMin && temp < adlGPU->GetTempTarget() - 3)
	{
		//Detect large swings of 5 degrees or more and change fan by a proportion more
		if (tdiff <= 0) 
		{
			std::cout << "Temperature " << 3 << " degrees below target, decreasing fanspeed" << std::endl;
			newpercent = adlGPU->GetTempTarget() - 1 + tdiff / 5;
		} 
		else if (tdiff >= 5) 
		{
			std::cout << "Temperature climbed" << tdiff << " while below target, increasing fanspeed" << std::endl;
			newpercent = adlGPU->GetTempTarget() + tdiff / 5;
		}
	} 
	else 
	{
		// We're in the optimal range, make minor adjustments if the temp is still drifting
		if (fanpercent > iMin && tdiff < 0 && lasttemp < adlGPU->GetTempTarget())
		{
			std::cout << "Temperature dropping while in target range, decreasing fanspeed" << std::endl;
			newpercent = fanpercent + tdiff;
		} 
		else if (fanpercent < iMax && tdiff > 0 && temp > adlGPU->GetTempTarget() - 3)
		{
			std::cout << "Temperature rising while in target range, increasing fanspeed" << std::endl;
			newpercent = fanpercent + tdiff;
		}
	}

	if (newpercent > iMax)
	{
		newpercent = iMax;
	}
	else if (newpercent < iMin)
	{
		newpercent = iMin;
	}

	if (newpercent != fanpercent) 
	{
		std::cout << "Setting GPU " << adlGPU->GetDeviceNumber() << " fan percentage to " << newpercent << std::endl;
		
		SetFanSpeed(adlGPU, newpercent);

		/* If the fanspeed is going down and we're below the top speed,
		 * consider the fan optimal to prevent minute changes in
		 * fanspeed delaying GPU engine speed changes */
		if (newpercent < fanpercent && newpercent < iMax)
		{
			return true;
		}

		return false;
	}
	return true;
}

ADLGPU* ADL::GetDevice(int iAdapterIndex)
{
	for (size_t devIndex = 0; devIndex < m_vecDevices.size(); ++devIndex)
	{
		if (m_vecDevices[devIndex]->GetAdapterIndex() == iAdapterIndex)
		{
			return m_vecDevices[devIndex];
		}
	}

	return NULL;
}

ADLGPU* ADL::GetDeviceInOrder(int nIndex)
{
	if (m_vecDevices.size() > nIndex)
	{
		return m_vecDevices[nIndex];
	}

	return NULL;
}


const float ADL::GetGPUTemp(ADLGPU* adlGPU)
{
	float retVal = -1.0f;
	int adlResult = 0;

	if (adlGPU == NULL || !m_bIsActive)
	{
		return false;
	}

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		//Verifies that thermal controller exists on the GPU.
		if (ADL_OD6_TCCAPS_THERMAL_CONTROLLER == (adlGPU->GetOD6Capabilities() & ADL_OD6_TCCAPS_THERMAL_CONTROLLER))
		{
			int temp = 0;
			m_bstADLLock->lock();
			{
				adlResult = ADL_Overdrive6_Temperature_Get(adlGPU->GetAdapterIndex(), &temp);
			}
			m_bstADLLock->unlock();

			if (adlResult == ADL_OK)
			{
				//The temperature is returned in mili-degree of Celsius 
				retVal = ((float)temp) * 0.001f;
				adlGPU->SetCurrentTemperature(retVal);
			} 
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		ADLTemperature adlTemp = { 0 };
		adlTemp.iSize = sizeof (ADLTemperature);

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_Temperature_Get(adlGPU->GetAdapterIndex(), adlGPU->GetThermalControllerIndex(), &adlTemp);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			//The temperature is returned in mili-degree of Celsius 
			retVal = ((float)adlTemp.iTemperature) * 0.001f;
			adlGPU->SetCurrentTemperature(retVal);
		}
	}
	
	return retVal;
}

const int ADL::GetMemoryClock(ADLGPU* adlGPU)
{
	int retVal = -1;
	int adlResult = 0;

	if (adlGPU == NULL || !m_bIsActive)
	{
		return false;
	}

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		ADLOD6CurrentStatus currentStatus = { 0 };
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_CurrentStatus_Get(adlGPU->GetAdapterIndex(), &currentStatus);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			retVal = currentStatus.iMemoryClock / 100;
			adlGPU->SetCurrentMemoryClock(retVal);
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		ADLPMActivity lpActivity = { 0 };
		lpActivity.iSize = sizeof (ADLPMActivity);

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpActivity);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			retVal = lpActivity.iMemoryClock / 100; 
			adlGPU->SetCurrentMemoryClock(retVal);
		}
	}

	return retVal;
}

const float ADL::GetVDDC(ADLGPU *adlGPU)
{
	float retVal = -1.0f;
	int adlResult = 0;

	if (adlGPU == NULL || !m_bIsActive)
	{
		return false;
	}

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		int nCurrentValue = 0;
		int nDefaultValue = 0;

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_VoltageControl_Get(adlGPU->GetAdapterIndex(), &nCurrentValue, &nDefaultValue);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			retVal = (((float)nCurrentValue) * 0.001f);
			adlGPU->SetCurrentVoltage(retVal);
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		ADLPMActivity lpActivity = { 0 };
		lpActivity.iSize = sizeof (ADLPMActivity);

		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpActivity);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			retVal = (((float)lpActivity.iVddc) * 0.001f);
			adlGPU->SetCurrentVoltage(retVal);
		}
	}

	return retVal;
}

const int ADL::GetActivity(ADLGPU* adlGPU)
{
	int retVal = -1;
	int adlResult = 0;

	if (adlGPU == NULL || !m_bIsActive)
	{
		return false;
	}

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		if ((adlGPU->GetOD6Capabilities() & ADL_OD6_CAPABILITY_GPU_ACTIVITY_MONITOR) == ADL_OD6_CAPABILITY_GPU_ACTIVITY_MONITOR)
		{
			ADLOD6CurrentStatus currentStatus = { 0 };
			m_bstADLLock->lock();
			{
				adlResult = ADL_Overdrive6_CurrentStatus_Get(adlGPU->GetAdapterIndex(), &currentStatus);
			}
			m_bstADLLock->unlock();

			if (adlResult == ADL_OK)
			{
				retVal = currentStatus.iActivityPercent;
				adlGPU->SetCurrentActivity(retVal);
			}
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		if (adlGPU->GetActivityReportingSupported())
		{
			ADLPMActivity lpActivity = { 0 };
			lpActivity.iSize = sizeof (ADLPMActivity);

			m_bstADLLock->lock();
			{
				adlResult = ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpActivity);
			}
			m_bstADLLock->unlock();

			if (adlResult == ADL_OK)
			{
				retVal = lpActivity.iActivityPercent;
				adlGPU->SetCurrentActivity(retVal);
			}
		}
	}

	return retVal;
}

const int ADL::GetPowertune(ADLGPU* adlGPU)
{
	int retVal = -1;
	int adlResult = 0;

	if (adlGPU == NULL || !m_bIsActive)
	{
		return retVal;
	}

	if (!adlGPU->GetPowerControlSupported())
	{
		return retVal;
	}

	int nCurrent = 0;
	int nDefault = 0;

	if (adlGPU->GetOverdriveVersion() == 6)
	{
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive6_PowerControl_Get(adlGPU->GetAdapterIndex(), &nCurrent, &nDefault);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetPowerControlCurrent(nCurrent);
			retVal = nCurrent;
		}
	}
	else if (adlGPU->GetOverdriveVersion() == 5)
	{
		m_bstADLLock->lock();
		{
			adlResult = ADL_Overdrive5_PowerControl_Get(adlGPU->GetAdapterIndex(), &nCurrent, &nDefault);
		}
		m_bstADLLock->unlock();

		if (adlResult == ADL_OK)
		{
			adlGPU->SetPowerControlCurrent(nCurrent);
			retVal = nCurrent;
		}
	}

	return retVal;
}

void ADL::SetDefaults(ADLGPU* adlGPU)
{
	if (adlGPU == NULL || !m_bIsActive)
	{
		return;
	}
	
	SetFanSpeed(adlGPU, adlGPU->GetDefaultFanSpeed());
	SetEngineClock(adlGPU, adlGPU->GetDefaultEngineClock());
	SetMemoryClock(adlGPU, adlGPU->GetDefaultMemoryClock());
	SetVDDC(adlGPU, adlGPU->GetDefaultVoltage());
	SetPowertune(adlGPU, adlGPU->GetDefaultPowertune());
}

void ADL::GetGPUStats(ADLGPU* adlGPU, float &temp, int &engineclock, int &memclock, float &vddc,
							int &activity, int &fanspeed, int &powertune)
{
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	temp = GetGPUTemp(adlGPU);
	activity = GetActivity(adlGPU);
	engineclock = GetEngineClock(adlGPU);
	memclock = GetMemoryClock(adlGPU);
	vddc = GetVDDC(adlGPU);
	fanspeed = GetFanSpeed(adlGPU);
	powertune = GetPowertune(adlGPU);
}

void ADL::ResetDevice(ADLGPU* adlGPU)
{
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	if (adlGPU->GetIsManaged())
	{
		SetDefaults(adlGPU);
	}
}

void ADL::HandleEvent(Event* pEvent)
{
	if (!pEvent)
	{
		return;
	}	

	switch (pEvent->GetEventType())
	{
		case Event::EVENT_TYPE::SET_ADL_GPU_FAN:
			{
				int nAdapterIndex = pEvent->GetParam<int>(0);
				float fFanSpeed = pEvent->GetParam<float>(1);
				
				int nFanSpeed = (int)fFanSpeed;
				SetFanSpeed(GetDevice(nAdapterIndex), nFanSpeed);
			}
			break;
		case Event::EVENT_TYPE::SET_ADL_GPU_ENGINE_CLOCK:
			{
				int nAdapterIndex = pEvent->GetParam<int>(0);
				int nEngineClock = pEvent->GetParam<int>(1);

				SetEngineClock(GetDevice(nAdapterIndex), nEngineClock);
			}
			break;
		case Event::EVENT_TYPE::SET_ADL_GPU_MEMORY_CLOCK:
			{
				int nAdapterIndex = pEvent->GetParam<int>(0);
				int nMemoryClock = pEvent->GetParam<int>(1);

				SetMemoryClock(GetDevice(nAdapterIndex), nMemoryClock);
			}
			break;
		case Event::EVENT_TYPE::SET_ADL_GPU_VOLTAGE:
			{
				int nAdapterIndex = pEvent->GetParam<int>(0);
				int nVoltage = pEvent->GetParam<int>(1);

				SetVDDC(GetDevice(nAdapterIndex), nVoltage);
			}
			break;
		case Event::EVENT_TYPE::SET_ADL_GPU_POWERTUNE:
			{
				int nAdapterIndex = pEvent->GetParam<int>(0);
				int nPowerTune = pEvent->GetParam<int>(1);

				SetPowertune(GetDevice(nAdapterIndex), nPowerTune);
			}
			break;

		default:
			break;
	}
}