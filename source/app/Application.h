////////////////////////////////////////////////
// File:	Application.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <string>

#include "../connections/ServerConnection.h"
#include "../config/ConfigConnection.h"
#include "../event/Event.h"
#include "../event/EventManager.h"
#include "../event/IListener.h"
#include <json_spirit.h>

class ADL;
class GPUData;
class RunOptions;
class ConfigConnection;

namespace Http
{
	namespace Server
	{
		class WebServer;
	}
}


class Application : public IListener
{

private:

	bool m_bIsrunning;
	int m_nWidth;
	int m_nHeight;
	double m_dMouseX;
	double m_dMouseY;
	double m_dStartTime;

	Http::Server::WebServer* m_pAPIServer;
	ADL* m_pADL;
	EventManager* m_pEventManager;
	RunOptions* m_pRunOptions;
	std::vector<std::string> m_vecLog;
	std::vector<GPUData*> m_vecGPUData;
    std::vector<ConfigConnection*> m_vecConnection;
	std::vector<ServerConnection*> m_vecServerConnections;

	Application();
	Application(const Application& app);
	void operator=(const Application& app);

	std::map<std::string,std::string> ParseOptions(json_spirit::mObject confObj);
	bool CheckOptions();
	void CreateKey(const std::string szKeyName);	

	json_spirit::mObject GetDefaultConfig();

protected:

	static Application* m_app;
	void HandleEvent(Event* pEvent);

public:

	~Application();

	///////////////////////////////////////////////////////////////////////////////
	//Singleton Declaration
	///////////////////////////////////////////////////////////////////////////////
	static Application* GetInstance();

	void Initialize(std::map<std::string,std::string> arguments);	
	
    void Shutdown();

	void Log(std::string szLogStr);

	void AddConnection(ServerConnection* pConnection) { this->m_vecServerConnections.push_back(pConnection); }
	void RemoveConnection(int nIndex);
	void ClearConnections() { this->m_vecServerConnections.clear(); }

	const std::vector<ServerConnection*>& GetConnections() const { return this->m_vecServerConnections; }

	ADL* GetADL() const { return this->m_pADL; }

	json_spirit::mObject GetData();
};

#endif //_APPLICATION_H_