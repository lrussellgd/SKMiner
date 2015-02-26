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
class GPUSetting;
class RunOptions;
class CLDevice;

std::vector<GPUData*> CreateOpenCLDevices(RunOptions* options);
std::vector<GPUData*> CreateOpenCLDevices(RunOptions* options, std::vector<GPUSetting*> gpuSettings);

void ReInitializeCLDevice(GPUData* pGPUData);

GPUData* CreateNewOpenCLDevice(GPUData* pMainGPUThread);
std::vector<CLDevice*> GetCLGPUs(const std::string& szPlatform = "Advanced Micro Devices");

void SetThreadConcurrency(GPUData* pGPU);

#endif //CL_FUNS_H_