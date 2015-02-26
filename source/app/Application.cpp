////////////////////////////////////////////////
// File:	Application.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////
#include "../web/WebJsonRequest.h"

#include "Application.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "../core/util.h"
#include "../gpu/BaseGPU.h"
#include "../gpu/GPUData.h"
#include "../driver/ADL.h"
#include "../gpu/ADLGPU.h"
#include "../compute/CLFuncs.h"
#include "../compute/BaseComputeDevice.h"
#include "../config/RunOptions.h"
#include "../config/ConfigConnection.h"
#include "../config/GPUSetting.h"
#include "../connections/SKServerConnection.h"
#include "../threads/MinerThread.h"
#include "../data/MinerData.h"
#include "../compute/CLDevice.h"

#include "../web/WebServer.h"

#include "../utilities/StringUtils.h"

#include "../file/FileLoader.h"
#include "../file/JSONFileLoader.h"

using namespace std;
using namespace boost::gregorian;

Application* Application::m_app = 0;

Application* Application::GetInstance()
{
	if(m_app == 0)
	{
		m_app = new Application();
	}

	return m_app;
}

Application::Application()
{ 
	m_nWidth = 640;
	m_nHeight = 480;

}
Application::Application(const Application& app) { }
void Application::operator=(const Application& app) { }
Application::~Application() 
{ 
}

void Application::Initialize(std::map<std::string,std::string> arguments)
{
	cout << "SKMiner for AMD GPUs - Version: " << VERSION << std::endl;
	cout << std::endl;
	cout << "Developed by Liam Russell A.K.A. BitSlapper for the GPU Channel of CoinShield" << std::endl;
	cout << std::endl;
	cout << std::endl;

	JSONFileLoader* pFileLoader = new JSONFileLoader();

	json_spirit::mObject readConfObj;
	json_spirit::mObject confObj;

	if (!pFileLoader->Load("miner.conf", readConfObj))
	{
		confObj = GetDefaultConfig();
		pFileLoader->Save("miner.conf", confObj);
	}
	else
	{
		confObj = readConfObj["DATA"].get_obj();
	}

	std::map<std::string, std::string> mapOptions = ParseOptions(confObj);
	for (std::map<std::string, std::string>::iterator iterator = arguments.begin(); iterator != arguments.end(); iterator++)
	{		
		mapOptions[iterator->first] = iterator->second;
	}

	m_pRunOptions = new RunOptions(mapOptions);
	
	if (CheckOptions())
	{
		m_pEventManager = EventManager::GetInstance();
		m_pEventManager->Initialize();
		m_pEventManager->Start();

		m_pEventManager->Register(Event::GET_MINING_INFO, Application::GetInstance());

		m_pADL = new ADL();	

		std::cout << std::endl;
		std::cout << "Starting SKMiner API on Port 8383" << std::endl;
		std::cout << std::endl;
		m_pAPIServer = new Http::Server::WebServer("127.0.0.1", "8383", "C:\\");
		m_pAPIServer->Run();

		if (m_pRunOptions->GetIsAutoSettings())
		{
			std::cout << "Auto Configuration of GPU Settings Selected" << std::endl;
			std::cout << std::endl;
			std::cout << "Getting GPU PCI IDs" << std::endl;
			std::cout << std::endl;

			std::vector<std::string> adlDevices = m_pADL->GetAdapterIDs();
			std::vector<GPUSetting*> vecGPUSettings;
			json_spirit::mArray resultsArray;

			std::string szQueryIDs = "";
			WebJsonRequest* pRequest = new WebJsonRequest();

			for (size_t stDevIndex = 0; stDevIndex < adlDevices.size(); ++stDevIndex)
			{
				json_spirit::mObject settingsObj;
				std::string szDeviceID = adlDevices[stDevIndex];
				if (pFileLoader->Load(szDeviceID + ".json", settingsObj, "settings"))
				{
					resultsArray.push_back(settingsObj["DATA"].get_obj());
				}	
				else if (szQueryIDs.find(szDeviceID) == std::string::npos)
				{
					std::string szRequest = std::string("http://www.skminer.io/api/settings/sk1024/") + szDeviceID;
					std::string szVal = pRequest->RequestJSONString(szRequest);
				
					json_spirit::mValue objVal;
					json_spirit::read(szVal, objVal);
					resultsArray.push_back(objVal);
				}
			}
					
			delete(pRequest);

			std::vector<int> vecUsedDevices;			
			for (size_t arrIndex = 0; arrIndex < resultsArray.size(); ++arrIndex)
			{
				json_spirit::mObject settingObj = resultsArray[arrIndex].get_obj();

				std::string szDeviceID = settingObj.find("device_hex_id")->second.get_str();

				bool settingExisted = false;
				json_spirit::mObject::const_iterator findIT = settingObj.find("saved");
				if (findIT != settingObj.end() && findIT->first == "saved")
				{
					settingExisted = true;
				}

				int foundInd = -1;
				for (size_t devIndex = 0; devIndex < adlDevices.size(); ++devIndex)
				{
					if (std::find(vecUsedDevices.begin(), vecUsedDevices.end(), devIndex) != vecUsedDevices.end())
					{
						continue;
					}

					if (szDeviceID.compare(adlDevices[devIndex]) == 0)
					{
						foundInd = devIndex;
						vecUsedDevices.push_back(devIndex);
						break;
					}
				}

				if (foundInd > -1)
				{
					std::cout << "Found Settings for Device " << szDeviceID << std::endl;
					std::cout << std::endl;

					GPUSetting* pGPUSetting = new GPUSetting();
					pGPUSetting->SetDeviceID(szDeviceID);

					std::string szDeviceName = settingObj.find("name")->second.get_str();
					pGPUSetting->SetDeviceName(szDeviceName);

					int nEngine = atoi(settingObj.find("gpu_engine")->second.get_str().c_str());
					pGPUSetting->SetEngineClock(nEngine);

					int nMemclock = atoi(settingObj.find("gpu_memclock")->second.get_str().c_str());
					pGPUSetting->SetMemclock(nMemclock);

					int nShaders = atoi(settingObj.find("gpu_shaders")->second.get_str().c_str());
					pGPUSetting->SetShaders(nShaders);

					int nThreads = atoi(settingObj.find("gpu_threads")->second.get_str().c_str());
					pGPUSetting->SetThreads(nThreads);

					int nVDDC = atoi(settingObj.find("gpu_vddc")->second.get_str().c_str());
					pGPUSetting->SetVoltage(nVDDC);

					int nPowerTune = atoi(settingObj.find("gpu_powertune")->second.get_str().c_str());
					pGPUSetting->SetPowerTune(nPowerTune);

					int nIntensity = atoi(settingObj.find("intensity")->second.get_str().c_str());
					pGPUSetting->SetIntensity(nIntensity);

					int nXIntensity = atoi(settingObj.find("xintensity")->second.get_str().c_str());
					pGPUSetting->SetXIntensity(nXIntensity);

					int nRawIntensity = atoi(settingObj.find("rawintensity")->second.get_str().c_str());
					pGPUSetting->SetRawIntensity(nRawIntensity);

					int nThreadConcurrency = atoi(settingObj.find("thread_concurrency")->second.get_str().c_str());
					pGPUSetting->SetThreadConcurrency(nThreadConcurrency);

					int nLookupGap = atoi(settingObj.find("lookup_gap")->second.get_str().c_str());
					pGPUSetting->SetLookupGap(nLookupGap);

					int nWorkSize = atoi(settingObj.find("worksize")->second.get_str().c_str());
					pGPUSetting->SetWorkSize(nWorkSize);

					int nVectors = atoi(settingObj.find("vectors")->second.get_str().c_str());
					pGPUSetting->SetVectors(nVectors);

					int nTempTarget = atoi(settingObj.find("temp_target")->second.get_str().c_str());
					pGPUSetting->SetTargetTemp(nTempTarget);

					int nTempOverheat = atoi(settingObj.find("temp_overheat")->second.get_str().c_str());
					pGPUSetting->SetOverHeatTemp(nTempOverheat);

					int nTempCutoff = atoi(settingObj.find("temp_cutoff")->second.get_str().c_str());
					pGPUSetting->SetCutOffTemp(nTempCutoff);

					vecGPUSettings.push_back(pGPUSetting);

					if (!settingExisted)
					{
						settingObj["saved"] = "true";

						std::cout << "Saving Settings for Device " << szDeviceID << std::endl;
						std::cout << std::endl;

						if (!pFileLoader->Save(szDeviceID + ".json", settingObj, "settings"))
						{
							std::cout << "Failed to Save Device " << szDeviceID << " Settings." << std::endl;
							std::cout << std::endl;
						}
					}
				}
			}

			//Setup GPUData vector
			std::cout << "Configuring all OpenCL Devices" << std::endl;
			std::cout << std::endl;

			m_vecGPUData = CreateOpenCLDevices(m_pRunOptions, vecGPUSettings);
		}
		else
		{
			m_vecGPUData = CreateOpenCLDevices(m_pRunOptions);
		}

		delete(pFileLoader);
		pFileLoader = NULL;

		std::cout << "Configuring AMD Overdrive Settings for Devices Found" << std::endl;
		std::cout << std::endl;

		m_pADL->SetupADLDevices(m_vecGPUData);		
		m_vecConnection = m_pRunOptions->GetConnections();

		m_vecLog.clear();
		std::string TheURL = m_vecConnection[0]->GetURL();
		size_t pos = TheURL.rfind(":");
		std::string TheIP = TheURL.substr(0,pos);
		std::string ThePort = TheURL.substr(pos+1, TheURL.size());		

		std::cout << "Connecting to Mining Server..." << std::endl;
		std::cout << std::endl;

		ServerConnection* pConnection = new SKServerConnection(m_vecGPUData,TheIP,ThePort);
		m_vecServerConnections.push_back(pConnection);
	}
	else
	{
		delete(pFileLoader);
		pFileLoader = NULL;
		std::cout << "SKMiner Failed to Start!" << std::endl;
	}
}

void Application::Shutdown()
{
	if (m_pAPIServer)
	{
		delete(m_pAPIServer);
		m_pAPIServer = NULL;
	}

	m_vecLog.clear();

	if(m_pADL)
	{
		m_pADL->Shutdown();
		delete(m_pADL);
		m_pADL = NULL;
	}
	
	for(size_t index = 0; index < m_vecGPUData.size(); ++index)
	{
		GPUData* pGPUData = m_vecGPUData[index];
		if (pGPUData)
		{
			delete(pGPUData);
			pGPUData = NULL;
		}
	}
	m_vecGPUData.clear();
	
	for (int srvIndex = 0; srvIndex < m_vecServerConnections.size(); ++srvIndex)
	{
		ServerConnection* pServerConnection = m_vecServerConnections[srvIndex];
		if (pServerConnection)
		{
			pServerConnection->SetIsShuttingDown(true);
			
			while (!pServerConnection->GetDidShutDown()){}

			delete(pServerConnection);
			pServerConnection = NULL;
		}
	}
	m_vecServerConnections.clear();

	if (m_pRunOptions)
	{
		delete(m_pRunOptions);
		m_pRunOptions = NULL;
	}

	m_pEventManager->Shutdown();
}

void Application::RemoveConnection(int nIndex)
{

}

void Application::Log(std::string szLogStr)
{
	m_vecLog.push_back(szLogStr);

	if(m_vecLog.size() > 10)
	{
		m_vecLog.erase(m_vecLog.begin());
	}

}

std::map<std::string, std::string> Application::ParseOptions(json_spirit::mObject confObj)
{
	std::map<std::string,std::string> optionsMap;

		
	json_spirit::mObject jsonObj = confObj;
	json_spirit::mArray arrConn = jsonObj.find("connections")->second.get_array();
			
	std::stringstream ssConnections;
	for (int connInd = 0; connInd < arrConn.size(); ++connInd)
	{
		if (connInd > 0)
		{
			ssConnections << ",";
		}

		json_spirit::mObject connObj = arrConn[connInd].get_obj();

		std::string szURL = connObj.find("url")->second.get_str();
		std::string szUser = connObj.find("user")->second.get_str();
		std::string szPassword = connObj.find("password")->second.get_str();

		ssConnections << szURL << " user " << szUser << " password " << szPassword;
	}

	std::string connValue = ssConnections.str();
	if (connValue.compare("") != 0)
	{
		optionsMap["--connections"] = connValue;
	}

	optionsMap["--algorithm"] = jsonObj.find("algorithm")->second.get_str();
	optionsMap["--auto-settings"] = jsonObj.find("auto-settings")->second.get_str();
	optionsMap["--gpu-engine"] = jsonObj.find("gpu-engine")->second.get_str();
	optionsMap["--gpu-memclock"] = jsonObj.find("gpu-memclock")->second.get_str();
	optionsMap["--gpu-shaders"] = jsonObj.find("gpu-shaders")->second.get_str();
	optionsMap["--gpu-threads"] = jsonObj.find("gpu-threads")->second.get_str();
	optionsMap["--gpu-powertune"] = jsonObj.find("gpu-powertune")->second.get_str();
	optionsMap["--gpu-vddc"] = jsonObj.find("gpu-vddc")->second.get_str();
	optionsMap["--intensity"] = jsonObj.find("intensity")->second.get_str();
	optionsMap["--xintensity"] = jsonObj.find("xintensity")->second.get_str();
	optionsMap["--rawintensity"] = jsonObj.find("rawintensity")->second.get_str();
	optionsMap["--thread-concurrency"] = jsonObj.find("thread-concurrency")->second.get_str();
	optionsMap["--lookup-gap"] = jsonObj.find("lookup-gap")->second.get_str();
	optionsMap["--worksize"] = jsonObj.find("worksize")->second.get_str();
	optionsMap["--temp-target"] = jsonObj.find("temp-target")->second.get_str();
	optionsMap["--temp-overheat"] = jsonObj.find("temp-overheat")->second.get_str();
	optionsMap["--temp-cutoff"] = jsonObj.find("temp-cutoff")->second.get_str();
	optionsMap["--auto-engine"] = jsonObj.find("auto-engine")->second.get_str();
	optionsMap["--auto-fan"] = jsonObj.find("auto-fan")->second.get_str();
	optionsMap["--submit-stale"] = jsonObj.find("submit-stale")->second.get_str();
	optionsMap["--vectors"] = jsonObj.find("vectors")->second.get_str();

	json_spirit::mObject::const_iterator findIT = jsonObj.find("devices");
	if (findIT != jsonObj.end() && findIT->first == "devices")
	{
		optionsMap["--devices"] = jsonObj.find("devices")->second.get_str();
	}

	return optionsMap;
}

bool Application::CheckOptions()
{
	if(m_pRunOptions->GetAlgorithm().compare("") == 0)
	{
		m_vecLog.push_back("The algorithm must be Set!");
		m_vecLog.push_back("Use either --algorithm or -A to set it");
		return false;
	}

	if(m_pRunOptions->GetConnections().size() == 0)
	{
		m_vecLog.push_back("At Least One Connection must be set!");
		m_vecLog.push_back("Use either --connection or -C to set it");
		return false;
	}

	return true;
}

json_spirit::mObject Application::GetDefaultConfig()
{
	json_spirit::mObject defConfig;

	json_spirit::mArray defConns;

	json_spirit::mObject firstConn;
	firstConn["url"] = "127.0.0.1:9325";
	firstConn["user"] = "x";
	firstConn["password"] = "x";

	defConns.push_back(firstConn);

	defConfig["connections"] = defConns;
	defConfig["algorithm"] = std::string("sk1024_djm");
	defConfig["gpu-engine"] = std::string("1050");
	defConfig["gpu-memclock"] = std::string("1500");
	defConfig["gpu-shaders"] = std::string("2048");
	defConfig["gpu-threads"] = std::string("1");
	defConfig["gpu-vddc"] = std::string("0");
	defConfig["gpu-powertune"] = std::string("0");
	defConfig["intensity"] = std::string("12");
	defConfig["xintensity"] = std::string("0");
	defConfig["rawintensity"] = std::string("0");
	defConfig["thread-concurrency"] = std::string("15232");
	defConfig["lookup-gap"] = std::string("2");
	defConfig["worksize"] = std::string("256");
	defConfig["temp-target"] = std::string("75");
	defConfig["temp-overheat"] = std::string("80");
	defConfig["temp-cutoff"] = std::string("95");
	defConfig["auto-engine"] = std::string("false");
	defConfig["auto-fan"] = std::string("false");
	defConfig["submit-stale"] = std::string("false");
	defConfig["vectors"] = std::string("1");
	defConfig["auto-settings"] = std::string("true");

	return defConfig;
}

void Application::HandleEvent(Event* pEvent)
{
	if (!pEvent)
	{
		return;
	}

	switch (pEvent->GetEventType())
	{
		case Event::EVENT_TYPE::GET_MINING_INFO:
		{
			json_spirit::mObject* pObj = new json_spirit::mObject(Application::GetData());
			EventManager::GetInstance()->AddEvent(Event::EVENT_TYPE::RETURN_MINING_INFO, ToVoid<json_spirit::mObject*>(pObj));
		}
		break;
	}
}

json_spirit::mObject Application::GetData()
{
	std::string szAlgorithm;
	json_spirit::mObject objData;

	objData["ADL_ACTIVE"] = m_pADL->GetIsActive();

	json_spirit::mArray connArr;
	for (size_t connInd = 0; connInd < m_vecServerConnections.size(); ++connInd)
	{
		json_spirit::mObject connObject;

		ServerConnection* pConn = m_vecServerConnections[connInd];

		connObject["ID"] = connInd;
		connObject["Hashes"] = pConn->GetCurrentHashes();
		connObject["Rolling"] = pConn->GetTotalRolling();
		connObject["Accepted"] = pConn->GetTotalAccepted();
		connObject["Rejected"] = pConn->GetTotalRejected();
		connObject["Hardware Errors"] = pConn->GetTotalRejected();

		int elapsedMS = pConn->GetTotalTimer().ElapsedMilliseconds();
		connObject["Time"] = elapsedMS;

		connObject["New Blocks"] = pConn->GetNewBlocks();

		connObject["URL"] = pConn->GetIP() + "::" + pConn->GetPort();
		connObject["IP"] = pConn->GetIP();
		connObject["Port"] = pConn->GetPort();
		connObject["User"] = "x"; //pConn->GetUser();

		if (pConn->GetCurrentHashes() > 0)
		{
			connObject["Status"] = "Alive";
		}
		else
		{
			connObject["Status"] = "Disabled";
		}

		connObject["Priority"] = 1;
		connObject["Quota"] = 0;
		connObject["Long Poll"] = "No";
		connObject["Getworks"] = 0;
		connObject["Works"] = 0;
		connObject["Discarded"] = 0;
		connObject["Stale"] = 0;
		connObject["Get Failures"] = 0;
		connObject["Remote Failures"] = 0;
		connObject["Last Share Time"] = 0;
		connObject["Diff1 Shares"] = 0;
		connObject["Proxy Type"] = "";
		connObject["Proxy"] = "";
		connObject["Difficulty Accepted"] = 0;
		connObject["Difficulty Rejected"] = 0;
		connObject["Difficulty Stale"] = 0;
		connObject["Last Share Difficulty"] = 0;
		connObject["Has Stratum"] = false;
		connObject["Stratum Active"] = false;
		connObject["Stratum URL"] = "";
		connObject["Has GBT"] = false;
		connObject["Best Share"] = 0;
		connObject["Pool Rejected%"] = 0.0f;
		connObject["Pool Stale%"] = 0.0f;

		connObject["Calls"] = 0;
		connObject["Wait"] = 0;
		connObject["Max"] = 0;
		connObject["Min"] = 0;
		connObject["Pool Calls"] = 0;
		connObject["Pool Attempts"] = 0;
		connObject["Pool Wait"] = 0;
		connObject["Pool Max"] = 0;
		connObject["Pool Min"] = 0;
		connObject["Pool Av"] = 0;
		connObject["Work Had Roll Time"] = 0;
		connObject["Work Can Roll"] = 0;
		connObject["Work Had Expire"] = 0;
		connObject["Work Roll Time"] = 0;
		connObject["Work Diff"] = 0;
		connObject["Min Diff"] = 0;
		connObject["Max Diff"] = 0;
		connObject["Min Diff Count"] = 0;
		connObject["Max Diff Count"] = 0;
		connObject["Times Sent"] = 0;
		connObject["Bytes Sent"] = 0;
		connObject["Times Recv"] = 0;
		connObject["Bytes Recv"] = 0;
		connObject["Net Bytes Sent"] = 0;
		connObject["Net Bytes Recv"] = 0;


		json_spirit::mArray devArr;
		std::vector<MinerThread*> pThreads = pConn->GetMiningThreads();
		for (size_t threadInd = 0; threadInd < pThreads.size(); ++threadInd)
		{
			json_spirit::mObject devObject;

			MinerThread* pMiningThread = pThreads[threadInd];
			MinerData* pMinerData = pMiningThread->GetMinerData();

			devObject["GPU"] = threadInd;
			devObject["Enabled"] = !pMinerData->GetGPUData()->GetIsDisabled();
			devObject["Status"] = "Alive";
			devObject["Long Name"] = pMinerData->GetGPUData()->GetGPUSetting()->GetDeviceName();

			devObject["Driver"] = ((CLDevice*)pMinerData->GetGPUData()->GetDevice())->GetOpenCLVersion();
			szAlgorithm = ((CLDevice*)pMinerData->GetGPUData()->GetDevice())->GetAlgorithm();
			devObject["Kernel"] = ((CLDevice*)pMinerData->GetGPUData()->GetDevice())->GetAlgorithm();
			devObject["Device Index"] = ((CLDevice*)pMinerData->GetGPUData()->GetDevice())->GetDeviceIndex();
			devObject["Device Name"] = ((CLDevice*)pMinerData->GetGPUData()->GetDevice())->GetName();
			devObject["Address Bits"] = (int)((CLDevice*)pMinerData->GetGPUData()->GetDevice())->GetAddressBits();
			devObject["Vendor Name"] = ((CLDevice*)pMinerData->GetGPUData()->GetDevice())->GetVendorName();

			ADLGPU* pADLGPU = NULL;

			int iAdapterIndex = pMinerData->GetGPUData()->GetAdapterIndex();
			if (iAdapterIndex > -1)
			{
				//Settings were autog configured so we have real data!
				pADLGPU = m_pADL->GetDevice(iAdapterIndex);
			}
			else
			{
				//Hopefully OpenCL and ADL are in order, not likely...
				pADLGPU = m_pADL->GetDeviceInOrder(pMinerData->GetGPUData()->GetID());
			}

			if (pADLGPU != NULL)
			{
				devObject["Name"] = pADLGPU->GetDisplayName();
				devObject["ID"] = pADLGPU->GetAdapterIndex();
				devObject["Model"] = pADLGPU->GetAdapterName();
				devObject["Device Path"] = pADLGPU->GetDriverPath();
				devObject["Temperature"] = m_pADL->GetGPUTemp(pADLGPU);
				devObject["Fan Speed"] = m_pADL->GetFanSpeed(pADLGPU);
				devObject["Fan Percent"] = m_pADL->GetFanSpeed(pADLGPU);
				devObject["GPU Clock"] = m_pADL->GetEngineClock(pADLGPU);
				devObject["Memory Clock"] = m_pADL->GetMemoryClock(pADLGPU);
				devObject["GPU Voltage"] = m_pADL->GetVDDC(pADLGPU);
				devObject["GPU Activity"] = m_pADL->GetActivity(pADLGPU);
				devObject["Powertune"] = m_pADL->GetPowertune(pADLGPU);
				devObject["Vendor ID"] = pADLGPU->GetVendorID();
				devObject["Sub System ID"] = pADLGPU->GetSubSystemID();
				devObject["Sub System Vendor ID"] = pADLGPU->GetSubSystemVendorID();
				devObject["PCI Device ID"] = pADLGPU->GetPCIDeviceID();
				devObject["Bios Part Number"] = pADLGPU->GetBiosPartNumber();
				devObject["Bios Version"] = pADLGPU->GetBiosVersion();
				devObject["Bios Date"] = pADLGPU->GetBiosDate();

			}
			else
			{
				devObject["Name"] = "";
				devObject["ID"] = 0;
				devObject["Model"] = "";
				devObject["Device Path"] = "";
				devObject["Temperature"] = 0.00f;
				devObject["Fan Speed"] = 0;
				devObject["Fan Percent"] = 0;
				devObject["GPU Clock"] = 0;
				devObject["Memory Clock"] = 0;
				devObject["GPU Voltage"] = 0.0f;
				devObject["GPU Activity"] = 0;
				devObject["Powertune"] = 0;
				devObject["Device Index"] = 0;
				devObject["Vendor ID"] = "";
				devObject["Sub System ID"] = "";
				devObject["Sub System Vendor ID"] = "";
				devObject["PCI Device ID"] = "";
				devObject["Bios Part Number"] = "";
				devObject["Bios Version"] = "";
				devObject["Bios Date"] = "";
			}

			devObject["Elapsed"] = elapsedMS;
			devObject["Calls"] = 0;
			devObject["Wait"] = 0;
			devObject["Max"] = 0;
			devObject["Min"] = 0;

			double elapsedSeconds = ((double)elapsedMS) * 0.001;

			double mhsAvg = pMiningThread->GetLastHashRate() * 0.001;
			devObject["MHS av"] = mhsAvg;
			devObject["MHS 10s"] = mhsAvg;

			devObject["Accepted"] = (int32)pMiningThread->GetBlocksFound();
			devObject["Rejected"] = (int32)pMiningThread->GetRejected();
			devObject["Hardware Errors"] = (int32)pMinerData->GetGPUData()->GetHardwareErrors();

			double dUtility = ((double)pMiningThread->GetBlocksFound()) / ((double)elapsedMS);
			devObject["Utility"] = dUtility;

			devObject["Intensity"] = pMinerData->GetGPUData()->GetGPUSetting()->GetIntensity();
			devObject["X Intensity"] = pMinerData->GetGPUData()->GetGPUSetting()->GetXIntensity();
			devObject["Raw Intensity"] = pMinerData->GetGPUData()->GetGPUSetting()->GetRawIntensity();

			devObject["Total MH"] = ((double)pMiningThread->GetTotalHashes()) * 0.001;
			devObject["Total Hashses"] = pMiningThread->GetTotalHashes();
			devObject["Last Hash Rate"] = pMiningThread->GetLastHashRate();

			//No Pools Yet
			devObject["Last Share Pool"] = 0;
			devObject["Last Share Pool Time"] = 0;
			devObject["Diff1 Work"] = 0;
			devObject["Difficulty Accepted"] = 0;
			devObject["Difficulty Rejected"] = 0;
			devObject["Last Share Difficulty"] = 0;
			devObject["Last Valid Work"] = 0;
			devObject["Device Hardware%"] = 0;
			devObject["Device Rejected%"] = 0;

			devObject["Device Elapsed"] = elapsedSeconds;

			devArr.push_back(devObject);
		}

		connObject["Devices"] = devArr;
		connArr.push_back(connObject);
	}


	objData["Connections"] = connArr;

	json_spirit::mObject coinObject;

	coinObject["Hash Method"] = szAlgorithm;
	coinObject["Current Block Time"] = 0.0f;
	coinObject["Current Block Hash"] = 0;
	coinObject["LP"] = false;
	coinObject["Network Difficulty"] = 0.0f;

	objData["Coin"] = coinObject;
	
	objData["Version"] = VERSION;

	json_spirit::mObject pObj;
	pObj["Data"] = objData;

	return pObj;
}