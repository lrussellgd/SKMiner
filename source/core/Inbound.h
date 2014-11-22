////////////////////////////////////////////////
// File:	Inbound.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _INBOUND_H_
#define _INBOUND_H_

#include "Connection.h"

struct Service_t;
struct Listener_t;

namespace LLP
{
	class Packet;

	class Inbound : public Connection
	{
	private:

		Listener_t* LISTENER;
		Endpoint_t* ENDPOINT;
		Service_t IO_SERVICE;
		std::string PORT;

	public:

		Inbound();
		Inbound(std::string port);
		Inbound(const Inbound& inbound);
		Inbound& operator=(const Inbound& inbound);
		~Inbound();

		void Start();
		void HandleAccept(const boost::system::error_code& ec);
		void HandleWrite(const boost::system::error_code& ec, size_t numBytes);

		const std::string&	GetPort()		const {		return this->PORT;		}
		Listener_t*			GetListener()	const {		return this->LISTENER;	}
		Endpoint_t*			GetEndpoint()	const {		return this->ENDPOINT;	}
	};
}



#endif //_INBOUND_H_