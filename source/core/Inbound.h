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
#include <vector>

namespace LLP
{
	class Packet;

	class Inbound : public Connection
	{
	private:

		std::vector<unsigned char> m_vecReadData;

		void AsyncRead();
		void HandleHeader(const boost::system::error_code& ec, std::size_t bytes_transferred);
		void HandleLength(const boost::system::error_code& ec, std::size_t bytes_transferred);
		void HandleRead(const boost::system::error_code& ec, std::size_t bytes_transferred);
		void HandleComplete(const boost::system::error_code& ec, std::size_t bytes_transferred);

		int m_nConnectionID;

	public:

		Inbound();
		Inbound(Socket_t* SOCKET_IN, DDOS_Filter* DDOS_IN);
		Inbound(const Inbound& inbound);
		Inbound& operator=(const Inbound& inbound);
		~Inbound();

		void Start();

		const int GetConnectionID() const { return this->m_nConnectionID; }

		void SetConnectionID(int connID) { this->m_nConnectionID = connID; }
	};
}



#endif //_INBOUND_H_