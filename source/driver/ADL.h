////////////////////////////////////////////////
// File:	ADL.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _ADL_H_
#define _ADL_H_

#include "../base/OSVersion.h"
#include "../base/Entity.h"
#include "../event/IListener.h"
#include "../event/Event.h"
#include "ADLFuncs.h"

#include <string>
#include <vector>
#include "../ADL_SDK/adl_sdk.h"

#include <boost\thread.hpp>

#define AMDVENDORID             (1002)
#define ADL_WARNING_NO_DATA      -100

class ADLGPU;
class GPUData;
class GPUSetting;

class ADL : public Entity, public IListener
{

private:

	INSTANCE_HANDLE							m_hDLL;
	bool									m_bIsActive;	
	bool									m_bOD5Found;
	bool									m_bOD6Found;
	int										m_nNumAdapters;
	int										m_nNumDevices;
	std::vector<AdapterInfo>				m_vecAdapterInfo;
	LPAdapterInfo							m_lpInfo;

	int										m_nOverdriveSupported;
	int										m_nOverdriveEnabled;
	int										m_nOverdriveVersion;

	boost::mutex*							m_bstADLLock;

	std::vector<int>						m_vecValidAdapters;
	std::vector<ADLGPU*>					m_vecDevices;

	ADL_MAIN_CONTROL_CREATE					ADL_Main_Control_Create;
	ADL_MAIN_CONTROL_DESTROY				ADL_Main_Control_Destroy;
	ADL_ADAPTER_NUMBEROFADAPTERS_GET		ADL_Adapter_NumberOfAdapters_Get;
	ADL_ADAPTER_ADAPTERINFO_GET				ADL_Adapter_AdapterInfo_Get;
	ADL_ADAPTER_ID_GET						ADL_Adapter_ID_Get;
	ADL_MAIN_CONTROL_REFRESH				ADL_Main_Control_Refresh;
	ADL_ADAPTER_VIDEOBIOSINFO_GET			ADL_Adapter_VideoBiosInfo_Get;
	ADL_DISPLAY_DISPLAYINFO_GET				ADL_Display_DisplayInfo_Get;
	ADL_ADAPTER_ACCESSIBILITY_GET			ADL_Adapter_Accessibility_Get;
	ADL_ADAPTER_ACTIVE_GET					ADL_Adapter_Active_Get;
	
	ADL_OVERDRIVE_CAPS						ADL_Overdrive_Caps;

	ADL_OVERDRIVE5_THERMALDEVICES_ENUM		ADL_Overdrive5_ThermalDevices_Enum;
	ADL_OVERDRIVE5_ODPARAMETERS_GET			ADL_Overdrive5_ODParameters_Get;
	ADL_OVERDRIVE5_TEMPERATURE_GET			ADL_Overdrive5_Temperature_Get;
	ADL_OVERDRIVE5_FANSPEED_GET				ADL_Overdrive5_FanSpeed_Get;
	ADL_OVERDRIVE5_FANSPEEDINFO_GET			ADL_Overdrive5_FanSpeedInfo_Get;
	ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	ADL_Overdrive5_ODPerformanceLevels_Get;
	ADL_OVERDRIVE5_CURRENTACTIVITY_GET		ADL_Overdrive5_CurrentActivity_Get;
	ADL_OVERDRIVE5_FANSPEED_SET				ADL_Overdrive5_FanSpeed_Set;
	ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET  ADL_Overdrive5_ODPerformanceLevels_Set;
	ADL_OVERDRIVE5_POWERCONTROL_CAPS		ADL_Overdrive5_PowerControl_Caps;
	ADL_OVERDRIVE5_POWERCONTROLINFO_GET		ADL_Overdrive5_PowerControlInfo_Get;
	ADL_OVERDRIVE5_POWERCONTROL_GET			ADL_Overdrive5_PowerControl_Get;
	ADL_OVERDRIVE5_POWERCONTROL_SET			ADL_Overdrive5_PowerControl_Set;
	ADL_OVERDRIVE5_FANSPEEDTODEFAULT_SET	ADL_Overdrive5_FanSpeedToDefault_Set;

	ADL_OVERDRIVE6_FANSPEED_GET				ADL_Overdrive6_FanSpeed_Get;
	ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS	ADL_Overdrive6_ThermalController_Caps;
	ADL_OVERDRIVE6_TEMPERATURE_GET			ADL_Overdrive6_Temperature_Get;
	ADL_OVERDRIVE6_CAPABILITIES_GET			ADL_Overdrive6_Capabilities_Get;
	ADL_OVERDRIVE6_STATEINFO_GET			ADL_Overdrive6_StateInfo_Get;
	ADL_OVERDRIVE6_CURRENTSTATUS_GET		ADL_Overdrive6_CurrentStatus_Get;
	ADL_OVERDRIVE6_POWERCONTROL_CAPS		ADL_Overdrive6_PowerControl_Caps;
	ADL_OVERDRIVE6_POWERCONTROLINFO_GET		ADL_Overdrive6_PowerControlInfo_Get;
	ADL_OVERDRIVE6_POWERCONTROL_GET			ADL_Overdrive6_PowerControl_Get;
	ADL_OVERDRIVE6_FANSPEED_SET				ADL_Overdrive6_FanSpeed_Set;
	ADL_OVERDRIVE6_STATE_SET				ADL_Overdrive6_State_Set;
	ADL_OVERDRIVE6_POWERCONTROL_SET			ADL_Overdrive6_PowerControl_Set;

	ADL_OVERDRIVE6_VOLTAGECONTROLINFO_GET	ADL_Overdrive6_VoltageControlInfo_Get;
	ADL_OVERDRIVE6_VOLTAGECONTROL_GET		ADL_Overdrive6_VoltageControl_Get;
	ADL_OVERDRIVE6_VOLTAGECONTROL_SET		ADL_Overdrive6_VoltageControl_Set;
	ADL_OVERDRIVE6_FANSPEED_RESET			ADL_Overdrive6_FanSpeed_Reset;
	ADL_OVERDRIVE6_STATE_RESET				ADL_Overdrive6_State_Reset;


	void SetupADL();
	void SetupOD5(ADLGPU* adlGPU, GPUSetting* gpuSetting);
	void SetupOD6(ADLGPU* adlGPU, GPUSetting* gpuSetting);

	void ClearDevices();

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	ADL& operator=(const ADL& adl) { return *this; }


protected:

	void HandleEvent(Event* pEvent);

public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	ADL();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	ADL(const ADL& adl);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~ADL();

	void SetupADLDevices(std::vector<GPUData*> gpus);


	static void* __stdcall ADL_Main_Memory_Alloc ( int iSize ) { void* lpBuffer = malloc ( iSize ); return lpBuffer; }
	static void __stdcall ADL_Main_Memory_Free ( void** lpBuffer ) { if ( NULL != *lpBuffer ) { free ( *lpBuffer ); *lpBuffer = NULL; } }

#if defined(UNIX)
	static void *GetProcAddress(void *pLibrary, const char *name) { return dlsym( pLibrary, name); }
#endif

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	const bool	SetFanSpeed(ADLGPU* gpu, int iFanSpeed);
	const bool	SetMemoryClock(ADLGPU* gpu, int iMemoryClock);
	const bool	SetEngineClock(ADLGPU* gpu, int iEngineClock);
	const bool  SetVDDC(ADLGPU* gpu, float fVddc);
	const bool  SetPowertune(ADLGPU* gpu, int iPowertune);	
	
	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const bool GetIsActive() const { return this->m_bIsActive; }

	ADLGPU* GetDevice(int iAdapterIndex);
	ADLGPU* GetDeviceInOrder(int nIndex);

	const std::vector<ADLGPU*> GetDevices() const { return this->m_vecDevices; }
	const std::vector<std::string> GetAdapterIDs();
	const float GetGPUTemp(ADLGPU* adlGPU);
	const bool	GetIsFanSpeedTwin(ADLGPU* adlGPUOne, ADLGPU* adlGPUTwo);
	const std::string& GetADLError(int error);	
	const int GetFanSpeed(ADLGPU* adlGPU);
	const int GetEngineClock(ADLGPU* adlGPU);
	const int GetMemoryClock(ADLGPU* adlGPU);
	const float GetVDDC(ADLGPU *adlGPU);
	const int GetActivity(ADLGPU* adlGPU);
	const int GetPowertune(ADLGPU* adlGPU);
	void GetGPUStats(ADLGPU* adlGPU, float &temp, int &engineclock, int &memclock, float &vddc, int &activity, int &fanspeed, int &powertune);	
	void GetMemoryRange(ADLGPU* adlGPU, int &imin, int &imax);
	void GetVDDCRange(ADLGPU* adlGPU, float &imin, float &imax);
	void GetFanRange(ADLGPU* adlGPU, int &imin, int &imax);
	void GetEngineRange(ADLGPU* adlGPU, int &imin, int &imax);
	void GetPowertuneRange(ADLGPU* adlGPU, int &imin, int &imax);

	void Shutdown();	
	void SetDefaults(ADLGPU* adlGPU);
	void ResetDevice(ADLGPU* adlGPU);
		
	const bool FanAutotune(ADLGPU* adlGPU, int temp, int fanpercent, int lasttemp, bool &fan_window);	

};

#endif // _ADL_H_