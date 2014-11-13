////////////////////////////////////////////////
// File:	SKServerConnection.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "SKServerConnection.h"
#include "../core/bignum.h"
#include "../gpu/BaseGPU.h"
#include "../config/GPUSetting.h"
#include "../threads/SKMinerThread.h"
#include "../data/SKMinerData.h"
#include "../kernel/KernelFuncs.h"

SKServerConnection::SKServerConnection()
{
}

SKServerConnection& SKServerConnection::operator=(const SKServerConnection& serverConnection)
{
	return *this;
}

SKServerConnection::SKServerConnection(std::vector<GPUData*> gpus, std::string ip, std::string port, int nMaxTimeout)
{
	this->IP = ip;
	this->PORT = port;

	THREAD = boost::thread(&SKServerConnection::ServerThread, this);

	nThreads = 0;
	for (int nIndex = 0; nIndex < gpus.size(); ++nIndex)
	{
		if (!gpus[nIndex]->GetGPU()->GetGPUSetting()->GetIsEnabled())
		{
			continue;
		}

		int currThreads = gpus[nIndex]->GetGPU()->GetGPUSetting()->GetThreads();
		for (int gpuThreads = 0; gpuThreads < currThreads; ++gpuThreads)
		{
			MinerData* pMinerData = new SKMinerData();
			pMinerData->SetGPUData(gpus[nIndex]->DeepCopy());
			pMinerData->SetBlock(NULL);
			((SKMinerData*)pMinerData)->SetTarget(new CBigNum());

			MinerThread* pThread = new SKMinerThread((SKMinerData*)pMinerData);
			pThread->SetHashFunc(sk1024_kernel_djm2);
			THREADS.push_back(pThread);

			nThreads++;
		}
	}
	nTimeout = nMaxTimeout;
}

SKServerConnection::~SKServerConnection()
{
	ServerConnection::~ServerConnection();
}

//Main Connection Thread. Handles all the networking to allow
//Mining threads the most performance.
void SKServerConnection::ServerThread()
{
	/** Don't begin until all mining threads are Created. **/
	while (THREADS.size() != nThreads)
		Sleep(1000);

	/** Initialize the Server Connection. **/
	CLIENT = new LLP::Miner(IP, PORT);

	/** Initialize a Timer for the Hash Meter. **/
	TIMER.Start();

	unsigned int nBestHeight = 0;
	loop
	{
		try
		{
			/** Run this thread at 1 Cycle per Second. **/
			Sleep(1000);

			/** Attempt with best efforts to keep the Connection Alive. **/
			if (!CLIENT->Connected() || CLIENT->Errors())
			{
				if (!CLIENT->Connect())
					continue;

				/** Send to the server the Channel we will be Mining For. **/
				else
					CLIENT->SetChannel(2);
			}

			/** Check the Block Height. **/
			unsigned int nHeight = CLIENT->GetHeight(5);
			if (nHeight == 0)
			{
				printf("Failed to Update Height...\n");
				CLIENT->Disconnect();
				continue;
			}

			/** If there is a new block, Flag the Threads to Stop Mining. **/
			if (nHeight != nBestHeight)
			{
				nBestHeight = nHeight;
				printf("[MASTER] Coinshield Network | New Block %u\n", nHeight);

				ResetThreads();
			}


			/** Rudimentary Meter **/
			if (TIMER.Elapsed() > 10)
			{
				unsigned int nElapsed = TIMER.ElapsedMilliseconds();
				unsigned int nHashes = Hashes();

				double KHASH = (double)nHashes / nElapsed;
				printf("[METERS] %u Hashes | %f KHash/s | Height = %u\n", nHashes, KHASH, nBestHeight);

				TIMER.Reset();
			}


			/** Check if there is work to do for each Miner Thread. **/
			for (int nIndex = 0; nIndex < THREADS.size(); nIndex++)
			{
				if (!THREADS[nIndex]->GetMinerData()->GetGPUData()->GetGPU()->GetGPUSetting()->GetIsEnabled())
				{
					continue;
				}

				/** Attempt to get a new block from the Server if Thread needs One. **/
				if (THREADS[nIndex]->GetIsNewBlock())
				{
					/** Delete the Block Pointer if it Exists. **/
					Core::CBlock* pBlock = THREADS[nIndex]->GetMinerData()->GetBlock();
					if (pBlock != NULL)
					{
						delete(pBlock);
					}

					/** Retrieve new block from Server. **/
					pBlock = CLIENT->GetBlock(5);

					/** If the block is good, tell the Mining Thread its okay to Mine. **/
					if (pBlock)
					{
						THREADS[nIndex]->SetIsBlockFound(false);
						THREADS[nIndex]->SetIsNewBlock(false);
						THREADS[nIndex]->GetMinerData()->SetBlock(pBlock);
					}
					/** If the Block didn't come in properly, Reconnect to the Server. **/
					else
					{
						CLIENT->Disconnect();
					}

				}
				/** Submit a block from Mining Thread if Flagged. **/
				else if (THREADS[nIndex]->GetIsBlockFound())
				{
					/** Attempt to Submit the Block to Network. **/
					unsigned char RESPONSE = CLIENT->SubmitBlock(THREADS[nIndex]->GetMinerData()->GetBlock()->GetMerkleRoot(), THREADS[nIndex]->GetMinerData()->GetBlock()->GetNonce(), 30);
					printf("[MASTER] Hash Found on Miner Thread %i\n", nIndex);

					unsigned long long truc = THREADS[nIndex]->GetMinerData()->GetBlock()->GetNonce();
					printf("[MASTER] nonce %08x %08x\n", (uint32_t)(truc >> 32)), (uint32_t)(truc & 0xFFFFFFFFULL);


					/** Check the Response from the Server.**/
					if (RESPONSE == 200)
					{
						printf("[MASTER] Block Accepted By Coinshield Network.\n");
					}
					else if (RESPONSE == 201)
					{
						printf("[MASTER] Block Rejected by Coinshield Network.\n");

						THREADS[nIndex]->SetIsNewBlock(true);
						THREADS[nIndex]->SetIsBlockFound(false);
					}

					/** If the Response was Incomplete, Reconnect to Server and try to Submit Block Again. **/
					else
					{
						printf("[MASTER] Failure to Submit Block. Reconnecting...\n");
						CLIENT->Disconnect();
					}

					break;                           
				}
			}
		}
		catch (std::exception& e)
		{ 
			std::cout << e.what() << std::endl;
		}
	}
}
