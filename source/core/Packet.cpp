////////////////////////////////////////////////
// File:	Packet.cpp
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "Packet.h"
#include "types.h"

namespace LLP
{
	Packet::Packet()
	{
		HEADER = ' ';
		LENGTH = 0;

	}

	Packet::Packet(const Packet& packet)
	{
		this->HEADER = packet.GetHeader();
		this->LENGTH = packet.GetLength();
		this->DATA = packet.GetData();

	}

	Packet& Packet::operator=(const Packet& packet)
	{
		this->HEADER = packet.GetHeader();
		this->LENGTH = packet.GetLength();
		this->DATA = packet.GetData();

		return *this;
	}
		
	Packet::~Packet()
	{
	}

	std::vector<unsigned char> Packet::GetBytes()
	{
		std::vector<unsigned char> BYTES(1, HEADER);
			
		/** Handle for Data Packets. **/
		if(HEADER < 128)
		{
			BYTES.push_back((LENGTH >> 24)); BYTES.push_back((LENGTH >> 16));
			BYTES.push_back((LENGTH >> 8));  BYTES.push_back(LENGTH);
				
			BYTES.insert(BYTES.end(),  DATA.begin(), DATA.end());
		}
			
		return BYTES;
	}


}