////////////////////////////////////////////////
// File:	Connection.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "Timer.h"
#include "Packet.h"

#include "types.h"

namespace LLP
{
	class DDOS_Filter;

	class Connection
	{
	protected:
		
		//Incoming Packet Being Built.
		Packet*        INCOMING;
		
		// Basic Connection Variables.
		Timer*        TIMER;
		Error_t       ERROR_HANDLE;
		Socket_t      SOCKET;
		
		//Connected Flag.
		bool CONNECTED;

		//  Virtual Event Function to be Overridden allowing Custom Read Events. 
		//	Each event fired on Header Complete, and each time data is read to fill packet.
		//	Useful to check Header length to maximum size of packet type for DDOS protection, 
		//	sending a keep-alive ping while downloading large files, etc.
		//
		//	nSize == 0 : Header Is Complete for Data Packet
		//	nSize  > 0 : Read nSize Bytes into Data Packet  
		virtual inline void Event(unsigned int nSize = 0){ }

	private:

		DDOS_Filter*   DDOS;

		// Lower level network communications: Read. Interacts with OS sockets.
		size_t Read(std::vector<unsigned char> &DATA, size_t nBytes);
											
		// Lower level network communications: Write. Interacts with OS sockets.
		void Write(std::vector<unsigned char> DATA);

	public:

		Connection();
		Connection(Socket_t SOCKET_IN, DDOS_Filter* DDOS_IN);
		Connection(const Connection& connection);
		Connection& operator=(const Connection& connection);
		~Connection();


		// Checks for any flags in the Error Handle.
		bool Errors();			
				
		// Determines if nTime seconds have elapsed since last Read / Write.
		bool Timeout(unsigned int nTime){ return (TIMER->Elapsed() >= nTime); }		
		
		// Flag to determine if TCP Connection is still alive.
		bool Connected(){ return CONNECTED; }		
		
		// Handles two types of packets, requests which are of header >= 128, and data which are of header < 128.
		bool PacketComplete(){ return INCOMING->Complete(); }		
		
		// Used to reset the packet to Null after it has been processed. This then flags the Connection to read another packet.
		void ResetPacket(){ INCOMING->SetNull(); }

		//Write a single packet to the TCP stream.
		void WritePacket(Packet* PACKET);
		
		//Non-Blocking Packet reader to build a packet from TCP Connection.
		//This keeps thread from spending too much time for each Connection.
		void ReadPacket();

		//Disconnect Socket. Cleans up memory usage to prevent "memory runs" from poor memory management.
		void Disconnect();

		///////////////////////////////////////////////////////////////////////////////
		//Accessors
		///////////////////////////////////////////////////////////////////////////////
		const bool			GetIsConnected()	const	{	return this->CONNECTED;				}
		const Packet*		GetIncoming()		const	{	return this->INCOMING;				}
		const Timer*		GetTimer()			const	{	return this->TIMER;					}
		const DDOS_Filter*	GetDDOSFilter()		const	{	return this->DDOS;					}
		const Error_t&		GetErrorHandle()	const	{	return this->ERROR_HANDLE;			}
		const Socket_t&		GetSocket()			const	{	return this->SOCKET;				}


		///////////////////////////////////////////////////////////////////////////////
		//Mutators
		///////////////////////////////////////////////////////////////////////////////
		void	SetIsConneced(bool bIsConnected)		{	this->CONNECTED = bIsConnected;		}
		void	SetIncoming(Packet* incoming)			{	this->INCOMING = incoming;			}
		void	SetTimer(Timer* timer)					{	this->TIMER = timer;				}
		void	SetDDOSFilter(DDOS_Filter* filter)		{	this->DDOS = filter;				}
		void	SetErrorHandle(Error_t errorHandle)		{	this->ERROR_HANDLE = errorHandle;	}
		void	SetSocket(Socket_t socket)				{	this->SOCKET = socket;				}
	};
}

#endif
