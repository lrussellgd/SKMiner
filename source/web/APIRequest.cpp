////////////////////////////////////////////////
// File:	APIRequest.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "APIRequest.h"

//////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
APIRequest::APIRequest()
{
	this->m_enmEntityType = ENTITY_TYPE::API_REQUEST;
}

//////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
APIRequest::APIRequest(	Http::Server::Request::RequestType enmRquestType,  
						std::string szCommand, 
						std::map<std::string, std::string> mapParameters) : APIRequest()
{
	this->m_enmRequestType = enmRquestType;
	this->m_szCommand = szCommand;
	this->m_mapParameters = mapParameters;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
APIRequest::~APIRequest()
{
	this->m_mapParameters.clear();
}

std::vector<std::string> APIRequest::ParametersAsList()
{
	std::vector<std::string> paramList;
	std::map<std::string, std::string>::iterator mapIt;
	for (mapIt = this->m_mapParameters.begin(); mapIt != this->m_mapParameters.end(); mapIt++)
	{
		paramList.push_back(mapIt->second);
	}

	return paramList;
}