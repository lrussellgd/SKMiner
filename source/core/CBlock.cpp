////////////////////////////////////////////////
// File:	CBlock.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//			djm34
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//				2014-2015 djm34
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "CBlock.h"
#include "util.h"
#include "../hash/skein_port.h"

namespace Core
{
	CBlock::CBlock()
	{
		this->m_unVersion		= 0;
		this->m_hashPrevBlock	= 0;
		this->m_hashMerkleRoot	= 0;
		this->m_unHeight		= 0;
		this->m_unBits			= 0;
		this->m_ullNonce		= 0;
	}

	CBlock::CBlock(const CBlock& block)
	{
		this->m_unVersion		= block.GetVersion();
		this->m_hashPrevBlock	= block.GetPrevBlock();
		this->m_hashMerkleRoot	= block.GetMerkleRoot();
		this->m_unHeight		= block.GetHeight();
		this->m_unBits			= block.GetBits();
		this->m_ullNonce		= block.GetNonce();
	}

	CBlock& CBlock::operator=(const CBlock& block)
	{
		this->m_unVersion		= block.GetVersion();
		this->m_hashPrevBlock	= block.GetPrevBlock();
		this->m_hashMerkleRoot	= block.GetMerkleRoot();
		this->m_unHeight		= block.GetHeight();
		this->m_unBits			= block.GetBits();
		this->m_ullNonce		= block.GetNonce();

		return *this;
	}

	CBlock::~CBlock()
	{
	}

	const unsigned int* CBlock::GetData()
	{
		static unsigned char pblank[1];
		unsigned char* pbegin = (unsigned char *)&m_unVersion;
		unsigned char* pend = (unsigned char *)&m_ullNonce;
		u08b_t *msg = (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]);
		unsigned int * pData = (unsigned int*)msg;
		return pData;
	}
}