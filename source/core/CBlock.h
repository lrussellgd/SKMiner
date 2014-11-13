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

#ifndef _CBLOCK_H_
#define _CBLOCK_H_

#include "../hash/uint1024.h"

namespace Core
{
	class CBlock
	{
	private:
		//Begin of Header.   BEGIN(nVersion)
		unsigned int		m_unVersion;
		uint1024			m_hashPrevBlock;
		uint512				m_hashMerkleRoot;
		unsigned int		m_unChannel;
		unsigned int		m_unHeight;
		unsigned int		m_unBits;
		unsigned long long	m_ullNonce;
		uint1024			m_hashData;
		//End of Header.     END(nNonce). 
		//	All the components to build an SK1024 Block Hash.
	public:		
			
		CBlock();
		CBlock(const CBlock& block);
		CBlock& operator=(const CBlock& block);
		~CBlock();
		
		const unsigned int * GetData();
		///////////////////////////////////////////////////////////////////////////////
		//Accessors
		///////////////////////////////////////////////////////////////////////////////
		const unsigned int			GetHeight()		const	{	return this->m_unHeight;				}
		const unsigned int			GetBits()		const	{	return this->m_unBits;					}
		const unsigned int			GetChannel()	const	{	return this->m_unChannel;				}
		const unsigned int			GetVersion()	const	{	return this->m_unVersion;				}
		const unsigned long long	GetNonce()		const	{	return this->m_ullNonce;				}
		const uint512&				GetMerkleRoot()	const	{	return this->m_hashMerkleRoot;			}
		const uint1024&				GetPrevBlock()	const	{	return this->m_hashPrevBlock;			}
		const uint1024&				GetHashData()	const	{	return this->m_hashData;				}

		///////////////////////////////////////////////////////////////////////////////
		//Mutators
		///////////////////////////////////////////////////////////////////////////////
		void	SetHeight(unsigned int unHeight)			{	this->m_unHeight = unHeight;			}
		void	SetBits(unsigned int unBits)				{	this->m_unBits = unBits;				}
		void	SetChannel(unsigned int unChannel)			{	this->m_unChannel = unChannel;			}
		void	SetVersion(unsigned int unVersion)			{	this->m_unVersion = unVersion;			}
		void	SetNonce(unsigned long long ullNonce)		{	this->m_ullNonce = ullNonce;			}
		void	SetMerkleRoot(uint512 merkleRoot)			{	this->m_hashMerkleRoot = merkleRoot;	}
		void	SetPrevBlock(uint1024 prevBlock)			{	this->m_hashPrevBlock = prevBlock;		}
		void	SetHashData(uint1024 hashData)				{	this->m_hashData = hashData;			}
		
	};

}

#endif