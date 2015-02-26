////////////////////////////////////////////////
// File:	APIHandler.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "APIHandler.h"

#include "../app/Application.h"
#include "../data/MinerData.h"
#include "../gpu/ADLGPU.h"
#include"../config/GPUSetting.h"
#include "../compute/BaseComputeDevice.h"
#include "../driver/ADL.h"
#include "../compute/CLDevice.h"
#include "../compute/CLFuncs.h"
#include "../utilities/StringUtils.h"
#include "../connections/SKServerConnection.h"
#include "../event/EventManager.h"
#include "MimeTypes.h"
#include "Reply.h"
#include "Header.h"

static const std::string API_VERSION = "1.0";

static const std::string STATUS_SUCCESS = "S";
static const std::string STATUS_FAILURE = "F";

static const std::string MESSAGE_SUCCESS = "Successfully made the API call to: ";
static const std::string MESSAGE_FAILURE = "Failed to make the API call to: ";

static const std::string COMMA = ",";
static const std::string SEPARATOR = "|";
static const std::string EQUALS = "=";

//////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
APIHandler::APIHandler()
{
	this->m_enmEntityType = ENTITY_TYPE::API_HANDLER;
	this->m_clLock = new boost::mutex();
	m_dataLock = new boost::mutex();

	m_mapAPICalls["version"] = &APIHandler::Version;
	m_mapAPICalls["config"] = &APIHandler::Config;
	m_mapAPICalls["devs"] = &APIHandler::Devs;
	m_mapAPICalls["pools"] = &APIHandler::Pools;
	m_mapAPICalls["summary"] = &APIHandler::Summary;
	m_mapAPICalls["gpuenable"] = &APIHandler::GPUEnable;
	m_mapAPICalls["gpudisable"] = &APIHandler::GPUDisable;
	m_mapAPICalls["gpurestart"] = &APIHandler::GPURestart;
	m_mapAPICalls["gpu"] = &APIHandler::GPU;
	m_mapAPICalls["gpucount"] = &APIHandler::GPUCount;
	m_mapAPICalls["switchpool"] = &APIHandler::SwitchPool;
	m_mapAPICalls["addpool"] = &APIHandler::AddPool;
	m_mapAPICalls["poolpriority"] = &APIHandler::PoolPriority;
	m_mapAPICalls["poolquota"] = &APIHandler::PoolQuota;
	m_mapAPICalls["enablepool"] = &APIHandler::EnablePool;
	m_mapAPICalls["disablepool"] = &APIHandler::DisablePool;
	m_mapAPICalls["removepool"] = &APIHandler::RemovePool;
	m_mapAPICalls["gpuintensity"] = &APIHandler::GPUIntensity;
	m_mapAPICalls["gpumem"] = &APIHandler::GPUMem;
	m_mapAPICalls["gpuengine"] = &APIHandler::GPUEngine;
	m_mapAPICalls["gpufan"] = &APIHandler::GPUFan;
	m_mapAPICalls["gpuvddc"] = &APIHandler::GPUVddc;
	m_mapAPICalls["save"] = &APIHandler::Save;
	m_mapAPICalls["quit"] = &APIHandler::Quit;
	m_mapAPICalls["privileged"] = &APIHandler::Privileged;
	m_mapAPICalls["notify"] = &APIHandler::Notify;
	m_mapAPICalls["devdetails"] = &APIHandler::DevDetails;
	m_mapAPICalls["restart"] = &APIHandler::Restart;
	m_mapAPICalls["stats"] = &APIHandler::Stats;
	m_mapAPICalls["check"] = &APIHandler::Check;
	m_mapAPICalls["failover-only"] = &APIHandler::Failover_Only;
	m_mapAPICalls["coin"] = &APIHandler::Coin;
	m_mapAPICalls["debug"] = &APIHandler::Debug;
	m_mapAPICalls["setconfig"] = &APIHandler::SetConfig;
	m_mapAPICalls["zero"] = &APIHandler::Zero;
	m_mapAPICalls["lockstats"] = &APIHandler::LockStats;
	m_mapAPICalls["getmininginfo"] = &APIHandler::GetMiningInfo;

	m_mapStrCalls["version"] = &APIHandler::Version_STR;
	m_mapStrCalls["config"] = &APIHandler::Config_STR;
	m_mapStrCalls["devs"] = &APIHandler::Devs_STR;
	m_mapStrCalls["pools"] = &APIHandler::Pools_STR;
	m_mapStrCalls["summary"] = &APIHandler::Summary_STR;
	m_mapStrCalls["gpuenable"] = &APIHandler::GPUEnable_STR;
	m_mapStrCalls["gpudisable"] = &APIHandler::GPUDisable_STR;
	m_mapStrCalls["gpurestart"] = &APIHandler::GPURestart_STR;
	m_mapStrCalls["gpu"] = &APIHandler::GPU_STR;
	m_mapStrCalls["gpucount"] = &APIHandler::GPUCount_STR;
	m_mapStrCalls["switchpool"] = &APIHandler::SwitchPool_STR;
	m_mapStrCalls["addpool"] = &APIHandler::AddPool_STR;
	m_mapStrCalls["poolpriority"] = &APIHandler::PoolPriority_STR;
	m_mapStrCalls["poolquota"] = &APIHandler::PoolQuota_STR;
	m_mapStrCalls["enablepool"] = &APIHandler::EnablePool_STR;
	m_mapStrCalls["disablepool"] = &APIHandler::DisablePool_STR;
	m_mapStrCalls["removepool"] = &APIHandler::RemovePool_STR;
	m_mapStrCalls["gpuintensity"] = &APIHandler::GPUIntensity_STR;
	m_mapStrCalls["gpumem"] = &APIHandler::GPUMem_STR;
	m_mapStrCalls["gpuengine"] = &APIHandler::GPUEngine_STR;
	m_mapStrCalls["gpufan"] = &APIHandler::GPUFan_STR;
	m_mapStrCalls["gpuvddc"] = &APIHandler::GPUVddc_STR;
	m_mapStrCalls["save"] = &APIHandler::Save_STR;
	m_mapStrCalls["quit"] = &APIHandler::Quit_STR;
	m_mapStrCalls["privileged"] = &APIHandler::Privileged_STR;
	m_mapStrCalls["notify"] = &APIHandler::Notify_STR;
	m_mapStrCalls["devdetails"] = &APIHandler::DevDetails_STR;
	m_mapStrCalls["restart"] = &APIHandler::Restart_STR;
	m_mapStrCalls["stats"] = &APIHandler::Stats_STR;
	m_mapStrCalls["check"] = &APIHandler::Check_STR;
	m_mapStrCalls["failover-only"] = &APIHandler::Failover_Only_STR;
	m_mapStrCalls["coin"] = &APIHandler::Coin_STR;
	m_mapStrCalls["debug"] = &APIHandler::Debug_STR;
	m_mapStrCalls["setconfig"] = &APIHandler::SetConfig_STR;
	m_mapStrCalls["zero"] = &APIHandler::Zero_STR;
	m_mapStrCalls["lockstats"] = &APIHandler::LockStats_STR;

	EventManager::GetInstance()->Register(Event::EVENT_TYPE::RETURN_MINING_INFO, this);

	m_appData = NULL;
	m_bShutdown = false;
	m_bDidShutdown = false;

	m_n32lastTime = 0;
	m_tDataTimer.Start();
	m_pTHREAD = new boost::thread(&APIHandler::UpdateData, this);
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
APIHandler::APIHandler(const APIHandler& apiHandler) : Entity(apiHandler)
{
	this->m_clLock = new boost::mutex();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
APIHandler& APIHandler::operator = (const APIHandler& apiHandler)
{
	this->m_enmEntityType = apiHandler.GetEntityType();
	this->m_clLock = new boost::mutex();
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
APIHandler::~APIHandler()
{
	EventManager::GetInstance()->Deregister(this);

	if (m_clLock)
	{
		m_clLock->unlock();
		delete(m_clLock);
		this->m_clLock = NULL;
	}

	if (m_dataLock)
	{
		m_dataLock->unlock();
		delete(m_dataLock);
		m_dataLock = NULL;
	}

	if (m_appData)
	{
		delete(m_appData);
		m_appData = NULL;
	}

	if (m_pTHREAD)
	{
		m_pTHREAD->detach();
		delete(m_pTHREAD);
		m_pTHREAD = NULL;
	}
}

void APIHandler::Shutdown()
{
	m_bShutdown = true;
}

void APIHandler::UpdateData()
{
	while (!m_bShutdown)
	{
		Sleep(10);
		int nCurrentTime = m_tDataTimer.ElapsedMilliseconds();

		if ((nCurrentTime - m_n32lastTime) > 1000)
		{
			m_n32lastTime = nCurrentTime;
			EventManager::GetInstance()->AddEvent(Event::EVENT_TYPE::GET_MINING_INFO);
		}
	}

	m_bDidShutdown = true;
}

std::string APIHandler::HandleRequest(APIRequest* pAPIRequest)
{
	std::string szContent = "";
	switch (pAPIRequest->GetRequestType())
	{
		case Http::Server::Request::RequestType::HTTP:
		{
			json_spirit::mObject jmObj;
			api_map::iterator iter;
			this->m_clLock->lock();
			{
				iter = m_mapAPICalls.find(pAPIRequest->GetCommand());
			}
			this->m_clLock->unlock();

			if (iter != m_mapAPICalls.end())
			{
				this->m_clLock->lock();
				{
					jmObj = CALL_MEMBER_FN(*this, m_mapAPICalls[pAPIRequest->GetCommand()])(pAPIRequest->GetParameters());
				}
				this->m_clLock->unlock();
			}
			else
			{
				SetupStatusObject(false, pAPIRequest->GetCommand(), jmObj);
			}

			szContent = json_spirit::write(jmObj, json_spirit::Output_options::pretty_print);
		}
		break;
		case Http::Server::Request::RequestType::STRING:
		{
			str_map::iterator iter;

			this->m_clLock->lock();
			{
				iter = m_mapStrCalls.find(pAPIRequest->GetCommand());
			}
			this->m_clLock->unlock();

			if (iter != m_mapStrCalls.end())
			{
				this->m_clLock->lock();
				{
					szContent = CALL_MEMBER_FN(*this, m_mapStrCalls[pAPIRequest->GetCommand()])(pAPIRequest->ParametersAsList());
				}
				this->m_clLock->unlock();
			}
			else
			{
				SetupStatusObject_STR(false, 0, pAPIRequest->GetCommand(), szContent);
			}
		}
		break;
	}

	return szContent;
}


//////////////////////////////////////////////////////////////////////////////
//Name:	SetupStatusObject
//
//Purpose:	
//		Sets the status of requested API call.
//
//Parameters:
//		bSuccess:		True if the API call was successful. False otherwise.
//		szCaller:		The API call.
//		returnObject:	The json object to add a status section to.
//
//Return:
//
//		Reference to a json object containing the status of the api call
//
//		Ex:
//
//			{
//				"STATUS":
//				{
//					"STATUS": "S",
//					"Msg": "Successfully made the API call to: summary",
//					"Code" : -1,
//					"When": 1422821846,
//					"Description": "1.0"
//				}
//			}
//
//
//		NOTE:	This is for CGWatcher Compatibility which uses the 
//				CGMiner API. The "Code" portion of the status object
//				is always set to -1. SKMiner does not use CGMiners
//				code system.
///////////////////////////////////////////////////////////////////////////////
void APIHandler::SetupStatusObject(bool bSuccess, const std::string szCaller, json_spirit::mObject& returnObject)
{
	json_spirit::mObject statusObject;

	if (bSuccess)
	{
		statusObject["STATUS"] = STATUS_SUCCESS;
		statusObject["Msg"] = MESSAGE_SUCCESS + szCaller;

	}
	else
	{
		statusObject["STATUS"] = STATUS_FAILURE;
		statusObject["Msg"] = MESSAGE_FAILURE + szCaller;
	}

	time_t timer;
	time(&timer);

	statusObject["Code"] = szCaller;
	statusObject["When"] = timer;
	statusObject["Description"] = API_VERSION;

	returnObject["STATUS"] = statusObject;
}

void APIHandler::SetupStatusObject_STR(bool bSuccess, const int nCode, const std::string& szMSG, std::string& returnObject)
{
	if (bSuccess)
	{
		returnObject += std::string("STATUS") + EQUALS + STATUS_SUCCESS + COMMA;
	}
	else
	{
		returnObject += std::string("STATUS") + EQUALS + STATUS_FAILURE + COMMA;
	}

	time_t timer;
	time(&timer);

	returnObject += std::string("Code") + EQUALS + std::to_string(nCode) + COMMA;
	returnObject += std::string("When") + EQUALS + std::to_string(timer) + COMMA;
	returnObject += std::string("Msg") + EQUALS + szMSG + COMMA;
	returnObject += std::string("Description") + EQUALS + SEPARATOR;
}



//////////////////////////////////////////////////////////////////////////////
//NAME:	Version
//
//PURPOSE:	
//		Gets the Version Information for SKMiner and the API
//
//PARAMETERS:
//		 mapParams: Empty. Doesn't take any parameters
//
//RETURN:
//		json object containing the Version Information
//
//		Ex:
//
//		{
//			"CGMiner": "1.1",
//			"API": "1.0",
//			"STATUS":
//				{
//					"STATUS": "S",
//					"Msg": "Successfully made the API call to: version",
//					"Code" : -1,
//					"When": 1422821846,
//					"Description": "1.0"
//				}
//		}
//
//		NOTE: "CGMiner" key is used for CGWatcher Compatibility
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Version(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	returnObject["CGMiner"] = VERSION;
	returnObject["API"] = API_VERSION;

	SetupStatusObject(true, "version", returnObject);

	return returnObject;
}


std::string APIHandler::Version_STR(parameter_list listParams)
{
	std::string returnObject;

	SetupStatusObject_STR(true, 22, "CGMiner versions", returnObject);
	
	//"CGMiner versions"
	//"VERSION"
	returnObject += std::string("VERSION") + COMMA;
	returnObject += std::string("CGMiner") + EQUALS + VERSION + COMMA + std::string("API") + EQUALS + API_VERSION + SEPARATOR;

	return returnObject;
}



void APIHandler::_config_Prepare(int& nGPUCount, int& nASCount, int& nPGACount, int& nPoolCount, std::string& szADL,
	std::string& szADLInUse, std::string& szStrategy, int& nInterval,
	std::string& szDevCode, std::string& szOS, bool& bFailover,
	int& nScanTime, int& nQueue, int& nExpiry)
{

	szADL = "N";
	szADLInUse = "N";
	
	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			bool bADLActive = dataObj.at("ADL_ACTIVE").get_bool();
			if (bADLActive)
			{
				szADL = "Y";
				szADLInUse = "Y";
			}

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				double hashes = connObj.at("Hashes").get_real();
				if (hashes > 0)
				{
					nGPUCount = connObj.at("Devices").get_array().size();
					break;
				}
			}
		}
	}
	m_dataLock->unlock();


	nASCount = 0;
	nPGACount = 0;
	szStrategy = "Failover";
	nInterval = 10;
	szOS = OS_VERSION;
	bFailover = true;
	nScanTime = 10;
	nQueue = 1;
	nExpiry = 120;
}


void APIHandler::_Config(int nGPUCount, int nASCount, int nPGACount, int nPoolCount, std::string szADL,
	std::string szADLInUse, std::string szStrategy, int nInterval,
	std::string szDevCode, std::string szOS, bool bFailover,
	int nScanTime, int nQueue, int nExpiry, json_spirit::mObject& returnObject)
{
	returnObject["GPU Count"] = nGPUCount;
	returnObject["ASC Count"] = nASCount;
	returnObject["PGA Count"] = nPGACount;
	returnObject["Pool Count"] = nPoolCount;
	returnObject["ADL"] = szADL;
	returnObject["ADL in use"] = szADLInUse;
	returnObject["Strategy"] = szStrategy;
	returnObject["Log Interval"] = nInterval;
	returnObject["Device Code"] = szDevCode;
	returnObject["OS"] = szOS;
	returnObject["Failover-Only"] = bFailover;
	returnObject["ScanTime"] = nScanTime;
	returnObject["Queue"] = nQueue;
	returnObject["Expiry"] = nExpiry;
}

void APIHandler::_ConfigStr(int nGPUCount, int nASCount, int nPGACount, int nPoolCount, std::string szADL,
	std::string szADLInUse, std::string szStrategy, int nInterval,
	std::string szDevCode, std::string szOS, bool bFailover,
	int nScanTime, int nQueue, int nExpiry, std::string& returnStr)
{

	returnStr += std::string("GPU Count") + EQUALS + std::to_string(nGPUCount) + COMMA;
	returnStr += std::string("ASC Count") + EQUALS + std::to_string(nASCount) + COMMA;
	returnStr += std::string("PGA Count") + EQUALS + std::to_string(nPGACount) + COMMA;
	returnStr += std::string("Pool Count") + EQUALS + std::to_string(nPoolCount) + COMMA;
	returnStr += std::string("ADL") + EQUALS + szADL + COMMA;
	returnStr += std::string("ADL in use") + EQUALS + szADLInUse + COMMA;
	returnStr += std::string("Strategy") + EQUALS + szStrategy + COMMA;
	returnStr += std::string("Log Interval") + EQUALS + std::to_string(nInterval) + COMMA;
	returnStr += std::string("Device Code") + EQUALS + szDevCode + COMMA;
	returnStr += std::string("OS") + EQUALS + szOS + COMMA;
	if (bFailover)
	{
		returnStr += std::string("Failover-Only") + EQUALS + std::string("true") + COMMA;
	}
	else
	{
		returnStr += std::string("Failover-Only") + EQUALS + std::string("false") + COMMA;
	}
	returnStr += std::string("ScanTime") + EQUALS + std::to_string(nScanTime) + COMMA;
	returnStr += std::string("Queue") + EQUALS + std::to_string(nQueue) + COMMA;
	returnStr += std::string("Expiry") + EQUALS + std::to_string(nExpiry);
}

//////////////////////////////////////////////////////////////////////////////
//NAME:	Config
//
//PURPOSE:	
//		Gets the Miner Config Information for SKMiner
//
//PARAMETERS:
//		 mapParams: Empty. Doesn't take any parameters
//
//RETURN:
//		json object containing the Miner Config Information
//
//		Ex:
//
//		{
//			"GPU Count": 1,
//			"ASC Count": 0,
//			"PGA Count": 0,
//			"Pool Count": 1,
//			"ADL": "Y", 
//			"ADL in use": "Y"
//			"Strategy": "Failover",
//			"Log Interval": 10,
//			"Device Code": "GPU ",
//			"OS": "Windows",
//			"Failover-Only": true,
//			"ScanTime": 30,
//			"Queue": 1,
//			"Expiry": 120,
//			"STATUS":
//				{
//					"STATUS": "S",
//					"Msg": "Successfully made the API call to: config",
//					"Code" : -1,
//					"When": 1422821846,
//					"Description": "1.0"
//				}
//		}
//
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Config(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	SetupStatusObject(true, "config", returnObject);

	bool bFailover;
	int nGPUCount, nASCount, nPGACount, nPoolCount, nInterval, nScanTime, nQueue, nExpiry;
	std::string szADL, szADLInUse, szStrategy, szDevCode, szOS;

	_config_Prepare(nGPUCount, nASCount, nPGACount, nPoolCount, szADL, szADLInUse, szStrategy, nInterval, szDevCode, szOS, bFailover, nScanTime, nQueue, nExpiry);

	_Config(nGPUCount, nASCount, nPGACount, nPoolCount, szADL, szADLInUse, szStrategy, nInterval, szDevCode, szOS, bFailover, nScanTime, nQueue, nExpiry, returnObject);

	return returnObject;
}

std::string APIHandler::Config_STR(parameter_list listParams)
{
	std::string returnObject;

	SetupStatusObject_STR(true, 33, "CGMiner config", returnObject);
	returnObject += std::string("CONFIG") + COMMA;


	bool bFailover;
	int nGPUCount, nASCount, nPGACount, nPoolCount, nInterval, nScanTime, nQueue, nExpiry;
	std::string szADL, szADLInUse, szStrategy, szDevCode, szOS;

	_config_Prepare(nGPUCount, nASCount, nPGACount, nPoolCount, szADL, szADLInUse, szStrategy, nInterval, szDevCode, szOS, bFailover, nScanTime, nQueue, nExpiry);
	_ConfigStr(nGPUCount, nASCount, nPGACount, nPoolCount, szADL, szADLInUse, szStrategy, nInterval, szDevCode, szOS, bFailover, nScanTime, nQueue, nExpiry, returnObject);

	returnObject += SEPARATOR;

	return returnObject;
}

//////////////////////////////////////////////////////////////////////////////
//NAME:	Devs
//
//PURPOSE:	
//		Gets the Devices Being Used for Mining
//
//PARAMETERS:
//		 mapParams: Empty. Doesn't take any parameters
//
//RETURN:
//		json object containing the Mining Devices
//
//		Ex:
//
//		{
//			"DEVS": [
//
//				{
//					"GPU" : 0,
//					"Enabled" : true,
//					"Status" : "Alive",
//					"Temperature" : 68.50,
//					"Fan Speed" : 1728,
//					"Fan Percent" : 1728,
//					"GPU Clock" : 1100,
//					"Memory Clock" : 1500,
//					"GPU Voltage" : 1.100,
//					"GPU Activity" : 90,
//					"Powertune" : 50,
//					"MHS av" : 40.10,
//					"MHS 10s" : 42.32,
//					"Accepted" : 5,
//					"Rejected" : 1,
//					"Hardware Errors" : 0,
//					"Utility" : 0.02,
//					"Intensity" : 21,
//					"X Intensity" : 0,
//					"Raw Intensity" : 0,
//					"Total MH" : 0,
//					"Last Share Pool" : -1
//					"Last Share Pool Time" : 0,
//					"Diff1 Work" : 0,
//					"Difficulty Accepted" : 0,
//					"Difficulty Rejected" : 0,
//					"Last Share Difficulty" : 0,
//					"Last Valid Work" : 0,
//					"Device Hardware%" : 0,
//					"Device Rejected%" : 0,
//					"Device Elapsed" : 0
//				}
//			],
//			"STATUS":
//				{
//					"STATUS": "S",
//					"Msg": "Successfully made the API call to: devs",
//					"Code" : -1,
//					"When": 1422821846,
//					"Description": "1.0"
//				}
//		}
//
//		NOTE: GPU Pools aren't supported yet for CoinShield
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Devs(parameter_map mapParams)
{
	json_spirit::mObject returnObject;
	json_spirit::mArray devContainer;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				double hashes = connObj.at("Hashes").get_real();
				if (hashes > 0)
				{

					json_spirit::mArray devArr = connObj.at("Devices").get_array();

					for (size_t devInd = 0; devInd < devArr.size(); ++devInd)
					{
						json_spirit::mObject currDevObj = devArr[devInd].get_obj();

						json_spirit::mObject retDevObj;

						retDevObj["GPU"] = currDevObj.at("GPU").get_int();
						retDevObj["Enabled"] = currDevObj.at("Enabled").get_bool();
						retDevObj["Status"] = currDevObj.at("Alive").get_str();
						retDevObj["Temperature"] = currDevObj.at("Temperature").get_real();
						retDevObj["Fan Speed"] = currDevObj.at("Fan Speed").get_int();
						retDevObj["Fan Percent"] = currDevObj.at("Fan Percent").get_int();
						retDevObj["GPU Clock"] = currDevObj.at("GPU Clock").get_int();
						retDevObj["Memory Clock"] = currDevObj.at("Memory Clock").get_int();
						retDevObj["GPU Voltage"] = currDevObj.at("GPU Voltage").get_real();
						retDevObj["GPU Activity"] = currDevObj.at("GPU Activity").get_int();
						retDevObj["Powertune"] = currDevObj.at("Powertune").get_int();
						retDevObj["MHS av"] = currDevObj.at("MHS av").get_real();
						retDevObj["MHS 10s"] = currDevObj.at("MHS 10s").get_real();
						retDevObj["Accepted"] = currDevObj.at("Accepted").get_int();
						retDevObj["Rejected"] = currDevObj.at("Rejected").get_int();
						retDevObj["Hardware Errors"] = currDevObj.at("Hardware Errors").get_int();
						retDevObj["Utility"] = currDevObj.at("Utility").get_real();
						retDevObj["Intensity"] = currDevObj.at("Intensity").get_int();
						retDevObj["X Intensity"] = currDevObj.at("X Intensity").get_int();
						retDevObj["Raw Intensity"] = currDevObj.at("Raw Intensity").get_int();
						retDevObj["Total MH"] = currDevObj.at("Total MH").get_real();
						retDevObj["Last Share Pool"] = currDevObj.at("Last Share Pool").get_int();
						retDevObj["Last Share Pool Time"] = currDevObj.at("Last Share Pool Time").get_int();
						retDevObj["Diff1 Work"] = currDevObj.at("Diff1 Work").get_int();
						retDevObj["Difficulty Accepted"] = currDevObj.at("Difficulty Accepted").get_int();
						retDevObj["Difficulty Rejected"] = currDevObj.at("Difficulty Rejected").get_int();
						retDevObj["Last Share Difficulty"] = currDevObj.at("Last Share Difficulty").get_int();
						retDevObj["Last Valid Work"] = currDevObj.at("Last Valid Work").get_int();
						retDevObj["Device Hardware%"] = currDevObj.at("Device Hardware%").get_int();
						retDevObj["Device Rejected%"] = currDevObj.at("Device Rejected%").get_int();
						retDevObj["Device Elapsed"] = currDevObj.at("Device Elapsed").get_real();

						devContainer.push_back(retDevObj);
					}
				}
			}

			returnObject["DEVS"] = devContainer;

			SetupStatusObject(true, "devs", returnObject);
		}
		else
		{
			SetupStatusObject(false, "devs", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}

std::string APIHandler::Devs_STR(parameter_list listParams)
{
	std::string returnObject;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			SetupStatusObject_STR(true, 9, "%d GPU(s)", returnObject);
			
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				double hashes = connObj.at("Hashes").get_real();
				if (hashes > 0)
				{

					json_spirit::mArray devArr = connObj.at("Devices").get_array();

					for (size_t devInd = 0; devInd < devArr.size(); ++devInd)
					{
						json_spirit::mObject currDevObj = devArr[devInd].get_obj();

						returnObject += std::string("GPU") + EQUALS + std::to_string(currDevObj.at("GPU").get_int()) + COMMA;
						if (!currDevObj.at("Enabled").get_bool())
						{
							returnObject += std::string("Enabled") + EQUALS + std::string("true") + COMMA;
						}
						else
						{
							returnObject += std::string("Enabled") + EQUALS + std::string("false") + COMMA;
						}

						returnObject += std::string("Temperature") + EQUALS + std::to_string(currDevObj.at("Temperature").get_real()) + COMMA;
						returnObject += std::string("Fan Speed") + EQUALS + std::to_string(currDevObj.at("Fan Speed").get_int()) + COMMA;
						returnObject += std::string("Fan Percent") + EQUALS + std::to_string(currDevObj.at("Fan Percent").get_int()) + COMMA;
						returnObject += std::string("GPU Clock") + EQUALS + std::to_string(currDevObj.at("GPU Clock").get_int()) + COMMA;
						returnObject += std::string("Memory Clock") + EQUALS + std::to_string(currDevObj.at("Memory Clock").get_int()) + COMMA;
						returnObject += std::string("GPU Voltage") + EQUALS + std::to_string(currDevObj.at("GPU Voltage").get_real()) + COMMA;
						returnObject += std::string("GPU Activity") + EQUALS + std::to_string(currDevObj.at("GPU Activity").get_int()) + COMMA;
						returnObject += std::string("Powertune") + EQUALS + std::to_string(currDevObj.at("Powertune").get_int()) + COMMA;
						returnObject += std::string("MHS av") + EQUALS + std::to_string(currDevObj.at("MHS av").get_real()) + COMMA;
						returnObject += std::string("MHS 10s") + EQUALS + std::to_string(currDevObj.at("MHS 10s").get_real()) + COMMA;
						returnObject += std::string("Accepted") + EQUALS + std::to_string(currDevObj.at("Accepted").get_int()) + COMMA;
						returnObject += std::string("Rejected") + EQUALS + std::to_string(currDevObj.at("Rejected").get_int()) + COMMA;
						returnObject += std::string("Hardware Errors") + EQUALS + std::to_string(currDevObj.at("Hardware Errors").get_int()) + COMMA;
						returnObject += std::string("Utility") + EQUALS + std::to_string(currDevObj.at("Utility").get_real()) + COMMA;
						returnObject += std::string("Intensity") + EQUALS + std::to_string(currDevObj.at("Intensity").get_int()) + COMMA;
						returnObject += std::string("X Intensity") + EQUALS + std::to_string(currDevObj.at("X Intensity").get_int()) + COMMA;
						returnObject += std::string("Raw Intensity") + EQUALS + std::to_string(currDevObj.at("Raw Intensity").get_int()) + COMMA;
						returnObject += std::string("Total MH") + EQUALS + std::to_string(currDevObj.at("Total MH").get_real()) + COMMA;
						returnObject += std::string("Last Share Pool") + EQUALS + std::to_string(currDevObj.at("Last Share Pool").get_int()) + COMMA;
						returnObject += std::string("Last Share Pool Time") + EQUALS + std::to_string(currDevObj.at("Last Share Pool Time").get_int()) + COMMA;
						returnObject += std::string("Diff1 Work") + EQUALS + std::to_string(currDevObj.at("Diff1 Work").get_int()) + COMMA;
						returnObject += std::string("Difficulty Accepted") + EQUALS + std::to_string(currDevObj.at("Difficulty Accepted").get_int()) + COMMA;
						returnObject += std::string("Difficulty Rejected") + EQUALS + std::to_string(currDevObj.at("Difficulty Rejected").get_int()) + COMMA;
						returnObject += std::string("Last Share Difficulty") + EQUALS + std::to_string(currDevObj.at("Last Share Difficulty").get_int()) + COMMA;
						returnObject += std::string("Last Valid Work") + EQUALS + std::to_string(currDevObj.at("Last Valid Work").get_int()) + COMMA;
						returnObject += std::string("Device Hardware%") + EQUALS + std::to_string(currDevObj.at("Device Hardware%").get_int()) + COMMA;
						returnObject += std::string("Device Rejected%") + EQUALS + std::to_string(currDevObj.at("Device Rejected%").get_int()) + COMMA;
						returnObject += std::string("Device Elapsed") + EQUALS + std::to_string(currDevObj.at("Device Elapsed").get_real()) + SEPARATOR;
					}
				}
			}
		}
		else
		{
			SetupStatusObject_STR(false, 9, "%d GPU(s)", returnObject);
		}
	}
	m_dataLock->unlock();


	return returnObject;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	Pools
//
//Purpose:	
//		Gets information about all of the pools
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters
//
//Return:
//		json object containing the Mining Devices
//
//		Ex:
//
//		{
//			"POOLS": [
//
//				{
//					"POOL": 0,
//					"URL": "127.0.0.1:9325",
//					"Status": "Alive",
//					"Priority": 1,
//					"Quota": 0,
//					"Long Poll": "No",
//					"Getworks": 0,
//					"Accepted": 3,
//					"Rejected": 0,
//					"Works": 0,
//					"Discarded": 0,
//					"Stale": 0,
//					"Get Failures": 0,
//					"Remote Failures": 0,
//					"User": "x",
//					"Last Share Time": 0,
//					"Diff1 Shares": 0,
//					"Proxy Type": "",
//					"Proxy": "",
//					"Difficulty Accepted": 0,
//					"Difficulty Rejected": 0,
//					"Difficulty Stale": 0,
//					"Last Share Difficulty": 0,
//					"Has Stratum": false,
//					"Stratum Active": false,
//					"Stratum URL": "",
//					"Has GBT": false,
//					"Best Share": 0,
//					"Pool Rejected%": 0,
//					"Pool Stale%": 0
//				}
//			],
//			"STATUS":
//				{
//					"STATUS": "S",
//					"Msg": "Successfully made the API call to: pools",
//					"Code" : -1,
//					"When": 1422821846,
//					"Description": "1.0"
//				}
//		}
//
//		NOTE:	GPU Pools aren't supported yet for CoinShield. 
//				Just use Connections
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Pools(parameter_map mapParams)
{
	json_spirit::mObject returnObject;
	json_spirit::mArray poolArr;
	

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();

				json_spirit::mObject poolsObject;

				poolsObject["POOL"] = connObj.at("ID").get_int();
				poolsObject["URL"] = connObj.at("URL").get_str();
				poolsObject["Status"] = connObj.at("Status").get_str();
				poolsObject["Priority"] = connObj.at("Priority").get_int();
				poolsObject["Quota"] = connObj.at("Quota").get_int();
				poolsObject["Long Poll"] = connObj.at("Long Poll").get_str();
				poolsObject["Getworks"] = connObj.at("Getworks").get_int();
				poolsObject["Accepted"] = connObj.at("Accepted").get_int();
				poolsObject["Rejected"] = connObj.at("Rejected").get_int();
				poolsObject["Works"] = connObj.at("Works").get_int();
				poolsObject["Discarded"] = connObj.at("Discarded").get_int();
				poolsObject["Stale"] = connObj.at("Stale").get_int();
				poolsObject["Get Failures"] = connObj.at("Get Failures").get_int();
				poolsObject["Remote Failures"] = connObj.at("Remote Failures").get_int();
				poolsObject["User"] = connObj.at("User").get_str();
				poolsObject["Last Share Time"] = connObj.at("Last Share Time").get_int();
				poolsObject["Diff1 Shares"] = connObj.at("Diff1 Shares").get_int();
				poolsObject["Proxy Type"] = connObj.at("Proxy Type").get_str();
				poolsObject["Proxy"] = connObj.at("Proxy").get_str();
				poolsObject["Difficulty Accepted"] = connObj.at("Difficulty Accepted").get_int();
				poolsObject["Difficulty Rejected"] = connObj.at("Difficulty Rejected").get_int();
				poolsObject["Difficulty Stale"] = connObj.at("Difficulty Stale").get_int();
				poolsObject["Last Share Difficulty"] = connObj.at("Last Share Difficulty").get_int();
				poolsObject["Has Stratum"] = connObj.at("Has Stratum").get_bool();
				poolsObject["Stratum Active"] = connObj.at("Stratum Active").get_bool();
				poolsObject["Stratum URL"] = connObj.at("Stratum URL").get_str();
				poolsObject["Has GBT"] = connObj.at("Has GBT").get_bool();
				poolsObject["Best Share"] = connObj.at("Best Share").get_int();
				poolsObject["Pool Rejected%"] = connObj.at("Pool Rejected%").get_real();
				poolsObject["Pool Stale%"] = connObj.at("Pool Stale%").get_real();

				poolArr.push_back(poolsObject);
			}

			returnObject["POOLS"] = poolArr;
			SetupStatusObject(true, "pools", returnObject);
		}
		else
		{
			SetupStatusObject(false, "pools", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}

std::string APIHandler::Pools_STR(parameter_list listParams)
{
	std::string returnObject;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			SetupStatusObject_STR(true,  7, "%d Pool(s)", returnObject);

			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();


			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();

				returnObject += std::string("POOL") + EQUALS + std::to_string(connObj.at("ID").get_int()) + COMMA;
				returnObject += std::string("URL") + EQUALS + connObj.at("URL").get_str() + COMMA;
				returnObject += std::string("Status") + EQUALS + connObj.at("Status").get_str() + COMMA;
				returnObject += std::string("Priority") + EQUALS + std::to_string(connObj.at("Priority").get_int()) + COMMA;
				returnObject += std::string("Quota") + EQUALS + std::to_string(connObj.at("Quota").get_int()) + COMMA;
				returnObject += std::string("Long Poll") + EQUALS + connObj.at("Long Poll").get_str() + COMMA;
				returnObject += std::string("Getworks") + EQUALS + std::to_string(connObj.at("Getworks").get_int()) + COMMA;
				returnObject += std::string("Accepted") + EQUALS + std::to_string(connObj.at("Accepted").get_int()) + COMMA;
				returnObject += std::string("Rejected") + EQUALS + std::to_string(connObj.at("Rejected").get_int()) + COMMA;
				returnObject += std::string("Works") + EQUALS + std::to_string(connObj.at("Works").get_int()) + COMMA;
				returnObject += std::string("Discarded") + EQUALS + std::to_string(connObj.at("Discarded").get_int()) + COMMA;
				returnObject += std::string("Stale") + EQUALS + std::to_string(connObj.at("Stale").get_int()) + COMMA;
				returnObject += std::string("Get Failures") + EQUALS + std::to_string(connObj.at("Get Failures").get_int()) + COMMA;
				returnObject += std::string("Remote Failures") + EQUALS + std::to_string(connObj.at("Remote Failures").get_int()) + COMMA;
				returnObject += std::string("User") + EQUALS + connObj.at("User").get_str() + COMMA;
				returnObject += std::string("Last Share Time") + EQUALS + std::to_string(connObj.at("Last Share Time").get_int()) + COMMA;
				returnObject += std::string("Diff1 Shares") + EQUALS + std::to_string(connObj.at("Diff1 Shares").get_int()) + COMMA;
				returnObject += std::string("Proxy Type") + EQUALS + connObj.at("Proxy Type").get_str() + COMMA;
				returnObject += std::string("Proxy") + EQUALS + connObj.at("Proxy").get_str() + COMMA;
				returnObject += std::string("Difficulty Accepted") + EQUALS + std::to_string(connObj.at("Difficulty Accepted").get_int()) + COMMA;
				returnObject += std::string("Difficulty Rejected") + EQUALS + std::to_string(connObj.at("Difficulty Rejected").get_int()) + COMMA;
				returnObject += std::string("Difficulty Stale") + EQUALS + std::to_string(connObj.at("Difficulty Stale").get_int()) + COMMA;
				returnObject += std::string("Last Share Difficulty") + EQUALS + std::to_string(connObj.at("Last Share Difficulty").get_int()) + COMMA;
				returnObject += std::string("Has Stratum") + EQUALS + std::to_string(connObj.at("Has Stratum").get_bool()) + COMMA;
				returnObject += std::string("Stratum Active") + EQUALS + std::to_string(connObj.at("Stratum Active").get_bool()) + COMMA;
				returnObject += std::string("Stratum URL") + EQUALS + connObj.at("Stratum URL").get_str() + COMMA;
				returnObject += std::string("Has GBT") + EQUALS + std::to_string(connObj.at("Has GBT").get_bool()) + COMMA;
				returnObject += std::string("Best Share") + EQUALS + std::to_string(connObj.at("Best Share").get_int()) + COMMA;
				returnObject += std::string("Pool Rejected%") + EQUALS + std::to_string(connObj.at("Pool Rejected%").get_real()) + COMMA;

				returnObject += std::string("Pool Stale%") + EQUALS + std::to_string(connObj.at("Pool Stale%").get_real()) + SEPARATOR;
			}
		}
		else
		{
			SetupStatusObject_STR(false, 7, "%d Pool(s)", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	Summary
//
//Purpose:	
//		Gets a summary of the mining stats
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters
//
//Return:
//
//		json object containing a Summary of the Mining Statistics
//
//		Ex:
//
//			{
//				"Elapsed": 7200,
//				"MHS av": 40.00,
//				"Accepted": 3,
//				"Rejected": 1,
//				"Hardware Errors": 0,
//				"Work Utility": 0.02,
//				"STATUS":
//				{
//					"STATUS": "S",
//					"Msg": "Successfully made the API call to: summary",
//					"Code" : -1,
//					"When": 1422821846,
//					"Description": "1.0"
//				}
//			}
//
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Summary(parameter_map mapParams)
{
	uint32 totalKHS = 0;
	uint32 totalAccepted = 0;
	uint32 totalRejected = 0;
	uint32 totalHardwareErrors = 0;
	uint32 totalMS = 0;
	double totalKHS10s = 0.0;
	double totalRolling = 0.0;
	int32 newBlocks = 0;


	json_spirit::mObject returnObject;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();


			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();

				int nHashes = connObj.at("Hashes").get_real();
				if (nHashes > 0)
				{
					if (srvIndex == 0)
					{
						totalMS = connObj.at("Time").get_int();
						newBlocks = connObj.at("New Blocks").get_int();
					}

					totalKHS += nHashes;
					totalRolling += connObj.at("Rolling").get_real();

					totalAccepted += connObj.at("Accepted").get_int();
					totalRejected += connObj.at("Rejected").get_int();
					totalHardwareErrors += connObj.at("Hardware Errors").get_int();

					json_spirit::mArray devArr = connObj.at("Devices").get_array();

					for (size_t thrdIndex = 0; thrdIndex < devArr.size(); ++thrdIndex)
					{
						json_spirit::mObject devObj = devArr[thrdIndex].get_obj();
						totalKHS10s += devObj.at("Last Hash Rate").get_real();
					}
				}
			}

			double totalSeconds = ((double)totalMS) * 0.001;

			double totalMHS = totalKHS * 0.001;

			//requires difficulty
			double fWorkUtility = ((double)totalAccepted) / (totalSeconds * 60.0f);

			double fUtility = ((double)totalAccepted) / (totalSeconds * 60.0f);

			//float mhsAvg = totalMHS / ((double)totalSeconds);

			json_spirit::mObject summaryObject;
			summaryObject["Elapsed"] = totalSeconds;
			summaryObject["MHS av"] = totalMHS;
			summaryObject["MHS 10s"] = totalMHS;
			summaryObject["Found Blocks"] = (int32)totalAccepted;
			summaryObject["Accepted"] = (int32)totalAccepted;
			summaryObject["Rejected"] = (int32)totalRejected;
			summaryObject["Hardware Errors"] = (int32)totalHardwareErrors;
			summaryObject["Utility"] = fUtility;
			summaryObject["Discarded"] = 0;
			summaryObject["Stale"] = 0;
			summaryObject["Get Failures"] = 0;
			summaryObject["Local Work"] = 0;
			summaryObject["Remote Failures"] = 0;
			summaryObject["Network Blocks"] = newBlocks;
			summaryObject["Total MH"] = (int32)totalMHS;
			summaryObject["Work Utility"] = fWorkUtility;
			summaryObject["Difficulty Accepted"] = (int32)totalAccepted;
			summaryObject["Difficulty Rejected"] = (int32)totalRejected;
			summaryObject["Difficulty Stale"] = 0;
			summaryObject["Best Share"] = 0;
			summaryObject["Device Hardware%"] = 0;
			summaryObject["Device Rejected%"] = 0;
			summaryObject["Pool Rejected%"] = 0;
			summaryObject["Pool Stale%"] = 0;

			returnObject["SUMMARY"] = summaryObject;

			SetupStatusObject(true, "summary", returnObject);
		}
		else
		{
			SetupStatusObject(false, "summary", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}

std::string APIHandler::Summary_STR(parameter_list listParams)
{
	std::string returnObject;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			SetupStatusObject_STR(true, 11, "Summary", returnObject);

			uint32 totalKHS = 0;
			uint32 totalAccepted = 0;
			uint32 totalRejected = 0;
			uint32 totalHardwareErrors = 0;
			uint32 totalMS = 0;
			double totalKHS10s = 0.0;
			double totalRolling = 0.0;
			int32 newBlocks = 0;

			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();

				int nHashes = connObj.at("Hashes").get_real();
				if (nHashes > 0)
				{
					if (srvIndex == 0)
					{
						totalMS = connObj.at("Time").get_int();
						newBlocks = connObj.at("New Blocks").get_int();
					}

					totalKHS += nHashes;
					totalRolling += connObj.at("Rolling").get_real();

					totalAccepted += connObj.at("Accepted").get_int();
					totalRejected += connObj.at("Rejected").get_int();
					totalHardwareErrors += connObj.at("Hardware Errors").get_int();

					json_spirit::mArray devArr = connObj.at("Devices").get_array();

					for (size_t thrdIndex = 0; thrdIndex < devArr.size(); ++thrdIndex)
					{
						json_spirit::mObject devObj = devArr[thrdIndex].get_obj();
						totalKHS10s += devObj.at("Last Hash Rate").get_real();
					}
				}
			}

			double totalSeconds = ((double)totalMS) * 0.001;

			//double totalMHS = totalKHS * 0.001;

			//requires difficulty
			double fWorkUtility = ((double)totalAccepted) / (totalSeconds * 60.0f);

			double fUtility = ((double)totalAccepted) / (totalSeconds * 60.0f);

			double totalMHS = totalKHS * 0.001;

			returnObject += std::string("Elapsed") + EQUALS + std::to_string(totalSeconds) + COMMA;
			returnObject += std::string("MHS av") + EQUALS + std::to_string(totalMHS) + COMMA;
			returnObject += std::string("MHS 10s") + EQUALS + std::to_string(totalMHS) + COMMA;
			returnObject += std::string("Found Blocks") + EQUALS + std::to_string(totalAccepted) + COMMA;
			returnObject += std::string("Accepted") + EQUALS + std::to_string(totalAccepted) + COMMA;
			returnObject += std::string("Rejected") + EQUALS + std::to_string(totalRejected) + COMMA;
			returnObject += std::string("Hardware Errors") + EQUALS + std::to_string(totalHardwareErrors) + COMMA;
			returnObject += std::string("Utility") + EQUALS + std::to_string(fUtility) + COMMA;
			returnObject += std::string("Discarded") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Stale") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Get Failures") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Local Work") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Remote Failures") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Network Blocks") + EQUALS + std::to_string(newBlocks) + COMMA;
			returnObject += std::string("Total MH") + EQUALS + std::to_string(totalMHS) + COMMA;
			returnObject += std::string("Work Utility") + EQUALS + std::to_string(fWorkUtility) + COMMA;
			returnObject += std::string("Difficulty Accepted") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Difficulty Rejected") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Difficulty Stale") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Best Share") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Device Hardware%") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Device Rejected%") + EQUALS + std::string("0") + COMMA;
			returnObject += std::string("Pool Rejected%") + EQUALS + std::string("0") + COMMA;

			returnObject += std::string("Pool Stale%") + EQUALS + std::string("0") + SEPARATOR;
		}
		else
		{
			SetupStatusObject_STR(false, 11, "Summary", returnObject);
		}
	}
	m_dataLock->unlock();
	
	return returnObject;
}


bool APIHandler::EnableGPU(int32 nGPUNum)
{
	EventManager::GetInstance()->AddEvent(Event::ENABLE_GPU, ToVoid<int>(nGPUNum));
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	GPUEnable
//
//Purpose:	
//		Enable the specified GPU and setup it's performance settings
//
//Parameters:
//		 mapParams: 
//			"GPU": The device index of the gpu to enable
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpuenable",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUEnable(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "gpuenable", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("GPU");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "gpuenable", returnObject);
		return returnObject;
	}

	std::string gpuNum = iter->second;
	int32 nGPUNum = atoi(gpuNum.c_str());

	bool bSuccess = EnableGPU(nGPUNum);
	SetupStatusObject(bSuccess, "gpuenable", returnObject);

	return returnObject;
}

std::string APIHandler::GPUEnable_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 5, "GPU %d sent enable message", returnObject);
		return returnObject;
	}

	std::string szGPUNum = listParams[0];
	if (szGPUNum.empty())
	{
		SetupStatusObject_STR(false, 5, "GPU %d sent enable message", returnObject);
		return returnObject;
	}

	int32 nGPUNum = atoi(szGPUNum.c_str());
	bool bSuccess = EnableGPU(nGPUNum);
	SetupStatusObject_STR(bSuccess, 5, std::string("GPU ") + szGPUNum + std::string(" sent enable message"), returnObject);

	return returnObject;
}


bool APIHandler::DiableGPU(int32 nGPUNum)
{
	EventManager::GetInstance()->AddEvent(Event::DISABLE_GPU, ToVoid<int>(nGPUNum));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	GPUDisable
//
//Purpose:	
//		Disable the specified GPU and set it's performance settings to default
//
//Parameters:
//		 mapParams: 
//			"GPU": The device index of the gpu to disnable
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpudisable",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUDisable(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "gpudisable", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("GPU");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "gpudisable", returnObject);
		return returnObject;
	}

	std::string gpuNum = iter->second;
	int32 nGPUNum = atoi(gpuNum.c_str());

	bool bSucces = DiableGPU(nGPUNum);
	SetupStatusObject(bSucces, "gpudisable", returnObject);

	return returnObject;
}

std::string APIHandler::GPUDisable_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 12, "GPU %d set disable flag", returnObject);
		return returnObject;
	}

	std::string szGPUNum = listParams[0];
	if (szGPUNum.empty())
	{
		SetupStatusObject_STR(false, 12, "GPU %d set disable flag", returnObject);
		return returnObject;
	}

	int32 nGPUNum = atoi(szGPUNum.c_str());
	bool bSucces = DiableGPU(nGPUNum);
	SetupStatusObject_STR(bSucces, 12, std::string("GPU ") + szGPUNum + std::string(" set disable flag"), returnObject);

	return returnObject;
}


bool APIHandler::RestartGPU(int32 nGPUNum)
{
	EventManager::GetInstance()->AddEvent(Event::RESTART_GPU, ToVoid<int>(nGPUNum));
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	GPURestart
//
//Purpose:	
//		Restart the specified GPU
//
//Parameters:
//		 mapParams: 
//			"GPU": The device index of the gpu to restart
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpurestart",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPURestart(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "gpurestart", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("GPU");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "gpurestart", returnObject);
		return returnObject;
	}

	std::string gpuNum = iter->second;
	int32 nGPUNum = atoi(gpuNum.c_str());

	bool bSuccess = RestartGPU(nGPUNum);
	SetupStatusObject(bSuccess, "gpurestart", returnObject);

	return returnObject;
}

std::string APIHandler::GPURestart_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 13, "GPU %d restart attempted", returnObject);
		return returnObject;
	}

	std::string szGPUNum = listParams[0];
	if (szGPUNum.empty())
	{
		SetupStatusObject_STR(false, 13, "GPU %d restart attempted", returnObject);
		return returnObject;
	}

	int32 nGPUNum = atoi(szGPUNum.c_str());

	bool bSuccess = RestartGPU(nGPUNum);
	SetupStatusObject_STR(bSuccess, 13, std::string("GPU ") + szGPUNum + std::string(" restart attempted"), returnObject);

	return returnObject;
}


//////////////////////////////////////////////////////////////////////////////
//Name:	GPU
//
//Purpose:	
//		Gets all of the Stats for the specified GPU
//
//Parameters:
//		 mapParams: 
//			"GPU": The device index of the gpu to get the stats for.
//Return:
//
//			json object containg the information for the specified GPU
//
//				{
//					"GPU" : 0,
//					"Enabled" : true,
//					"Status" : "Alive",
//					"Temperature" : 68.50,
//					"Fan Speed" : 1728,
//					"Fan Percent" : 1728,
//					"GPU Clock" : 1100,
//					"Memory Clock" : 1500,
//					"GPU Voltage" : 1.100,
//					"GPU Activity" : 90,
//					"Powertune" : 50,
//					"MHS av" : 40.10,
//					"MHS 10s" : 42.32,
//					"Accepted" : 5,
//					"Rejected" : 1,
//					"Hardware Errors" : 0,
//					"Utility" : 0.02,
//					"Intensity" : 21,
//					"X Intensity" : 0,
//					"Raw Intensity" : 0,
//					"Total MH" : 0,
//					"Last Share Pool" : -1
//					"Last Share Pool Time" : 0,
//					"Diff1 Work" : 0,
//					"Difficulty Accepted" : 0,
//					"Difficulty Rejected" : 0,
//					"Last Share Difficulty" : 0,
//					"Last Valid Work" : 0,
//					"Device Hardware%" : 0,
//					"Device Rejected%" : 0,
//					"Device Elapsed" : 0,
//					"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpu",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//				}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPU(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "gpu", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("GPU");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "gpu", returnObject);
		return returnObject;
	}

	std::string gpuNum = iter->second;
	int32 nGPUNum = atoi(gpuNum.c_str());

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();
			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			if (connArr.size() > 0)
			{
				json_spirit::mObject connObj = connArr[0].get_obj();
				json_spirit::mArray devArr = connObj.at("Devices").get_array();

				if (devArr.size() >= nGPUNum)
				{

					json_spirit::mObject currDevObj = devArr[nGPUNum].get_obj();
					json_spirit::mObject retDevObj;

					retDevObj["GPU"] = currDevObj.at("GPU").get_int();
					retDevObj["Enabled"] = currDevObj.at("Enabled").get_bool();
					retDevObj["Status"] = currDevObj.at("Alive").get_str();
					retDevObj["Temperature"] = currDevObj.at("Temperature").get_real();
					retDevObj["Fan Speed"] = currDevObj.at("Fan Speed").get_int();
					retDevObj["Fan Percent"] = currDevObj.at("Fan Percent").get_int();
					retDevObj["GPU Clock"] = currDevObj.at("GPU Clock").get_int();
					retDevObj["Memory Clock"] = currDevObj.at("Memory Clock").get_int();
					retDevObj["GPU Voltage"] = currDevObj.at("GPU Voltage").get_real();
					retDevObj["GPU Activity"] = currDevObj.at("GPU Activity").get_int();
					retDevObj["Powertune"] = currDevObj.at("Powertune").get_int();
					retDevObj["MHS av"] = currDevObj.at("MHS av").get_real();
					retDevObj["MHS 10s"] = currDevObj.at("MHS 10s").get_real();
					retDevObj["Accepted"] = currDevObj.at("Accepted").get_int();
					retDevObj["Rejected"] = currDevObj.at("Rejected").get_int();
					retDevObj["Hardware Errors"] = currDevObj.at("Hardware Errors").get_int();
					retDevObj["Utility"] = currDevObj.at("Utility").get_real();
					retDevObj["Intensity"] = currDevObj.at("Intensity").get_int();
					retDevObj["X Intensity"] = currDevObj.at("X Intensity").get_int();
					retDevObj["Raw Intensity"] = currDevObj.at("Raw Intensity").get_int();
					retDevObj["Total MH"] = currDevObj.at("Total MH").get_real();
					retDevObj["Last Share Pool"] = currDevObj.at("Last Share Pool").get_int();
					retDevObj["Last Share Pool Time"] = currDevObj.at("Last Share Pool Time").get_int();
					retDevObj["Diff1 Work"] = currDevObj.at("Diff1 Work").get_int();
					retDevObj["Difficulty Accepted"] = currDevObj.at("Difficulty Accepted").get_int();
					retDevObj["Difficulty Rejected"] = currDevObj.at("Difficulty Rejected").get_int();
					retDevObj["Last Share Difficulty"] = currDevObj.at("Last Share Difficulty").get_int();
					retDevObj["Last Valid Work"] = currDevObj.at("Last Valid Work").get_int();
					retDevObj["Device Hardware%"] = currDevObj.at("Device Hardware%").get_int();
					retDevObj["Device Rejected%"] = currDevObj.at("Device Rejected%").get_int();
					retDevObj["Device Elapsed"] = currDevObj.at("Device Elapsed").get_real();

					returnObject = retDevObj;
					SetupStatusObject(true, "gpu", returnObject);
				}
				else
				{
					SetupStatusObject(false, "gpu", returnObject);
				}
			}
			else
			{
				SetupStatusObject(false, "gpu", returnObject);
			}
		}
		else
		{
			SetupStatusObject(false, "gpu", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}

std::string APIHandler::GPU_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 17, "GPU%d", returnObject);
		return returnObject;
	}

	std::string szGPUNum = listParams[0];
	if (szGPUNum.empty())
	{
		SetupStatusObject_STR(false, 17, "GPU%d", returnObject);
		return returnObject;
	}

	int32 nGPUNum = atoi(szGPUNum.c_str());


	m_dataLock->lock();
	{
		if (m_appData)
		{
			SetupStatusObject_STR(true, 17, std::string("GPU") + szGPUNum, returnObject);

			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();
			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			if (connArr.size() > 0)
			{
				json_spirit::mObject connObj = connArr[0].get_obj();
				json_spirit::mArray devArr = connObj.at("Devices").get_array();

				if (devArr.size() >= nGPUNum)
				{
					json_spirit::mObject currDevObj = devArr[nGPUNum].get_obj();

					returnObject += std::string("GPU") + EQUALS + std::to_string(currDevObj.at("GPU").get_int()) + COMMA;
					if (!currDevObj.at("Enabled").get_bool())
					{
						returnObject += std::string("Enabled") + EQUALS + std::string("true") + COMMA;
					}
					else
					{
						returnObject += std::string("Enabled") + EQUALS + std::string("false") + COMMA;
					}

					returnObject += std::string("Temperature") + EQUALS + std::to_string(currDevObj.at("Temperature").get_real()) + COMMA;
					returnObject += std::string("Fan Speed") + EQUALS + std::to_string(currDevObj.at("Fan Speed").get_int()) + COMMA;
					returnObject += std::string("Fan Percent") + EQUALS + std::to_string(currDevObj.at("Fan Percent").get_int()) + COMMA;
					returnObject += std::string("GPU Clock") + EQUALS + std::to_string(currDevObj.at("GPU Clock").get_int()) + COMMA;
					returnObject += std::string("Memory Clock") + EQUALS + std::to_string(currDevObj.at("Memory Clock").get_int()) + COMMA;
					returnObject += std::string("GPU Voltage") + EQUALS + std::to_string(currDevObj.at("GPU Voltage").get_real()) + COMMA;
					returnObject += std::string("GPU Activity") + EQUALS + std::to_string(currDevObj.at("GPU Activity").get_int()) + COMMA;
					returnObject += std::string("Powertune") + EQUALS + std::to_string(currDevObj.at("Powertune").get_int()) + COMMA;
					returnObject += std::string("MHS av") + EQUALS + std::to_string(currDevObj.at("MHS av").get_real()) + COMMA;
					returnObject += std::string("MHS 10s") + EQUALS + std::to_string(currDevObj.at("MHS 10s").get_real()) + COMMA;
					returnObject += std::string("Accepted") + EQUALS + std::to_string(currDevObj.at("Accepted").get_int()) + COMMA;
					returnObject += std::string("Rejected") + EQUALS + std::to_string(currDevObj.at("Rejected").get_int()) + COMMA;
					returnObject += std::string("Hardware Errors") + EQUALS + std::to_string(currDevObj.at("Hardware Errors").get_int()) + COMMA;
					returnObject += std::string("Utility") + EQUALS + std::to_string(currDevObj.at("Utility").get_real()) + COMMA;
					returnObject += std::string("Intensity") + EQUALS + std::to_string(currDevObj.at("Intensity").get_int()) + COMMA;
					returnObject += std::string("X Intensity") + EQUALS + std::to_string(currDevObj.at("X Intensity").get_int()) + COMMA;
					returnObject += std::string("Raw Intensity") + EQUALS + std::to_string(currDevObj.at("Raw Intensity").get_int()) + COMMA;
					returnObject += std::string("Total MH") + EQUALS + std::to_string(currDevObj.at("Total MH").get_real()) + COMMA;
					returnObject += std::string("Last Share Pool") + EQUALS + std::to_string(currDevObj.at("Last Share Pool").get_int()) + COMMA;
					returnObject += std::string("Last Share Pool Time") + EQUALS + std::to_string(currDevObj.at("Last Share Pool Time").get_int()) + COMMA;
					returnObject += std::string("Diff1 Work") + EQUALS + std::to_string(currDevObj.at("Diff1 Work").get_int()) + COMMA;
					returnObject += std::string("Difficulty Accepted") + EQUALS + std::to_string(currDevObj.at("Difficulty Accepted").get_int()) + COMMA;
					returnObject += std::string("Difficulty Rejected") + EQUALS + std::to_string(currDevObj.at("Difficulty Rejected").get_int()) + COMMA;
					returnObject += std::string("Last Share Difficulty") + EQUALS + std::to_string(currDevObj.at("Last Share Difficulty").get_int()) + COMMA;
					returnObject += std::string("Last Valid Work") + EQUALS + std::to_string(currDevObj.at("Last Valid Work").get_int()) + COMMA;
					returnObject += std::string("Device Hardware%") + EQUALS + std::to_string(currDevObj.at("Device Hardware%").get_int()) + COMMA;
					returnObject += std::string("Device Rejected%") + EQUALS + std::to_string(currDevObj.at("Device Rejected%").get_int()) + COMMA;
					returnObject += std::string("Device Elapsed") + EQUALS + std::to_string(currDevObj.at("Device Elapsed").get_real()) + SEPARATOR;
				}
				else
				{
					SetupStatusObject_STR(false, 17, std::string("GPU") + szGPUNum, returnObject);
				}
			}
			else
			{
				SetupStatusObject_STR(false, 17, std::string("GPU") + szGPUNum, returnObject);
			}
		}
		else
		{
			SetupStatusObject_STR(false, 17, std::string("GPU") + szGPUNum, returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}

int32 APIHandler::GetGPUCount()
{
	int32 numDevices = 0;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				json_spirit::mArray devArr = connObj.at("Devices").get_array();

				numDevices += devArr.size();
			}
		}
	}
	m_dataLock->unlock();

	return numDevices;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	GPUCount
//
//Purpose:	
//		Gets the number of GPUs being used for mining
//
//Parameters:
//		 mapParams: Empty. No parameters used
//
//Return:
//
//			json object containg the information for number of GPUs int the system
//
//				{
//					"Count" : 1,
//					"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpucount",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//				}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUCount(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	returnObject["Count"] = GetGPUCount();

	SetupStatusObject(true, "gpucount", returnObject);

	return returnObject;
}

std::string APIHandler::GPUCount_STR(parameter_list listParams)
{
	std::string returnObject;

	SetupStatusObject_STR(true, 33, "CGMiner config", returnObject);

	returnObject += std::string("Count") + EQUALS + std::to_string(GetGPUCount()) + SEPARATOR;

	return returnObject;
}


bool APIHandler::SwitchMiningPool(int32 nPoolNum)
{
	EventManager::GetInstance()->AddEvent(Event::SWITCH_POOL, ToVoid<int>(nPoolNum));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	SwitchPool
//
//Purpose:	
//		Switch to the specified pool
//
//Parameters:
//		 mapParams: 
//			"POOL": The index of the pool to switch to
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: switchpool",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::SwitchPool(parameter_map mapParams)
{
	json_spirit::mObject returnObject;


	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "switchpool", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("POOL");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "switchpool", returnObject);
		return returnObject;
	}

	std::string szPoolNum = iter->second;
	int32 nPoolNum = atoi(szPoolNum.c_str());

	bool bSuccess = SwitchMiningPool(nPoolNum);

	SetupStatusObject(bSuccess, "switchpool", returnObject);

	return returnObject;
}


std::string APIHandler::SwitchPool_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 27, "Switching to pool %d:'%s'", returnObject);
		return returnObject;
	}

	int32 nPoolNum = atoi(listParams[0].c_str());

	bool bSuccess = SwitchMiningPool(nPoolNum);

	SetupStatusObject_STR(bSuccess, 27, std::string("Switching to pool '") + std::to_string(nPoolNum) + std::string("'"), returnObject);

	return returnObject;
}


bool APIHandler::AddMiningPool(std::string szURL, std::string szUser, std::string szPassword)
{
	EventManager::GetInstance()->AddEvent(Event::ADD_POOL, ToVoid<std::string>(szURL), ToVoid<std::string>(szUser), ToVoid<std::string>(szPassword));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	AddPool
//
//Purpose:	
//		Add the specified pool to the connections
//
//Parameters:
//		 mapParams: 
//			"POOL": The comma separated pool string
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: addpool",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::AddPool(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "addpool", returnObject);
		return returnObject;
	}

	std::string szURL = mapParams["Url"];
	std::string szUser = mapParams["User"];
	std::string szPassword = mapParams["Password"];

	bool bSuccess = AddMiningPool(szURL, szUser, szPassword);

	SetupStatusObject(bSuccess, "addpool", returnObject);

	return returnObject;
}

std::string APIHandler::AddPool_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 55, "Added pool '%s'", returnObject);
		return returnObject;
	}

	std::string szURL = listParams[0];
	std::string szUser = listParams[1];
	std::string szPassword = listParams[2];

	bool bSuccess = AddMiningPool(szURL, szUser, szPassword);

	SetupStatusObject_STR(bSuccess, 55, std::string("Added pool '") + szURL + std::string("'"), returnObject);

	return returnObject;
}


bool APIHandler::SetPoolPriority(std::vector<int32> vecPoolIDs)
{
	EventManager::GetInstance()->AddEvent(Event::SET_POOL_PRIORITY, ToVoid<std::vector<int32>>(vecPoolIDs));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	PoolPriority
//
//Purpose:	
//		Add the specified pool to the connections
//
//Parameters:
//		 mapParams: 
//			"POOLS": The comma separated pool ids
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: poolpriority",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::PoolPriority(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "poolpriority", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("Pools");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "poolpriority", returnObject);
		return returnObject;
	}

	std::string szPoolInfo = iter->second;
	if (szPoolInfo.empty())
	{
		SetupStatusObject(false, "poolpriority", returnObject);
		return returnObject;
	}

	std::vector<std::string> poolParams = split(szPoolInfo, ',');
	if (poolParams.size() == 0)
	{
		SetupStatusObject(false, "poolpriority", returnObject);
		return returnObject;
	}

	std::vector<int32> poolList;
	for (size_t poolIndex = 0; poolIndex < poolParams.size(); ++poolIndex)
	{
		std::string szPoolID = poolParams[poolIndex];
		int32 nPoolID = atoi(szPoolID.c_str());
		poolList.push_back(nPoolID);
	}

	//NOT IMPLEMENTED YET
	bool bSuccess = SetPoolPriority(poolList);

	SetupStatusObject(bSuccess, "poolpriority", returnObject);


	return returnObject;
}


std::string APIHandler::PoolPriority_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 73, "Changed pool priorities", returnObject);
		return returnObject;
	}

	std::vector<int32> poolList;
	for (size_t poolIndex = 0; poolIndex < listParams.size(); ++poolIndex)
	{
		std::string szPoolID = listParams[poolIndex];
		int32 nPoolID = atoi(szPoolID.c_str());
		poolList.push_back(nPoolID);
	}

	//NOT IMPLEMENTED YET
	bool bSuccess = SetPoolPriority(poolList);

	SetupStatusObject_STR(bSuccess, 73, "Changed pool priorities", returnObject);

	return returnObject;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	PoolQuota
//
//Purpose:	
//		Get the PoolQuota
//
//Parameters:
//		 mapParams: 
//			"POOL": The ID of the pool to get the quota of
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: poolquota",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::PoolQuota(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//Application* pApp = Application::GetInstance();
	//ADL* pADL = pApp->GetADL();


	//NOT IMPLEMENTED YET
	SetupStatusObject(false, "poolquota", returnObject);

	///TODO: Build Return JSON Object


	//SetupStatusObject(true, "poolquota", returnObject);

	return returnObject;
}

std::string APIHandler::PoolQuota_STR(parameter_list listParams)
{
	std::string returnObject;

	//Application* pApp = Application::GetInstance();
	//ADL* pADL = pApp->GetADL();


	//NOT IMPLEMENTED YET
	SetupStatusObject_STR(false, 122, "Set pool '%s' to quota %d'", returnObject);

	///TODO: Build Return JSON Object


	//SetupStatusObject(true, "poolquota", returnObject);

	return returnObject;
}


bool APIHandler::EnableMiningPool(int32 nPoolID)
{
	EventManager::GetInstance()->AddEvent(Event::ENABLE_POOL, ToVoid<int>(nPoolID));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	EnablePool
//
//Purpose:	
//		Enable the pool specified
//
//Parameters:
//		 mapParams: 
//			"POOL": The ID of the pool to enable
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: EnablePool",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::EnablePool(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "enablepool", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("Pool");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "enablepool", returnObject);
		return returnObject;
	}

	std::string szPoolInfo = iter->second;
	if (szPoolInfo.empty())
	{
		SetupStatusObject(false, "enablepool", returnObject);
		return returnObject;
	}

	std::string szPoolNum = iter->second;
	int32 nPoolNum = atoi(szPoolNum.c_str());

	//NOT IMPLEMENTED YET
	bool bSuccess = EnableMiningPool(nPoolNum);

	SetupStatusObject(bSuccess, "enablepool", returnObject);

	return returnObject;
}

std::string APIHandler::EnablePool_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 47, "Enabling pool %d:'%s'", returnObject);
		return returnObject;
	}

	std::string szPoolNum = listParams[0];
	int32 nPoolNum = atoi(szPoolNum.c_str());

	//NOT IMPLEMENTED YET
	bool bSuccess = EnableMiningPool(nPoolNum);
	SetupStatusObject_STR(bSuccess, 47, std::string("Enabling pool '") + szPoolNum + std::string("'"), returnObject);

	return returnObject;
}


bool APIHandler::DisableMiningPool(int32 nPoolID)
{
	EventManager::GetInstance()->AddEvent(Event::DISABLE_POOL, ToVoid<int>(nPoolID));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	DisablePool
//
//Purpose:	
//		Disable the specified pool
//
//Parameters:
//		 mapParams: 
//			"POOLS": The ID of the pool to disable
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: disablepool",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::DisablePool(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "disablepool", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("Pool");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "disablepool", returnObject);
		return returnObject;
	}

	std::string szPoolInfo = iter->second;
	if (szPoolInfo.empty())
	{
		SetupStatusObject(false, "disablepool", returnObject);
		return returnObject;
	}

	std::string szPoolNum = iter->second;
	int32 nPoolNum = atoi(szPoolNum.c_str());

	//NOT IMPLEMENTED YET
	bool bSuccess = DisableMiningPool(nPoolNum);

	SetupStatusObject(bSuccess, "disablepool", returnObject);

	return returnObject;
}

std::string APIHandler::DisablePool_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 48, "Disabling pool %d:'%s'", returnObject);
		return returnObject;
	}

	std::string szPoolNum = listParams[0];
	int32 nPoolNum = atoi(szPoolNum.c_str());

	//NOT IMPLEMENTED YET
	bool bSuccess = DisableMiningPool(nPoolNum);
	SetupStatusObject_STR(bSuccess, 48, std::string("Disabling pool '") + szPoolNum + std::string("'"), returnObject);

	return returnObject;
}


bool APIHandler::RemoveMiningPool(int32 nPoolID)
{
	EventManager::GetInstance()->AddEvent(Event::REMOVE_POOL, ToVoid<int>(nPoolID));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	RemovePool
//
//Purpose:	
//		Remove the specified pool
//
//Parameters:
//		 mapParams: 
//			"POOL": The ID of the pool to remove
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: removepool",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::RemovePool(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() == 0)
	{
		SetupStatusObject(false, "removepool", returnObject);
		return returnObject;
	}

	parameter_map::const_iterator iter = mapParams.find("Pool");
	if (iter == mapParams.end())
	{
		SetupStatusObject(false, "removepool", returnObject);
		return returnObject;
	}

	std::string szPoolInfo = iter->second;
	if (szPoolInfo.empty())
	{
		SetupStatusObject(false, "removepool", returnObject);
		return returnObject;
	}

	std::string szPoolNum = iter->second;
	int32 nPoolNum = atoi(szPoolNum.c_str());

	//NOT IMPLEMENTED YET
	bool bSuccess = RemoveMiningPool(nPoolNum);

	SetupStatusObject(bSuccess, "removepool", returnObject);

	return returnObject;
}

std::string APIHandler::RemovePool_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() == 0)
	{
		SetupStatusObject_STR(false, 68, "Removed pool %d:'%s'", returnObject);
		return returnObject;
	}

	std::string szPoolNum = listParams[0];
	int32 nPoolNum = atoi(szPoolNum.c_str());

	//NOT IMPLEMENTED YET
	bool bSuccess = RemoveMiningPool(nPoolNum);
	SetupStatusObject_STR(bSuccess, 68, std::string("Removed pool '") + szPoolNum + std::string("'"), returnObject);

	return returnObject;
}


bool APIHandler::SetIntensity(int32 nIntensity, int32 nGPU)
{
	EventManager::GetInstance()->AddEvent(Event::SET_GPU_INTENSTY, ToVoid<int>(nGPU), ToVoid<int>(nIntensity));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	GPUIntensity
//
//Purpose:	
//		Set the Intensity of the specified GPU
//
//Parameters:
//		 mapParams: 
//			"INTENSITY": The intensity to set
//			"GPU": The ID of the gpu
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpuintensity",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUIntensity(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() < 2)
	{
		SetupStatusObject(false, "gpuintensity", returnObject);
		return returnObject;
	}

	std::string szIntensity = mapParams["Intensity"];
	std::string szGPUID = mapParams["GPUID"];

	int32 nIntensity = atoi(szIntensity.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetIntensity(nIntensity, nGPUID);
	SetupStatusObject(bSuccess, "gpuintensity", returnObject);

	return returnObject;
}

std::string APIHandler::GPUIntensity_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() < 2)
	{
		SetupStatusObject_STR(false, 32, "GPU %d set new intensity to %s", returnObject);
		return returnObject;
	}

	std::string szIntensity = listParams[0];
	std::string szGPUID = listParams[1];

	int32 nIntensity = atoi(szIntensity.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetIntensity(nIntensity, nGPUID);
	SetupStatusObject_STR(bSuccess, 32, std::string("GPU ") + szGPUID + std::string(" set new intensity to ") + szIntensity, returnObject);

	return returnObject;
}




bool APIHandler::SetXIntensity(int32 nXIntensity, int32 nGPU)
{
	EventManager::GetInstance()->AddEvent(Event::SET_GPU_X_INTENSTY, ToVoid<int>(nGPU), ToVoid<int>(nXIntensity));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	GPUXIntensity
//
//Purpose:	
//		Set the X-Intensity of the specified GPU
//
//Parameters:
//		 mapParams: 
//			"XINTENSITY": The x-intensity to set
//			"GPU": The ID of the gpu
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpuxintensity",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUXIntensity(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() < 2)
	{
		SetupStatusObject(false, "gpuxintensity", returnObject);
		return returnObject;
	}

	std::string szXIntensity = mapParams["XIntensity"];
	std::string szGPUID = mapParams["GPUID"];

	int32 nXIntensity = atoi(szXIntensity.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetXIntensity(nXIntensity, nGPUID);
	SetupStatusObject(true, "gpuxintensity", returnObject);

	return returnObject;
}

std::string APIHandler::GPUXIntensity_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() < 2)
	{
		SetupStatusObject_STR(false, 32, "GPU %d set new intensity to %s", returnObject);
		return returnObject;
	}

	std::string szXIntensity = listParams[0];
	std::string szGPUID = listParams[1];

	int32 nXIntensity = atoi(szXIntensity.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetXIntensity(nXIntensity, nGPUID);
	SetupStatusObject_STR(true, 32, std::string("GPU ") + szGPUID + std::string(" set new intensity to ") + szXIntensity, returnObject);

	return returnObject;
}


bool APIHandler::SetRawIntensity(int32 nRawIntensity, int32 nGPU)
{
	EventManager::GetInstance()->AddEvent(Event::SET_GPU_RAW_INTENSTY, ToVoid<int>(nGPU), ToVoid<int>(nRawIntensity));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	GPURawIntensity
//
//Purpose:	
//		Set the Raw-Intensity of the specified GPU
//
//Parameters:
//		 mapParams: 
//			"RAWINTENSITY": The raw-intensity to set
//			"GPU": The ID of the gpu
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpurawintensity",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPURawIntensity(parameter_map mapParams)
{
	json_spirit::mObject returnObject;
	if (mapParams.size() < 2)
	{
		SetupStatusObject(false, "gpurawintensity", returnObject);
		return returnObject;
	}

	std::string szRawIntensity = mapParams["RawIntensity"];
	std::string szGPUID = mapParams["GPUID"];

	int32 nRawIntensity = atoi(szRawIntensity.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetRawIntensity(nRawIntensity, nGPUID);
	SetupStatusObject(bSuccess, "gpurawintensity", returnObject);

	return returnObject;
}

std::string APIHandler::GPURawIntensity_STR(parameter_list listParams)
{
	std::string  returnObject;
	if (listParams.size() < 2)
	{
		SetupStatusObject_STR(false, 32, "GPU %d set new intensity to %s", returnObject);
		return returnObject;
	}

	std::string szRawIntensity = listParams[0];
	std::string szGPUID = listParams[1];

	int32 nRawIntensity = atoi(szRawIntensity.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetRawIntensity(nRawIntensity, nGPUID);
	SetupStatusObject_STR(bSuccess, 32, std::string("GPU ") + szGPUID + std::string(" set new intensity to ") + szRawIntensity, returnObject);

	return returnObject;
}


bool APIHandler::SetGPUMemclock(int32 nGPUMemclock, int32 nGPU)
{
	EventManager::GetInstance()->AddEvent(Event::SET_GPU_MEMORY_CLOCK, ToVoid<int>(nGPU), ToVoid<int>(nGPUMemclock));
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	GPUMem
//
//Purpose:	
//		Set the Memory of the specified GPU
//
//Parameters:
//		 mapParams: 
//			"MEMORY": The memory to set
//			"GPU": The ID of the gpu
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpumem",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUMem(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() < 2)
	{
		SetupStatusObject(false, "gpumem", returnObject);
		return returnObject;
	}

	std::string szMemclock = mapParams["Memclock"];
	std::string szGPUID = mapParams["GPUID"];

	int32 nMemclock = atoi(szMemclock.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetGPUMemclock(nMemclock, nGPUID);
	SetupStatusObject(bSuccess, "gpumem", returnObject);

	return returnObject;
}


std::string APIHandler::GPUMem_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() < 2)
	{
		SetupStatusObject_STR(false, 35, "Setting GPU %d memoryclock to (%s) reported success", returnObject);
		return returnObject;
	}

	std::string szMemclock = listParams[0];
	std::string szGPUID = listParams[1];

	int32 nMemclock = atoi(szMemclock.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetGPUMemclock(nMemclock, nGPUID);
	SetupStatusObject_STR(bSuccess, 35, std::string("Setting GPU ") + szGPUID + std::string(" memoryclock to(") + szMemclock + std::string(") reported success"), returnObject);

	return returnObject;
}


bool APIHandler::SetGPUEngineClock(int32 nGPUEngineClock, int32 nGPU)
{
	EventManager::GetInstance()->AddEvent(Event::SET_GPU_ENGINE_CLOCK, ToVoid<int>(nGPU), ToVoid<int>(nGPUEngineClock));
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//Name:	GPUEngine
//
//Purpose:	
//		Set the Engine of the specified GPU
//
//Parameters:
//		 mapParams: 
//			"ENGINE": The engine to set
//			"GPU": The ID of the gpu
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpuengine",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUEngine(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() < 2)
	{
		SetupStatusObject(false, "gpuengine", returnObject);
		return returnObject;
	}

	std::string szEngineClock = mapParams["EngineClock"];
	std::string szGPUID = mapParams["GPUID"];

	int32 nEngineClock = atoi(szEngineClock.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetGPUEngineClock(nEngineClock, nGPUID);
	SetupStatusObject(bSuccess, "gpuengine", returnObject);

	return returnObject;
}

std::string APIHandler::GPUEngine_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() < 2)
	{
		SetupStatusObject_STR(false, 47, "Setting GPU %d clock to (%s) reported success", returnObject);
		return returnObject;
	}

	std::string szEngineClock = listParams[0];
	std::string szGPUID = listParams[1];

	int32 nEngineClock = atoi(szEngineClock.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetGPUEngineClock(nEngineClock, nGPUID);
	SetupStatusObject_STR(bSuccess, 47, std::string("Setting GPU ") + szGPUID + std::string(" clock to(") + szEngineClock + std::string(") reported success"), returnObject);

	return returnObject;
}



bool APIHandler::SetGPUFan(int32 nGPUFan, int32 nGPU)
{
	EventManager::GetInstance()->AddEvent(Event::SET_GPU_FAN, ToVoid<int>(nGPU), ToVoid<int>(nGPUFan));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	GPUFan
//
//Purpose:	
//		Set the Fan of the specified GPU
//
//Parameters:
//		 mapParams: 
//			"FAN": The fan to set
//			"GPU": The ID of the gpu
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpufan",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUFan(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() < 2)
	{
		SetupStatusObject(false, "gpufan", returnObject);
		return returnObject;
	}

	std::string szFanSpeed = mapParams["FanSpeed"];
	std::string szGPUID = mapParams["GPUID"];

	int32 nFanSpeed = atoi(szFanSpeed.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetGPUFan(nFanSpeed, nGPUID);
	SetupStatusObject(bSuccess, "gpufan", returnObject);

	return returnObject;
}

std::string APIHandler::GPUFan_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() < 2)
	{
		SetupStatusObject_STR(false, 41, "Setting GPU %d fan to (%s) reported success", returnObject);
		return returnObject;
	}

	std::string szFanSpeed = listParams[0];
	std::string szGPUID = listParams[1];

	int32 nFanSpeed = atoi(szFanSpeed.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetGPUFan(nFanSpeed, nGPUID);
	SetupStatusObject_STR(bSuccess, 41, std::string("Setting GPU ") + szGPUID + std::string(" fan to(") + szFanSpeed + std::string(") reported success"), returnObject);

	return returnObject;
}


bool APIHandler::SetGPUVDDC(float fGPU_VDDC, int32 nGPU)
{
	EventManager::GetInstance()->AddEvent(Event::SET_GPU_FAN, ToVoid<int>(nGPU), ToVoid<float>(fGPU_VDDC));
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	GPUVddc
//
//Purpose:	
//		Set the Voltage of the specified GPU
//
//Parameters:
//		 mapParams: 
//			"VOLTAGE": The fan to set
//			"GPU": The ID of the gpu
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: gpuvddc",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GPUVddc(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	if (mapParams.size() < 2)
	{
		SetupStatusObject(false, "gpuvddc", returnObject);
		return returnObject;
	}

	std::string szVoltage = mapParams["Voltage"];
	std::string szGPUID = mapParams["GPUID"];

	float fVoltage = atof(szVoltage.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetGPUVDDC(fVoltage, nGPUID);
	SetupStatusObject(bSuccess, "gpuvddc", returnObject);

	return returnObject;
}

std::string APIHandler::GPUVddc_STR(parameter_list listParams)
{
	std::string returnObject;

	if (listParams.size() < 2)
	{
		SetupStatusObject_STR(false, 39, "Setting GPU %d vddc to (%s) reported success", returnObject);
		return returnObject;
	}

	std::string szVoltage = listParams[0];
	std::string szGPUID = listParams[1];

	float fVoltage = atof(szVoltage.c_str());
	int32 nGPUID = atoi(szGPUID.c_str());

	bool bSuccess = SetGPUVDDC(fVoltage, nGPUID);
	SetupStatusObject_STR(bSuccess, 39, std::string("Setting GPU ") + szGPUID + std::string(" vddc to(") + szVoltage + std::string(") reported success"), returnObject);

	return returnObject;
}



bool APIHandler::SaveSettings()
{
	EventManager::GetInstance()->AddEvent(Event::SAVE_SETTINGS);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Save
//
//Purpose:	
//		Save the current settings
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: save",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Save(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	bool bSuccess = SaveSettings();

	//NOT IMPLEMENTED YET
	SetupStatusObject(bSuccess, "save", returnObject);

	return returnObject;
}

std::string APIHandler::Save_STR(parameter_list listParams)
{
	std::string returnObject;

	bool bSuccess = SaveSettings();

	//NOT IMPLEMENTED YET
	SetupStatusObject_STR(bSuccess, 44, "Configuration saved to file '%s'", returnObject);

	return returnObject;
}


bool APIHandler::QuitMiner()
{
	EventManager::GetInstance()->AddEvent(Event::QUIT);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Quit
//
//Purpose:	
//		Quit the Mining Application
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: quit",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Quit(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	bool bSuccess = QuitMiner();
	SetupStatusObject(bSuccess, "quit", returnObject);

	return returnObject;
}

std::string APIHandler::Quit_STR(parameter_list listParams)
{
	std::string returnObject;

	bool bSuccess = QuitMiner();
	SetupStatusObject_STR(bSuccess, 0, "quit", returnObject);

	return returnObject;
}


bool APIHandler::PrivelegedInfo()
{
	bool bSuccess = true;

	//Application* pApp = Application::GetInstance();

	return bSuccess;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Privileged
//
//Purpose:	
//		Privileged
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: privileged",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Privileged(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = PrivelegedInfo();
	SetupStatusObject(bSuccess, "privileged", returnObject);

	return returnObject;
}

std::string APIHandler::Privileged_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = PrivelegedInfo();
	SetupStatusObject_STR(bSuccess, 46, "Privileged access OK", returnObject);

	return returnObject;
}


bool APIHandler::NotifyInfo()
{
	bool bSuccess = false;

	//Application* pApp = Application::GetInstance();

	return bSuccess;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Notify
//
//Purpose:	
//		Notify
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: notify",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Notify(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = NotifyInfo();
	SetupStatusObject(false, "notify", returnObject);

	return returnObject;
}

std::string APIHandler::Notify_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = NotifyInfo();
	SetupStatusObject_STR(false, 60, "Notify", returnObject);

	return returnObject;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	DevDetails
//
//Purpose:	
//		DevDetails
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"DEVS": [
//					{
//						"DEVDETAILS": 0,
//						"Name": "R9 280X",
//						"ID": 10,
//						"Driver": "",
//						"Kernel": "sk1024",
//						"Model": "AMD R9 200 Series",
//						"Device Path": ""
//					}
//				],
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: devdetails",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::DevDetails(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			json_spirit::mArray devContainer;
			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				json_spirit::mArray devArr = connObj.at("Devices").get_array();

				for (size_t devIndex = 0; devIndex < devArr.size(); ++devIndex)
				{
					json_spirit::mObject currDevObj = devArr[devIndex].get_obj();
					json_spirit::mObject retDevObj;

					retDevObj["DEVDETAILS"] = currDevObj.at("ID").get_int();
					retDevObj["Name"] = currDevObj.at("Name").get_str();
					retDevObj["ID"] = currDevObj.at("ID").get_int();
					retDevObj["Driver"] = currDevObj.at("Driver").get_str();
					retDevObj["Kernel"] = currDevObj.at("Kernel").get_str();
					retDevObj["Model"] = currDevObj.at("Model").get_str();
					retDevObj["Device Path"] = currDevObj.at("Device Path").get_str();

					devContainer.push_back(retDevObj);
				}
			}

			returnObject["DEVS"] = devContainer;

			SetupStatusObject(true, "devdetails", returnObject);
		}
		else
		{
			SetupStatusObject(false, "devdetails", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}

std::string APIHandler::DevDetails_STR(parameter_list listParams)
{
	std::string returnObject;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			SetupStatusObject_STR(true, 69, "Device Details", returnObject);

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				json_spirit::mArray devArr = connObj.at("Devices").get_array();

				for (size_t devIndex = 0; devIndex < devArr.size(); ++devIndex)
				{
					json_spirit::mObject currDevObj = devArr[devIndex].get_obj();

					returnObject += std::string("DEVDETAILS") + EQUALS + std::to_string(currDevObj.at("ID").get_int()) + COMMA;
					returnObject += std::string("Name") + EQUALS + currDevObj.at("Name").get_str() + COMMA;
					returnObject += std::string("ID") + EQUALS + std::to_string(currDevObj.at("ID").get_int()) + COMMA;
					returnObject += std::string("Driver") + EQUALS + currDevObj.at("Driver").get_str() + COMMA;
					returnObject += std::string("Kernel") + EQUALS + currDevObj.at("Kernel").get_str() + COMMA;
					returnObject += std::string("Model") + EQUALS + currDevObj.at("Model").get_str() + COMMA;
					returnObject += std::string("Device Path") + EQUALS + currDevObj.at("Device Path").get_str() + SEPARATOR;
				}
			}
		}
		else
		{
			SetupStatusObject_STR(false, 69, "Device Details", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}


bool APIHandler::RestartMiner()
{
	EventManager::GetInstance()->AddEvent(Event::RESTART_MINER);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Restart
//
//Purpose:	
//		Restart the Miner
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: restart",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Restart(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = RestartMiner();
	SetupStatusObject(bSuccess, "restart", returnObject);

	return returnObject;
}

std::string APIHandler::Restart_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = RestartMiner();
	SetupStatusObject_STR(bSuccess, 0, "restart", returnObject);

	return returnObject;
}

//////////////////////////////////////////////////////////////////////////////
//Name:	Stats
//
//Purpose:	
//		Restart the Miner
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATS": [
//					
//					{
//							
//						"ID": "R9 280X",
//						"Elapsed": 0,
//						"Calls": 0,
//						"Wait": 0,  
//						"Max": 0,
//						"Min": 0
//					},
//					{						
//						"ID": "POOL0",
//						"Elapsed": 0,
//						"Calls": 0,
//						"Wait": 0,
//						"Max": 0,
//						"Min": 0,
//						"Pool Calls": 0,
//						"Pool Attempts": 0,
//						"Pool Wait": 0,
//						"Pool Max": 0,
//						"Pool Min": 0,
//						"Pool Av": 0,
//						"Work Had Roll Time": 0,
//						"Work Can Roll": 0,
//						"Work Had Expire": 0,
//						"Work Roll Time": 0,
//						"Work Diff": 0,
//						"Min Diff": 0,
//						"Max Diff": 0,
//						"Min Diff Count": 0,
//						"Max Diff Count": 0,
//						"Times Sent": 0,
//						"Bytes Sent": 0,
//						"Times Recv": 0,
//						"Bytes Recv": 0,
//						"Net Bytes Sent": 0,
//						"Net Bytes Recv": 0
//					}
//
//				]
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: stats",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Stats(parameter_map mapParams)
{
	json_spirit::mObject returnObject;


	json_spirit::mArray statsArr;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				double hashes = connObj.at("Hashes").get_real();
				if (hashes > 0)
				{

					json_spirit::mArray devArr = connObj.at("Devices").get_array();

					for (size_t devInd = 0; devInd < devArr.size(); ++devInd)
					{
						json_spirit::mObject currDevObj = devArr[devInd].get_obj();

						json_spirit::mObject statsObject;

						statsObject["ID"] = currDevObj.at("Name").get_str() + std::to_string(currDevObj.at("Device Index").get_int());
						statsObject["Elapsed"] = currDevObj.at("Elapsed").get_int();
						statsObject["Calls"] = currDevObj.at("Calls").get_int();
						statsObject["Wait"] = currDevObj.at("Wait").get_int();
						statsObject["Max"] = currDevObj.at("Max").get_int();
						statsObject["Min"] = currDevObj.at("Min").get_int();

						statsArr.push_back(statsObject);
					}

					json_spirit::mObject poolObject;

					poolObject["ID"] = std::string("POOL") + std::to_string(connObj.at("ID").get_int());
					poolObject["Elapsed"] = ((double)connObj["Time"].get_int()) / 1000.0;
					poolObject["Calls"] = connObj.at("Calls").get_int();
					poolObject["Wait"] = connObj.at("Wait").get_int();
					poolObject["Max"] = connObj.at("Max").get_int();
					poolObject["Min"] = connObj.at("Min").get_int();
					poolObject["Pool Calls"] = connObj.at("Pool Calls").get_int();
					poolObject["Pool Attempts"] = connObj.at("Pool Attempts").get_int();
					poolObject["Pool Wait"] = connObj.at("Pool Wait").get_int();
					poolObject["Pool Max"] = connObj.at("Pool Max").get_int();
					poolObject["Pool Min"] = connObj.at("Pool Min").get_int();
					poolObject["Pool Av"] = connObj.at("Pool Av").get_int();
					poolObject["Work Had Roll Time"] = connObj.at("Work Had Roll Time").get_int();
					poolObject["Work Can Roll"] = connObj.at("Work Can Roll").get_int();
					poolObject["Work Had Expire"] = connObj.at("Work Had Expire").get_int();
					poolObject["Work Roll Time"] = connObj.at("Work Roll Time").get_int();
					poolObject["Work Diff"] = connObj.at("Work Diff").get_int();
					poolObject["Min Diff"] = connObj.at("Min Diff").get_int();
					poolObject["Max Diff"] = connObj.at("Max Diff").get_int();
					poolObject["Min Diff Count"] = connObj.at("Min Diff Count").get_int();
					poolObject["Max Diff Count"] = connObj.at("Max Diff Count").get_int();
					poolObject["Times Sent"] = connObj.at("Times Sent").get_int();
					poolObject["Bytes Sent"] = connObj.at("Bytes Sent").get_int();
					poolObject["Times Recv"] = connObj.at("Times Recv").get_int();
					poolObject["Bytes Recv"] = connObj.at("Bytes Recv").get_int();
					poolObject["Net Bytes Sent"] = connObj.at("Net Bytes Sent").get_int();
					poolObject["Net Bytes Recv"] = connObj.at("Net Bytes Recv").get_int();

					statsArr.push_back(poolObject);
				}
			}

			returnObject["STATS"] = statsArr;

			SetupStatusObject(true, "stats", returnObject);
		}
		else
		{
			SetupStatusObject(false, "stats", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}

std::string APIHandler::Stats_STR(parameter_list listParams)
{
	std::string returnObject;

	std::string poolData = "";

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			SetupStatusObject_STR(true, 70, "CGMiner stats", returnObject);

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				double hashes = connObj.at("Hashes").get_real();
				if (hashes > 0)
				{

					json_spirit::mArray devArr = connObj.at("Devices").get_array();

					for (size_t devInd = 0; devInd < devArr.size(); ++devInd)
					{
						json_spirit::mObject currDevObj = devArr[devInd].get_obj();

						returnObject += std::string("ID") + EQUALS + currDevObj.at("Name").get_str() + std::to_string(currDevObj.at("Device Index").get_int()) + COMMA;
						returnObject += std::string("Elapsed") + EQUALS + std::to_string(currDevObj.at("Elapsed").get_int()) + COMMA;
						returnObject += std::string("Calls") + EQUALS + std::to_string(currDevObj.at("Calls").get_int()) + COMMA;
						returnObject += std::string("Wait") + EQUALS + std::to_string(currDevObj.at("Wait").get_int()) + COMMA;
						returnObject += std::string("Max") + EQUALS + std::to_string(currDevObj.at("Max").get_int()) + COMMA;
						returnObject += std::string("Min") + EQUALS + std::to_string(currDevObj.at("Min").get_int()) + SEPARATOR;
					}

					poolData += std::string("ID") + EQUALS + std::string("POOL") + std::to_string(connObj.at("ID").get_int()) + COMMA;
					poolData += std::string("Elapsed") + EQUALS + std::to_string(((double)connObj["Time"].get_int()) / 1000.0) + COMMA;
					poolData += std::string("Calls") + EQUALS + std::to_string(connObj.at("Calls").get_int()) + COMMA;
					poolData += std::string("Wait") + EQUALS + std::to_string(connObj.at("Wait").get_int()) + COMMA;
					poolData += std::string("Max") + EQUALS + std::to_string(connObj.at("Max").get_int()) + COMMA;
					poolData += std::string("Min") + EQUALS + std::to_string(connObj.at("Min").get_int()) + COMMA;
					poolData += std::string("Pool Calls") + EQUALS + std::to_string(connObj.at("Pool Calls").get_int()) + COMMA;
					poolData += std::string("Pool Attempts") + EQUALS + std::to_string(connObj.at("Pool Attempts").get_int()) + COMMA;
					poolData += std::string("Pool Wait") + EQUALS + std::to_string(connObj.at("Pool Wait").get_int()) + COMMA;
					poolData += std::string("Pool Max") + EQUALS + std::to_string(connObj.at("Pool Max").get_int()) + COMMA;
					poolData += std::string("Pool Min") + EQUALS + std::to_string(connObj.at("Pool Min").get_int()) + COMMA;
					poolData += std::string("Pool Av") + EQUALS + std::to_string(connObj.at("Pool Av").get_int()) + COMMA;
					poolData += std::string("Work Had Roll Time") + EQUALS + std::to_string(connObj.at("Work Had Roll Time").get_int()) + COMMA;
					poolData += std::string("Work Can Roll") + EQUALS + std::to_string(connObj.at("Work Can Roll").get_int()) + COMMA;
					poolData += std::string("Work Roll Time") + EQUALS + std::to_string(connObj.at("Work Roll Time").get_int()) + COMMA;
					poolData += std::string("Work Had Expire") + EQUALS + std::to_string(connObj.at("Work Had Expire").get_int()) + COMMA;
					poolData += std::string("Work Diff") + EQUALS + std::to_string(connObj.at("Work Diff").get_int()) + COMMA;
					poolData += std::string("Min Diff") + EQUALS + std::to_string(connObj.at("Min Diff").get_int()) + COMMA;
					poolData += std::string("Max Diff") + EQUALS + std::to_string(connObj.at("Max Diff").get_int()) + COMMA;
					poolData += std::string("Min Diff Count") + EQUALS + std::to_string(connObj.at("Min Diff Count").get_int()) + COMMA;
					poolData += std::string("Max Diff Count") + EQUALS + std::to_string(connObj.at("Max Diff Count").get_int()) + COMMA;
					poolData += std::string("Times Sent") + EQUALS + std::to_string(connObj.at("Times Sent").get_int()) + COMMA;
					poolData += std::string("Bytes Sent") + EQUALS + std::to_string(connObj.at("Bytes Sent").get_int()) + COMMA;
					poolData += std::string("Times Recv") + EQUALS + std::to_string(connObj.at("Times Recv").get_int()) + COMMA;
					poolData += std::string("Bytes Recv") + EQUALS + std::to_string(connObj.at("Bytes Recv").get_int()) + COMMA;
					poolData += std::string("Net Bytes Sent") + EQUALS + std::to_string(connObj.at("Net Bytes Sent").get_int()) + COMMA;
					poolData += std::string("Net Bytes Recv") + EQUALS + std::to_string(connObj.at("Net Bytes Recv").get_int()) + SEPARATOR;
				}
			}

			returnObject += poolData;
		}
		else
		{
			SetupStatusObject_STR(false, 70, "CGMiner stats", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}



bool APIHandler::CheckMiner()
{
	bool bSuccess = false;

	//Application* pApp = Application::GetInstance();
	//ADL* pADL = pApp->GetADL();

	return bSuccess;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Check
//
//Purpose:	
//		Check
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: check",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Check(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = CheckMiner();
	SetupStatusObject(bSuccess, "check", returnObject);

	return returnObject;
}

std::string APIHandler::Check_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = CheckMiner();
	SetupStatusObject_STR(bSuccess, 72, "Check command", returnObject);

	return returnObject;
}


bool APIHandler::SetFailoverOnly()
{
	bool bSuccess = false;

	//Application* pApp = Application::GetInstance();

	return bSuccess;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Failover_Only
//
//Purpose:	
//		Failover_Only
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: failover-only",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Failover_Only(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetFailoverOnly();
	SetupStatusObject(bSuccess, "failover-only", returnObject);

	return returnObject;
}

std::string APIHandler::Failover_Only_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetFailoverOnly();
	SetupStatusObject_STR(bSuccess, 77, "Failover-Only set to %s", returnObject);

	return returnObject;

}
//////////////////////////////////////////////////////////////////////////////
//Name:	Coin
//
//Purpose:	
//		Coin
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"COIN":
//				{
//					"Hash Method": "sk1024",
//					"Current Block Time": 0,
//					"Current Block Hash": 0,
//					"LP": false,
//					"Network Difficulty": 0
//				},
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: Coin",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Coin(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			json_spirit::mObject currCoinObject = dataObj.at("Coin").get_obj();

			json_spirit::mObject coinObject;

			coinObject["Hash Method"] = currCoinObject.at("Hash Method").get_str();
			coinObject["Current Block Time"] = currCoinObject.at("Current Block Time").get_real();
			coinObject["Current Block Hash"] = currCoinObject.at("Current Block Hash").get_int();
			coinObject["LP"] = currCoinObject.at("LP").get_bool();
			coinObject["Network Difficulty"] = currCoinObject.at("Network Difficulty").get_real();

			returnObject["COIN"] = coinObject;

			SetupStatusObject(true, "coin", returnObject);
		}
		else
		{
			SetupStatusObject(false, "coin", returnObject);
		}		
	}
	m_dataLock->unlock();

	return returnObject;
}


std::string APIHandler::Coin_STR(parameter_list listParams)
{
	std::string returnObject;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			json_spirit::mObject dataObj = m_appData->at("Data").get_obj();

			SetupStatusObject_STR(true, 78, "CGMiner coin", returnObject);

			json_spirit::mObject currCoinObject = dataObj.at("Coin").get_obj();

			returnObject += std::string("Hash Method") + EQUALS + currCoinObject.at("Hash Method").get_str() + COMMA;
			returnObject += std::string("Current Block Time") + EQUALS + std::to_string(currCoinObject.at("Current Block Time").get_real()) + COMMA;
			returnObject += std::string("Current Block Hash") + EQUALS + std::to_string(currCoinObject.at("Current Block Hash").get_int()) + COMMA;
			returnObject += std::string("LP") + EQUALS + std::to_string(currCoinObject.at("LP").get_bool()) + COMMA;
			returnObject += std::string("Network Difficulty") + EQUALS + std::to_string(currCoinObject.at("Network Difficulty").get_real()) + SEPARATOR;
		}
		else
		{
			SetupStatusObject_STR(false, 78, "CGMiner coin", returnObject);
		}
	}
	m_dataLock->unlock();

	return returnObject;
}



bool APIHandler::SetDebug()
{
	EventManager::GetInstance()->AddEvent(Event::SET_DEBUG_MODE);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Debug
//
//Purpose:	
//		Debug
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: debug",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Debug(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetDebug();
	SetupStatusObject(bSuccess, "debug", returnObject);

	return returnObject;
}

std::string APIHandler::Debug_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetDebug();
	SetupStatusObject_STR(bSuccess, 79, "Debug settings", returnObject);

	return returnObject;
}


bool APIHandler::SetConfigData()
{
	bool bSuccess = false;

	//Application* pApp = Application::GetInstance();
	//ADL* pADL = pApp->GetADL();

	return bSuccess;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	SetConfig
//
//Purpose:	
//		SetConfig
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: setconfig",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::SetConfig(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetConfigData();
	SetupStatusObject(bSuccess, "setconfig", returnObject);

	return returnObject;
}

std::string APIHandler::SetConfig_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetConfigData();
	SetupStatusObject_STR(bSuccess, 82, "Set config '%s' to %d", returnObject);

	return returnObject;
}


bool APIHandler::SetZero()
{
	EventManager::GetInstance()->AddEvent(Event::SET_ZERO);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	Zero
//
//Purpose:	
//		Sets all of the Stat Totals to 0
//
//Parameters:
//		 mapParams: O
//Return:
//
//Created:Constructor
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::Zero(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetZero();
	SetupStatusObject(bSuccess, "zero", returnObject);

	return returnObject;
}

std::string APIHandler::Zero_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetZero();
	SetupStatusObject_STR(bSuccess, 96, "Zeroed %s stats with summary", returnObject);

	return returnObject;
}


bool APIHandler::SetLockStats()
{
	EventManager::GetInstance()->AddEvent(Event::LOCK_STATS);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	LockStats
//
//Purpose:	
//		LockStats
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters 
//
//Return:
//
//			json status object
//
//			{
//				"STATUS":
//					{
//						"STATUS": "S",
//						"Msg": "Successfully made the API call to: lockstats",
//						"Code" : -1,
//						"When": 1422821846,
//						"Description": "1.0"
//					}
//			}
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::LockStats(parameter_map mapParams)
{
	json_spirit::mObject returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetLockStats();
	SetupStatusObject(bSuccess, "lockstats", returnObject);

	return returnObject;
}

std::string APIHandler::LockStats_STR(parameter_list listParams)
{
	std::string returnObject;

	//NOT IMPLEMENTED YET
	bool bSuccess = SetLockStats();
	SetupStatusObject_STR(bSuccess, 123, "Lock stats created", returnObject);

	return returnObject;
}
//////////////////////////////////////////////////////////////////////////////
//Name:	GetMiningInfo
//
//Purpose:	
//		Gets all of the Information for the Mining Application, Connections, and Devices.
//
//Parameters:
//		 mapParams: Empty. Doesn't take any parameters
//
//Return:
//
//		json object containing a Summary of the Mining Statistics
//
//		Ex:
//
//			{
//				"MHS av": 40.00,
//				"Accepted": 3,
//				"Rejected": 1,
//				"Hardware Errors": 0,
//				"Work Utility": 0.02,
//				"STATUS":
//				{
//					"STATUS": "S"
//				}
//			}
//
///////////////////////////////////////////////////////////////////////////////
json_spirit::mObject APIHandler::GetMiningInfo(parameter_map mapParams)
{
	json_spirit::mObject returnObject;
	
	bool foundData = false;
	json_spirit::mObject dataObj;

	m_dataLock->lock();
	{
		if (m_appData)
		{
			foundData = true;
			dataObj = m_appData->at("Data").get_obj();

			json_spirit::mArray connArr = dataObj.at("Connections").get_array();
			json_spirit::mArray retConnArray;

			for (size_t srvIndex = 0; srvIndex < connArr.size(); ++srvIndex)
			{
				json_spirit::mObject connObj = connArr[srvIndex].get_obj();
				json_spirit::mObject retConnObject;

				retConnObject["id"] = connObj.at("ID").get_int();
				retConnObject["ip"] = connObj.at("IP").get_str();
				retConnObject["port"] = connObj.at("Port").get_str();
				retConnObject["totalHashes"] = connObj.at("Hashes").get_real();

				json_spirit::mArray devArr = connObj.at("Devices").get_array();
				json_spirit::mArray retThreadArr;

				for (size_t devInd = 0; devInd < devArr.size(); ++devInd)
				{
					json_spirit::mObject currDevObj = devArr[devInd].get_obj();
					json_spirit::mObject retThreadObj;

					retThreadObj["id"] = devInd;
					retThreadObj["accepted"] = currDevObj.at("Accepted").get_int();
					retThreadObj["rejected"] = currDevObj.at("Rejected").get_int();
					retThreadObj["hashRate"] = currDevObj.at("Last Hash Rate").get_real();

					json_spirit::mObject retDevObj;

					retDevObj["id"] = devInd;
					retDevObj["longName"] = currDevObj.at("Long Name").get_str();
					retDevObj["intentsity"] = currDevObj.at("Intensity").get_int();
					retDevObj["xintentsity"] = currDevObj.at("X Intensity").get_int();
					retDevObj["rawintentsity"] = currDevObj.at("Raw Intensity").get_int();
					retDevObj["algorithm"] = currDevObj.at("Kernel").get_str();
					retDevObj["adapterIndex"] = currDevObj.at("ID").get_int();
					retDevObj["adapterName"] = currDevObj.at("Model").get_str();
					retDevObj["vendorID"] = currDevObj.at("Vendor ID").get_int();
					retDevObj["subSystemID"] = currDevObj.at("Sub System ID").get_str();
					retDevObj["subSystemVendorID"] = currDevObj.at("Sub System Vendor ID").get_str();
					retDevObj["pciDeviceID"] = currDevObj.at("PCI Device ID").get_str();
					retDevObj["displayName"] = currDevObj.at("Name").get_str();
					retDevObj["biosPartNumber"] = currDevObj.at("Bios Part Number").get_str();
					retDevObj["biosVersion"] = currDevObj.at("Bios Version").get_str();
					retDevObj["biosDate"] = currDevObj.at("Bios Date").get_str();
					retDevObj["temperature"] = currDevObj.at("Temperature").get_real();
					retDevObj["fan"] = currDevObj.at("Fan Speed").get_int();
					retDevObj["activity"] = currDevObj.at("GPU Activity").get_int();
					retDevObj["name"] = currDevObj.at("Device Name").get_str();
					retDevObj["addressBits"] = currDevObj.at("Address Bits").get_int();
					retDevObj["openCLVersion"] = currDevObj.at("Driver").get_str();
					retDevObj["vendorName"] = currDevObj.at("Vendor Name").get_str();

					retThreadObj["gpu"] = retDevObj;
					retThreadArr.push_back(retThreadObj);
				}

				retConnObject["threads"] = retThreadArr;
				retConnArray.push_back(retConnObject);
			}

			returnObject["miningInfo"] = retConnArray;
			returnObject["version"] = dataObj.at("Version").get_str();
		}
	}
	m_dataLock->unlock();

	if (!foundData)
	{
		SetupStatusObject(false, "getmininginfo", returnObject);
		return returnObject;
	}
	else
	{
		SetupStatusObject(true, "getmininginfo", returnObject);
	}

	return returnObject;
}

void APIHandler::HandleEvent(Event* pEvent)
{
	if (!pEvent)
	{
		return;
	}

	switch (pEvent->GetEventType())
	{
		case Event::EVENT_TYPE::RETURN_MINING_INFO:
		{
			json_spirit::mObject* pData = pEvent->GetParam<json_spirit::mObject*>(0);

			m_dataLock->lock();
			{
				if (m_appData)
				{
					delete(m_appData);
					m_appData = NULL;
				}
				
				m_appData = new json_spirit::mObject(*pData);
				delete(pData);
				pData = NULL;
			}
			m_dataLock->unlock();
		}
		break;
	}
}