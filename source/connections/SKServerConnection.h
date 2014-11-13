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

public:

	SKServerConnection();
	SKServerConnection(std::vector<GPUData*> gpus, std::string ip, std::string port, int nMaxTimeout = 30);
	SKServerConnection& operator=(const SKServerConnection& serverConnection);
	~SKServerConnection();

	void ServerThread();

};

#endif //_SKSERVERCONNECTION_H_