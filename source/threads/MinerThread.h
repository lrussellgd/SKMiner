////////////////////////////////////////////////
// File:	MinerThread.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _MINERTHREAD_H_
#define _MINERTHREAD_H_

#include "../core/types.h"
#include "../gpu/GPUData.h"
#include <boost/thread/mutex.hpp>
 
class MinerData;

typedef bool(*HashFuncPtrType)(MinerData*);

class MinerThread
	{

	protected:

		MinerData* m_pMinerData;
		bool m_bBlockFound, m_bNewBlock, m_bReady;
		LLP::Thread_t* m_pTHREAD;
		unsigned int m_unHashes;
		double m_dLastHashRate;
		double total_mhashes_done;
		boost::mutex* m_clLock;
		bool m_bShutown;
		bool m_bDidShutDown;
		size_t m_stBlocksFound;
		size_t m_stRejected;

		HashFuncPtrType m_hashFunPtr;

	public:			
		
		MinerThread();
		MinerThread(MinerData* pData);
		MinerThread(const MinerThread& miner);
		MinerThread& operator=(const MinerThread& miner);
		~MinerThread();

		
		//Main Miner Thread. Bound to the class with boost. Might take some rearranging to get working with OpenCL.
		void Miner() { }

		void Lock()		{ if (m_clLock) { m_clLock->lock(); } }
		void Unlock()	{ if (m_clLock)	{ m_clLock->unlock(); } }
			
		///////////////////////////////////////////////////////////////////////////////
		//Accessors
		///////////////////////////////////////////////////////////////////////////////
		const bool				GetIsShuttingDown() const	{	return this->m_bShutown;			}	
		const bool				GetDidShutDown()	const	{	return this->m_bDidShutDown;		}
		const bool				GetIsBlockFound()	const	{	return this->m_bBlockFound;			}
		const bool				GetIsNewBlock()		const	{	return this->m_bNewBlock;			}
		const bool				GetIsReady()		const	{	return this->m_bReady;				}
		const unsigned int		GetHashes()			const	{	return this->m_unHashes;			}
		const unsigned int		GetBlocksFound()	const	{	return this->m_stBlocksFound;		}
		const unsigned int		GetRejected()		const	{	return this->m_stRejected;			}		
		const double			GetLastHashRate()	const	{	return this->m_dLastHashRate;		}	
		MinerData*				GetMinerData()		const	{	return this->m_pMinerData;			}
		
		///////////////////////////////////////////////////////////////////////////////
		//Mutators
		///////////////////////////////////////////////////////////////////////////////
		void    SetIsShuttingDown(const bool bIsShuttingDown)		{	this->m_bShutown = bIsShuttingDown;		}
		void	SetIsBlockFound(const bool bFoundBlock)				{	this->m_bBlockFound = bFoundBlock;		}
		void	SetIsNewBlock(const bool bNewBlock)					{	this->m_bNewBlock = bNewBlock;			}
		void	SetIsReady(const bool bReady)						{	this->m_bReady = bReady;				}
		void	SetHashes(const unsigned int unHashes)				{	this->m_unHashes = unHashes;			}
		void	SetLastHashRate(double dLastRate)					{	this->m_dLastHashRate = dLastRate;		}
		void	SetBlocksFound(const unsigned int unBlocksFound)	{	this->m_stBlocksFound = unBlocksFound;	}
		void	SetRejected(const unsigned int unRejected)			{	this->m_stRejected = unRejected;		}	
		void	SetMinerData(MinerData* pMinerData)					{	this->m_pMinerData = pMinerData;		}
		void	SetHashFunc(HashFuncPtrType hashFunc)				{	this->m_hashFunPtr = hashFunc;			}

};

#endif //_MINERTHREAD_H_