////////////////////////////////////////////////
// File:	RunOptions.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _RUNOPTIONS_H_
#define _RUNOPTIONS_H_

#include <string>
#include <vector>
#include <map>

#include "../base/IPrototype.h"

class ConfigConnection;
class GPUSetting;

class RunOptions : public IPrototype
{
private:

	std::string						m_szAlgorithm;
	std::string						m_szDevices;
	std::vector<ConfigConnection*>	m_vecConnections;
	std::vector<GPUSetting*>		m_vecGPUSettings;	

public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	RunOptions();

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	RunOptions(std::map<std::string,std::string> arguments);

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	RunOptions(const RunOptions& options);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	RunOptions& operator=(const RunOptions& options);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~RunOptions();

	RunOptions* Clone();
	RunOptions* DeepCopy();

	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const std::string&						GetAlgorithm()		const			{	return this->m_szAlgorithm;					}
	const std::string&						GetDevices()		const			{	return this->m_szDevices;					}	
	const std::vector<ConfigConnection*>	GetConnections()	const			{	return this->m_vecConnections;				}
	const std::vector<GPUSetting*>			GetGPUSettings()	const			{	return this->m_vecGPUSettings;				}

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void	SetAlgorithm(const std::string& szAlgorithm)					{	this->m_szAlgorithm = szAlgorithm;				}
	void	SetDevices(const std::string& szDevices)						{	this->m_szDevices = szDevices;					}
	void	SetConnections(std::vector<ConfigConnection*> vecConnections)	{	this->m_vecConnections = vecConnections;		}
	void	SetGPUSettings(std::vector<GPUSetting*> vecGPUSettings)			{	this->m_vecGPUSettings = vecGPUSettings;		}
	void	AddConnection(ConfigConnection* connection)						{	this->m_vecConnections.push_back(connection);	}
	void	AddGPUSetting(GPUSetting* setting)								{	this->m_vecGPUSettings.push_back(setting);		}
};


#endif //_RUNOPTIONS_H_