////////////////////////////////////////////////
// File:	Application.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

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
#include "../compute/CLFuncs.h"
#include "../compute/BaseComputeDevice.h"
#include "../config/RunOptions.h"
#include "../config/ConfigConnection.h"
#include "../config/GPUSetting.h"
#include "../connections/SKServerConnection.h"
#include "../utilities/JSONUtils.h"
#include "../threads/MinerThread.h"
#include "../data/MinerData.h"

#include "../web/WebServer.h"

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
	//std::string szToday = to_simple_string(day_clock::local_day());
	
	std::map<std::string, std::string> mapOptions = ParseOptions(".\\resources\\config\\miner.conf");

	for (std::map<std::string, std::string>::iterator iterator = arguments.begin(); iterator != arguments.end(); iterator++)
	{		
		mapOptions[iterator->first] = iterator->second;
	}

	m_pRunOptions = new RunOptions(mapOptions);
	
	if (CheckOptions())
	{

		m_pEventManager = Events::EventManager::GetInstance();
		m_pEventManager->Initialize();

		m_vecGPUData = CreateOpenCLDevices(m_pRunOptions);
		m_vecConnection = m_pRunOptions->GetConnections();
		m_pADL = new ADL(m_vecGPUData);
		//m_pUIData = new UIData(".\\resources\\ui\\minerui.xml", m_vecGPUData.size());

		//std::stringstream ssVersion;
		//ssVersion << VERSION;
		//m_pUIData->SetParameter("VERSION", ssVersion.str());
		//m_pUIData->SetParameter("DATE", szToday);

		//httpserver(8383);
		//MyStubServer s(httpserver);
		//s.StartListening();

		//s.StopListening();

		//jsonrpc::HttpServer httpServer(8383);
		//m_pAPIServer = new APIServer(httpServer);

		//m_pAPIServer->StartListening();

		m_pAPIServer = new Http::Server::WebServer("127.0.0.1", "8383", "C:\\");
		m_pAPIServer->Run();

		m_vecLog.clear();
		std::string TheURL = m_vecConnection[0]->GetURL();
		size_t pos = TheURL.rfind(":");
		std::string TheIP = TheURL.substr(0,pos);
		std::string ThePort = TheURL.substr(pos+1, TheURL.size());

		std::cout << "SKMiner for AMD GPUs" << std::endl;

		ServerConnection* pConnection = new SKServerConnection(m_vecGPUData,TheIP,ThePort);

		m_vecServerConnections.push_back(pConnection);
	}
}

void Application::Shutdown()
{
	
	/*if (m_pAPIServer)
	{
		m_pAPIServer->StopListening();
		delete(m_pAPIServer);
		m_pAPIServer = NULL;
	}*/

	if (m_pAPIServer)
	{
		delete(m_pAPIServer);
		m_pAPIServer = NULL;
	}

	m_vecLog.clear();

	

	//if(m_pUIData)
	//{
		//delete(m_pUIData);
		//m_pUIData = NULL;
	//}

	if(m_pADL)
	{
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

void Application::Log(std::string szLogStr)
{
	m_vecLog.push_back(szLogStr);

	if(m_vecLog.size() > 10)
	{
		m_vecLog.erase(m_vecLog.begin());
	}

}

std::map<std::string,std::string> Application::ParseOptions(std::string fileName)
{
	std::map<std::string,std::string> optionsMap;

	std::stringstream fileData;
	
	ifstream myfile (fileName);
	if(myfile.is_open())
	{
		std::string szline;
		while (getline(myfile,szline))
		{
			fileData << szline;
		}
	}
	else
	{
		return optionsMap;
	}

	json_t* jsonDta;

	std::string szData = fileData.str();
	if(szData.compare("") != 0)
	{
		std::string jsonStr(szData);
		jsonDta = Parse_JSON(jsonStr);

		if(!jsonDta)
		{
			return optionsMap;
		}
		
		json_t* connections = json_object_get(jsonDta, "connections");
		if (connections != NULL && json_is_array(connections))
		{
			std::stringstream ssConnections;

			for (int arrInd = 0; arrInd < json_array_size(connections); ++arrInd)
			{
				if (arrInd > 0)
				{
					ssConnections << ",";
				}

				json_t* jsonArrDta = json_array_get(connections, arrInd);
				if (!json_is_object(jsonArrDta))
				{
					continue;
				}

				json_t* urlStr = json_object_get(jsonArrDta, "url");
				if (!json_is_string(urlStr))
				{
					continue;
				}

				std::string szURL(json_string_value(urlStr));

				json_t* userStr = json_object_get(jsonArrDta, "user");
				if (!json_is_string(userStr))
				{
					continue;
				}

				std::string szUser(json_string_value(userStr));

				json_t* passStr = json_object_get(jsonArrDta, "password");
				if (!json_is_string(passStr))
				{
					continue;
				}

				std::string szPassword(json_string_value(passStr));

				ssConnections << szURL << " user " << szUser << " password " << szPassword;
			}

			std::string connValue = ssConnections.str();
			if (connValue.compare("") != 0)
			{
				optionsMap["--connections"] = connValue;
			}
		}

		json_t* algorithm = json_object_get(jsonDta, "algorithm");
		if (algorithm != NULL && json_is_string(algorithm))
		{
			optionsMap["--algorithm"] = json_string_value(algorithm);
		}

		json_t* gpuEngine = json_object_get(jsonDta, "gpu-engine");
		if (gpuEngine != NULL && json_is_string(gpuEngine))
		{
			optionsMap["--gpu-engine"] = json_string_value(gpuEngine);
		}

		json_t* gpuMemclock = json_object_get(jsonDta, "gpu-memclock");
		if (gpuMemclock != NULL && json_is_string(gpuMemclock))
		{
			optionsMap["--gpu-memclock"] = json_string_value(gpuMemclock);
		}		

		json_t* gpuShaders = json_object_get(jsonDta, "gpu-shaders");
		if (gpuShaders != NULL && json_is_string(gpuShaders))
		{
			optionsMap["--gpu-shaders"] = json_string_value(gpuShaders);
		}		

		json_t* gpuThreads = json_object_get(jsonDta, "gpu-threads");
		if (gpuThreads != NULL && json_is_string(gpuThreads))
		{
			optionsMap["--gpu-threads"] = json_string_value(gpuThreads);
		}
		
		json_t* gpuPowertune = json_object_get(jsonDta, "gpu-powertune");
		if (gpuPowertune != NULL && json_is_string(gpuPowertune))
		{
			optionsMap["--gpu-powertune"] = json_string_value(gpuPowertune);
		}

		json_t* gpuVDDC = json_object_get(jsonDta, "gpu-vddc");
		if (gpuVDDC != NULL && json_is_string(gpuVDDC))
		{
			optionsMap["--gpu-vddc"] = json_string_value(gpuVDDC);
		}		

		json_t* intensity = json_object_get(jsonDta, "intensity");
		if (intensity != NULL && json_is_string(intensity))
		{
			optionsMap["--intensity"] = json_string_value(intensity);
		}

		json_t* xintensity = json_object_get(jsonDta, "xintensity");
		if (xintensity != NULL && json_is_string(xintensity))
		{
			optionsMap["--xintensity"] = json_string_value(xintensity);
		}		

		json_t* rawintensity = json_object_get(jsonDta, "rawintensity");
		if (rawintensity != NULL && json_is_string(rawintensity))
		{
			optionsMap["--rawintensity"] = json_string_value(rawintensity);
		}

		json_t* threadConcurrency = json_object_get(jsonDta, "thread-concurrency");
		if (threadConcurrency != NULL && json_is_string(threadConcurrency))
		{
			optionsMap["--thread-concurrency"] = json_string_value(threadConcurrency);
		}	

		json_t* lookupGap = json_object_get(jsonDta, "lookup-gap");
		if (lookupGap != NULL && json_is_string(lookupGap))
		{
			optionsMap["--lookup-gap"] = json_string_value(lookupGap);
		}		

		json_t* worksize = json_object_get(jsonDta, "worksize");
		if (worksize != NULL && json_is_string(worksize))
		{
			optionsMap["--worksize"] = json_string_value(worksize);
		}

		json_t* tempTarget = json_object_get(jsonDta, "temp-target");
		if (tempTarget != NULL && json_is_string(tempTarget))
		{
			optionsMap["--temp-target"] = json_string_value(tempTarget);
		}		

		json_t* tempOverheat = json_object_get(jsonDta, "temp-overheat");
		if (tempOverheat != NULL && json_is_string(tempOverheat))
		{
			optionsMap["--temp-overheat"] = json_string_value(tempOverheat);
		}		

		json_t* tempCutoff = json_object_get(jsonDta, "temp-cutoff");
		if (tempCutoff != NULL && json_is_string(tempCutoff))
		{
			optionsMap["--temp-cutoff"] = json_string_value(tempCutoff);
		}		

		json_t* autoEngine = json_object_get(jsonDta, "auto-engine");
		if (autoEngine != NULL && json_is_string(autoEngine))
		{
			optionsMap["--auto-engine"] = json_string_value(autoEngine);
		}		

		json_t* autoFan = json_object_get(jsonDta, "auto-fan");
		if (autoFan != NULL && json_is_string(autoFan))
		{
			optionsMap["--auto-fan"] = json_string_value(autoFan);
		}		

		json_t* submitStale = json_object_get(jsonDta, "submit-stale");
		if (submitStale != NULL && json_is_string(submitStale))
		{
			optionsMap["--submit-stale"] = json_string_value(submitStale);
		}		

		json_t* vectors = json_object_get(jsonDta, "vectors");
		if (vectors != NULL && json_is_string(vectors))
		{
			optionsMap["--vectors"] = json_string_value(vectors);
		}

		json_t* devices = json_object_get(jsonDta, "devices");
		if (devices != NULL && json_is_string(devices))
		{
			optionsMap["--devices"] = json_string_value(devices);
		}

		
	}

	if (jsonDta)
	{
		delete(jsonDta);
		jsonDta = NULL;
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

void Application::HandleEvent(Events::Event* pEvent)
{
}