#include "SKMinerThread.h"

#include "../kernel/KernelFuncs.h"
#include "../gpu/BaseGPU.h"
#include "../core/util.h"
#include "../data/MinerData.h"
#include "../data/SKMinerData.h"
#include "../core/bignum.h"

SKMinerThread::SKMinerThread(SKMinerData* pData) : MinerThread(pData)
{
	m_pTHREAD = boost::thread(&SKMinerThread::Miner, this);
}

SKMinerThread::SKMinerThread(const SKMinerThread& miner) : MinerThread(miner)
{
	m_pTHREAD = boost::thread(&SKMinerThread::Miner, this);
}

SKMinerThread& SKMinerThread::operator=(const SKMinerThread& miner)
{
	m_pMinerData = miner.GetMinerData();
	m_bBlockFound = miner.GetIsBlockFound();
	m_bNewBlock = miner.GetIsNewBlock();
	m_bReady = miner.GetIsReady();
	m_unHashes = miner.GetHashes();
	m_pTHREAD = boost::thread(&SKMinerThread::Miner, this);

	return *this;
}

SKMinerThread::~SKMinerThread()
{
	MinerThread::~MinerThread();
}


void SKMinerThread::Miner()
{
	loop
	{
		try
		{
			/** Don't mine if the Connection Thread is Submitting Block or Receiving New. **/
			while (m_bNewBlock || m_bBlockFound || !m_pMinerData->GetBlock())
				Sleep(10);

			/** Set the Target from nBits. **/
			((SKMinerData*)m_pMinerData)->GetTarget()->SetCompact(m_pMinerData->GetBlock()->GetBits());

			while (!m_bNewBlock)
			{
				bool found = false;

				m_clLock.lock();
				{
					found = m_hashFunPtr(m_pMinerData);
				}
				m_clLock.unlock();
				m_unHashes += m_pMinerData->GetGPUData()->GetHashes();

				if (found)
				{
					m_bBlockFound = true;
					break;
				}

				if (m_pMinerData->GetBlock()->GetNonce() >= MAX_THREADS)
				{
					m_bNewBlock = true;
					break;
				}
			}

		}
		catch (std::exception& e)
		{
			printf("ERROR: %s\n", e.what());
		}
	}
}