////////////////////////////////////////////////
// File:	SKServerConnection.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _SKSERVERCONNECTION_H_
#define _SKSERVERCONNECTION_H_

#include "ServerConnection.h"


class SKServerConnection : public ServerConnection
{

private:

	std::vector<GPUData*> m_vecGPUs;

public:

	SKServerConnection();
	SKServerConnection(const SKServerConnection& skServerConnection);
	SKServerConnection(std::vector<GPUData*> gpus, std::string ip, std::string port, int nMaxTimeout = 30);
	SKServerConnection& operator=(const SKServerConnection& serverConnection);
	~SKServerConnection();

	void ServerThread();

	const std::vector<GPUData*> GetGPUs()	const { return this->m_vecGPUs; }

};

#endif //_SKSERVERCONNECTION_H_