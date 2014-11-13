////////////////////////////////////////////////
// File:	Miner.cpp
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#include "Miner.h"
#include "Packet.h"

namespace LLP
{
	Miner::Miner() : Outbound()
	{

	}
	
	Miner::Miner(std::string ip, std::string port) : Outbound(ip,port)
	{
	}


	Miner::Miner(const Miner& miner)
	{
	}

	Miner& Miner::operator=(const Miner& miner)
	{
		return *this;
	}

	Miner::~Miner()
	{
	}


	void Miner::SetChannel(unsigned int nChannel)
	{
		Packet* packet = new Packet();
		packet->SetHeader(SET_CHANNEL);
		packet->SetLength(4);
		packet->SetData(uint2bytes(nChannel));
			
		this->WritePacket(packet);

		delete(packet);
	}

	Core::CBlock* Miner::GetBlock(int nTimeout)
	{
		Packet* packet = new Packet();
		packet->SetHeader(GET_BLOCK);
		this->WritePacket(packet);
		delete(packet);	


		Packet RESPONSE = ReadNextPacket(nTimeout);
			
		if(RESPONSE.IsNull())
			return NULL;
				
		Core::CBlock* BLOCK = DeserializeBlock(RESPONSE.GetData());
		ResetPacket();
			
		return BLOCK;
	}

	unsigned int Miner::GetHeight(int nTimeout)
	{
		Packet* packet = new Packet();
		packet->SetHeader((unsigned char)GET_HEIGHT);
		this->WritePacket(packet);
		delete(packet);
			
		Packet RESPONSE = ReadNextPacket(nTimeout);
			
		if(RESPONSE.IsNull())
			return 0;
				
		if(RESPONSE.GetData().size() == 0)
		{
			return 0;
		}

		unsigned int nHeight = bytes2uint(RESPONSE.GetData());
		ResetPacket();
			
		return nHeight;
	}

	unsigned char Miner::SubmitBlock(uint512 hashMerkleRoot, unsigned long long nNonce, int nTimeout)
	{
		Packet* PACKET = new Packet();
		PACKET->SetHeader(SUBMIT_BLOCK);
			
		PACKET->SetData(hashMerkleRoot.GetBytes());
		std::vector<unsigned char> NONCE  = uint2bytes64(nNonce);
			
		std::vector<unsigned char> pckt = PACKET->GetData();
		pckt.insert(pckt.end(), NONCE.begin(), NONCE.end());
		PACKET->SetData(pckt);
		PACKET->SetLength(72);
			
		this->WritePacket(PACKET);
		delete(PACKET);

		Packet RESPONSE = ReadNextPacket(nTimeout);
		if(RESPONSE.IsNull())
			return 0;
			
		ResetPacket();
			
		return RESPONSE.GetHeader();
	}


	Core::CBlock* Miner::DeserializeBlock(std::vector<unsigned char> DATA)
	{
		Core::CBlock* BLOCK = new Core::CBlock();
		BLOCK->SetVersion(bytes2uint(std::vector<unsigned char>(DATA.begin(), DATA.begin() + 4)));
			
		uint1024 prevBlock = BLOCK->GetPrevBlock();
		prevBlock.SetBytes(std::vector<unsigned char>(DATA.begin() + 4, DATA.begin() + 132));
		BLOCK->SetPrevBlock(prevBlock);

		uint512 merkleRoot = BLOCK->GetMerkleRoot();
		merkleRoot.SetBytes(std::vector<unsigned char>(DATA.begin() + 132, DATA.end() - 20));
		BLOCK->SetMerkleRoot(merkleRoot);

		BLOCK->SetChannel(bytes2uint(std::vector<unsigned char>(DATA.end() - 20, DATA.end() - 16)));			
		BLOCK->SetHeight(bytes2uint(std::vector<unsigned char>(DATA.end() - 16, DATA.end() - 12)));
		BLOCK->SetBits(bytes2uint(std::vector<unsigned char>(DATA.end() - 12,  DATA.end() - 8)));
		BLOCK->SetNonce(bytes2uint64(std::vector<unsigned char>(DATA.end() - 8,  DATA.end())));
			
		return BLOCK;
	}

	std::vector<unsigned char> Miner::uint2bytes(unsigned int UINT)
	{
		std::vector<unsigned char> BYTES(4, 0);
		BYTES[0] = UINT >> 24;
		BYTES[1] = UINT >> 16;
		BYTES[2] = UINT >> 8;
		BYTES[3] = UINT;
			
		return BYTES;
	}

	std::vector<unsigned char> Miner::uint2bytes64(unsigned long long UINT)
	{
		std::vector<unsigned char> INTS[2];
		INTS[0] = uint2bytes((unsigned int)UINT);
		INTS[1] = uint2bytes((unsigned int)(UINT >> 32));
		std::vector<unsigned char> BYTES;
		BYTES.insert(BYTES.end(), INTS[0].begin(), INTS[0].end());
		BYTES.insert(BYTES.end(), INTS[1].begin(), INTS[1].end());
		return BYTES;

	}
	
}