////////////////////////////////////////////////
// File:	Outbound.cpp
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "Outbound.h"
#include "Packet.h"
#include "types.h"

namespace LLP
{
	Outbound::Outbound() : Connection()
	{
	}

	Outbound::Outbound(std::string ip, std::string port) : Connection()
	{
		IP = ip;
		PORT = port;
	}

	Outbound::Outbound(const Outbound& outbound)
	{
	}
	
	Outbound& Outbound::operator=(const Outbound& outbound)
	{
		return *this;
	}
	
	Outbound::~Outbound()
	{
	}

	Packet Outbound::ReadNextPacket(int nTimeout)
	{
		Packet NULL_PACKET;
		while(!PacketComplete())
		{
			ReadPacket();
			if(Timeout(nTimeout) || Errors()) { return NULL_PACKET; }
			
			Sleep(1);
		}
			
		return *this->INCOMING;
	}

	bool Outbound::Connect()
	{
		try
		{
			using boost::asio::ip::tcp;
				
			tcp::resolver 			  RESOLVER(IO_SERVICE);
			tcp::resolver::query      QUERY(tcp::v4(), IP.c_str(), PORT.c_str());
			tcp::resolver::iterator   ADDRESS = RESOLVER.resolve(QUERY);
			
			if (this->SOCKET)
			{
				delete(this->SOCKET);
				this->SOCKET = NULL;
			}

			this->SOCKET = new Socket_t(new tcp::socket(IO_SERVICE));

			this->SOCKET->get()->connect(*ADDRESS, this->ERROR_HANDLE);
				
			if(Errors())
			{
				this->Disconnect();
					
				printf("Failed to Connect to Mining LLP Server...please verify IP=%s and port=%s\n",IP.c_str(),PORT.c_str());
				return false;
			}
				
			this->CONNECTED = true;
			this->TIMER->Start();
				
			printf("Connected to %s:%s...\n", IP.c_str(), PORT.c_str());

			return true;
		}
		catch(...)
		{ 
			this->Disconnect(); 
			return false;
		}
	}

	void Outbound::Disconnect()
	{
		if (!CONNECTED)
			return;

		try
		{
			if (this->SOCKET)
			{
				SOCKET->get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ERROR_HANDLE);
				SOCKET->get()->close();

				delete(this->SOCKET);
				this->SOCKET = NULL;
			}
		}
		catch (...){}

		CONNECTED = false;
	}
}