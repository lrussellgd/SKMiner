////////////////////////////////////////////////
// File:	Outbound.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _OUTBOUND_H_
#define _OUTBOUND_H_

#include "Connection.h"

struct Service_t;

namespace LLP
{
	class Packet;

	class Outbound : public Connection
	{
	private:
		Service_t IO_SERVICE;
		std::string IP, PORT;

	public:

		Outbound();
		Outbound(std::string ip, std::string port);
		Outbound(const Outbound& outbound);
		Outbound& operator=(const Outbound& outbound);
		~Outbound();
		
		Packet ReadNextPacket(int nTimeout = 30);
		bool Connect();
	};
}


#endif //_OUTBOUND_H_