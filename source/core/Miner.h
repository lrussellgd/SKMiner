////////////////////////////////////////////////
// File:	Miner.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _MINER_H_
#define _MINER_H_

#include "Outbound.h"
#include "CBlock.h"

namespace LLP
{
	class Miner : public Outbound
	{
	private:

		Core::CBlock* DeserializeBlock(std::vector<unsigned char> DATA);
		std::vector<unsigned char> uint2bytes(unsigned int UINT);
		std::vector<unsigned char> uint2bytes64(unsigned long long UINT);
		unsigned int bytes2uint(std::vector<unsigned char> BYTES, int nOffset = 0) {
			return (BYTES[0 + nOffset] << 24) + (BYTES[1 + nOffset] << 16) + (BYTES[2 + nOffset] << 8) + BYTES[3 + nOffset];
		}

		uint64 bytes2uint64(std::vector<unsigned char> BYTES)
		{
			return (bytes2uint(BYTES) | ((uint64)bytes2uint(BYTES, 4) << 32));
		}


	public:

		enum
		{
			//DATA PACKETS
			BLOCK_DATA   = 0,
			SUBMIT_BLOCK = 1,
			BLOCK_HEIGHT = 2,
			SET_CHANNEL  = 3,
					
			//REQUEST PACKETS
			GET_BLOCK    = 129,
			GET_HEIGHT   = 130,
			
			//RESPONSE PACKETS
			GOOD     = 200,
			FAIL     = 201,
					
			//GENERIC
			PING     = 253,
			CLOSE    = 254
		};

		Miner();
		Miner(std::string ip, std::string port);
		Miner(const Miner& miner);
		Miner& operator=(const Miner& miner);
		~Miner();

		void SetChannel(unsigned int nChannel);
		
		Core::CBlock* GetBlock(int nTimeout = 30);
		unsigned int GetHeight(int nTimeout = 30);
		unsigned char SubmitBlock(uint512 hashMerkleRoot, unsigned long long nNonce, int nTimeout = 30);
	};
}



#endif //_MINER_H_