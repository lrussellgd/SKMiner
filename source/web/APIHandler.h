////////////////////////////////////////////////
// File:	APIHandler.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _APIHANDLER_H_
#define _APIHANDLER_H_

#include "../core/types.h"
#include "../core/Timer.h"

#include <string>
#include <algorithm>
#include <deque>
#include <set>
#include <map>

#include <json_spirit.h>

#include <boost/thread/mutex.hpp>
#include "Request.h"

#include "HttpConnection.h"

#include "APIRequest.h"
#include "../base/Entity.h"
#include "../event/IListener.h"
#include "../event/Event.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

class APIHandler : public Entity, public IListener
{
private:

	
	boost::mutex* m_clLock;
	boost::mutex* m_dataLock;
	
	typedef std::map<std::string, std::string> parameter_map;
	typedef std::vector<std::string> parameter_list;

	typedef json_spirit::mObject(APIHandler::*APIFunc)(parameter_map);
	typedef std::string(APIHandler::*APIStrFunc)(parameter_list);

	typedef std::map<std::string, APIFunc> api_map;
	typedef std::map<std::string, APIStrFunc> str_map;	


	api_map m_mapAPICalls;
	str_map m_mapStrCalls;

	int m_n32lastTime;

	LLP::Thread_t* m_pTHREAD;
	LLP::Timer m_tDataTimer;

	bool m_bShutdown, m_bDidShutdown;

	json_spirit::mObject* m_appData;

	void UpdateData();
	void ProcessNextMessage();

protected:

	void HandleEvent(Event* pEvent);

public:

	//////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	APIHandler();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	APIHandler(const APIHandler& apiHandler);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	APIHandler& operator=(const APIHandler& apiHandler);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~APIHandler();

	void Shutdown();

	std::string HandleRequest(APIRequest* pAPIRequest);

	const bool GetDidShutdown() const { return this->m_bDidShutdown; }

private:
	void SetupStatusObject(bool bSuccess, const std::string szCaller, json_spirit::mObject& returnObject);
	void SetupStatusObject_STR(bool bSuccess, const int nCode, const std::string& szMSG, std::string& returnObject);


	json_spirit::mObject Version(parameter_map mapParams = parameter_map());
	std::string Version_STR(parameter_list listParams = parameter_list());


	void _config_Prepare(int& nGPUCount, int& nASCount, int& nPGACount, int& nPoolCount, std::string& szADL,
		std::string& szADLInUse, std::string& szStrategy, int& nInterval,
		std::string& szDevCode, std::string& szOS, bool& bFailover,
		int& nScanTime, int& nQueue, int& nExpiry);


	void _Config(int nGPUCount, int nASCount, int nPGACount, int nPoolCount, std::string szADL,
		std::string szADLInUse, std::string szStrategy, int nInterval,
		std::string szDevCode, std::string szOS, bool bFailover,
		int nScanTime, int nQueue, int nExpiry, json_spirit::mObject& returnObject);

	void _ConfigStr(int nGPUCount, int nASCount, int nPGACount, int nPoolCount, std::string szADL,
		std::string szADLInUse, std::string szStrategy, int nInterval,
		std::string szDevCode, std::string szOS, bool bFailover,
		int nScanTime, int nQueue, int nExpiry, std::string& returnStr);


	json_spirit::mObject Config(parameter_map mapParams = parameter_map());
	std::string Config_STR(parameter_list listParams = parameter_list());

	json_spirit::mObject Devs(parameter_map mapParams = parameter_map());

	std::string Devs_STR(parameter_list listParams = parameter_list());


	json_spirit::mObject Pools(parameter_map mapParams = parameter_map());
	std::string Pools_STR(parameter_list listParams = parameter_list());


	json_spirit::mObject Summary(parameter_map mapParams = parameter_map());
	std::string Summary_STR(parameter_list listParams = parameter_list());

	bool EnableGPU(int32 nGPUNum);
	json_spirit::mObject GPUEnable(parameter_map mapParams = parameter_map());
	std::string GPUEnable_STR(parameter_list listParams = parameter_list());

	bool DiableGPU(int32 nGPUNum);
	json_spirit::mObject GPUDisable(parameter_map mapParams = parameter_map());
	std::string GPUDisable_STR(parameter_list listParams = parameter_list());

	bool RestartGPU(int32 nGPUNum);
	json_spirit::mObject GPURestart(parameter_map mapParams = parameter_map());
	std::string GPURestart_STR(parameter_list listParams = parameter_list());

	json_spirit::mObject GPU(parameter_map mapParams = parameter_map());
	std::string GPU_STR(parameter_list listParams = parameter_list());

	int32 GetGPUCount();
	json_spirit::mObject GPUCount(parameter_map mapParams = parameter_map());
	std::string GPUCount_STR(parameter_list listParams = parameter_list());

	bool SwitchMiningPool(int32 nPoolNum);
	json_spirit::mObject SwitchPool(parameter_map mapParams = parameter_map());
	std::string SwitchPool_STR(parameter_list listParams = parameter_list());

	bool AddMiningPool(std::string szURL, std::string szUser, std::string szPassword);
	json_spirit::mObject AddPool(parameter_map mapParams = parameter_map());
	std::string AddPool_STR(parameter_list listParams = parameter_list());

	bool SetPoolPriority(std::vector<int32> vecPoolIDs);
	json_spirit::mObject PoolPriority(parameter_map mapParams = parameter_map());
	std::string PoolPriority_STR(parameter_list listParams = parameter_list());

	json_spirit::mObject PoolQuota(parameter_map mapParams = parameter_map());
	std::string PoolQuota_STR(parameter_list listParams = parameter_list());

	bool EnableMiningPool(int32 nPoolID);
	json_spirit::mObject EnablePool(parameter_map mapParams = parameter_map());
	std::string EnablePool_STR(parameter_list listParams = parameter_list());

	bool DisableMiningPool(int32 nPoolID);
	json_spirit::mObject DisablePool(parameter_map mapParams = parameter_map());
	std::string DisablePool_STR(parameter_list listParams = parameter_list());

	bool RemoveMiningPool(int32 nPoolID);
	json_spirit::mObject RemovePool(parameter_map mapParams = parameter_map());
	std::string RemovePool_STR(parameter_list listParams = parameter_list());

	bool SetIntensity(int32 nIntensity, int32 nGPU);
	json_spirit::mObject GPUIntensity(parameter_map mapParams = parameter_map());
	std::string GPUIntensity_STR(parameter_list listParams = parameter_list());

	bool SetXIntensity(int32 nXIntensity, int32 nGPU);
	json_spirit::mObject GPUXIntensity(parameter_map mapParams = parameter_map());
	std::string GPUXIntensity_STR(parameter_list listParams = parameter_list());

	bool SetRawIntensity(int32 nRawIntensity, int32 nGPU);
	json_spirit::mObject GPURawIntensity(parameter_map mapParams = parameter_map());
	std::string GPURawIntensity_STR(parameter_list listParams = parameter_list());

	bool SetGPUMemclock(int32 nGPUMemclock, int32 nGPU);
	json_spirit::mObject GPUMem(parameter_map mapParams = parameter_map());
	std::string GPUMem_STR(parameter_list listParams = parameter_list());

	bool SetGPUEngineClock(int32 nGPUEngineClock, int32 nGPU);
	json_spirit::mObject GPUEngine(parameter_map mapParams = parameter_map());
	std::string GPUEngine_STR(parameter_list listParams = parameter_list());

	bool SetGPUFan(int32 nGPUFan, int32 nGPU);
	json_spirit::mObject GPUFan(parameter_map mapParams = parameter_map());
	std::string GPUFan_STR(parameter_list listParams = parameter_list());

	bool SetGPUVDDC(float fGPU_VDDC, int32 nGPU);
	json_spirit::mObject GPUVddc(parameter_map mapParams = parameter_map());
	std::string GPUVddc_STR(parameter_list listParams = parameter_list());

	bool SaveSettings();
	json_spirit::mObject Save(parameter_map mapParams = parameter_map());
	std::string Save_STR(parameter_list listParams = parameter_list());

	bool QuitMiner();
	json_spirit::mObject Quit(parameter_map mapParams = parameter_map());
	std::string Quit_STR(parameter_list listParams = parameter_list());

	bool PrivelegedInfo();
	json_spirit::mObject Privileged(parameter_map mapParams = parameter_map());
	std::string Privileged_STR(parameter_list listParams = parameter_list());

	bool NotifyInfo();
	json_spirit::mObject Notify(parameter_map mapParams = parameter_map());
	std::string Notify_STR(parameter_list listParams = parameter_list());

	json_spirit::mObject DevDetails(parameter_map mapParams = parameter_map());
	std::string DevDetails_STR(parameter_list listParams = parameter_list());

	bool RestartMiner();
	json_spirit::mObject Restart(parameter_map mapParams = parameter_map());
	std::string Restart_STR(parameter_list listParams = parameter_list());

	json_spirit::mObject Stats(parameter_map mapParams = parameter_map());
	std::string Stats_STR(parameter_list listParams = parameter_list());

	bool CheckMiner();
	json_spirit::mObject Check(parameter_map mapParams = parameter_map());
	std::string Check_STR(parameter_list listParams = parameter_list());

	bool SetFailoverOnly();
	json_spirit::mObject Failover_Only(parameter_map mapParams = parameter_map());
	std::string Failover_Only_STR(parameter_list listParams = parameter_list());

	json_spirit::mObject Coin(parameter_map mapParams = parameter_map());
	std::string Coin_STR(parameter_list listParams = parameter_list());

	bool SetDebug();
	json_spirit::mObject Debug(parameter_map mapParams = parameter_map());
	std::string Debug_STR(parameter_list listParams = parameter_list());

	bool SetConfigData();
	json_spirit::mObject SetConfig(parameter_map mapParams = parameter_map());
	std::string SetConfig_STR(parameter_list listParams = parameter_list());

	bool SetZero();
	json_spirit::mObject Zero(parameter_map mapParams = parameter_map());
	std::string Zero_STR(parameter_list listParams = parameter_list());

	bool SetLockStats();
	json_spirit::mObject LockStats(parameter_map mapParams = parameter_map());
	std::string LockStats_STR(parameter_list listParams = parameter_list());

	json_spirit::mObject GetMiningInfo(parameter_map mapParams = parameter_map());
};


#endif //_APIHANDLER_H_