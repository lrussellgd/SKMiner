////////////////////////////////////////////////
// File:	KernelFuncs.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//			djm34
//
//			CalculateThreads and SetBufferSize are
//			based off of code produced from the 
//			authors of cgminer and sph-sgminer
//
// Copyright:	2014-2015 Liam Russell
//				2014-2015 djm34	
//				2011 - 2012 Con Kolivas
//				2011 - 2012 Luke Dashjr
//				2010 Jeff Garzik
//			  
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "../core/util.h"
#include "KernelFuncs.h"

#include <string>
#include <time.h>
#include "../compute/CLStructs.h"
#include "../compute/CLKernel.h"
#include "../gpu/BaseGPU.h"
#include "../gpu/ADLGPU.h"
#include "../config/GPUSetting.h"
#include "../data/SKMinerData.h"
#include "../core/bignum.h"
#include "../core/CBlock.h"
#include <boost/thread/mutex.hpp>

#define ROL64(x, n)        (((x) << (n)) | ((x) >> (64 - (n))))


void CalculateThreads(GPUData* pGPU, CLDevice* clDevice, CLKernel* clKernel, size_t* globalThreads, size_t* localThreads, unsigned int* hashes, size_t* buffsize)
{
	if (clDevice == NULL)
	{
		return;
	}

	*localThreads = clKernel->GetWorksize();

	int minThreads = (int)(*localThreads);
	int rawintensity = pGPU->GetGPU()->GetGPUSetting()->GetRawIntensity();
	int xIntensity = pGPU->GetGPU()->GetGPUSetting()->GetXIntensity();
	int intensity = pGPU->GetGPU()->GetGPUSetting()->GetIntensity();


	unsigned int threads = 0;
	while (threads < minThreads)
	{
		if (rawintensity > 0)
		{
			threads = rawintensity;
		}
		else if (xIntensity > 0)
		{
			threads = (unsigned int)(clDevice->GetComputeShaders() * xIntensity);
		}
		else
		{
			threads = 1 << intensity;
		}
		if (threads < minThreads)
		{
			if (intensity < 32)
			{
				intensity++;
			}
			else
			{
				threads = minThreads;
			}
		}
	}

	pGPU->GetGPU()->GetGPUSetting()->SetIntensity(intensity);

	*globalThreads = threads;
	*hashes = threads * pGPU->GetGPU()->GetGPUSetting()->GetVectors();

	*buffsize = (1 * threads); // return threads number

	if ((*buffsize > clDevice->GetMaxMemoryAllocSize()) || (*buffsize == 0))
	{
		std::cout << "Error	Maximum buffer memory device " << pGPU->GetGPU()->GetAdapterIndex() << "supports says " << clDevice->GetMaxMemoryAllocSize() << " max_alloc" << std::endl;
		std::cout << "Your settings come to " << buffsize << "bufsize" << std::endl;
		return;
	}


}


bool sk1024_kernel_djm2(MinerData* pMinerData)
{
	unsigned long long theNonce = ((SKMinerData*)pMinerData)->GetBlock()->GetNonce();

	unsigned int nNonce[2];
	nNonce[0] = (unsigned int)(theNonce & 0xFFFFFFFFULL);
	nNonce[1] = (unsigned int)(theNonce >> 32);

	CLDevice* clDevice = dynamic_cast<CLDevice*>(((SKMinerData*)pMinerData)->GetGPUData()->GetDevice());

	if (clDevice == NULL)
	{
		std::cout << "Error! GPU Device " << ((SKMinerData*)pMinerData)->GetGPUData()->GetGPU()->GetAdapterIndex() << " doesn't have a valid OpenCL device initialized!" << std::endl;
		return false;
	}

	if (clDevice->GetAddressBits() != 64)
	{
		std::cout << "Error! Must use setx GPU_FORCE_64BIT_PTR 1 !" << std::endl;
		std::cout << "If this value is set then GPU Device " << ((SKMinerData*)pMinerData)->GetGPUData()->GetID() << " doesn't support 64 bit and is not valid for the sk1024 kernel!" << std::endl;
		std::cout << "The specified Address Bits are " << clDevice->GetAddressBits() << std::endl;
		std::cout << "Disable this device and restart the miner!" << std::endl;

		system("pause");

		return false;
	}

	cl_mem NonceBuffer; // nonce candidate
	cl_mem DataMsg;  // input data msg
	//cl_mem HashBuffer; // communication between kernel (not used in current one kernel setting)
	cl_mem HvBuffer; // h state after 1st Skein loop
	cl_ulong StartNonce = ((uint64_t*)nNonce)[0];  //starting nonce value


	uint1024 target = ((SKMinerData*)pMinerData)->GetTarget()->getuint1024();
	cl_ulong TheTarget = ((unsigned long long*)&target)[15];  //target value, we take only  
	cl_ulong SkeinHv[17];

	unsigned int* pData = (unsigned int*)((SKMinerData*)pMinerData)->GetBlock()->GetData();

	unsigned long long* SkeinC = (unsigned long long*)malloc(sizeof(unsigned long long) * 17);
	SkeinFirstRound(pData, SkeinC);

	memcpy(SkeinHv, SkeinC, sizeof(unsigned long long) * 17);
	
	free(SkeinC);

	cl_int error = CL_SUCCESS;
	CLKernel* skeinProcess = clDevice->GetKernel("sk1024");
	size_t globalSize = 8192;
	size_t localSize = 128;
	size_t buffSize = 0;

	unsigned int hashes;

	CalculateThreads(((SKMinerData*)pMinerData)->GetGPUData(), clDevice, skeinProcess, &globalSize, &localSize, &hashes, &buffSize);

	if (hashes > ((SKMinerData*)pMinerData)->GetGPUData()->GetMaxHashes())
	{
		((SKMinerData*)pMinerData)->GetGPUData()->SetMaxHashes((unsigned int)globalSize);
	}

	cl::NDRange globalRange(globalSize);
	cl::NDRange localRange(localSize);

	cl_context TheContext = clDevice->GetContext()();

	NonceBuffer = clCreateBuffer(TheContext, CL_MEM_READ_WRITE, sizeof(cl_ulong), NULL, &error);
	if (error != 0) 
	{ 
		std::cout << "problem creating NonceBuffer"; 
	}

	DataMsg = clCreateBuffer(TheContext, CL_MEM_READ_ONLY, sizeof(cl_ulong)* 27, NULL, &error);
	if (error != 0) 
	{ 
		std::cout << "problem creating DataMsg"; 
	}

	//HashBuffer = clCreateBuffer(TheContext, CL_MEM_READ_WRITE, buffSize*sizeof(cl_ulong)* 16, NULL, &error);
	//if (error != 0) { std::cout << "problem creating HashBuffer"; }

	HvBuffer = clCreateBuffer(TheContext, CL_MEM_READ_ONLY, sizeof(cl_ulong)* 17, NULL, &error);
	if (error != 0) 
	{ 
		std::cout << "problem creating HvBuffer"; 
	}

	cl::Kernel SkeinProcess = skeinProcess->GetKernel();

	SkeinProcess.setArg(0, DataMsg);
	SkeinProcess.setArg(1, HvBuffer);
	SkeinProcess.setArg(2, StartNonce);
	SkeinProcess.setArg(3, NonceBuffer);
	SkeinProcess.setArg(4, TheTarget);


	cl_command_queue TheQueue = clDevice->GetCommandQueue()();

	clEnqueueWriteBuffer(TheQueue, DataMsg, CL_TRUE, 0, sizeof(cl_ulong)* 27, pData, 0, NULL, NULL);
	clEnqueueWriteBuffer(TheQueue, NonceBuffer, CL_TRUE, 0, sizeof(cl_ulong), 0, 0, NULL, NULL);
	clEnqueueWriteBuffer(TheQueue, HvBuffer, CL_TRUE, 0, sizeof(cl_ulong)* 17, SkeinHv, 0, NULL, NULL);

	clEnqueueNDRangeKernel(TheQueue, SkeinProcess(), 1, NULL, globalRange, localRange, 0, NULL, NULL);

	unsigned long long TheNonce[1] = { 0xffffffffffffffffull };
	clEnqueueReadBuffer(TheQueue, NonceBuffer, CL_FALSE, 0, sizeof(cl_ulong), TheNonce, 0, NULL, NULL);

	clDevice->GetCommandQueue().finish();


	bool isFound = false;
	

	if (TheNonce[0])
	{
		hashes = TheNonce[0] - ((uint64_t*)nNonce)[0];
		nNonce[0] = (unsigned int)(TheNonce[0] & 0xFFFFFFFFULL);
		nNonce[1] = (unsigned int)(TheNonce[0] >> 32);

		isFound = true;
	}
	else 
	{
		((uint64_t*)nNonce)[0] += globalSize; // no Nonce found, just set nNonce to maxhashes... need to make sure this logic doesn't skip nonce
		hashes = (uint32_t)globalSize;
	}

	((SKMinerData*)pMinerData)->GetGPUData()->SetHashes(hashes);
	((SKMinerData*)pMinerData)->GetBlock()->SetNonce(((unsigned long long)nNonce[0] | (((unsigned long long)nNonce[1]) << 32)));

	return isFound;
	
}

static void Round1024_host(uint64_t &p0, uint64_t &p1, uint64_t &p2, uint64_t &p3, uint64_t &p4, uint64_t &p5, uint64_t &p6, uint64_t &p7,
	uint64_t &p8, uint64_t &p9, uint64_t &pA, uint64_t &pB, uint64_t &pC, uint64_t &pD, uint64_t &pE, uint64_t &pF, int ROT)
{

	static const int cpu_ROT1024[8][8] =
	{
		{ 55, 43, 37, 40, 16, 22, 38, 12 },
		{ 25, 25, 46, 13, 14, 13, 52, 57 },
		{ 33, 8, 18, 57, 21, 12, 32, 54 },
		{ 34, 43, 25, 60, 44, 9, 59, 34 },
		{ 28, 7, 47, 48, 51, 9, 35, 41 },
		{ 17, 6, 18, 25, 43, 42, 40, 15 },
		{ 58, 7, 32, 45, 19, 18, 2, 56 },
		{ 47, 49, 27, 58, 37, 48, 53, 56 }
	};



	p0 += p1;
	p1 = ROL64(p1, cpu_ROT1024[ROT][0]);
	p1 ^= p0;
	p2 += p3;
	p3 = ROL64(p3, cpu_ROT1024[ROT][1]);
	p3 ^= p2;
	p4 += p5;
	p5 = ROL64(p5, cpu_ROT1024[ROT][2]);
	p5 ^= p4;
	p6 += p7;
	p7 = ROL64(p7, cpu_ROT1024[ROT][3]);
	p7 ^= p6;
	p8 += p9;
	p9 = ROL64(p9, cpu_ROT1024[ROT][4]);
	p9 ^= p8;
	pA += pB;
	pB = ROL64(pB, cpu_ROT1024[ROT][5]);
	pB ^= pA;
	pC += pD;
	pD = ROL64(pD, cpu_ROT1024[ROT][6]);
	pD ^= pC;
	pE += pF;
	pF = ROL64(pF, cpu_ROT1024[ROT][7]);
	pF ^= pE;
}

void SkeinFirstRound(unsigned int *pData, unsigned long long* skeinC)
{
/// first round of skein performed on cpu ==> constant on gpu

	static const uint64_t cpu_SKEIN1024_IV_1024[16] =
	{
		//     lo           hi
		0x5A4352BE62092156,
		0x5F6E8B1A72F001CA,
		0xFFCBFE9CA1A2CE26,
		0x6C23C39667038BCA,
		0x583A8BFCCE34EB6C,
		0x3FDBFB11D4A46A3E,
		0x3304ACFCA8300998,
		0xB2F6675FA17F0FD2,
		0x9D2599730EF7AB6B,
		0x0914A20D3DFEA9E4,
		0xCC1A9CAFA494DBD3,
		0x9828030DA0A6388C,
		0x0D339D5DAADEE3DC,
		0xFC46DE35C4E2A086,
		0x53D6E4F52E19A6D1,
		0x5663952F715D1DDD,
	};
	
	uint64_t t[3];
	uint64_t h[17];
	uint64_t p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;

	uint64_t cpu_skein_ks_parity = 0x5555555555555555;
	h[16] = cpu_skein_ks_parity;
	for (int i = 0; i<16; i++) {
		h[i] = cpu_SKEIN1024_IV_1024[i];
		h[16] ^= h[i];
	}
	uint64_t* alt_data = (uint64_t*)pData;
	/////////////////////// round 1 //////////////////////////// should be on cpu => constant on gpu
	p0 = alt_data[0];
	p1 = alt_data[1];
	p2 = alt_data[2];
	p3 = alt_data[3];
	p4 = alt_data[4];
	p5 = alt_data[5];
	p6 = alt_data[6];
	p7 = alt_data[7];
	p8 = alt_data[8];
	p9 = alt_data[9];
	p10 = alt_data[10];
	p11 = alt_data[11];
	p12 = alt_data[12];
	p13 = alt_data[13];
	p14 = alt_data[14];
	p15 = alt_data[15];
	t[0] = 0x80; // ptr  
	t[1] = 0x7000000000000000; // etype
	t[2] = 0x7000000000000080;

	p0 += h[0];
	p1 += h[1];
	p2 += h[2];
	p3 += h[3];
	p4 += h[4];
	p5 += h[5];
	p6 += h[6];
	p7 += h[7];
	p8 += h[8];
	p9 += h[9];
	p10 += h[10];
	p11 += h[11];
	p12 += h[12];
	p13 += h[13] + t[0];
	p14 += h[14] + t[1];
	p15 += h[15];

	for (int i = 1; i < 21; i += 2)
	{

		Round1024_host(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 0);
		Round1024_host(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 1);
		Round1024_host(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 2);
		Round1024_host(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 3);

		p0 += h[(i + 0) % 17];
		p1 += h[(i + 1) % 17];
		p2 += h[(i + 2) % 17];
		p3 += h[(i + 3) % 17];
		p4 += h[(i + 4) % 17];
		p5 += h[(i + 5) % 17];
		p6 += h[(i + 6) % 17];
		p7 += h[(i + 7) % 17];
		p8 += h[(i + 8) % 17];
		p9 += h[(i + 9) % 17];
		p10 += h[(i + 10) % 17];
		p11 += h[(i + 11) % 17];
		p12 += h[(i + 12) % 17];
		p13 += h[(i + 13) % 17] + t[(i + 0) % 3];
		p14 += h[(i + 14) % 17] + t[(i + 1) % 3];
		p15 += h[(i + 15) % 17] + (uint64_t)i;

		Round1024_host(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 4);
		Round1024_host(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 5);
		Round1024_host(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 6);
		Round1024_host(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 7);

		p0 += h[(i + 1) % 17];
		p1 += h[(i + 2) % 17];
		p2 += h[(i + 3) % 17];
		p3 += h[(i + 4) % 17];
		p4 += h[(i + 5) % 17];
		p5 += h[(i + 6) % 17];
		p6 += h[(i + 7) % 17];
		p7 += h[(i + 8) % 17];
		p8 += h[(i + 9) % 17];
		p9 += h[(i + 10) % 17];
		p10 += h[(i + 11) % 17];
		p11 += h[(i + 12) % 17];
		p12 += h[(i + 13) % 17];
		p13 += h[(i + 14) % 17] + t[(i + 1) % 3];
		p14 += h[(i + 15) % 17] + t[(i + 2) % 3];
		p15 += h[(i + 16) % 17] + (uint64_t)(i + 1);


	}

	h[0] = p0^alt_data[0];
	h[1] = p1^alt_data[1];
	h[2] = p2^alt_data[2];
	h[3] = p3^alt_data[3];
	h[4] = p4^alt_data[4];
	h[5] = p5^alt_data[5];
	h[6] = p6^alt_data[6];
	h[7] = p7^alt_data[7];
	h[8] = p8^alt_data[8];
	h[9] = p9^alt_data[9];
	h[10] = p10^alt_data[10];
	h[11] = p11^alt_data[11];
	h[12] = p12^alt_data[12];
	h[13] = p13^alt_data[13];
	h[14] = p14^alt_data[14];
	h[15] = p15^alt_data[15];
	h[16] = cpu_skein_ks_parity;
	for (int i = 0; i<16; i++) { h[16] ^= h[i]; }


	memcpy(skeinC, h, sizeof(unsigned long long) * 17);
}
