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
#include "../gpu/GPUData.h"
#include "../gpu/ADLGPU.h"
#include "../config/GPUSetting.h"

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
ADL::ADL(std::vector<GPUData*> gpus)
{
	

#if defined (UNIX)
	m_hDLL = dlopen("libatiadlxx.so", RTLD_LAZY|RTLD_GLOBAL);
#else
	m_hDLL = LoadLibrary(_T("atiadlxx.dll"));
	if (m_hDLL == NULL)
	{
		m_hDLL = LoadLibrary(_T("atiadlxy.dll"));
	}
#endif

	if (m_hDLL == NULL) 
	{
		std::cout << "Unable to load ATI ADL library." << std::endl;
		return;
	}

	if(gpus.size() == 0)
	{
		return;
	}



	ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE) GetProcAddress(m_hDLL,"ADL_Main_Control_Create");
	ADL_Main_Control_Refresh = (ADL_MAIN_CONTROL_REFRESH) GetProcAddress(m_hDLL, "ADL_Main_Control_Refresh");
	ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY) GetProcAddress(m_hDLL,"ADL_Main_Control_Destroy");	
	ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET) GetProcAddress(m_hDLL,"ADL_Adapter_NumberOfAdapters_Get");
	ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET) GetProcAddress(m_hDLL,"ADL_Adapter_AdapterInfo_Get");
	ADL_Display_DisplayInfo_Get = (ADL_DISPLAY_DISPLAYINFO_GET) GetProcAddress(m_hDLL,"ADL_Display_DisplayInfo_Get");
	ADL_Adapter_ID_Get = (ADL_ADAPTER_ID_GET) GetProcAddress(m_hDLL,"ADL_Adapter_ID_Get");	
	ADL_Adapter_VideoBiosInfo_Get = (ADL_ADAPTER_VIDEOBIOSINFO_GET)GetProcAddress(m_hDLL,"ADL_Adapter_VideoBiosInfo_Get");
	ADL_Adapter_Active_Get = (ADL_ADAPTER_ACTIVE_GET)GetProcAddress(m_hDLL, "ADL_Adapter_Active_Get");
	ADL_Overdrive_Caps = (ADL_OVERDRIVE_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive_Caps");


	if ( NULL == ADL_Main_Control_Create ||
            NULL == ADL_Main_Control_Destroy ||
            NULL == ADL_Adapter_NumberOfAdapters_Get ||
            NULL == ADL_Adapter_AdapterInfo_Get ||
			NULL == ADL_Adapter_Active_Get ||
			NULL == ADL_Overdrive_Caps)
	{
		std::cout << "ADL API Missing!" << std::endl;
	}


	int adlResult = ADL_Main_Control_Create (ADL_Main_Memory_Alloc, 1);

	if (ADL_OK != adlResult)
	{
		std::cout << "ADL Initialization Error: " << GetADLError(adlResult) << std::endl;
	}

	adlResult = ADL_Main_Control_Refresh();
	if (ADL_OK != adlResult)
	{
		std::cout << "ADL Refresh Error: " << GetADLError(adlResult) << std::endl;
	}

	ADL_Overdrive5_ThermalDevices_Enum = (ADL_OVERDRIVE5_THERMALDEVICES_ENUM) GetProcAddress (m_hDLL, "ADL_Overdrive5_ThermalDevices_Enum");
	ADL_Overdrive5_ODParameters_Get = (ADL_OVERDRIVE5_ODPARAMETERS_GET) GetProcAddress (m_hDLL, "ADL_Overdrive5_ODParameters_Get");
	ADL_Overdrive5_Temperature_Get = (ADL_OVERDRIVE5_TEMPERATURE_GET) GetProcAddress (m_hDLL, "ADL_Overdrive5_Temperature_Get");
	ADL_Overdrive5_FanSpeed_Get = (ADL_OVERDRIVE5_FANSPEED_GET) GetProcAddress (m_hDLL, "ADL_Overdrive5_FanSpeed_Get");
	ADL_Overdrive5_FanSpeedInfo_Get = (ADL_OVERDRIVE5_FANSPEEDINFO_GET ) GetProcAddress (m_hDLL, "ADL_Overdrive5_FanSpeedInfo_Get");
	ADL_Overdrive5_ODPerformanceLevels_Get = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET) GetProcAddress (m_hDLL, "ADL_Overdrive5_ODPerformanceLevels_Get");		
	ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET) GetProcAddress (m_hDLL, "ADL_Overdrive5_CurrentActivity_Get");
	ADL_Overdrive5_FanSpeed_Set = (ADL_OVERDRIVE5_FANSPEED_SET)GetProcAddress (m_hDLL, "ADL_Overdrive5_FanSpeed_Set");
	ADL_Overdrive5_ODPerformanceLevels_Set = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET ) GetProcAddress(m_hDLL, "ADL_Overdrive5_ODPerformanceLevels_Set");
	ADL_Overdrive5_PowerControl_Caps = (ADL_OVERDRIVE5_POWERCONTROL_CAPS) GetProcAddress (m_hDLL, "ADL_Overdrive5_PowerControl_Caps");
	ADL_Overdrive5_PowerControlInfo_Get = (ADL_OVERDRIVE5_POWERCONTROLINFO_GET) GetProcAddress (m_hDLL, "ADL_Overdrive5_PowerControlInfo_Get");
	ADL_Overdrive5_PowerControl_Get = (ADL_OVERDRIVE5_POWERCONTROL_GET ) GetProcAddress (m_hDLL, "ADL_Overdrive5_PowerControl_Get");
	ADL_Overdrive5_PowerControl_Set = (ADL_OVERDRIVE5_POWERCONTROL_SET) GetProcAddress (m_hDLL, "ADL_Overdrive5_PowerControl_Set");
	ADL_Overdrive6_State_Set = (ADL_OVERDRIVE6_STATE_SET) GetProcAddress (m_hDLL, "ADL_Overdrive6_State_Set");


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
		std::cout << "ADL Overdrive 5 Not Found!" << std::endl;
	}
	else
	{
		std::cout << "ADL Overdrive 5 Was Found!" << std::endl;
	}


	ADL_Overdrive6_FanSpeed_Get = (ADL_OVERDRIVE6_FANSPEED_GET) GetProcAddress(m_hDLL,"ADL_Overdrive6_FanSpeed_Get");
	ADL_Overdrive6_ThermalController_Caps = (ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS)GetProcAddress (m_hDLL, "ADL_Overdrive6_ThermalController_Caps");
	ADL_Overdrive6_Temperature_Get = (ADL_OVERDRIVE6_TEMPERATURE_GET)GetProcAddress (m_hDLL, "ADL_Overdrive6_Temperature_Get");
	ADL_Overdrive6_Capabilities_Get = (ADL_OVERDRIVE6_CAPABILITIES_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_Capabilities_Get");
	ADL_Overdrive6_StateInfo_Get = (ADL_OVERDRIVE6_STATEINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_StateInfo_Get");
	ADL_Overdrive6_CurrentStatus_Get = (ADL_OVERDRIVE6_CURRENTSTATUS_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_CurrentStatus_Get");
	ADL_Overdrive6_PowerControl_Caps = (ADL_OVERDRIVE6_POWERCONTROL_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControl_Caps");
	ADL_Overdrive6_PowerControlInfo_Get = (ADL_OVERDRIVE6_POWERCONTROLINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControlInfo_Get");
	ADL_Overdrive6_PowerControl_Get = (ADL_OVERDRIVE6_POWERCONTROL_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControl_Get");
	ADL_Overdrive6_FanSpeed_Set  = (ADL_OVERDRIVE6_FANSPEED_SET)GetProcAddress(m_hDLL, "ADL_Overdrive6_FanSpeed_Set");
	ADL_Overdrive6_State_Set = (ADL_OVERDRIVE6_STATE_SET)GetProcAddress(m_hDLL, "ADL_Overdrive6_State_Set");
	ADL_Overdrive6_PowerControl_Set = (ADL_OVERDRIVE6_POWERCONTROL_SET) GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControl_Set");


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
		NULL == ADL_Overdrive6_PowerControl_Set)
	{
		std::cout << "ADL Overdrive 6 Not Found!" << std::endl;
	}
	else
	{
		std::cout << "ADL Overdrive 6 Was Found!" << std::endl;
	}

	adlResult = ADL_Adapter_NumberOfAdapters_Get(&m_nNumAdapters);
	if ( ADL_OK != adlResult)
	{
	   std::cout << "Cannot get the number of adapters!" << std::endl;
	}


	if (m_nNumAdapters > 0)
    {
		m_lpInfo = (LPAdapterInfo)malloc( sizeof (AdapterInfo) * m_nNumAdapters );
        memset(m_lpInfo,'\0', sizeof (AdapterInfo) * m_nNumAdapters );

		m_lpInfo->iSize = sizeof(m_lpInfo);

        // Get the AdapterInfo structure for all adapters in the system
        adlResult = ADL_Adapter_AdapterInfo_Get(m_lpInfo, sizeof (AdapterInfo) * m_nNumAdapters);
		if(ADL_OK != adlResult)
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

	int nLastAdapter = -1;
	int nDevNum = (int)gpus.size();
	m_nNumDevices = 0;
	bool bDevicesMatch = true;

    for (int i = 0; i < m_nNumAdapters; ++i)
    {
		int adapterIndex = m_lpInfo[i].iAdapterIndex;
		
		adlResult = ADL_Adapter_ID_Get(adapterIndex, &adapterId);
		if(adlResult != ADL_OK)
		{
			std::cout << "ADL_Adapter_ID_Get Failed. Error " << adlResult << std::endl;
			if(adlResult == -10)
			{
				std::cout << "Device is not enabled" << adlResult << std::endl;
			}
			continue;
		}

		m_nNumDevices++;
		nLastAdapter = adapterId;
		if(adapterId == 0)
		{
			std::cout << "Adapter returns ID 0 meaning not AMD. Card order might be confused" << std::endl;
			continue;
		}

		int adapterActive = 0;			
		AdapterInfo adapterInfo = m_lpInfo[i];            
	
		if(adapterInfo.iVendorID == AMDVENDORID)
		{
			m_vecAdapterInfo.push_back(adapterInfo);
			ADL_Adapter_Active_Get(adapterInfo.iAdapterIndex , &adapterActive);
			if(adapterActive)
			{
				adapterId = adapterInfo.iAdapterIndex;
			}
		}
	}



	if (adapterId == -1)
	{
		std::cout << "Cannot find active AMD adapter!" << std::endl;
	}

	
	if(m_nNumDevices > ((int)gpus.size()))
	{
		m_nNumDevices = (int)gpus.size();
	}

	if (m_nNumDevices > 0)
    {
		m_lpInfo = (LPAdapterInfo)malloc ( sizeof (AdapterInfo) * m_nNumAdapters );
        memset(m_lpInfo,'\0', sizeof (AdapterInfo) * m_nNumAdapters );

		m_lpInfo->iSize = sizeof(m_lpInfo);

        // Get the AdapterInfo structure for all adapters in the system
        adlResult = ADL_Adapter_AdapterInfo_Get(m_lpInfo, sizeof (AdapterInfo) * m_nNumAdapters);
		if(ADL_OK != adlResult)
		{
			std::cout << "DL_Adapter_AdapterInfo_Get Error! Error: " << adlResult << std::endl;
		}
    }
	else
	{
		std::cout << "No Adapters were found!" << std::endl;
		return;
	}

	ADLBiosInfo BiosInfo;
	adapterId = -1;
    for (int i = 0; i < m_nNumDevices; ++i)
    {
		if(gpus[i]->GetIsDisabled())
		{
			gpus[i]->SetGPUEngine(0);
			gpus[i]->SetGPUMemclock(0);
			gpus[i]->SetGPUVDDC(0);
			gpus[i]->SetGPUFan(0);
			gpus[i]->SetGPUPowerTune(0);
			continue;
		}


		int iAdapterIndex = m_vecAdapterInfo[i].iAdapterIndex;		
		int lpAdapterID;
		adlResult = ADL_Adapter_ID_Get(iAdapterIndex, &lpAdapterID);
		if(ADL_OK != adlResult)
		{
			continue;
		}

		gpus[i]->SetName(m_lpInfo[i].strAdapterName);
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
		
		adlResult = ADL_Adapter_VideoBiosInfo_Get(iAdapterIndex, &BiosInfo);
		if (adlResult != ADL_ERR)
		{
			std::cout << "ADL index " << iAdapterIndex << ", id " << lpAdapterID << " - BIOS partno.: " << BiosInfo.strPartNumber << ", version: " << BiosInfo.strVersion << ", date: " << BiosInfo.strDate << std::endl;
		}
		
		ADLODParameters adlParams = adlGPU->GetADLODParameters();
		adlParams.iSize = sizeof(ADLODParameters);

		adlResult = ADL_Overdrive5_ODParameters_Get(iAdapterIndex, &adlParams);
		if(adlResult != ADL_OK)
		{
			std::cout << "ADL_Overdrive5_ODParameters_Get Failed!" << std::endl;
		}

		int level = adlParams.iNumberOfPerformanceLevels - 1;

		adlGPU->SetADLODParameters(adlParams);

		size_t perfLevelSize = sizeof(ADLODPerformanceLevels) + level * sizeof(ADLODPerformanceLevel);
		ADLODPerformanceLevels* pOdPerformanceLevels = (ADLODPerformanceLevels *)malloc(perfLevelSize);
		pOdPerformanceLevels->iSize = (int)perfLevelSize;

		adlResult = ADL_Overdrive5_ODPerformanceLevels_Get(iAdapterIndex, 1, pOdPerformanceLevels);
		if (adlResult != ADL_OK)
		{
			std::cout << "ADL_Overdrive5_ODPerformanceLevels_Get Failed!" << std::endl;
		}

		int levelInd = level - 1;
		int engClk = pOdPerformanceLevels->aLevels[levelInd].iEngineClock;
		adlGPU->GetGPUSetting()->SetMaxEngineSpeed(engClk);

		ADLTemperature adlTemp = adlGPU->GetADLTemperature();
		adlTemp.iSize = sizeof(ADLTemperature);
		adlGPU->SetADLTemperature(adlTemp);


		ADLFanSpeedInfo adlFanSpeed = adlGPU->GetADLFanSpeedInfo();
		adlFanSpeed.iSize = sizeof(ADLFanSpeedInfo);
		adlGPU->SetADLFanSpeedInfo(adlFanSpeed);

		ADLFanSpeedValue adlFanSpeedValue = adlGPU->GetADLFanSpeedValue();
		adlFanSpeedValue.iSize = sizeof(ADLFanSpeedValue);
		adlFanSpeedValue.iSpeedType = ADL_DL_FANCTRL_SPEED_TYPE_RPM;
		adlGPU->SetADLFanSpeedValue(adlFanSpeedValue);

		ADLFanSpeedValue adlDefFSV = adlGPU->GetADLDefFanSpeedValue();
		adlDefFSV.iSize = sizeof(ADLFanSpeedValue);
		adlDefFSV.iSpeedType = ADL_DL_FANCTRL_SPEED_TYPE_RPM;
		adlGPU->SetADLDefFanSpeedValue(adlDefFSV);

		adlGPU->SetADLODPerformanceLevels(pOdPerformanceLevels);

		if(gpus[i]->GetGPU()->GetGPUSetting()->GetEngineClock() > 0)
		{
			int setengine = gpus[i]->GetGPU()->GetGPUSetting()->GetEngineClock() * 100;

			for (int j = 0; j < level; j++) 
			{
				if (pOdPerformanceLevels->aLevels[j].iEngineClock > setengine)
				{
					pOdPerformanceLevels->aLevels[j].iEngineClock = setengine;
				}
			}

			pOdPerformanceLevels->aLevels[levelInd].iEngineClock = setengine;

			std::cout << "Setting GPU " << i << " engine clock to " << gpus[i]->GetGPU()->GetGPUSetting()->GetEngineClock() << std::endl;
			ADL_Overdrive5_ODPerformanceLevels_Set(iAdapterIndex, pOdPerformanceLevels);

			gpus[i]->SetGPUEngine(gpus[i]->GetGPU()->GetGPUSetting()->GetEngineClock());

			adlGPU->GetGPUSetting()->SetMaxEngineSpeed(setengine);

			if(gpus[i]->GetMinEngine() > 0)
			{
				adlGPU->GetGPUSetting()->SetMinEngineSpeed(gpus[i]->GetMinEngine() * 100);
			}

			adlGPU->SetIsManaged(true);

			if(gpus[i]->GetGPUMemDiff() > 0)
			{
				SetMemoryClock(gpus[i], gpus[i]->GetGPUEngine() + gpus[i]->GetGPUMemDiff());
			}
		}

		if (gpus[i]->GetGPU()->GetGPUSetting()->GetMemclock() > 0)
		{
			int setmem = gpus[i]->GetGPU()->GetGPUSetting()->GetMemclock() * 100;

			for (int j = 0; j < level; j++) 
			{
				if (pOdPerformanceLevels->aLevels[j].iMemoryClock > setmem)
				{
					pOdPerformanceLevels->aLevels[j].iMemoryClock = setmem;
				}
			}

			pOdPerformanceLevels->aLevels[levelInd].iMemoryClock = setmem;

			std::cout << "Setting GPU " << i << " memory clock to " << gpus[i]->GetGPU()->GetGPUSetting()->GetMemclock() << std::endl;
			ADL_Overdrive5_ODPerformanceLevels_Set(iAdapterIndex, pOdPerformanceLevels);
			
			gpus[i]->SetGPUMemclock(gpus[i]->GetGPU()->GetGPUSetting()->GetMemclock());

			adlGPU->SetIsManaged(true);
		}

		if (gpus[i]->GetGPU()->GetGPUSetting()->GetVoltage() > 0.0f)
		{
			int setv = (int)(gpus[i]->GetGPU()->GetGPUSetting()->GetVoltage() * 1000.0f);

			for (int j = 0; j < level; j++) 
			{
				if (pOdPerformanceLevels->aLevels[j].iVddc > setv)
					pOdPerformanceLevels->aLevels[j].iVddc = setv;
			}
			pOdPerformanceLevels->aLevels[levelInd].iVddc = setv;

			std::cout << "Setting GPU " << i << " voltage to " << gpus[i]->GetGPU()->GetGPUSetting()->GetVoltage() << std::endl;
			ADL_Overdrive5_ODPerformanceLevels_Set(iAdapterIndex, pOdPerformanceLevels);

			gpus[i]->SetGPUVDDC(gpus[i]->GetGPU()->GetGPUSetting()->GetVoltage());

			adlGPU->SetIsManaged(true);
		}

		ADL_Overdrive5_ODPerformanceLevels_Get(iAdapterIndex, 0, pOdPerformanceLevels);
		adlGPU->GetGPUSetting()->SetEngineClock(pOdPerformanceLevels->aLevels[levelInd].iEngineClock);
		adlGPU->GetGPUSetting()->SetMemclock(pOdPerformanceLevels->aLevels[levelInd].iMemoryClock);
		adlGPU->GetGPUSetting()->SetVoltage((float)pOdPerformanceLevels->aLevels[levelInd].iVddc);
		adlGPU->SetBusNumber(m_lpInfo[i].iBusNumber);
		
		ADLFanSpeedInfo adlFS = adlGPU->GetADLFanSpeedInfo();

		adlResult = ADL_Overdrive5_FanSpeedInfo_Get(iAdapterIndex, 0, &adlFS);
		if (adlResult != ADL_OK)
		{
			std::cout << "ADL_Overdrive5_FanSpeedInfo_Get Failed!" << std::endl;
		}

		adlGPU->SetADLFanSpeedInfo(adlFS);

		if(!(adlGPU->GetADLFanSpeedInfo().iFlags & (ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE | ADL_DL_FANCTRL_SUPPORTS_PERCENT_WRITE)))
		{
			adlGPU->GetGPUSetting()->SetHasFanSpeed(false);
		}
		else
		{
			adlGPU->GetGPUSetting()->SetHasFanSpeed(true);
		}


		//Save the fanspeed values as defaults
		ADLFanSpeedValue defValue = adlGPU->GetADLDefFanSpeedValue();
		adlResult = ADL_Overdrive5_FanSpeed_Get(iAdapterIndex, 0, &defValue);
		if (adlResult != ADL_OK)
		{
			std::cout << "ADL_Overdrive5_FanSpeed_Get failed for default value!" << std::endl;
		}
		else
		{
			adlGPU->SetIsDefFanValid(true);
		}

		adlGPU->SetADLDefFanSpeedValue(defValue);

		if (gpus[i]->GetGPUFan() > 0)
		{
			SetFanSpeed(gpus[i], gpus[i]->GetGPUFan());
		}
		else
		{
			//Set an upper limit of 85%
			gpus[i]->SetGPUFan(85); 
		}

		int nPercentage = adlGPU->GetGPUSetting()->GetPowerTune();
		int nDummy = 0;
		
		adlResult = ADL_Overdrive5_PowerControl_Get(iAdapterIndex, &nPercentage, &nDummy);
		if (adlResult != ADL_OK)
		{
			std::cout << "ADL_Overdrive5_PowerControl_get Failed!" << std::endl;
		}

		if (gpus[i]->GetGPUPowerTune() > 0) 
		{
			ADL_Overdrive5_PowerControl_Set(iAdapterIndex, gpus[i]->GetGPUPowerTune());
			ADL_Overdrive5_PowerControl_Get(iAdapterIndex, &nPercentage, &nDummy);

			adlGPU->GetGPUSetting()->SetPowerTune(nPercentage);
			adlGPU->SetIsManaged(true);
		}


		if (!(adlGPU->GetGPUSetting()->GetTargetTemp() > 0))
		{
			adlGPU->GetGPUSetting()->SetTargetTemp(75);
		}
		if (!(adlGPU->GetGPUSetting()->GetOverHeatTemp() > 0))
		{
			adlGPU->GetGPUSetting()->SetOverHeatTemp(85);
		}
		if (!(adlGPU->GetGPUSetting()->GetCutOffTemp() > 0))
		{
			adlGPU->GetGPUSetting()->SetCutOffTemp(95);
		}

		//change to auto fan bool
		if(true)
		{
			int nominal = 50;

			if (nominal > gpus[i]->GetGPUFan())
			{
				nominal = gpus[i]->GetGPUFan();
			}

			if (nominal < gpus[i]->GetMinFan())
			{
				nominal = gpus[i]->GetMinFan();
			}
		
			adlGPU->GetGPUSetting()->SetIsAutoFan(true);
			adlGPU->SetIsManaged(true);

			SetFanSpeed(gpus[i], nominal);
		}


		//set to auto engine bool
		if (true) 
		{
			adlGPU->GetGPUSetting()->SetIsAutoEngine(true);
			adlGPU->SetIsManaged(true);
		}

		adlTemp = adlGPU->GetADLTemperature();

		adlResult = ADL_Overdrive5_Temperature_Get(adlGPU->GetAdapterIndex(), 0, &adlTemp);
		if(adlResult != ADL_OK)
		{
			adlGPU->SetLastTemp(-1);
		}
		else
		{
			adlGPU->SetLastTemp((int)((float)adlTemp.iTemperature / 1000.0f));
			adlGPU->SetADLTemperature(adlTemp);
		}

		adlGPU->SetADLODPerformanceLevels(pOdPerformanceLevels);

		gpus[i]->SetGPU(adlGPU);
	}

	for (int i = 0; i < m_nNumDevices; ++i)
    {
		ADLGPU* adlGPU = (ADLGPU*)gpus[i]->GetGPU();

		for (int j = 0; j < m_nNumDevices; j++) 
		{
			if (j == i)
				continue;

			ADLGPU* otherGPU = (ADLGPU*)gpus[j]->GetGPU();

			if (!adlGPU->GetGPUSetting()->GetHasFanSpeed()) 
			{
				if (GetIsFanSpeedTwin(adlGPU, otherGPU)) 
				{
					std::cout << "Dual GPUs Detected: " << adlGPU->GetGPUID() << " and " << otherGPU->GetGPUID() << std::endl;

					adlGPU->SetTwin(otherGPU);
					otherGPU->SetTwin(adlGPU);

					gpus[i]->SetGPU(adlGPU);
					gpus[j]->SetGPU(otherGPU);					
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
ADL::ADL(const ADL& adl)
{
}


///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
ADL::~ADL()
{
}

void ADL::Shutdown(std::vector<GPUData*> gpus)
{
	Clear(gpus);

	ADL_Main_Memory_Free ((void **)&m_lpInfo);
	ADL_Main_Control_Destroy ();
#if defined (UNIX)
	dlclose(m_hDLL);
#else
	FreeLibrary(m_hDLL);
#endif

}


std::string ADL::GetADLError(int error)
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

const bool ADL::SetFanSpeed(GPUData* gpu, int nFanSpeed)
{
	bool retVal = false;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		std::cout << "Set fanspeed not supported!" << std::endl;
		return retVal;
	}
	

	if (!(adlGPU->GetADLFanSpeedInfo().iFlags & (ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE | ADL_DL_FANCTRL_SUPPORTS_PERCENT_WRITE )))
	{ 
		std::cout << "GPU " << gpu->GetID() << " doesn't support rpm or percent write!" << std::endl;
		return retVal;
	}

	m_bstADLLock.lock();
	{
		adlGPU->GetGPUSetting()->SetTargetFan(nFanSpeed);

		ADLFanSpeedValue adlFS = adlGPU->GetADLFanSpeedValue();
		adlFS.iSpeedType = ADL_DL_FANCTRL_SPEED_TYPE_RPM;

		int adlError = ADL_Overdrive5_FanSpeed_Get(adlGPU->GetAdapterIndex(), 0, &adlFS);
		if (adlError != ADL_OK) 
		{
			std::cout << "GPU " << gpu->GetID() << " call to fanspeed get failed!" << std::endl;
		}

		if (!(adlFS.iFlags & ADL_DL_FANCTRL_FLAG_USER_DEFINED_SPEED)) 
		{
			//If user defined is not already specified, set it first
			adlFS.iFlags |= ADL_DL_FANCTRL_FLAG_USER_DEFINED_SPEED;
			ADL_Overdrive5_FanSpeed_Set(adlGPU->GetAdapterIndex(), 0, &adlFS);
		}

		if (!(adlFS.iFlags & ADL_DL_FANCTRL_SUPPORTS_PERCENT_WRITE)) 
		{
			//Convert speed to an RPM
			nFanSpeed = adlGPU->GetADLFanSpeedInfo().iMaxRPM * nFanSpeed / 100;
			adlFS.iSpeedType = ADL_DL_FANCTRL_SPEED_TYPE_RPM;
		} 
		else
		{
			adlFS.iSpeedType = ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
		}

		adlFS.iFanSpeed = nFanSpeed;

		int adlRet = ADL_Overdrive5_FanSpeed_Set(adlGPU->GetAdapterIndex(), 0, &adlFS);
		if(adlRet == ADL_OK)
		{
			retVal = true;
		}

		adlGPU->SetIsManaged(true);
		adlGPU->SetADLDefFanSpeedValue(adlFS);
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return retVal;
}

const bool ADL::SetMemoryClock(GPUData* gpu, int iMemoryClock)
{
	
	int level;
	bool retVal = false;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		std::cout << "Set memoryclock not supported!" <<std::endl;
		return retVal;
	}

	iMemoryClock *= 100;

	ADLODPerformanceLevels *pOdPerformanceLevels = adlGPU->GetADLODPerformanceLevels();
	if(pOdPerformanceLevels)
	{
		_freea(pOdPerformanceLevels);
	}

	level = adlGPU->GetADLODParameters().iNumberOfPerformanceLevels - 1;
	pOdPerformanceLevels = (ADLODPerformanceLevels *)alloca(sizeof(ADLODPerformanceLevels)+(level * sizeof(ADLODPerformanceLevel)));
	pOdPerformanceLevels->iSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * level;

	m_bstADLLock.lock();
	if (ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels) != ADL_OK)
	{
		m_bstADLLock.unlock();
		return retVal;
	}

	int levelInd = level - 1;
	pOdPerformanceLevels->aLevels[levelInd].iMemoryClock = iMemoryClock;
	for (int i = 0; i < level; i++) 
	{
		if (pOdPerformanceLevels->aLevels[i].iMemoryClock > iMemoryClock)
			pOdPerformanceLevels->aLevels[i].iMemoryClock = iMemoryClock;
	}

	ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), pOdPerformanceLevels);
	ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels);

	if (pOdPerformanceLevels->aLevels[levelInd].iMemoryClock == iMemoryClock)
	{
		retVal = true;
	}

	m_bstADLLock.unlock();

	adlGPU->GetGPUSetting()->SetEngineClock(pOdPerformanceLevels->aLevels[levelInd].iEngineClock);
	adlGPU->GetGPUSetting()->SetMemclock(pOdPerformanceLevels->aLevels[levelInd].iMemoryClock);
	adlGPU->GetGPUSetting()->SetVoltage((float)pOdPerformanceLevels->aLevels[levelInd].iVddc);
	adlGPU->SetIsManaged(true);
	adlGPU->SetADLODPerformanceLevels(pOdPerformanceLevels);

	gpu->SetGPU(adlGPU);

	return retVal;
}

const bool ADL::GetIsFanSpeedTwin(ADLGPU* adlGPU, ADLGPU* otherGPU)
{
	if (!otherGPU->GetGPUSetting()->GetHasFanSpeed())
	{
		return false;
	}

	if (abs(adlGPU->GetBusNumber() - otherGPU->GetBusNumber()) != 1)
	{
		return false;
	}

	if (strcmp(adlGPU->GetAdapterName().c_str(), otherGPU->GetAdapterName().c_str()))
	{
		return false;
	}

	return true;
}


const bool ADL::SetEngineClock(GPUData* gpu, int iEngineClock)
{	
	bool retVal = false;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive)
	{	
		//wlogprint("Set engineclock not supported\n");
		return retVal;
	}

	ADLODPerformanceLevels *pOdPerformanceLevels = adlGPU->GetADLODPerformanceLevels();
	if(pOdPerformanceLevels)
	{
		_freea(pOdPerformanceLevels);
	}

	iEngineClock *= 100;
	adlGPU->SetLastEngine(iEngineClock);

	int level = adlGPU->GetADLODParameters().iNumberOfPerformanceLevels - 1;
	pOdPerformanceLevels = (ADLODPerformanceLevels*)_malloca(sizeof(ADLODPerformanceLevels) + level * sizeof(ADLODPerformanceLevel));
	pOdPerformanceLevels->iSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * level;

	m_bstADLLock.lock();
	if (ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels) != ADL_OK)
	{
		m_bstADLLock.unlock();

		if (gpu->GetGPUMemDiff() > 0)
		{
			SetMemoryClock(gpu, iEngineClock / 100 + gpu->GetGPUMemDiff());
		}

		return retVal;
	}


	for (int i = 0; i < level; i++) 
	{
		if (pOdPerformanceLevels->aLevels[i].iEngineClock > iEngineClock)
		{
			pOdPerformanceLevels->aLevels[i].iEngineClock = iEngineClock;
		}
	}

	int nLastInd = level - 1;
	pOdPerformanceLevels->aLevels[nLastInd].iEngineClock = iEngineClock;
	ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), pOdPerformanceLevels);
	ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels);
	
	if (pOdPerformanceLevels->aLevels[nLastInd].iEngineClock == iEngineClock)
	{
		retVal = true;
	}

	adlGPU->GetGPUSetting()->SetEngineClock(pOdPerformanceLevels->aLevels[nLastInd].iEngineClock);
	
	if (adlGPU->GetGPUSetting()->GetEngineClock() > adlGPU->GetGPUSetting()->GetMaxEngineSpeed())
	{
		adlGPU->GetGPUSetting()->SetMaxEngineSpeed(adlGPU->GetGPUSetting()->GetEngineClock());
	}

	if (adlGPU->GetGPUSetting()->GetEngineClock() < adlGPU->GetGPUSetting()->GetMinEngineSpeed())
	{
		adlGPU->GetGPUSetting()->SetMinEngineSpeed(adlGPU->GetGPUSetting()->GetEngineClock());
	}

	adlGPU->GetGPUSetting()->SetMemclock(pOdPerformanceLevels->aLevels[nLastInd].iMemoryClock);
	adlGPU->GetGPUSetting()->SetVoltage((float)pOdPerformanceLevels->aLevels[nLastInd].iVddc);
	adlGPU->SetIsManaged(true);

	adlGPU->SetADLODPerformanceLevels(pOdPerformanceLevels);

	m_bstADLLock.unlock();
	
	gpu->SetGPU(adlGPU);

	if (gpu->GetGPUMemDiff() > 0)
	{
		SetMemoryClock(gpu, iEngineClock / 100 + gpu->GetGPUMemDiff());
	}

	return retVal;
}


const bool ADL::SetVDDC(GPUData* gpu, float fVddc)
{
	bool retVal = false;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	int iVddc = (int)(1000 * fVddc);

	ADLODPerformanceLevels *pOdPerformanceLevels = adlGPU->GetADLODPerformanceLevels();
	if(pOdPerformanceLevels)
	{
		_freea(pOdPerformanceLevels);
	}

	int level = adlGPU->GetADLODParameters().iNumberOfPerformanceLevels - 1;
	pOdPerformanceLevels = (ADLODPerformanceLevels*)_malloca(sizeof(ADLODPerformanceLevels) + (level * sizeof(ADLODPerformanceLevel)));
	pOdPerformanceLevels->iSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * level;

	m_bstADLLock.lock();
	if (ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels) != ADL_OK)
	{
		_freea(pOdPerformanceLevels);

		m_bstADLLock.unlock();
		return false;
	}

	for (int i = 0; i < level; i++) 
	{
		if (pOdPerformanceLevels->aLevels[i].iVddc > iVddc)
		{
			pOdPerformanceLevels->aLevels[i].iVddc = iVddc;
		}
	}

	int levelInd = level - 1;

	pOdPerformanceLevels->aLevels[levelInd].iVddc = iVddc;
	ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), pOdPerformanceLevels);
	ADL_Overdrive5_ODPerformanceLevels_Get(adlGPU->GetAdapterIndex(), 0, pOdPerformanceLevels);
	if (pOdPerformanceLevels->aLevels[levelInd].iVddc == iVddc)
	{
		retVal = true;
	}

	adlGPU->GetGPUSetting()->SetEngineClock(pOdPerformanceLevels->aLevels[levelInd].iEngineClock);
	adlGPU->GetGPUSetting()->SetMemclock(pOdPerformanceLevels->aLevels[levelInd].iMemoryClock);
	adlGPU->GetGPUSetting()->SetVoltage((float)pOdPerformanceLevels->aLevels[levelInd].iVddc);
	adlGPU->SetIsManaged(true);

	adlGPU->SetADLODPerformanceLevels(pOdPerformanceLevels);

	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return retVal;
}

const bool ADL::SetPowertune(GPUData* gpu, int iPercentage)
{
	int dummy;
	bool retVal = false;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	m_bstADLLock.lock();
	{
		int nPercentage = adlGPU->GetGPUSetting()->GetPowerTune();

		ADL_Overdrive5_PowerControl_Set(adlGPU->GetAdapterIndex(), iPercentage);
		ADL_Overdrive5_PowerControl_Get(adlGPU->GetAdapterIndex(), &nPercentage, &dummy);
		if (nPercentage == iPercentage)
		{
			retVal = true;
		}
	
		adlGPU->GetGPUSetting()->SetPowerTune(nPercentage);
		adlGPU->SetIsManaged(true);
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return retVal;
}

const int ADL::GetFanPercent(ADLGPU* adlGPU)
{
	if (!adlGPU->GetGPUSetting()->GetHasFanSpeed() && adlGPU->GetTwin() != NULL)
	{
		return GetFanPercent((ADLGPU*)adlGPU->GetTwin());
	}

	if (!(adlGPU->GetADLFanSpeedInfo().iFlags & ADL_DL_FANCTRL_SUPPORTS_PERCENT_READ ))
	{
		return -1;
	}

	ADLFanSpeedValue fsValue = adlGPU->GetADLFanSpeedValue();

	fsValue.iSpeedType = ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
	if (ADL_Overdrive5_FanSpeed_Get(adlGPU->GetAdapterIndex(), 0, &fsValue) != ADL_OK)
	{
		return -1;
	}

	adlGPU->SetADLFanSpeedValue(fsValue);

	return adlGPU->GetADLFanSpeedValue().iFanSpeed;
}

const int ADL::GetEngineClock(ADLGPU* adlGPU)
{
	return adlGPU->GetADLPMActivity().iEngineClock / 100;
}

const int ADL::GetEngineClock(GPUData* gpu)
{
	int retVal = -1;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	m_bstADLLock.lock();
	
	ADLPMActivity pmActivity = adlGPU->GetADLPMActivity();

	if (ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &pmActivity) != ADL_OK)
	{
		m_bstADLLock.unlock();

		return retVal;
	}

	adlGPU->SetADLPMActivity(pmActivity);
	retVal = GetEngineClock(adlGPU);

	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return retVal;
}


void ADL::GetMemoryRange(GPUData* gpu, int &imin, int &imax)
{
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		//wlogprint("Get memoryrange not supported\n");
		return;
	}

	imin = adlGPU->GetADLODParameters().sMemoryClock.iMin / 100;
	imax = adlGPU->GetADLODParameters().sMemoryClock.iMax / 100;
}

void ADL::GetVDDCRange(GPUData* gpu, float &imin, float &imax)
{
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	imin = (float)adlGPU->GetADLODParameters().sVddc.iMin / 1000;
	imax = (float)adlGPU->GetADLODParameters().sVddc.iMax / 1000;
}

void ADL::GetFanRange(GPUData* gpu, int &imin, int &imax)
{
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	imin = adlGPU->GetADLFanSpeedInfo().iMinPercent;
	imax = adlGPU->GetADLFanSpeedInfo().iMaxPercent;
}

void ADL::GetEngineRange(GPUData* gpu, int &imin, int &imax)
{
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	imin = adlGPU->GetADLODParameters().sEngineClock.iMin / 100;
	imax = adlGPU->GetADLODParameters().sEngineClock.iMax / 100;
}

const bool ADL::FanAutotune(GPUData* gpu, int temp, int fanpercent, int lasttemp, bool &fan_window)
{
	int tdiff = (int)std::ceil((float)temp - (float)lasttemp);

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return false;
	}

	int top = gpu->GetGPUFan();
	int bot = gpu->GetMinFan();
	int newpercent = fanpercent;
	int iMin = 0, iMax = 100;

	GetFanRange(gpu, iMin, iMax);
	
	if (temp > adlGPU->GetGPUSetting()->GetOverHeatTemp() && fanpercent < iMax) 
	{
		std::cout << "Overheat detected on GPU " << gpu->GetID() << " increasing fan to 100%%" << std::endl;
		newpercent = iMax;
		//dev_error(cgpu, REASON_DEV_OVER_HEAT);
	} 
	else if (temp > adlGPU->GetGPUSetting()->GetTargetTemp() && fanpercent < top && tdiff >= 0) 
	{
		std::cout << "Temperature over target, increasing fanspeed" << std::endl;

		if (temp > adlGPU->GetGPUSetting()->GetTargetTemp() + 3)
		{
			newpercent = adlGPU->GetGPUSetting()->GetTargetFan() + 10;
		}
		else
		{
			newpercent = adlGPU->GetGPUSetting()->GetTargetFan() + 5;
		}

		if (newpercent > top)
		{
			newpercent = top;
		}
	} 
	else if (fanpercent > bot && temp < adlGPU->GetGPUSetting()->GetTargetTemp() - 3) 
	{
		//Detect large swings of 5 degrees or more and change fan by a proportion more
		if (tdiff <= 0) 
		{
			std::cout << "Temperature " << 3 << " degrees below target, decreasing fanspeed" << std::endl;
			newpercent = adlGPU->GetGPUSetting()->GetTargetFan() - 1 + tdiff / 5;
		} 
		else if (tdiff >= 5) 
		{
			std::cout << "Temperature climbed" << tdiff << " while below target, increasing fanspeed" << std::endl;
			newpercent = adlGPU->GetGPUSetting()->GetTargetFan() + tdiff / 5;
		}
	} 
	else 
	{
		// We're in the optimal range, make minor adjustments if the temp is still drifting
		if (fanpercent > bot && tdiff < 0 && lasttemp < adlGPU->GetGPUSetting()->GetTargetTemp()) 
		{
			std::cout << "Temperature dropping while in target range, decreasing fanspeed" << std::endl;
			newpercent = adlGPU->GetGPUSetting()->GetTargetFan() + tdiff;
		} 
		else if (fanpercent < top && tdiff > 0 && temp > adlGPU->GetGPUSetting()->GetTargetTemp() - 3) 
		{
			std::cout << "Temperature rising while in target range, increasing fanspeed" << std::endl;
			newpercent = adlGPU->GetGPUSetting()->GetTargetFan() + tdiff;
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

	if (newpercent <= top)
	{
		fan_window = true;
	}
	else
	{
		fan_window = false;
	}

	if (newpercent != fanpercent) 
	{
		std::cout << "Setting GPU " << gpu->GetID() << " fan percentage to " << newpercent << std::endl;
		
		SetFanSpeed(gpu, newpercent);

		/* If the fanspeed is going down and we're below the top speed,
		 * consider the fan optimal to prevent minute changes in
		 * fanspeed delaying GPU engine speed changes */
		if (newpercent < fanpercent && fan_window)
		{
			return true;
		}

		return false;
	}
	return true;
}


const float ADL::GetGPUTemp(ADLGPU* adlGPU)
{
	ADLTemperature adlTemp = adlGPU->GetADLTemperature();
	if (ADL_Overdrive5_Temperature_Get(adlGPU->GetAdapterIndex(), 0, &adlTemp) != ADL_OK)
	{
		return -1;
	}

	adlGPU->SetADLTemperature(adlTemp);

	return (float)adlGPU->GetADLTemperature().iTemperature / 1000;
}

const float ADL::GetGPUTemp(GPUData* gpu)
{
	
	float retVal = -1;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	m_bstADLLock.lock();
	{
		retVal = GetGPUTemp(adlGPU);
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);
	gpu->SetGPUTemp(retVal);

	return retVal;
}

const int ADL::GetMemclock(ADLGPU* adlGPU)
{
	return adlGPU->GetADLPMActivity().iMemoryClock / 100;
}

const int ADL::GetMemclock(GPUData* gpu)
{
	int retVal = -1;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	m_bstADLLock.lock();
	
	ADLPMActivity lpActivity = adlGPU->GetADLPMActivity();

	if (ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpActivity) != ADL_OK)
	{	
		boost::mutex::scoped_lock
		unlock(m_bstADLLock);

		return retVal;
	}

	adlGPU->SetADLPMActivity(lpActivity);	
	retVal = GetMemclock(adlGPU);

	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return retVal;
}

const float ADL::GetVDDC(ADLGPU *adlGPU)
{
	return (float)adlGPU->GetADLPMActivity().iVddc / 1000;
}

const float ADL::GetVDDC(GPUData* gpu)
{
	float retVal = -1;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	m_bstADLLock.lock();
	
	ADLPMActivity lpActivity = adlGPU->GetADLPMActivity();
	if (ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpActivity) != ADL_OK)
	{
		boost::mutex::scoped_lock
		unlock(m_bstADLLock);
	}
	
	adlGPU->SetADLPMActivity(lpActivity);
	retVal = GetVDDC(adlGPU);

	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return retVal;
}

const int ADL::GetActivity(ADLGPU* adlGPU)
{
	if (!adlGPU->GetADLODParameters().iActivityReportingSupported)
	{
		return -1;
	}

	return adlGPU->GetADLPMActivity().iActivityPercent;
}

const int ADL::GetActivity(GPUData* gpu)
{
	int retVal = -1;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	ADLPMActivity lpActivity = adlGPU->GetADLPMActivity();

	m_bstADLLock.lock();
	{
		retVal = ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpActivity);
	}
	m_bstADLLock.unlock();

	adlGPU->SetADLPMActivity(lpActivity);

	if (retVal != ADL_OK)
	{
		return retVal;
	}

	if (!adlGPU->GetADLODParameters().iActivityReportingSupported)
	{
		return retVal;
	}

	gpu->SetGPU(adlGPU);

	return adlGPU->GetADLPMActivity().iActivityPercent;
}

const int ADL::GetFanSpeed(ADLGPU* adlGPU)
{
	if (!adlGPU->GetGPUSetting()->GetHasFanSpeed() && adlGPU->GetTwin() != NULL)
	{
		return GetFanSpeed((ADLGPU*)adlGPU->GetTwin());
	}

	if (!(adlGPU->GetADLFanSpeedInfo().iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_READ))
	{
		return -1;
	}

	ADLFanSpeedValue adlFS = adlGPU->GetADLFanSpeedValue();
	adlFS.iSpeedType = ADL_DL_FANCTRL_SPEED_TYPE_RPM;

	int adlRet = ADL_Overdrive5_FanSpeed_Get(adlGPU->GetAdapterIndex(), 0, &adlFS);
	adlGPU->SetADLFanSpeedValue(adlFS);

	if (adlRet != ADL_OK)
	{
		return -1;
	}

	return adlFS.iFanSpeed;
}

const int ADL::GetFanSpeed(GPUData* gpu)
{
	int retVal = -1;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	m_bstADLLock.lock();
	{
		retVal = GetFanSpeed(adlGPU);
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return retVal;
}

const int ADL::GetPowertune(ADLGPU* adlGPU)
{
	int dummy = 0;

	int nPercentage = adlGPU->GetGPUSetting()->GetPowerTune();

	int retADL = ADL_Overdrive5_PowerControl_Get(adlGPU->GetAdapterIndex(), &nPercentage, &dummy);
	adlGPU->GetGPUSetting()->SetPowerTune(nPercentage);

	if (retADL != ADL_OK)
	{
		return -1;
	}

	return nPercentage;
}

const int ADL::GetPowertune(GPUData* gpu)
{
	int retVal = -1;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return retVal;
	}

	m_bstADLLock.lock();
	{
		retVal = GetPowertune(adlGPU);
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return retVal;
}

//gpus[adlGPU->GetTwin()->GetGPUID()];
void ADL::GPUAutotune(GPUData* gpu, GPUData* twin, int &denable)
{
	int temp, fanpercent, engine, newengine, twintemp = 0;
	bool fan_optimal = true, fan_window = true;

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	m_bstADLLock.lock();
	{
		ADLPMActivity lpActivity = adlGPU->GetADLPMActivity();

		ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpActivity);
		adlGPU->SetADLPMActivity(lpActivity);

		temp = (int)GetGPUTemp(adlGPU);
		if (adlGPU->GetTwin() != NULL)
		{
			twintemp = (int)GetGPUTemp((ADLGPU*)adlGPU->GetTwin());
		}

		fanpercent = GetFanPercent(adlGPU);
	}
	m_bstADLLock.unlock();

	newengine = engine =  GetEngineClock(gpu) * 100;

	if (temp && fanpercent >= 0 && adlGPU->GetGPUSetting()->GetIsAutoFan()) 
	{
		if (!adlGPU->GetTwin() != NULL)
		{
			fan_optimal = FanAutotune(gpu, temp, fanpercent, adlGPU->GetLastTemp(), fan_window);
		}
		else if (adlGPU->GetGPUSetting()->GetIsAutoFan() && (adlGPU->GetGPUSetting()->GetHasFanSpeed() || !adlGPU->GetTwin()->GetGPUSetting()->GetIsAutoFan())) 
		{
			// On linked GPUs, we autotune the fan only once, based
			// on the highest temperature from either GPUs
			GPUData* fan_gpu;
			int hightemp;
			int lasttemp;

			if (twintemp > temp) 
			{
				lasttemp = adlGPU->GetTwin()->GetLastTemp();
				hightemp = twintemp;
			} 
			else 
			{
				lasttemp = adlGPU->GetLastTemp();
				hightemp = temp;
			}
			
			if (adlGPU->GetGPUSetting()->GetHasFanSpeed())
			{
				fan_gpu = gpu;
			}
			else
			{
				fan_gpu = twin;
			}

			fan_optimal = FanAutotune(fan_gpu, hightemp, fanpercent, lasttemp, fan_window);
		}
	}

	if (engine && adlGPU->GetGPUSetting()->GetIsAutoEngine()) 
	{
		if (temp > adlGPU->GetGPUSetting()->GetCutOffTemp()) 
		{
			std::cout << "Hit thermal cutoff limit on GPU " << gpu->GetID() << ", disabling!" << std::endl;
			denable = 0;
			newengine = adlGPU->GetGPUSetting()->GetMinEngineSpeed();
			//dev_error(cgpu, REASON_DEV_THERMAL_CUTOFF);
		} 
		else if (temp > adlGPU->GetGPUSetting()->GetOverHeatTemp() && engine > adlGPU->GetGPUSetting()->GetMinEngineSpeed()) 
		{
			std::cout << "Overheat detected, decreasing GPU " << gpu->GetID() << " clock speed" << std::endl;
			newengine = adlGPU->GetGPUSetting()->GetMinEngineSpeed();

			//dev_error(cgpu, REASON_DEV_OVER_HEAT);
		} 
		else if (temp > adlGPU->GetGPUSetting()->GetTargetTemp() + 3 && engine > adlGPU->GetGPUSetting()->GetMinEngineSpeed() && fan_optimal) 
		{
			std::cout << "Temperature " << 3 << " degrees over target, decreasing clock speed" << std::endl;
			newengine = engine - adlGPU->GetADLODParameters().sEngineClock.iStep;
			// Only try to tune engine speed up if this GPU is not disabled
		} 
		else if (temp < adlGPU->GetGPUSetting()->GetTargetTemp() && engine < adlGPU->GetGPUSetting()->GetMaxEngineSpeed() && fan_window && denable == 0) 
		{
			int iStep = adlGPU->GetADLODParameters().sEngineClock.iStep;

			std::cout << "Temperature below target, increasing clock speed" << std::endl;

			if (temp < adlGPU->GetGPUSetting()->GetTargetTemp() - 3)
			{
				iStep *= 2;
			}
			newengine = engine + iStep;

		} 
		else if (temp < adlGPU->GetGPUSetting()->GetTargetTemp() && denable == 2) 
		{
			std::cout << "Device recovered to temperature below target, re-enabling" << std::endl;
			denable = 0;
		}

		if (newengine > adlGPU->GetGPUSetting()->GetMaxEngineSpeed())
		{
			newengine =  adlGPU->GetGPUSetting()->GetMaxEngineSpeed();
		}
		else if (newengine < adlGPU->GetGPUSetting()->GetMinEngineSpeed())
		{
			newengine = adlGPU->GetGPUSetting()->GetMinEngineSpeed();
		}

		//Adjust engine clock speed if it's lower, or if it's higher
		//but higher than the last intended value as well as the
		//current speed, to avoid setting the engine clock speed to
		//a speed relateive to a lower profile during idle periods.
		if (newengine < engine || (newengine > engine && newengine > adlGPU->GetLastEngine())) 
		{
			newengine /= 100;
			std::cout << "Setting GPU " << gpu->GetID() << " engine clock to " << newengine << std::endl;

			SetEngineClock(gpu, newengine);
		}
	}

	adlGPU->SetLastTemp(temp);

	gpu->SetGPU(adlGPU);
}

void ADL::SetDefaultEngine(GPUData* gpu)
{	
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	m_bstADLLock.lock();
	{
		if (adlGPU->GetADLODPerformanceLevels() != NULL)
		{
			ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), adlGPU->GetADLODPerformanceLevels());
		}
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);
}

void ADL::SetDefaultFan(GPUData* gpu)
{	
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	m_bstADLLock.lock();
	{
		if(adlGPU->GetIsDefFanValid())
		{
			ADLFanSpeedValue adlFan = adlGPU->GetADLDefFanSpeedValue();
			ADL_Overdrive5_FanSpeed_Set(adlGPU->GetAdapterIndex(), 0, &adlFan);
			adlGPU->SetADLDefFanSpeedValue(adlFan);
		}
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);
}



void ADL::ChangeAutoSettings(GPUData* gpu)
{
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	std::string szInput;
	int val;

	std::cout << "Target temperature: " << adlGPU->GetGPUSetting()->GetTargetTemp() << std::endl;
	std::cout << "Overheat temperature: " << adlGPU->GetGPUSetting()->GetOverHeatTemp() << std::endl;
	std::cout << "Cutoff temperature: " << adlGPU->GetGPUSetting()->GetCutOffTemp() << std::endl;
	std::cout << "Toggle [F]an auto [G]PU auto" << std::endl;
	std::cout << "Change [T]arget [O]verheat [C]utoff" << std::endl;
	std::cout << "Or press any other key to continue" << std::endl;
	
	std::cin >> szInput;

	if(boost::iequals(szInput, "f"))
	{
		adlGPU->GetGPUSetting()->SetIsAutoFan(!adlGPU->GetGPUSetting()->GetIsAutoFan());

		std::string szAuto;
		if(adlGPU->GetGPUSetting()->GetIsAutoFan())
		{
			szAuto = "enabled";
		}
		else
		{
			szAuto = "disabled";
			std::cout << "Resetting fan to startup settings" << std::endl;
			SetDefaultFan(gpu);
		}

		std::cout << "Fan autotune is now " << szAuto << std::endl;
	}
	else if(boost::iequals(szInput, "g"))
	{
		adlGPU->GetGPUSetting()->SetIsAutoEngine(!adlGPU->GetGPUSetting()->GetIsAutoEngine());
		
		std::string szAuto;
		if(adlGPU->GetGPUSetting()->GetIsAutoEngine())
		{
			szAuto = "enabled";
		}
		else
		{
			szAuto = "disabled";
			std::cout << "Resetting GPU engine clock to startup settings" << std::endl;
			SetDefaultEngine(gpu);
		}

		std::cout << "Engine Clock autotune is now " << szAuto << std::endl;
	}
	else if(boost::iequals(szInput, "t"))
	{
		std::cout << "Enter target temperature for this GPU in C (0-200)" << std::endl;
		std::cin >> val;
		if(!std::cin || (val < 0 || val > 200))
		{
			std::cout << "Invalid Temperature!" << std::endl;
		}
		else
		{
			adlGPU->GetGPUSetting()->SetTargetTemp(val);
		}
	}
	else if(boost::iequals(szInput, "o"))
	{
		std::cout << "Enter overheat temperature for this GPU in C (" << adlGPU->GetGPUSetting()->GetTargetTemp() << "+)" << std::endl;
		std::cin >> val;
		if(!std::cin || (val < adlGPU->GetGPUSetting()->GetTargetTemp() || val > 200))
		{
			std::cout << "Invalid Temperature!" << std::endl;
		}
		else
		{
			adlGPU->GetGPUSetting()->SetOverHeatTemp(val);
		}
	}
	else if(boost::iequals(szInput, "c"))
	{
		std::cout << "Enter cutoff temperature for this GPU in C (" << adlGPU->GetGPUSetting()->GetOverHeatTemp() << "+)" << std::endl;
		std::cin >> val;
		if(!std::cin || (val < adlGPU->GetGPUSetting()->GetOverHeatTemp() || val > 200))
		{
			std::cout << "Invalid Temperature!" << std::endl;
		}
		else
		{
			adlGPU->GetGPUSetting()->SetCutOffTemp(val);
		}
	}

	gpu->SetGPU(adlGPU);
}

const bool ADL::GetGPUStats(GPUData* gpu, float &temp, int &engineclock, int &memclock, float &vddc, 
							int &activity, int &fanspeed, int &fanpercent, int &powertune)
{

	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return false;
	}

	m_bstADLLock.lock();
	{
		temp = GetGPUTemp(adlGPU);
		ADLPMActivity lpActivity = adlGPU->GetADLPMActivity();

		if(ADL_Overdrive5_CurrentActivity_Get(adlGPU->GetAdapterIndex(), &lpActivity) != ADL_OK)
		{
			engineclock = 0;
			memclock = 0;
			vddc = 0;
			activity = 0;
			adlGPU->SetADLPMActivity(lpActivity);
		}
		else
		{
			adlGPU->SetADLPMActivity(lpActivity);
			engineclock = GetEngineClock(adlGPU);
			memclock = GetMemclock(adlGPU);
			vddc = GetVDDC(adlGPU);
			activity = GetActivity(adlGPU);;
		}

		fanspeed = GetFanSpeed(adlGPU);
		fanpercent = GetFanPercent(adlGPU);
		powertune = GetPowertune(adlGPU);		
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);

	return true;
}


void ADL::ChangeGPUSettings(GPUData* gpu)
{
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	std::string szInput;

	float fVal, fMin, fMax, fTemp, fVddc;
	int nVal, nMin, nMax, nEngineClock, nMemClock, nActivity, nFanSpeed, nFanPercent, nPowertune;

	nMin = nMax = nEngineClock = nMemClock = nActivity = nFanSpeed = nFanPercent = nPowertune = 0;
	fVal = fMin = fMax = fTemp = fVddc = 0.0f;

	while(true)
	{
		if (GetGPUStats(gpu, fTemp, nEngineClock, nMemClock, fVddc, nActivity, nFanSpeed, nFanPercent, nPowertune))
		{
			std::cout << "Temp: " << fTemp << " C" << std::endl;
		}

		if (nFanPercent >= 0 || nFanSpeed >= 0) 
		{
			std::cout << "Fan Speed: ";
			if(nFanPercent >= 0)
			{
				std::cout << nFanPercent << "% ";
			}
			if(nFanSpeed >= 0)
			{
				std::cout << nFanSpeed << " RPM";
			}

			std::cout << std::endl;
		}

		std::cout << "Engine Clock: "	<< nEngineClock		<< "MHz"	<< std::endl;
		std::cout << "Memory Clock: "	<< nMemClock		<< "Mhz"	<< std::endl;
		std::cout << "Vddc: "			<< fVddc			<< "V"		<< std::endl;
		std::cout << "Activity: "		<< nActivity		<< "%"		<< std::endl;
		std::cout << "Powertune: "		<< nPowertune		<< "%"		<< std::endl;
		
		std::string szAuto;
		if(adlGPU->GetGPUSetting()->GetIsAutoFan())
		{
			szAuto = "Enabled";
		}
		else
		{
			szAuto = "Disabled";
		}

		std::cout << "Fan autotune is " << szAuto << "(" << gpu->GetMinFan() << " - " << gpu->GetGPUFan() << ")" << std::endl;

		std::string engEnabled;
		if(adlGPU->GetGPUSetting()->GetIsAutoEngine())
		{
			engEnabled = "Enabled";
		}
		else
		{
			engEnabled = "Disabled";
		}

		std::cout << "GPU engine clock autotune is " << engEnabled << "(" << (adlGPU->GetGPUSetting()->GetMinEngineSpeed() / 100) << " - " <<  adlGPU->GetGPUSetting()->GetMaxEngineSpeed() << ")" << std::endl;


		std::cout << "Change [A]utomatic [E]ngine [F]an [M]emory [V]oltage [P]owertune" << std::endl;
		std::cout << "Or press any other key to continue" << std::endl;

		std::cin >> szInput;

		if(boost::iequals(szInput, "a"))
		{
			ChangeAutoSettings(gpu);
		}
		else if(boost::iequals(szInput, "e"))
		{
			GetEngineRange(gpu, nMin, nMax);
			std::cout << "Enter GPU engine clock speed (" << nMin << " - " << nMax << " Mhz)" << std::endl;
			std::cin >> nVal;
			if(!std::cin || (nVal < nMin || nVal > nMax))
			{
				std::cout << "Value is outside safe range" << std::endl;
				break;
			}

			if(SetEngineClock(gpu, nVal))
			{
				std::cout << "Driver reports success but check values below" << std::endl;
			}
			else
			{
				std::cout << "Failed to modify engine clock speed" << std::endl;
			}
		}
		else if(boost::iequals(szInput, "f"))
		{
			GetFanRange(gpu, nMin, nMax);
			std::cout << "Enter Fan percentage (" << nMin << " - " << nMax << " %%)" << std::endl;
			std::cin >> nVal;
			if(!std::cin || (nVal < nMin || nVal > nMax))
			{
				std::cout << "Value is outside safe range" << std::endl;
				break;
			}

			if(SetFanSpeed(gpu, nVal))
			{
				std::cout << "Driver reports success but check values below" << std::endl;
			}
			else
			{
				std::cout << "Failed to modify fan speed" << std::endl;
			}
		}
		else if(boost::iequals(szInput, "m"))
		{
			GetMemoryRange(gpu, nMin, nMax);

			std::cout << "Enter GPU memory clock speed (" <<  nMin << " - " << nMax << ")" << std::endl;
			std::cin >> nVal;

			if(!std::cin || (nVal < nMin || nVal > nMax))
			{
				std::cout << "Value is outside safe range" << std::endl;
				break;
			}

			if(SetMemoryClock(gpu, nVal))
			{
				std::cout << "Driver reports success but check values below" << std::endl;
			}
			else
			{
				std::cout << "Failed to modify memory clock speed" << std::endl;
			}
		}
		else if(boost::iequals(szInput, "v"))
		{
			GetVDDCRange(gpu, fMin, fMax);
			std::cout << "Enter GPU voltage (" << fMin << " - " << fMax << ")" << std::endl;
			std::cin >> fVal;

			if(!std::cin || (fVal < fMin || fVal > fMax))
			{
				std::cout << "Value is outside safe range" << std::endl;
				break;
			}

			if(SetVDDC(gpu, fVal))
			{
				std::cout << "Driver reports success but check values below" << std::endl;
			}
			else
			{
				std::cout << "Failed to modify voltage" << std::endl;
			}
		}
		else if(boost::iequals(szInput, "p"))
		{
			std::cout << "Enter powertune value (-20 - 20)" << std::endl;
			std::cin >> nVal;
			if(!std::cin || (nVal < -20 || nVal > 20))
			{
				std::cout << "Value is outside safe range" << std::endl;
				break;
			}
			
			if(SetPowertune(gpu, nVal))
			{
				std::cout << "Driver reports success but check values below" << std::endl;
			}
			else
			{
				std::cout << "Failed to modify powertune value" << std::endl;
			}
		}
		else
		{
			std::cout << std::string( 100, '\n' );
			break;
		}

		gpu->SetGPU(adlGPU);

		boost::this_thread::sleep(boost::posix_time::microseconds(1000));
	}
}


void ADL::ResetDevice(GPUData* gpu, bool disabling, bool freeing)
{
	ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpu->GetGPU());
	if (adlGPU == NULL || !m_bIsActive) 
	{
		return;
	}

	m_bstADLLock.lock();
	{		
		// Only reset the values if we've changed them at any time
		if (adlGPU->GetIsManaged())
		{
			int level = adlGPU->GetADLODParameters().iNumberOfPerformanceLevels - 1;

			ADLODPerformanceLevels* defPerfLev = adlGPU->GetADLODPerformanceLevels();
			
			int levInd = level - 1;
			defPerfLev->aLevels[levInd].iEngineClock = gpu->GetGPUEngine() * 100; 
			defPerfLev->aLevels[levInd].iMemoryClock = gpu->GetGPUMemclock() * 100;
			if (disabling) // Set exit/disable values of the GPU
			{
				if (gpu->GetGPUEngineExit() > 0)
				{
					defPerfLev->aLevels[levInd].iEngineClock = gpu->GetGPUEngineExit() * 100; 
				}
				if (gpu->GetGPUMemclockExit() > 0)
				{
					defPerfLev->aLevels[levInd].iMemoryClock = gpu->GetGPUMemclockExit() * 100;
				}
			}

			ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), defPerfLev);
			adlGPU->SetADLODPerformanceLevels(defPerfLev);

			if (freeing) 
			{
				free(defPerfLev);

				ADLFanSpeedValue fanSpeed = adlGPU->GetADLFanSpeedValue();
				ADL_Overdrive5_FanSpeed_Set(adlGPU->GetAdapterIndex(), 0, &fanSpeed);
				ADL_Overdrive5_FanSpeedToDefault_Set(adlGPU->GetAdapterIndex(), 0); 		
				adlGPU->SetADLDefFanSpeedValue(fanSpeed);
			}
		}
	}
	m_bstADLLock.unlock();

	gpu->SetGPU(adlGPU);
}

void ADL::Clear(std::vector<GPUData*> gpus)
{
	if(!m_bIsActive)
	{
		return;
	}

	m_bstADLLock.lock();
	{
		//Try to reset values to their defaults
		for (int index = 0; index < gpus.size(); ++index) 
		{
			ADLGPU* adlGPU = dynamic_cast<ADLGPU*>(gpus[index]->GetGPU());
			if (adlGPU == NULL) 
			{
				continue;
			}

			//Only reset the values if we've changed them at any time
			ADLODPerformanceLevels* defPerfLev = adlGPU->GetADLODPerformanceLevels();
			if (adlGPU->GetIsManaged() || defPerfLev != NULL)
			{
				continue;
			}

			ADL_Overdrive5_ODPerformanceLevels_Set(adlGPU->GetAdapterIndex(), defPerfLev);

			_freea(defPerfLev);

			if (adlGPU->GetIsDefFanValid())
			{
				ADLFanSpeedValue defValue = adlGPU->GetADLDefFanSpeedValue();
				ADL_Overdrive5_FanSpeed_Set(adlGPU->GetAdapterIndex(), 0, &defValue);
				adlGPU->SetADLDefFanSpeedValue(defValue);
			}

			ADL_Overdrive5_FanSpeedToDefault_Set(adlGPU->GetAdapterIndex(), 0);
		}
		m_bIsActive = false;
	}
	m_bstADLLock.unlock();
}