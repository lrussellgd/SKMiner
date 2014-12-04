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

//#define MHD_PLATFORM_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <string>
//#include "../ui/UIData.h"
#include "../connections/ServerConnection.h"
#include "../config/ConfigConnection.h"
#include <jansson.h>

#include "../core/Event.h"
#include "../core/EventManager.h"
#include "../core/IListener.h"

#define VERSION 1.0

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

	//UIData* m_pUIData;
	
	//APIServer* m_pAPIServer;
	Http::Server::WebServer* m_pAPIServer;
	ADL* m_pADL;
	Events::EventManager* m_pEventManager;
	RunOptions* m_pRunOptions;
	std::vector<std::string> m_vecLog;
	std::vector<GPUData*> m_vecGPUData;
    std::vector<ConfigConnection*> m_vecConnection;
	std::vector<ServerConnection*> m_vecServerConnections;

	Application();
	Application(const Application& app);
	void operator=(const Application& app);

	std::map<std::string,std::string> ParseOptions(std::string fileName);
	bool CheckOptions();
	void CreateKey(const std::string szKeyName);


protected:

	static Application* m_app;

	void HandleEvent(Events::Event* pEvent);

public:

	~Application();

	///////////////////////////////////////////////////////////////////////////////
	//Singleton Declaration
	///////////////////////////////////////////////////////////////////////////////
	static Application* GetInstance();

	void Initialize(std::map<std::string,std::string> arguments);	
	
    void Shutdown();

	void Log(std::string szLogStr);

	const std::vector<ServerConnection*>& GetConnections() const { return this->m_vecServerConnections; }
	ADL* GetADL() const { return this->m_pADL; }
};

#endif //_APPLICATION_H_