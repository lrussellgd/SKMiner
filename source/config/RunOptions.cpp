////////////////////////////////////////////////
// File:	RunOptions.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "RunOptions.h"
#include "ConfigConnection.h"
#include "GPUSetting.h"
#include "../utilities/StringUtils.h"
#include <boost/algorithm/string.hpp>

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
RunOptions::RunOptions()
{
	this->m_enmEntityType = ENTITY_TYPE::RUN_OPTIONS;
	m_szAlgorithm = "";
	m_bIsAutoSettings = false;
}

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
RunOptions::RunOptions(std::map<std::string, std::string> arguments) : RunOptions()
{
	m_bIsAutoSettings = false;

	std::string szAlgorithm;
	std::string szConnection;
	std::string szEngine;
	std::string szMemclock;
	std::string szVoltage;
	std::string szShaders;
	std::string szPowertune;
	std::string szThreads;
	std::string szIntensity;
	std::string szAutoSettings;

	if(arguments.size() != 0)
	{
		std::map<std::string, std::string>::iterator algorithmIT = arguments.find("--algorithm");
		std::map<std::string, std::string>::iterator aIT = arguments.find("-A");

		if (algorithmIT != arguments.end())
		{
			SetAlgorithm(arguments["--algorithm"]);
		}
		else if (aIT != arguments.end())
		{
			SetAlgorithm(arguments["-A"]);
		}

		std::map<std::string,std::string>::iterator connectionIT = arguments.find("--connections");
		std::map<std::string,std::string>::iterator cIT = arguments.find("-C");
		if(connectionIT != arguments.end())
		{
			szConnection = arguments["--connections"];
		}
		else if(cIT != arguments.end())
		{
			szConnection = arguments["-C"];
		}

		if(szConnection.compare("") != 0)
		{
			std::vector<ConfigConnection*> vecConnections;

			size_t foundInd = szConnection.find(',');
			if(foundInd != std::string::npos)
			{
				std::vector<std::string> vecStrConnections = split(szConnection, ',');
				
				for(size_t conInd = 0; conInd < vecStrConnections.size(); ++conInd)
				{
					std::string conStr = vecStrConnections[conInd];
					size_t foundUser = conStr.find("user");

					std::string szURL = conStr.substr(0, foundUser);
					size_t endUrl = szURL.find(' ');
					szURL = szURL.substr(0, endUrl);

					size_t endUser = conStr.find("password");
					std::string szUser = conStr.substr(foundUser, (endUser - foundUser));
					size_t startUser = szUser.find(' ');
					szUser = szUser.substr(startUser + 1, endUser - (startUser + 1));
					size_t endUseName = szUser.find(' ');
					szUser = szUser.substr(0, endUseName);

					std::string szPassword = conStr.substr(endUser, conStr.length() - endUser);
					size_t startPwd = szPassword.find(' ');
					szPassword = szPassword.substr(startPwd + 1, (conStr.length() - endUser) - (startPwd + 1));

					ConfigConnection* conn = new ConfigConnection();
					conn->SetURL(szURL);
					conn->SetUser(szUser);
					conn->SetPassword(szPassword);

					m_vecConnections.push_back(conn);
				}

			}
			else
			{
				size_t foundUser = szConnection.find("user");

				std::string szURL = szConnection.substr(0, foundUser);
				size_t endUrl = szURL.find(' ');
				szURL = szURL.substr(0, endUrl);

				size_t endUser = szConnection.find("password");
				std::string szUser = szConnection.substr(foundUser, (endUser - foundUser));
				size_t startUser = szUser.find(' ');
				szUser = szUser.substr(startUser + 1, endUser - (startUser + 1));
				size_t endUseName = szUser.find(' ');
				szUser = szUser.substr(0, endUseName);

				std::string szPassword = szConnection.substr(endUser, szConnection.length() - endUser);
				size_t startPwd = szPassword.find(' ');
				szPassword = szPassword.substr(startPwd + 1, (szConnection.length() - endUser) - (startPwd + 1));

				ConfigConnection* conn = new ConfigConnection();
				conn->SetURL(szURL);
				conn->SetUser(szUser);
				conn->SetPassword(szPassword);

				m_vecConnections.push_back(conn);
			}
		}

		std::map<std::string, std::string>::iterator deviceIT = arguments.find("--devices");
		std::map<std::string, std::string>::iterator dIT = arguments.find("-D");

		if (deviceIT != arguments.end())
		{
			SetDevices(arguments["--devices"]);
		}
		else if (dIT != arguments.end())
		{
			SetDevices(arguments["-D"]);
		}


		std::map<std::string, std::string>::iterator autoSettingsIT = arguments.find("--auto-settings");
		std::map<std::string, std::string>::iterator autoSIT = arguments.find("-AS");
		if (autoSettingsIT != arguments.end())
		{
			szAutoSettings = arguments["--auto-settings"];
		}
		else if (autoSIT != arguments.end())
		{
			szAutoSettings = arguments["-AS"];
		}

		if (szAutoSettings.compare("") != 0 && szAutoSettings.compare(" ") != 0)
		{
			if (boost::iequals(szAutoSettings, "true"))
			{
				m_bIsAutoSettings = true;
			}
			else
			{
				m_bIsAutoSettings = false;
			}
		}

		//Check to see if GPU Settings will be automatically configured
		if (!m_bIsAutoSettings)
		{

			std::map<std::string, std::string>::iterator engineIT = arguments.find("--gpu-engine");
			std::map<std::string, std::string>::iterator eIT = arguments.find("-E");
			if (engineIT != arguments.end())
			{
				szEngine = arguments["--gpu-engine"];
			}
			else if (eIT != arguments.end())
			{
				szEngine = arguments["-E"];
			}

			if (szEngine.compare("") != 0 && szEngine.compare(" ") != 0)
			{
				size_t foundInd = szEngine.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrEngines = split(szEngine, ',');
					for (size_t engInd = 0; engInd < vecStrEngines.size(); ++engInd)
					{
						std::string engStr = vecStrEngines[engInd];
						long nEngine = strtol(engStr.c_str(), NULL, 0);

						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetEngineClock((int)nEngine);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
				else
				{
					long nEngine = strtol(szEngine.c_str(), NULL, 0);

					GPUSetting* gpuSetting = new GPUSetting();
					gpuSetting->SetEngineClock(nEngine);
					m_vecGPUSettings.push_back(gpuSetting);
				}
			}

			std::map<std::string, std::string>::iterator memclockIT = arguments.find("--gpu-memclock");
			std::map<std::string, std::string>::iterator mIT = arguments.find("-M");
			if (memclockIT != arguments.end())
			{
				szMemclock = arguments["--gpu-memclock"];
			}
			else if (mIT != arguments.end())
			{
				szMemclock = arguments["-M"];
			}

			if (szMemclock.compare("") != 0 && szMemclock.compare(" ") != 0)
			{
				size_t foundInd = szMemclock.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrMemclocks = split(szMemclock, ',');
					for (size_t memInd = 0; memInd < vecStrMemclocks.size(); ++memInd)
					{
						std::string memclockStr = vecStrMemclocks[memInd];
						long nMemclock = strtol(memclockStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= memInd)
						{
							m_vecGPUSettings[memInd]->SetMemclock(nMemclock);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetMemclock(nMemclock);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nMemclock = strtol(szMemclock.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetMemclock(nMemclock);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetMemclock(nMemclock);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::map<std::string, std::string>::iterator voltageIT = arguments.find("--gpu-vddc");
			std::map<std::string, std::string>::iterator vIT = arguments.find("-F");
			if (voltageIT != arguments.end())
			{
				szVoltage = arguments["--gpu-vddc"];
			}
			else if (vIT != arguments.end())
			{
				szVoltage = arguments["-F"];
			}

			if (szVoltage.compare("") != 0 && szVoltage.compare(" ") != 0)
			{
				size_t foundInd = szVoltage.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrVoltage = split(szVoltage, ',');
					for (size_t voltInd = 0; voltInd < vecStrVoltage.size(); ++voltInd)
					{
						std::string voltageStr = vecStrVoltage[voltInd];
						long nVoltage = strtol(voltageStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= voltInd)
						{
							m_vecGPUSettings[voltInd]->SetVoltage(((float)nVoltage) / 1000.0f);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetVoltage(((float)nVoltage) / 1000.0f);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nVoltage = strtol(szVoltage.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetVoltage((float)nVoltage);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetVoltage(((float)nVoltage) / 1000.0f);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::map<std::string, std::string>::iterator shadersIT = arguments.find("--gpu-shaders");
			std::map<std::string, std::string>::iterator sIT = arguments.find("-S");
			if (shadersIT != arguments.end())
			{
				szShaders = arguments["--gpu-shaders"];
			}
			else if (sIT != arguments.end())
			{
				szShaders = arguments["-S"];
			}

			if (szShaders.compare("") != 0 && szShaders.compare(" ") != 0)
			{
				size_t foundInd = szShaders.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrShaders = split(szShaders, ',');
					for (size_t shaderInd = 0; shaderInd < vecStrShaders.size(); ++shaderInd)
					{
						std::string shaderStr = vecStrShaders[shaderInd];
						long nShaders = strtol(shaderStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= shaderInd)
						{
							m_vecGPUSettings[shaderInd]->SetShaders((int)nShaders);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetShaders((int)nShaders);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nShaders = strtol(szShaders.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetShaders(nShaders);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetShaders((int)nShaders);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::map<std::string, std::string>::iterator powertuneIT = arguments.find("--gpu-powertune");
			std::map<std::string, std::string>::iterator pIT = arguments.find("-P");
			if (powertuneIT != arguments.end())
			{
				szPowertune = arguments["--gpu-powertune"];
			}
			else if (pIT != arguments.end())
			{
				szPowertune = arguments["-P"];
			}

			if (szPowertune.compare("") != 0 && szPowertune.compare(" ") != 0)
			{
				size_t foundInd = szPowertune.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrPowertune = split(szPowertune, ',');
					for (size_t powerInd = 0; powerInd < vecStrPowertune.size(); ++powerInd)
					{
						std::string powerStr = vecStrPowertune[powerInd];
						long nPower = strtol(powerStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= powerInd)
						{
							m_vecGPUSettings[powerInd]->SetPowerTune((int)nPower);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetPowerTune((int)nPower);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nPower = strtol(szPowertune.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetPowerTune(nPower);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetPowerTune((int)nPower);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::map<std::string, std::string>::iterator threadsIT = arguments.find("--gpu-threads");
			std::map<std::string, std::string>::iterator tIT = arguments.find("-T");
			if (threadsIT != arguments.end())
			{
				szThreads = arguments["--gpu-threads"];
			}
			else if (tIT != arguments.end())
			{
				szThreads = arguments["-T"];
			}

			if (szThreads.compare("") != 0 && szThreads.compare(" ") != 0)
			{
				size_t foundInd = szThreads.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrThreads = split(szThreads, ',');
					for (size_t threadInd = 0; threadInd < vecStrThreads.size(); ++threadInd)
					{
						std::string threadStr = vecStrThreads[threadInd];
						long nThreads = strtol(threadStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= threadInd)
						{
							m_vecGPUSettings[threadInd]->SetThreads((int)nThreads);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetThreads((int)nThreads);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nThreads = strtol(szThreads.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetThreads(nThreads);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetThreads((int)nThreads);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			//szIntensity
			std::map<std::string, std::string>::iterator intensityIT = arguments.find("--intensity");
			std::map<std::string, std::string>::iterator iIT = arguments.find("-I");
			if (intensityIT != arguments.end())
			{
				szIntensity = arguments["--intensity"];
			}
			else if (iIT != arguments.end())
			{
				szIntensity = arguments["-I"];
			}

			if (szIntensity.compare("") != 0 && szIntensity.compare(" ") != 0)
			{
				size_t foundInd = szIntensity.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrIntensity = split(szIntensity, ',');
					for (size_t intensityInd = 0; intensityInd < vecStrIntensity.size(); ++intensityInd)
					{
						std::string intensityStr = vecStrIntensity[intensityInd];
						long nIntensity = strtol(intensityStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= intensityInd)
						{
							m_vecGPUSettings[intensityInd]->SetIntensity((int)nIntensity);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetIntensity((int)nIntensity);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nIntensity = strtol(szIntensity.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetIntensity(nIntensity);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetIntensity((int)nIntensity);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::string szXIntensity;
			std::map<std::string, std::string>::iterator xintensityIT = arguments.find("--xintensity");
			std::map<std::string, std::string>::iterator xiIT = arguments.find("-X");
			if (xintensityIT != arguments.end())
			{
				szXIntensity = arguments["--xintensity"];
			}
			else if (xiIT != arguments.end())
			{
				szXIntensity = arguments["-X"];
			}

			if (szXIntensity.compare("") != 0 && szXIntensity.compare(" ") != 0)
			{
				size_t foundInd = szXIntensity.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrXIntensity = split(szXIntensity, ',');
					for (size_t xintensityInd = 0; xintensityInd < vecStrXIntensity.size(); ++xintensityInd)
					{
						std::string xintensityStr = vecStrXIntensity[xintensityInd];
						long nXIntensity = strtol(xintensityStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= xintensityInd)
						{
							m_vecGPUSettings[xintensityInd]->SetXIntensity((int)nXIntensity);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetXIntensity((int)nXIntensity);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nXIntensity = strtol(szXIntensity.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetXIntensity(nXIntensity);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetXIntensity((int)nXIntensity);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::string szRawIntensity;
			std::map<std::string, std::string>::iterator rawintensityIT = arguments.find("--rawintensity");
			std::map<std::string, std::string>::iterator rawiIT = arguments.find("-R");
			if (rawintensityIT != arguments.end())
			{
				szRawIntensity = arguments["--rawintensity"];
			}
			else if (rawiIT != arguments.end())
			{
				szRawIntensity = arguments["-R"];
			}

			if (szRawIntensity.compare("") != 0 && szRawIntensity.compare(" ") != 0)
			{
				size_t foundInd = szRawIntensity.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrRawIntensity = split(szRawIntensity, ',');
					for (size_t rawintensityInd = 0; rawintensityInd < vecStrRawIntensity.size(); ++rawintensityInd)
					{
						std::string rawintensityStr = vecStrRawIntensity[rawintensityInd];
						long nRawIntensity = strtol(rawintensityStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= rawintensityInd)
						{
							m_vecGPUSettings[rawintensityInd]->SetRawIntensity((int)nRawIntensity);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetRawIntensity((int)nRawIntensity);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nRawIntensity = strtol(szRawIntensity.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetRawIntensity(nRawIntensity);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetRawIntensity((int)nRawIntensity);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}


			std::map<std::string, std::string>::iterator tcIT = arguments.find("--thread-concurrency");
			std::string szTC = "";
			//std::map<std::string,std::string>::iterator IT = arguments.find("-E");
			if (tcIT != arguments.end())
			{
				szTC = arguments["--thread-concurrency"];
			}

			if (szTC.compare("") != 0 && szTC.compare(" ") != 0)
			{
				size_t foundInd = szTC.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrTCs = split(szTC, ',');
					for (size_t tcInd = 0; tcInd < vecStrTCs.size(); ++tcInd)
					{
						std::string tcStr = vecStrTCs[tcInd];
						long nThreadConcurrnecy = strtol(tcStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= tcInd)
						{
							m_vecGPUSettings[tcInd]->SetThreadConcurrency((int)nThreadConcurrnecy);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetThreadConcurrency((int)nThreadConcurrnecy);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nThreadConcurrnecy = strtol(szTC.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetThreadConcurrency(nThreadConcurrnecy);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetThreadConcurrency((int)nThreadConcurrnecy);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::map<std::string, std::string>::iterator lookupGapIT = arguments.find("--lookup-gap");
			std::map<std::string, std::string>::iterator lIT = arguments.find("-L");

			std::string szLG = "";
			if (lookupGapIT != arguments.end())
			{
				szLG = arguments["--lookup-gap"];
			}
			else if (lIT != arguments.end())
			{
				szLG = arguments["-L"];
			}

			if (szLG.compare("") != 0 && szLG.compare(" ") != 0)
			{
				size_t foundInd = szLG.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrLGs = split(szLG, ',');
					for (size_t lgInd = 0; lgInd < vecStrLGs.size(); ++lgInd)
					{
						std::string lgStr = vecStrLGs[lgInd];
						long nLookupGap = strtol(lgStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= lgInd)
						{
							m_vecGPUSettings[lgInd]->SetLookupGap((int)nLookupGap);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetLookupGap((int)nLookupGap);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nLookupGap = strtol(szLG.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetLookupGap(nLookupGap);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetLookupGap((int)nLookupGap);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::map<std::string, std::string>::iterator worksizeIT = arguments.find("--worksize");
			std::map<std::string, std::string>::iterator wIT = arguments.find("-W");

			std::string szWS = "";
			if (worksizeIT != arguments.end())
			{
				szWS = arguments["--worksize"];
			}
			else if (wIT != arguments.end())
			{
				szWS = arguments["-W"];
			}

			if (szWS.compare("") != 0 && szWS.compare(" ") != 0)
			{
				size_t foundInd = szWS.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrWSs = split(szWS, ',');
					for (size_t wsInd = 0; wsInd < vecStrWSs.size(); ++wsInd)
					{
						std::string wsStr = vecStrWSs[wsInd];
						long nWorkSize = strtol(wsStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= wsInd)
						{
							m_vecGPUSettings[wsInd]->SetWorkSize((int)nWorkSize);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetWorkSize((int)nWorkSize);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nWorkSize = strtol(szWS.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetWorkSize(nWorkSize);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetWorkSize((int)nWorkSize);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}

			std::map<std::string, std::string>::iterator verctorIT = arguments.find("--vectors");
			std::map<std::string, std::string>::iterator vecIT = arguments.find("-V");

			std::string szVectors = "";
			if (verctorIT != arguments.end())
			{
				szVectors = arguments["--vectors"];
			}
			else if (vecIT != arguments.end())
			{
				szVectors = arguments["-V"];
			}

			if (szVectors.compare("") != 0 && szVectors.compare(" ") != 0)
			{
				size_t foundInd = szVectors.find(',');
				if (foundInd != std::string::npos)
				{
					std::vector<std::string> vecStrVectorss = split(szVectors, ',');
					for (size_t vectorsInd = 0; vectorsInd < vecStrVectorss.size(); ++vectorsInd)
					{
						std::string vectorsStr = vecStrVectorss[vectorsInd];
						long nVectors = strtol(vectorsStr.c_str(), NULL, 0);

						if ((m_vecGPUSettings.size() - 1) >= nVectors)
						{
							m_vecGPUSettings[vectorsInd]->SetVectors((int)nVectors);
						}
						else
						{
							GPUSetting* gpuSetting = new GPUSetting();
							gpuSetting->SetVectors((int)nVectors);
							m_vecGPUSettings.push_back(gpuSetting);
						}
					}
				}
				else
				{
					long nVectors = strtol(szVectors.c_str(), NULL, 0);
					if (m_vecGPUSettings.size() > 0)
					{
						m_vecGPUSettings[0]->SetVectors(nVectors);
					}
					else
					{
						GPUSetting* gpuSetting = new GPUSetting();
						gpuSetting->SetVectors((int)nVectors);
						m_vecGPUSettings.push_back(gpuSetting);
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
RunOptions::RunOptions(const RunOptions& options) : Entity(options)
{
	this->SetAlgorithm(options.GetAlgorithm());

	std::vector<ConfigConnection*> vecConnections = options.GetConnections();
	for(size_t index = 0; index < vecConnections.size(); ++index)
	{
		this->m_vecConnections.push_back(vecConnections[index]);
	}

	std::vector<GPUSetting*> vecGPUs = options.GetGPUSettings();
	for(size_t index = 0; index < vecGPUs.size(); ++index)
	{
		this->m_vecGPUSettings.push_back(vecGPUs[index]);
	}
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
RunOptions& RunOptions::operator=(const RunOptions& options)
{
	this->m_enmEntityType = options.GetEntityType();
	this->SetAlgorithm(options.GetAlgorithm());

	const std::vector<ConfigConnection*> vecConnections = options.GetConnections();
	for(size_t index = 0; index < vecConnections.size(); ++index)
	{
		this->m_vecConnections.push_back(vecConnections[index]);
	}

	const std::vector<GPUSetting*> vecGPUs = options.GetGPUSettings();
	for(size_t index = 0; index < vecGPUs.size(); ++index)
	{
		this->m_vecGPUSettings.push_back(vecGPUs[index]);
	}

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
RunOptions::~RunOptions()
{
	for(size_t index = 0; index < m_vecConnections.size(); ++index)
	{
		ConfigConnection* pConfCon = m_vecConnections[index];
		if (pConfCon)
		{			
			delete(pConfCon);
			pConfCon = NULL;
		}		
	}
	m_vecConnections.clear();

	for(size_t index = 0; index < m_vecGPUSettings.size(); ++index)
	{
		GPUSetting* pSetting = m_vecGPUSettings[index];
		if (pSetting)
		{
			delete(pSetting);
			pSetting = NULL;
		}
	}
	m_vecGPUSettings.clear();
}

RunOptions* RunOptions::Clone()
{
	RunOptions* pRunOptions = new RunOptions();

	pRunOptions->SetAlgorithm(this->m_szAlgorithm);

	for (size_t index = 0; index < this->m_vecConnections.size(); ++index)
	{
		pRunOptions->AddConnection(this->m_vecConnections[index]);
	}

	for (size_t index = 0; index < this->m_vecGPUSettings.size(); ++index)
	{
		pRunOptions->AddGPUSetting(this->m_vecGPUSettings[index]);
	}

	return pRunOptions;
}

RunOptions* RunOptions::DeepCopy()
{
	RunOptions* pRunOptions = new RunOptions();

	pRunOptions->SetAlgorithm(this->m_szAlgorithm);

	for (size_t index = 0; index < this->m_vecConnections.size(); ++index)
	{
		pRunOptions->AddConnection(this->m_vecConnections[index]->DeepCopy());
	}

	for (size_t index = 0; index < this->m_vecGPUSettings.size(); ++index)
	{
		pRunOptions->AddGPUSetting(this->m_vecGPUSettings[index]->DeepCopy());
	}

	return pRunOptions;
}