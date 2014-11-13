////////////////////////////////////////////////
// File:	CLFuncs.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "CLFuncs.h"
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "CLDevice.h"
#include "CLKernel.h"
#include "../gpu/GPUData.h"
#include "../gpu/ADLGPU.h"
#include "../config/RunOptions.h"
#include "../config/GPUSetting.h"

std::vector<GPUData*> CreateOpenCLDevices(RunOptions* options)
{	
	std::string algorithmName = options->GetAlgorithm();

	std::vector<GPUData*> gpuDevices(options->GetGPUSettings().size());

	std::vector<cl::Platform> vecPlatforms;
	cl::Platform::get(&vecPlatforms);

	size_t platformSize = vecPlatforms.size();
	
	for(size_t index = 0; index < platformSize; ++index)
	{
		cl::Platform plat = vecPlatforms[index];

		std::string platformName = plat.getInfo<CL_PLATFORM_VENDOR>();
		
		boost::iterator_range<std::string::iterator> amdRange = boost::algorithm::ifind_first( platformName, "Advanced Micro Devices");
		if( !amdRange.empty()) 
		{
			//Found AMD OpenCL Implementation

			std::stringstream ssName;
			ssName << ".\\resources\\kernel\\" << algorithmName << ".cl";

			std::ifstream programFile(ssName.str());
			std::string programString(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));
			cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));


			std::vector<cl::Device> amdDevices;
			plat.getDevices(CL_DEVICE_TYPE_GPU, &amdDevices);

			std::vector<GPUSetting*> settings = options->GetGPUSettings();
             
			for(size_t devIndex = 0; devIndex < amdDevices.size(); ++ devIndex)
			{				
				GPUData* gpuDta = new GPUData();
				gpuDta->SetID((int)devIndex);
				gpuDta->SetGPU(new ADLGPU());

				GPUSetting* pGPUSetting;
				if (devIndex > settings.size() - 1)
				{
					pGPUSetting = settings[0];
				}
				else
				{
					pGPUSetting = settings[devIndex];
				}

				gpuDta->GetGPU()->SetGPUSetting(pGPUSetting);
                
				cl::Device currDevice = amdDevices[devIndex];

				CLDevice* amdDev = new CLDevice(currDevice);
				gpuDta->SetDevice(amdDev);
				cl::Context amdContext(currDevice);
				cl::Program amdProgram(amdContext, source);
				
				amdDev->SetAlgorithm(algorithmName);
				amdDev->SetContext(amdContext);
				cl::CommandQueue commQueue(amdDev->GetContext(), amdDev->GetDeviceID()); 
				amdDev->SetCommandQueue(commQueue);
				


				if(gpuDta->GetGPU()->GetGPUSetting()->GetLookupGap() <= 0)
				{
					gpuDta->GetGPU()->GetGPUSetting()->SetLookupGap(2);
				}
				
				if(gpuDta->GetGPU()->GetGPUSetting()->GetVectors() <= 0)
				{
					gpuDta->GetGPU()->GetGPUSetting()->SetVectors(amdDev->GetPreferredVectorWidth());
				}

				if(gpuDta->GetGPU()->GetGPUSetting()->GetWorkSize() <= 0 || gpuDta->GetGPU()->GetGPUSetting()->GetWorkSize() > amdDev->GetMaxWorkGroupSize())
				{
					gpuDta->GetGPU()->GetGPUSetting()->SetWorkSize(256);
				}

				SetThreadConcurrency(gpuDta);

				std::stringstream csoptions;
				csoptions << "-D LOOKUP_GAP=" << gpuDta->GetGPU()->GetGPUSetting()->GetLookupGap();
				csoptions << " -D CONCURRENT_THREADS=" << gpuDta->GetGPU()->GetGPUSetting()->GetThreadConcurrency();
				csoptions << " -D WORKSIZE=" << gpuDta->GetGPU()->GetGPUSetting()->GetWorkSize();
				csoptions << " -D VECTORS=" << gpuDta->GetGPU()->GetGPUSetting()->GetVectors();
				csoptions << " -D WORKVEC=" << gpuDta->GetGPU()->GetGPUSetting()->GetWorkSize() * gpuDta->GetGPU()->GetGPUSetting()->GetVectors();

				std::string devInfo = amdDevices[devIndex].getInfo<CL_DEVICE_EXTENSIONS>();
				

				std::string devVersion = amdDevices[devIndex].getInfo<CL_DEVICE_VERSION>();
				boost::iterator_range<std::string::iterator> devVersionRange = boost::algorithm::ifind_first(devVersion, "OpenCL 1.0");
				if(devVersionRange.empty()) 
				{
					devVersionRange = boost::algorithm::ifind_first(devVersion, "OpenCL 1.1");
					if(devVersionRange.empty())
					{
						std::string name = amdDev->GetName();

						//OpenCL 1.2
						boost::iterator_range<std::string::iterator> bitAlignRange = boost::algorithm::ifind_first(devInfo, "cl_amd_media_ops");
						if (!bitAlignRange.empty())
						{
							csoptions << " -D BITALIGN";

							if (name.compare("Cedar") == 0 || name.compare("Redwood") == 0 || name.compare("Juniper") == 0 ||
								name.compare("Cypress") == 0 || name.compare("Hemlock") == 0 || name.compare("Caicos") == 0 ||
								name.compare("Turks") == 0 || name.compare("Barts") == 0 || name.compare("Cayman") == 0 ||
								name.compare("Antilles") == 0 || name.compare("Wrestler") == 0 || name.compare("Zacate") == 0 ||
								name.compare("WinterPark") == 0)
							{
								csoptions << " -D BFI_INT";
							}
						}

					}
				}
				else
				{
					csoptions << " -D OCL1";
				}

				csoptions << " -D GOFFSET";
				
				std::string szCompilerOptions = csoptions.str();

				try
				{
					std::vector<cl::Device> oneDevice;
					oneDevice.push_back(currDevice);

					cl_int result = amdProgram.build(oneDevice, szCompilerOptions.c_str());
				}
				catch(cl::Error e)
				{
					std::cout << e.what() << ": Error code " << e.err() << std::endl;

					std::vector<cl::Device> devices = amdProgram.getInfo<CL_PROGRAM_DEVICES>();
					const cl::Device dev = devices[0];

					std::string build_log;
					cl_int err = amdProgram.getBuildInfo(dev, CL_PROGRAM_BUILD_LOG, &build_log);

					std::cout << "Start Build Log: " << std::endl;
					std::cout << build_log << std::endl;
					std::cout << "End Build Log: " << std::endl;
				}

				std::vector<cl::Kernel> allKernels;
				amdProgram.createKernels(&allKernels);	
				for(size_t kernelIndex = 0; kernelIndex < allKernels.size(); ++kernelIndex)
				{
					CLKernel* pKernel = new CLKernel();

					std::string kernelName = allKernels[kernelIndex].getInfo<CL_KERNEL_FUNCTION_NAME>();
					

					size_t wrkSize;
					allKernels[kernelIndex].getWorkGroupInfo(amdDevices[devIndex], CL_KERNEL_WORK_GROUP_SIZE, &wrkSize);

					size_t prefWrkSize;
					allKernels[kernelIndex].getWorkGroupInfo(amdDevices[devIndex], CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &prefWrkSize);

					size_t reqWGS[3];
					allKernels[kernelIndex].getWorkGroupInfo(amdDevices[devIndex], CL_KERNEL_COMPILE_WORK_GROUP_SIZE, &reqWGS);

					pKernel->SetName(kernelName);
					pKernel->SetKernel(allKernels[kernelIndex]);
					if (gpuDta->GetGPU()->GetGPUSetting()->GetWorkSize() < wrkSize)
					{
						pKernel->SetWorksize(gpuDta->GetGPU()->GetGPUSetting()->GetWorkSize());
					}
					else
					{
						pKernel->SetWorksize(wrkSize);
					}
					pKernel->SetPrefWorkSize(prefWrkSize);
					pKernel->SetReqdWorkSizes(reqWGS);

					amdDev->SetKernel(kernelName, pKernel);
				}

				gpuDta->SetDevice(amdDev);
				gpuDevices[gpuDta->GetID()] = gpuDta;
			}
		}
		
		boost::iterator_range<std::string::iterator> nvidiaRange = boost::algorithm::ifind_first( platformName, "NVIDIA" );
		if(!nvidiaRange.empty()) 
		{
			//Found NVIDIA OpenCL Implementation

			//std::stringstream ssName;
			//ssName << ".\\Resources\\Kernel\\" << algorithmName << ".cu";
			
			std::vector<cl::Device> nvidiaDevices;
			plat.getDevices(CL_DEVICE_TYPE_GPU, &nvidiaDevices);

			cl::Context nvidiaContext(nvidiaDevices);
		}
	}

	return gpuDevices;
}


void SetThreadConcurrency(GPUData* pGPU)
{
	CLDevice* clDevice = dynamic_cast<CLDevice*>(pGPU->GetDevice());
	if (clDevice == NULL)
	{
		return;
	}

	if(pGPU->GetGPU()->GetGPUSetting()->GetThreadConcurrency() <= 0)
	{
		unsigned long long sixtyfours = clDevice->GetMaxMemoryAllocSize() / 131072ULL / 64ULL - 1ULL;
		pGPU->GetGPU()->GetGPUSetting()->SetThreadConcurrency((int)(sixtyfours * 64));
		if (pGPU->GetGPU()->GetGPUSetting()->GetShaders() > 0 && pGPU->GetGPU()->GetGPUSetting()->GetThreadConcurrency() > pGPU->GetGPU()->GetGPUSetting()->GetShaders()) 
		{
			pGPU->GetGPU()->GetGPUSetting()->SetThreadConcurrency(pGPU->GetGPU()->GetGPUSetting()->GetThreadConcurrency() - (pGPU->GetGPU()->GetGPUSetting()->GetThreadConcurrency() % pGPU->GetGPU()->GetGPUSetting()->GetShaders()));
			if (pGPU->GetGPU()->GetGPUSetting()->GetThreadConcurrency() > (pGPU->GetGPU()->GetGPUSetting()->GetShaders() * 5))
			{
				pGPU->GetGPU()->GetGPUSetting()->SetThreadConcurrency(pGPU->GetGPU()->GetGPUSetting()->GetShaders() * 5);
			}
		}
	}
}