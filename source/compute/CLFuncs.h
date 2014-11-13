////////////////////////////////////////////////
// File:	CLFuncs.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _CLFUNCS_H_
#define _CLFUNCS_H_

#include <vector>

class GPUData;
class RunOptions;

std::vector<GPUData*> CreateOpenCLDevices(RunOptions* options);

void SetThreadConcurrency(GPUData* pGPU);

#endif //CL_FUNS_H_