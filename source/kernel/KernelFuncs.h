////////////////////////////////////////////////
// File:	KernelFuncs.h
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


#ifndef _KERNELFUNCS_H_
#define _KERNELFUNCS_H_

#include <string>

#include "../hash/uint1024.h"
#include "../core/CBlock.h"
#include "../gpu/GPUData.h"
#include "../compute/CLDevice.h"
#include "../data/MinerData.h"



void SetBufferSize(GPUData* pGPU,CLDevice* dev, CLKernel* pKernel, size_t* buffsize);
void CalculateThreads(GPUData* pGPU, CLDevice* dev, CLKernel* clKernel, size_t* globalThreads, size_t* localThreads, unsigned int* hashes, size_t* buffsize);

bool sk1024_kernel_djm2(MinerData* pMinerData);

void SkeinFirstRound(unsigned int *pData, unsigned long long* skeinC);
static void Round1024_host(uint64_t &p0, uint64_t &p1, uint64_t &p2, uint64_t &p3, uint64_t &p4, uint64_t &p5, uint64_t &p6, uint64_t &p7,
	uint64_t &p8, uint64_t &p9, uint64_t &pA, uint64_t &pB, uint64_t &pC, uint64_t &pD, uint64_t &pE, uint64_t &pF, int ROT);

#endif //_KERNELFUNCS_H_