////////////////////////////////////////////////
// File:	Inbound.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "Inbound.h"
#include "Packet.h"
#include "types.h"

namespace LLP
{
	Inbound::Inbound() : PORT("13")
	{
		this->LISTENER = NULL;
		this->ENDPOINT = NULL;
	}

	Inbound::Inbound(std::string port)
	{
		this->LISTENER = NULL;
		this->ENDPOINT = NULL;

		this->PORT = port;
	}

	Inbound::Inbound(const Inbound& inbound)
	{
		this->LISTENER = inbound.GetListener();
		this->ENDPOINT = inbound.GetEndpoint();
		this->PORT = inbound.GetPort();
	}

	Inbound& Inbound::operator = (const Inbound& inbound)
	{
		this->LISTENER = inbound.GetListener();
		this->ENDPOINT = inbound.GetEndpoint();
		this->PORT = inbound.GetPort();

		return *this;
	}

	Inbound::~Inbound()
	{
		if (LISTENER)
		{
			delete(LISTENER);
			LISTENER = NULL;
		}

		if (ENDPOINT)
		{ 
			delete(ENDPOINT);
			ENDPOINT = NULL;
		}
	}

	void Inbound::Start()
	{
		if (LISTENER)
		{
			delete(LISTENER);
			LISTENER = NULL;
		}

		if (ENDPOINT)
		{
			delete(ENDPOINT);
			ENDPOINT = NULL;
		}

		ENDPOINT = new Endpoint_t(boost::asio::ip::tcp::v4(), atoi(this->PORT.c_str()));
		LISTENER = new Listener_t(IO_SERVICE, *ENDPOINT, true);

		this->SOCKET = new Socket_t(new boost::asio::ip::tcp::socket(LISTENER->get_io_service()));

		LISTENER->async_accept(*this->SOCKET->get(), boost::bind(&Inbound::HandleAccept, this, boost::asio::placeholders::error));
	}

	void Inbound::HandleAccept(const boost::system::error_code& ec)
	{
		if (!ec)
		{
			//Start Read Logic
		}
	}

	void Inbound::HandleWrite(const boost::system::error_code& ec, size_t numBytes)
	{
		if (!ec)
		{
			//Successfully Written
		}
	}
}