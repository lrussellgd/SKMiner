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

#include"../app/Application.h"
#include "../data/MinerData.h"
#include "../gpu//ADLGPU.h"
#include"../config/GPUSetting.h"
#include "../compute/BaseComputeDevice.h"
#include "../driver/ADL.h"

//////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
APIHandler::APIHandler()
{
	m_pApp = Application::GetInstance();
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
APIHandler::APIHandler(const APIHandler& apiHandler)
{
	m_mqRecentMessages = apiHandler.GetMessageQueue();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
APIHandler& APIHandler::operator = (const APIHandler& apiHandler)
{
	m_mqRecentMessages = apiHandler.GetMessageQueue();
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
APIHandler::~APIHandler()
{
	m_mqRecentMessages.clear();
}

std::string APIHandler::HandleRequest(const std::string& szQuery, const std::map<std::string, std::string>& mapParams)
{	
	json_spirit::Object obj;

	ProcessRequest(szQuery, mapParams, obj);

	std::string szFormatted = json_spirit::write(obj, json_spirit::Output_options::pretty_print);

	return szFormatted;
}

void APIHandler::ProcessRequest(const std::string& szQuery, const std::map<std::string, std::string>& mapParams, json_spirit::Object& pObject)
{
	/*if (szQuery.compare("getinfo") == 0)
	{
	}
	else*/ if (szQuery.compare("getmininginfo") == 0)
	{
		GetMiningInfo(pObject);
	}
	/*else if (szQuery.compare("getdevice") == 0)
	{

	}
	else if (szQuery.compare("getlog") == 0)
	{

	}*/
}

void APIHandler::GetMiningInfo(json_spirit::Object& pObject)
{
	std::vector<ServerConnection*> vecConnections = m_pApp->GetConnections();

	json_spirit::Array pConnArray;

	for (size_t srvIndex = 0; srvIndex < vecConnections.size(); ++srvIndex)
	{
		ServerConnection* pServerConnection = vecConnections[srvIndex];

		json_spirit::Object pConnData;
		pConnData.push_back(json_spirit::Pair("id", srvIndex));
		pConnData.push_back(json_spirit::Pair("ip", pServerConnection->GetIP()));
		pConnData.push_back(json_spirit::Pair("port", pServerConnection->GetPort()));
		pConnData.push_back(json_spirit::Pair("totalHashes", pServerConnection->GetCurrentHashes()));

		json_spirit::Array pThreadArr;

		std::vector<MinerThread*> vecThreads = pServerConnection->GetMiningThreads();
		for (size_t threadIndex = 0; threadIndex < vecThreads.size(); ++threadIndex)
		{
			MinerThread* pMinerThread = vecThreads[threadIndex];

			json_spirit::Object pThreadData;
			pThreadData.push_back(json_spirit::Pair("id", threadIndex));
			pThreadData.push_back(json_spirit::Pair("accepted", (int)pMinerThread->GetBlocksFound()));
			pThreadData.push_back(json_spirit::Pair("rejected", (int)pMinerThread->GetRejected()));
			pThreadData.push_back(json_spirit::Pair("hashRate", pMinerThread->GetLastHashRate()));

			MinerData* pMinerData = pMinerThread->GetMinerData();

			json_spirit::Object pGPUData;
			pGPUData.push_back(json_spirit::Pair("id", pMinerData->GetGPUData()->GetID()));
			pGPUData.push_back(json_spirit::Pair("intentsity", pMinerData->GetGPUData()->GetGPU()->GetGPUSetting()->GetIntensity()));
			pGPUData.push_back(json_spirit::Pair("xintentsity", pMinerData->GetGPUData()->GetGPU()->GetGPUSetting()->GetXIntensity()));
			pGPUData.push_back(json_spirit::Pair("rawintentsity", pMinerData->GetGPUData()->GetGPU()->GetGPUSetting()->GetRawIntensity()));
			pGPUData.push_back(json_spirit::Pair("algorithm", ((BaseComputeDevice*)pMinerData->GetGPUData()->GetDevice())->GetAlgorithm()));
			pGPUData.push_back(json_spirit::Pair("temperature", m_pApp->GetADL()->GetGPUTemp(pMinerData->GetGPUData())));

			if (pMinerData->GetGPUData()->GetGPU()->GetGPUSetting()->GetHasFanSpeed())
			{
				pGPUData.push_back(json_spirit::Pair("fan", m_pApp->GetADL()->GetFanSpeed(pMinerData->GetGPUData())));
			}
			else
			{
				pGPUData.push_back(json_spirit::Pair("fan", m_pApp->GetADL()->GetFanPercent((ADLGPU*)pMinerData->GetGPUData()->GetGPU())));
			}

			pThreadData.push_back(json_spirit::Pair("gpu", pGPUData));

			pThreadArr.push_back(pThreadData);
		}

		pConnData.push_back(json_spirit::Pair("threads", pThreadArr));

		pConnArray.push_back(pConnData);
	}

	pObject.push_back(json_spirit::Pair("miningInfo", pConnArray));
}