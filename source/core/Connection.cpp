////////////////////////////////////////////////
// File:	Connection.cpp
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "Connection.h"
#include "DDOS_Filter.h"

namespace LLP
{
	Connection::Connection()
	{
		this->DDOS = NULL;
		this->CONNECTED = false;		
		this->INCOMING = new Packet();
		this->INCOMING->SetNull();
		this->TIMER = new Timer();
		this->SOCKET = new Socket_t();
	}

	Connection::Connection(Socket_t* SOCKET_IN, DDOS_Filter* DDOS_IN)
	{
		if (SOCKET_IN)
		{
			this->SOCKET = SOCKET_IN;
		}

		if (DDOS_IN)
		{
			this->DDOS = new DDOS_Filter(*DDOS_IN);
		}
		
		this->CONNECTED = true;
		this->TIMER = new Timer();
		this->TIMER->Start();
		this->INCOMING = new Packet();
	}

	Connection::Connection(const Connection& connection)
	{
		this->SOCKET = connection.GetSocket();
		this->CONNECTED = connection.GetIsConnected();
		this->DDOS = new DDOS_Filter(*connection.GetDDOSFilter());
		this->INCOMING = new Packet(*connection.GetIncoming());
		this->TIMER = new Timer(*connection.GetTimer());
		this->ERROR_HANDLE = connection.GetErrorHandle();
	}
	
	Connection& Connection::operator=(const Connection& connection)
	{
		this->SOCKET = connection.GetSocket();
		this->CONNECTED = connection.GetIsConnected();
		this->DDOS = new DDOS_Filter(*connection.GetDDOSFilter());
		this->INCOMING = new Packet(*connection.GetIncoming());
		this->TIMER = new Timer(*connection.GetTimer());
		this->ERROR_HANDLE = connection.GetErrorHandle();

		return *this;
	}
	
	Connection::~Connection()
	{
		if (SOCKET)
		{
			delete(SOCKET);
			SOCKET = NULL;
		}

		if(INCOMING)
		{
			delete(INCOMING);
			INCOMING = NULL;
		}

		if(TIMER)
		{
			TIMER->Stop();
			delete(TIMER);
			TIMER = NULL;
		}

		if(DDOS)
		{
			delete(DDOS);
			DDOS = NULL;
		}
	}


	void Connection::WritePacket(Packet* PACKET)
	{
		if(Errors())
			return;
				
		Write(PACKET->GetBytes());
	}

	void Connection::ReadPacket()
	{
		if(Errors())
			return;
				
		/** Handle Reading Packet Type Header. **/
		if (!SOCKET)
		{
			return;
		}

		if(SOCKET->get()->available() > 0 && INCOMING->GetHeader() == 255)
		{
			std::vector<unsigned char> HEADER(1, 255);
			if(Read(HEADER, 1) == 1)
				INCOMING->SetHeader(HEADER[0]);
					
			return;
		}
				
		if(INCOMING->GetHeader() != 255 && !INCOMING->Complete())
		{
			
			/** Handle Reading Packet Length Header. **/
			if (SOCKET->get()->available() >= 4 && INCOMING->GetLength() == 0)
			{
				std::vector<unsigned char> BYTES(4, 0);
				if(Read(BYTES, 4) == 4)
				{
					INCOMING->SetLength(BYTES);
					Event(0);
				}
					
				return;
			}
					
			/** Handle Reading Packet Data. **/
			unsigned int nAvailable = (unsigned int)SOCKET->get()->available();
			if(nAvailable > 0 && INCOMING->GetLength() > 0 && INCOMING->GetData().size() < INCOMING->GetLength())
			{
				std::vector<unsigned char> DATA( std::min(nAvailable, (unsigned int)(INCOMING->GetLength() - INCOMING->GetData().size())), 0);
				unsigned int nRead = (unsigned int)Read(DATA, DATA.size());
					
				if(nRead == DATA.size())
				{
					std::vector<unsigned char> dta = INCOMING->GetData();
					try
					{
						dta.insert(dta.end(), DATA.begin(), DATA.end());
					}
					catch(const std::exception e)
					{
						return;
					}
					INCOMING->SetData(dta);

					Event(nRead);
				}
					
				return;
			}
		}
	}

	void Connection::Disconnect()
	{
		if(!CONNECTED)
			return;
				
		try
		{
			if (!SOCKET)
			{
				CONNECTED = false;
				return;
			}

			SOCKET->get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ERROR_HANDLE);
			SOCKET->get()->close();
		}
		catch(...){}
			
		CONNECTED = false;
	}

	size_t Connection::Read(std::vector<unsigned char> &DATA, size_t nBytes) 
	{ 
		TIMER->Reset(); return  boost::asio::read(*SOCKET->get(), boost::asio::buffer(DATA, nBytes), ERROR_HANDLE); 
	}

	void Connection::Write(std::vector<unsigned char> DATA) 
	{	
		TIMER->Reset(); boost::asio::write(*SOCKET->get(), boost::asio::buffer(DATA, DATA.size()), ERROR_HANDLE); 
	}

	bool Connection::Errors()
	{	
		return (ERROR_HANDLE == boost::asio::error::eof || ERROR_HANDLE); 
	}	
}