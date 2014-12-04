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

	std::vector<GPUData*> gpuDevices;

	std::vector<cl::Platform> vecPlatforms;
	cl::Platform::get(&vecPlatforms);

	size_t platformSize = vecPlatforms.size();

	bool bHasDevices = false;
	std::string szDevices = options->GetDevices();
	boost::trim(szDevices);

	if (szDevices.compare("") != 0)
	{
		bHasDevices = true;
	}
	
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

			for (size_t devIndex = 0; devIndex < amdDevices.size(); ++devIndex)
			{
				GPUData* gpuDta = new GPUData();
				gpuDta->SetID((int)devIndex);
				gpuDta->SetGPU(new ADLGPU());

				GPUSetting* pGPUSetting;

				bool bIsEnabled = true;
				if (bHasDevices)
				{
					std::string strInd = std::to_string(devIndex);
					if (szDevices.find(strInd) == std::string::npos)
					{
						bIsEnabled = false;
					}
				}

			
				if (settings.size() == 0)
				{
					pGPUSetting = new GPUSetting();
					pGPUSetting->SetIsEnabled(bIsEnabled);
					pGPUSetting->SetThreads(1);
				}
				else
				{
					if (devIndex > settings.size() - 1)
					{
						pGPUSetting = settings[0]->DeepCopy();
					}
					else
					{
						pGPUSetting = settings[devIndex]->DeepCopy();

						if (devIndex != 0)
						{
							if (pGPUSetting->GetEngineClock() <= 0)
							{
								pGPUSetting->SetEngineClock(settings[0]->GetEngineClock());
							}

							if (pGPUSetting->GetMemclock() <= 0)
							{
								pGPUSetting->SetMemclock(settings[0]->GetMemclock());
							}

							if (pGPUSetting->GetShaders() <= 0)
							{
								pGPUSetting->SetShaders(settings[0]->GetShaders());
							}

							if (pGPUSetting->GetThreads() <= 0)
							{
								pGPUSetting->SetThreads(settings[0]->GetThreads());
							}

							if (pGPUSetting->GetIntensity() <= 0 && pGPUSetting->GetXIntensity() <= 0 && pGPUSetting->GetRawIntensity() <= 0)
							{
								pGPUSetting->SetIntensity(settings[0]->GetIntensity());
								pGPUSetting->SetXIntensity(settings[0]->GetXIntensity());
								pGPUSetting->SetRawIntensity(settings[0]->GetRawIntensity());
							}

							if (pGPUSetting->GetThreadConcurrency() <= 0)
							{
								pGPUSetting->SetThreadConcurrency(settings[0]->GetThreadConcurrency());
							}

							if (pGPUSetting->GetLookupGap() <= 0)
							{
								pGPUSetting->SetLookupGap(settings[0]->GetLookupGap());
							}

							if (pGPUSetting->GetWorkSize() <= 0)
							{
								pGPUSetting->SetWorkSize(settings[0]->GetWorkSize());
							}

							if (pGPUSetting->GetTargetTemp() <= 0)
							{
								pGPUSetting->SetTargetTemp(settings[0]->GetTargetTemp());
							}

							if (pGPUSetting->GetOverHeatTemp() <= 0)
							{
								pGPUSetting->SetOverHeatTemp(settings[0]->GetOverHeatTemp());
							}

							if (pGPUSetting->GetCutOffTemp() <= 0)
							{
								pGPUSetting->SetCutOffTemp(settings[0]->GetCutOffTemp());
							}

							if (pGPUSetting->GetVectors() <= 0)
							{
								pGPUSetting->SetVectors(settings[0]->GetVectors());
							}
						}
					}

					if (pGPUSetting->GetThreads() <= 0)
					{
						pGPUSetting->SetThreads(1);
					}

					pGPUSetting->SetIsEnabled(bIsEnabled);
				}

				gpuDta->GetGPU()->SetGPUSetting(pGPUSetting);

				cl::Device currDevice = amdDevices[devIndex];

				CLDevice* amdDev = new CLDevice(currDevice);
				amdDev->SetPlatform(plat);
				amdDev->SetDeviceIndex(devIndex);

				cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)plat(), 0 };
				cl_int status = -1;

				cl::Context amdContext(CL_DEVICE_TYPE_GPU, cps, NULL, NULL, &status);
				cl::Program amdProgram(amdContext, source);

				amdDev->SetAlgorithm(algorithmName);
				amdDev->SetContext(amdContext);

				cl::CommandQueue commQueue(amdDev->GetContext(), amdDev->GetDeviceID(), 0, &status);
				amdDev->SetCommandQueue(commQueue);

				if (gpuDta->GetGPU()->GetGPUSetting()->GetLookupGap() <= 0)
				{
					if (devIndex != 0 && settings.size() > 0 && settings[0]->GetLookupGap() > 0)
					{
						gpuDta->GetGPU()->GetGPUSetting()->SetLookupGap(settings[0]->GetLookupGap());
					}
					else
					{
						gpuDta->GetGPU()->GetGPUSetting()->SetLookupGap(2);
					}
				}

				if (gpuDta->GetGPU()->GetGPUSetting()->GetVectors() <= 0)
				{
					if (devIndex != 0 && settings.size() > 0 && settings[0]->GetVectors() > 0)
					{
						gpuDta->GetGPU()->GetGPUSetting()->SetVectors(settings[0]->GetVectors());
					}
					else
					{
						gpuDta->GetGPU()->GetGPUSetting()->SetVectors(amdDev->GetPreferredVectorWidth());
					}
				}

				if (gpuDta->GetGPU()->GetGPUSetting()->GetWorkSize() <= 0)
				{
					if (devIndex != 0 && settings.size() > 0 && settings[0]->GetWorkSize() > 0 && settings[0]->GetWorkSize() <= amdDev->GetMaxWorkGroupSize())
					{
						gpuDta->GetGPU()->GetGPUSetting()->SetWorkSize(settings[0]->GetWorkSize());
					}
					else
					{
						gpuDta->GetGPU()->GetGPUSetting()->SetWorkSize(amdDev->GetMaxWorkGroupSize());
					}
				}
				else if (gpuDta->GetGPU()->GetGPUSetting()->GetWorkSize() > amdDev->GetMaxWorkGroupSize())
				{
					gpuDta->GetGPU()->GetGPUSetting()->SetWorkSize(amdDev->GetMaxWorkGroupSize());
				}


				gpuDta->SetDevice(amdDev);

				if (gpuDta->GetGPU()->GetGPUSetting()->GetThreadConcurrency() <= 0 && devIndex != 0 && settings.size() > 0 && settings[0]->GetThreadConcurrency() > 0)
				{
					gpuDta->GetGPU()->GetGPUSetting()->SetThreadConcurrency(settings[0]->GetThreadConcurrency());
				}
				else
				{
					SetThreadConcurrency(gpuDta);
				}				

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
						std::string name = gpuDta->GetDevice()->GetName();

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
					oneDevice.push_back(((CLDevice*)gpuDta->GetDevice())->GetDeviceID());

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

				gpuDevices.push_back(gpuDta);
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


GPUData* CreateNewOpenCLDevice(GPUData* pMainGPUThread)
{
	CLDevice* pCLDevice = dynamic_cast<CLDevice*>(pMainGPUThread->GetDevice());
	if (!pCLDevice)
	{
		return NULL;
	}

	cl::Platform plat = pCLDevice->GetPlatform();	

	std::stringstream ssName;
	ssName << ".\\resources\\kernel\\" << pCLDevice->GetAlgorithm() << ".cl";

	std::ifstream programFile(ssName.str());
	std::string programString(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));


	std::vector<cl::Device> amdDevices;
	plat.getDevices(CL_DEVICE_TYPE_GPU, &amdDevices);

	size_t devIndex = pCLDevice->GetDeviceIndex();

	if ((amdDevices.size() - 1) < devIndex)
	{
		return NULL;
	}

	GPUData* pNewGPU = new GPUData();

	pNewGPU->SetIsActive(pMainGPUThread->GetIsActive());
	pNewGPU->SetIsDisabled(pMainGPUThread->GetIsDisabled());
	pNewGPU->SetID(pMainGPUThread->GetID());
	pNewGPU->SetDeviceID(pMainGPUThread->GetDeviceID());
	pNewGPU->SetGPUEngine(pMainGPUThread->GetGPUEngine());
	pNewGPU->SetMinEngine(pMainGPUThread->GetMinEngine());
	pNewGPU->SetGPUFan(pMainGPUThread->GetGPUFan());
	pNewGPU->SetMinFan(pMainGPUThread->GetMinFan());
	pNewGPU->SetGPUMemclock(pMainGPUThread->GetGPUMemclock());
	pNewGPU->SetGPUMemDiff(pMainGPUThread->GetGPUMemDiff());
	pNewGPU->SetGPUPowerTune(pMainGPUThread->GetGPUPowerTune());
	pNewGPU->SetGPUEngineExit(pMainGPUThread->GetGPUEngineExit());
	pNewGPU->SetGPUMemclockExit(pMainGPUThread->GetGPUMemclockExit());
	pNewGPU->SetHashes(pMainGPUThread->GetHashes());
	pNewGPU->SetMaxHashes(pMainGPUThread->GetMaxHashes());
	pNewGPU->SetWorksize(pMainGPUThread->GetGPUVDDC());
	pNewGPU->SetGPUTemp(pMainGPUThread->GetGPUTemp());
	pNewGPU->SetGPUVDDC(pMainGPUThread->GetWorkSize());
	pNewGPU->SetDevicePath(pMainGPUThread->GetDevicePath());
	pNewGPU->SetGPU(pMainGPUThread->GetGPU()->DeepCopy());


	cl::Device newCLDevice = amdDevices[devIndex];

	CLDevice* pNewCLDevice = new CLDevice(newCLDevice);
	pNewCLDevice->SetPlatform(plat);
	pNewCLDevice->SetAlgorithm(pCLDevice->GetAlgorithm());

	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)plat(), 0 };
	cl_int status = -1;

	cl::Context amdContext(CL_DEVICE_TYPE_GPU, cps, NULL, NULL, &status);
	pNewCLDevice->SetContext(amdContext);

	cl::Program amdProgram(pNewCLDevice->GetContext(), source);

	cl::CommandQueue commQueue(pNewCLDevice->GetContext(), pNewCLDevice->GetDeviceID(), 0, &status);
	pNewCLDevice->SetCommandQueue(commQueue);



	std::stringstream csoptions;
	csoptions << "-D LOOKUP_GAP=" << pNewGPU->GetGPU()->GetGPUSetting()->GetLookupGap();
	csoptions << " -D CONCURRENT_THREADS=" << pNewGPU->GetGPU()->GetGPUSetting()->GetThreadConcurrency();
	csoptions << " -D WORKSIZE=" << pNewGPU->GetGPU()->GetGPUSetting()->GetWorkSize();
	csoptions << " -D VECTORS=" << pNewGPU->GetGPU()->GetGPUSetting()->GetVectors();
	csoptions << " -D WORKVEC=" << pNewGPU->GetGPU()->GetGPUSetting()->GetWorkSize() * pNewGPU->GetGPU()->GetGPUSetting()->GetVectors();

	std::string devInfo = newCLDevice.getInfo<CL_DEVICE_EXTENSIONS>();


	std::string devVersion = newCLDevice.getInfo<CL_DEVICE_VERSION>();
	boost::iterator_range<std::string::iterator> devVersionRange = boost::algorithm::ifind_first(devVersion, "OpenCL 1.0");
	if (devVersionRange.empty())
	{
		devVersionRange = boost::algorithm::ifind_first(devVersion, "OpenCL 1.1");
		if (devVersionRange.empty())
		{
			std::string name = pNewCLDevice->GetName();

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
		oneDevice.push_back(pNewCLDevice->GetDeviceID());

		cl_int result = amdProgram.build(oneDevice, szCompilerOptions.c_str());
	}
	catch (cl::Error e)
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
	for (size_t kernelIndex = 0; kernelIndex < allKernels.size(); ++kernelIndex)
	{
		CLKernel* pKernel = new CLKernel();

		std::string kernelName = allKernels[kernelIndex].getInfo<CL_KERNEL_FUNCTION_NAME>();


		size_t wrkSize;
		allKernels[kernelIndex].getWorkGroupInfo(newCLDevice, CL_KERNEL_WORK_GROUP_SIZE, &wrkSize);

		size_t prefWrkSize;
		allKernels[kernelIndex].getWorkGroupInfo(newCLDevice, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &prefWrkSize);

		size_t reqWGS[3];
		allKernels[kernelIndex].getWorkGroupInfo(newCLDevice, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, &reqWGS);

		pKernel->SetName(kernelName);
		pKernel->SetKernel(allKernels[kernelIndex]);
		if (pNewGPU->GetGPU()->GetGPUSetting()->GetWorkSize() < wrkSize)
		{
			pKernel->SetWorksize(pNewGPU->GetGPU()->GetGPUSetting()->GetWorkSize());
		}
		else
		{
			pKernel->SetWorksize(wrkSize);
		}
		pKernel->SetPrefWorkSize(prefWrkSize);
		pKernel->SetReqdWorkSizes(reqWGS);

		pNewCLDevice->SetKernel(kernelName, pKernel);
	}

	pNewGPU->SetDevice(pNewCLDevice);

	return pNewGPU;
}