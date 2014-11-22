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
//#include "../ui/UIData.h"
#include "../connections/ServerConnection.h"
#include "../config/ConfigConnection.h"
#include <jansson.h>

#define VERSION 1.0

class ADL;
class GPUData;
class RunOptions;
class ConfigConnection;

class Application
{

private:

	bool m_bIsrunning;
	int m_nWidth;
	int m_nHeight;
	double m_dMouseX;
	double m_dMouseY;
	double m_dStartTime;

	//UIData* m_pUIData;
	
	ADL* m_pADL;
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

public:

	~Application();

	///////////////////////////////////////////////////////////////////////////////
	//Singleton Declaration
	///////////////////////////////////////////////////////////////////////////////
	static Application* GetInstance();

	void Initialize(std::map<std::string,std::string> arguments);	
	
    void Shutdown();

	void Log(std::string szLogStr);
};

#endif //_APPLICATION_H_