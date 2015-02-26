////////////////////////////////////////////////
// File:	SKMinerThread.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "SKMinerThread.h"

#include "../kernel/KernelFuncs.h"
#include "../gpu/BaseGPU.h"
#include "../core/util.h"
#include "../core/bignum.h"
#include "../data/MinerData.h"
#include "../data/SKMinerData.h"
#include "../compute/CLDevice.h"
#include "../compute/CLKernel.h"
#include "../compute/CLMemory.h"

SKMinerThread::SKMinerThread() : MinerThread()
{
	this->m_enmEntityType = ENTITY_TYPE::SK_MINER_THREAD;
	m_pTHREAD = new boost::thread(&SKMinerThread::Miner, this);
}

SKMinerThread::SKMinerThread(SKMinerData* pData) : MinerThread(pData)
{
	if (m_pTHREAD)
	{
		delete(m_pTHREAD);
	}
	m_pTHREAD = new boost::thread(&SKMinerThread::Miner, this);
}

SKMinerThread::SKMinerThread(const SKMinerThread& miner) : MinerThread(miner)
{
	if (m_pTHREAD)
	{
		delete(m_pTHREAD);
	}
	m_pTHREAD = new boost::thread(&SKMinerThread::Miner, this);
}

SKMinerThread& SKMinerThread::operator=(const SKMinerThread& miner)
{
	m_enmEntityType = miner.GetEntityType();
	m_pMinerData = miner.GetMinerData();
	m_bBlockFound = miner.GetIsBlockFound();
	m_bNewBlock = miner.GetIsNewBlock();
	m_bReady = miner.GetIsReady();
	m_unHashes = miner.GetHashes();
	m_pTHREAD = new boost::thread(&SKMinerThread::Miner, this);
	m_bShutown = miner.GetIsShuttingDown();
	m_bDidShutDown = miner.GetDidShutDown();

	return *this;
}

SKMinerThread::~SKMinerThread()
{
	MinerThread::~MinerThread();
}


void SKMinerThread::Initialize()
{
	//Initialize the OpenCL Memory Buffers for use in the Kernel

	cl_int error = CL_SUCCESS;

	CLDevice* pDevice = (CLDevice*)((SKMinerData*)m_pMinerData)->GetGPUData()->GetDevice();
	CLKernel* pKernel = pDevice->GetKernel("sk1024");

	cl_context TheContext = pDevice->GetContext()();

	// nonce candidate

	cl_mem NonceBuffer = clCreateBuffer(TheContext, CL_MEM_READ_WRITE, sizeof(cl_ulong), NULL, &error);

	CLMemory* pNonceMemory = new CLMemory();
	pNonceMemory->SetBufferName("NonceBuffer");
	pNonceMemory->SetBufferSize(sizeof(cl_ulong));
	pNonceMemory->SetBuffer(NonceBuffer);
	pNonceMemory->SetBufferType(CL_MEM_READ_WRITE);

	unsigned long long* pEmptyNonce = (unsigned long long*)calloc(sizeof(unsigned long long), 1);
	pNonceMemory->SetEmptyBuffer((void*)pEmptyNonce);

	// input data msg
	cl_mem DataMsg = clCreateBuffer(TheContext, CL_MEM_READ_ONLY, sizeof(cl_ulong)* 27, NULL, &error);
	CLMemory* pDataMsgMemory = new CLMemory();
	pDataMsgMemory->SetBufferName("DataMsg");
	pDataMsgMemory->SetBufferSize(sizeof(cl_ulong)* 27);
	pDataMsgMemory->SetBuffer(DataMsg);
	pDataMsgMemory->SetBufferType(CL_MEM_READ_ONLY);

	// communication between kernel (not used in current one kernel setting)
	//cl_mem HashBuffer; 

	// h state after 1st Skein loop
	cl_mem HvBuffer = clCreateBuffer(TheContext, CL_MEM_READ_ONLY, sizeof(cl_ulong)* 17, NULL, &error);
	CLMemory* pHvBufferMemory = new CLMemory();
	pHvBufferMemory->SetBufferName("HvBuffer");
	pHvBufferMemory->SetBufferSize(sizeof(cl_ulong)* 17);
	pHvBufferMemory->SetBuffer(HvBuffer);
	pHvBufferMemory->SetBufferType(CL_MEM_READ_ONLY);

	pKernel->SetMemoryBuffer("NonceBuffer", pNonceMemory);
	pKernel->SetMemoryBuffer("DataMsg", pDataMsgMemory);
	pKernel->SetMemoryBuffer("HvBuffer", pHvBufferMemory);
}

void SKMinerThread::Miner()
{
	Initialize();

	loop
	{
		try
		{
			/** Don't mine if the Connection Thread is Submitting Block or Receiving New. **/
			while ((m_bNewBlock || m_bBlockFound || !m_pMinerData->GetBlock()) && !this->m_bShutown)
			{
				Sleep(10);
			}
			

			if (m_bShutown)
			{
				break;
			}

			/** Set the Target from nBits. **/	
			((SKMinerData*)m_pMinerData)->GetTarget()->SetCompact(m_pMinerData->GetBlock()->GetBits());

			while (!m_bNewBlock && !m_bShutown)
			{

				/*m_pKernelThread = new boost::thread(m_hashFunPtr, m_pMinerData);
				m_pKernelThread->join();
	
				m_pKernelThread->detach();
				delete(m_pKernelThread);
				m_pKernelThread = NULL;*/

				bool bIsFound = false;
				Lock();
				{
					bIsFound = m_hashFunPtr(m_pMinerData);
				}
				Unlock();
			
				total_mhashes_done += m_pMinerData->GetGPUData()->GetHashes();
				m_unHashes += m_pMinerData->GetGPUData()->GetHashes();				

				if (m_pMinerData->GetBlock()->GetNonce() >= MAX_THREADS)
				{
					if (bIsFound)
					{
						//Hardware Error
						m_pMinerData->GetGPUData()->SetHardwareErrors(m_pMinerData->GetGPUData()->GetHardwareErrors() + 1);
					}

					m_bNewBlock = true;
					break;
				}

				if (bIsFound)
				{
					m_bBlockFound = true;
					break;
				}
			}

			if (m_bShutown)
			{
				m_bDidShutDown = true;
				break;
			}

		}
		catch (std::exception& e)
		{
			std::cout << "ERROR: " << e.what() << std::endl;
		}
	}
}