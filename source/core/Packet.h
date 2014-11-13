////////////////////////////////////////////////
// File:	Packet.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//
//Definition:
//		Class to handle sending and receiving of LLP Packets.
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef _PACKET_H_
#define _PACKET_H_

#include <vector>

namespace LLP
{
	class Packet
	{
	private:

		////////////////////////////////
		// Components of an LLP Packet.
		//	BYTE 0       : Header
		//	BYTE 1 - 5   : Length
		//	BYTE 6 - End : Data      
		///////////////////////////////
		unsigned char    HEADER;
		unsigned int     LENGTH;
		std::vector<unsigned char> DATA;

	public:

		Packet();
		Packet(const Packet& packet);
		Packet& operator=(const Packet& packet);
		~Packet();


		inline void SetNull() { HEADER   = 255; LENGTH   = 0; DATA.clear(); }
		
		//Packet Null Flag. Header = 255.
		bool IsNull() { return (HEADER == 255); }
		
		//Determine if a packet is fully read.
		bool Complete() { return (Header() && DATA.size() == LENGTH); }
		
		//Determine if header is fully read.
		bool Header() { return IsNull() ? false : (HEADER < 128 && LENGTH > 0) || (HEADER >= 128 && HEADER < 255 && LENGTH == 0); }
		
		//Sets the size of the packet from Byte Vector.
		void SetLength(std::vector<unsigned char> BYTES) { LENGTH = (BYTES[0] << 24) + (BYTES[1] << 16) + (BYTES[2] << 8) + (BYTES[3] ); }

		// Serializes class into a Byte Vector. Used to write Packet to Sockets.
		std::vector<unsigned char> GetBytes();

		///////////////////////////////////////////////////////////////////////////////
		// Accessors
		///////////////////////////////////////////////////////////////////////////////
		const unsigned char					GetHeader() const	{	return this->HEADER;	}
		const unsigned int					GetLength() const	{	return this->LENGTH;	}
		const std::vector<unsigned char>	GetData()	const	{	return this->DATA;		}

		///////////////////////////////////////////////////////////////////////////////
		// Mutators
		///////////////////////////////////////////////////////////////////////////////
		void	SetHeader(unsigned char header)					{	this->HEADER = header;			}
		void	SetLength(unsigned int	length)					{	this->LENGTH = length;			}
		void	SetData(std::vector<unsigned char> data)		{	this->DATA = data;				}
		void	AddData(unsigned char chData)					{	this->DATA.push_back(chData);	}
	};
}

#endif //_PACKET_H_