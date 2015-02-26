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

				gpuDta->SetGPUSetting(pGPUSetting);

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

				if (gpuDta->GetGPUSetting()->GetLookupGap() <= 0)
				{
					if (devIndex != 0 && settings.size() > 0 && settings[0]->GetLookupGap() > 0)
					{
						gpuDta->GetGPUSetting()->SetLookupGap(settings[0]->GetLookupGap());
					}
					else
					{
						gpuDta->GetGPUSetting()->SetLookupGap(2);
					}
				}

				if (gpuDta->GetGPUSetting()->GetVectors() <= 0)
				{
					if (devIndex != 0 && settings.size() > 0 && settings[0]->GetVectors() > 0)
					{
						gpuDta->GetGPUSetting()->SetVectors(settings[0]->GetVectors());
					}
					else
					{
						gpuDta->GetGPUSetting()->SetVectors(amdDev->GetPreferredVectorWidth());
					}
				}

				if (gpuDta->GetGPUSetting()->GetWorkSize() <= 0)
				{
					if (devIndex != 0 && settings.size() > 0 && settings[0]->GetWorkSize() > 0 && settings[0]->GetWorkSize() <= amdDev->GetMaxWorkGroupSize())
					{
						gpuDta->GetGPUSetting()->SetWorkSize(settings[0]->GetWorkSize());
					}
					else
					{
						gpuDta->GetGPUSetting()->SetWorkSize(amdDev->GetMaxWorkGroupSize());
					}
				}
				else if (gpuDta->GetGPUSetting()->GetWorkSize() > amdDev->GetMaxWorkGroupSize())
				{
					gpuDta->GetGPUSetting()->SetWorkSize(amdDev->GetMaxWorkGroupSize());
				}


				gpuDta->SetDevice(amdDev);

				if (gpuDta->GetGPUSetting()->GetThreadConcurrency() <= 0 && devIndex != 0 && settings.size() > 0 && settings[0]->GetThreadConcurrency() > 0)
				{
					gpuDta->GetGPUSetting()->SetThreadConcurrency(settings[0]->GetThreadConcurrency());
				}
				else
				{
					SetThreadConcurrency(gpuDta);
				}				

				std::stringstream csoptions;
				csoptions << "-D LOOKUP_GAP=" << gpuDta->GetGPUSetting()->GetLookupGap();
				csoptions << " -D CONCURRENT_THREADS=" << gpuDta->GetGPUSetting()->GetThreadConcurrency();
				csoptions << " -D WORKSIZE=" << gpuDta->GetGPUSetting()->GetWorkSize();
				csoptions << " -D VECTORS=" << gpuDta->GetGPUSetting()->GetVectors();
				csoptions << " -D WORKVEC=" << gpuDta->GetGPUSetting()->GetWorkSize() * gpuDta->GetGPUSetting()->GetVectors();

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
					if (gpuDta->GetGPUSetting()->GetWorkSize() < wrkSize)
					{
						pKernel->SetWorksize(gpuDta->GetGPUSetting()->GetWorkSize());
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

std::vector<GPUData*> CreateOpenCLDevices(RunOptions* options, std::vector<GPUSetting*> gpuSettings)
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

	for (size_t index = 0; index < platformSize; ++index)
	{
		cl::Platform plat = vecPlatforms[index];

		std::string platformName = plat.getInfo<CL_PLATFORM_VENDOR>();

		boost::iterator_range<std::string::iterator> amdRange = boost::algorithm::ifind_first(platformName, "Advanced Micro Devices");
		if (!amdRange.empty())
		{
			//Found AMD OpenCL Implementation

			std::stringstream ssName;
			ssName << ".\\resources\\kernel\\" << algorithmName << ".cl";

			std::ifstream programFile(ssName.str());
			std::string programString(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));
			cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));


			std::vector<cl::Device> amdDevices;
			plat.getDevices(CL_DEVICE_TYPE_GPU, &amdDevices);

			for (size_t devIndex = 0; devIndex < amdDevices.size(); ++devIndex)
			{
				cl::Device currDevice = amdDevices[devIndex];
				CLDevice* amdDev = new CLDevice(currDevice);
				amdDev->SetPlatform(plat);
				amdDev->SetDeviceIndex(devIndex);

				GPUData* gpuDta = new GPUData();
				gpuDta->SetID((int)devIndex);

				GPUSetting* pGPUSetting;

				std::cout << "Finding GPU Settings for OpenCL Device Named: " << amdDev->GetName() << std::endl;
				std::cout << std::endl;

				for (size_t settingIndex = 0; settingIndex < gpuSettings.size(); ++settingIndex)
				{
					if (settingIndex == 0)
					{
						pGPUSetting = gpuSettings[settingIndex];
					}
					else if (gpuSettings[settingIndex]->GetDeviceName().find(amdDev->GetName()) != std::string::npos)
					{
						//Use the lowest settings if multiple cards have the same name
						//For Example: R9 280X and R9 280 both display the name 'Tahiti' for OpenCL
						if (pGPUSetting->GetDeviceName().find(amdDev->GetName()) != std::string::npos && gpuSettings[settingIndex]->GetShaders() < pGPUSetting->GetShaders())
						{
							pGPUSetting = gpuSettings[settingIndex];
						}
						else
						{
							pGPUSetting = gpuSettings[settingIndex];
						}
					}
				}

				bool bIsEnabled = true;
				if (bHasDevices)
				{
					std::string strInd = std::to_string(devIndex);
					if (szDevices.find(strInd) == std::string::npos)
					{
						bIsEnabled = false;
					}
				}

				if (pGPUSetting == NULL)
				{
					std::cout << "Failed to Find GPU Settings for OpenCL Device Named: " << amdDev->GetName() << std::endl;
					std::cout << std::endl;

					pGPUSetting = new GPUSetting();
					pGPUSetting->SetIsEnabled(bIsEnabled);
					pGPUSetting->SetThreads(1);
					pGPUSetting->SetDeviceName(amdDev->GetName());
				}
				else
				{
					std::cout << "Found GPU Settings for OpenCL Device Named: " << pGPUSetting->GetDeviceName() << std::endl;
					std::cout << std::endl;

					if (pGPUSetting->GetEngineClock() < 0)
					{
						pGPUSetting->SetEngineClock(0);
					}

					if (pGPUSetting->GetMemclock() < 0)
					{
						pGPUSetting->SetMemclock(0);
					}

					if (pGPUSetting->GetShaders() < 0)
					{
						pGPUSetting->SetShaders(0);
					}

					if (pGPUSetting->GetThreads() < 1)
					{
						pGPUSetting->SetThreads(1);
					}

					if (pGPUSetting->GetIntensity() <= 0 && pGPUSetting->GetXIntensity() <= 0 && pGPUSetting->GetRawIntensity() <= 0)
					{
						pGPUSetting->SetIntensity(8);
						pGPUSetting->SetXIntensity(0);
						pGPUSetting->SetRawIntensity(0);
					}

					if (pGPUSetting->GetThreadConcurrency() < 0)
					{
						pGPUSetting->SetThreadConcurrency(0);
					}

					if (pGPUSetting->GetLookupGap() < 0)
					{
						pGPUSetting->SetLookupGap(0);
					}

					if (pGPUSetting->GetWorkSize() <= 0)
					{
						pGPUSetting->SetWorkSize(64);
					}

					if (pGPUSetting->GetTargetTemp() < 0)
					{
						pGPUSetting->SetTargetTemp(0);
					}

					if (pGPUSetting->GetOverHeatTemp() < 0)
					{
						pGPUSetting->SetOverHeatTemp(0);
					}

					if (pGPUSetting->GetCutOffTemp() < 0)
					{
						pGPUSetting->SetCutOffTemp(0);
					}

					if (pGPUSetting->GetVectors() < 0)
					{
						pGPUSetting->SetVectors(0);
					}

					pGPUSetting->SetIsEnabled(bIsEnabled);
				}

				gpuDta->SetGPUSetting(pGPUSetting);

				

				cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)plat(), 0 };
				cl_int status = -1;

				cl::Context amdContext(CL_DEVICE_TYPE_GPU, cps, NULL, NULL, &status);
				cl::Program amdProgram(amdContext, source);

				amdDev->SetAlgorithm(algorithmName);
				amdDev->SetContext(amdContext);

				cl::CommandQueue commQueue(amdDev->GetContext(), amdDev->GetDeviceID(), 0, &status);
				amdDev->SetCommandQueue(commQueue);

				if (gpuDta->GetGPUSetting()->GetLookupGap() <= 0)
				{
					gpuDta->GetGPUSetting()->SetLookupGap(2);
				}

				if (gpuDta->GetGPUSetting()->GetVectors() <= 0)
				{
					gpuDta->GetGPUSetting()->SetVectors(amdDev->GetPreferredVectorWidth());
				}

				if (gpuDta->GetGPUSetting()->GetWorkSize() <= 0)
				{
					gpuDta->GetGPUSetting()->SetWorkSize(amdDev->GetMaxWorkGroupSize());
				}
				else if (gpuDta->GetGPUSetting()->GetWorkSize() > amdDev->GetMaxWorkGroupSize())
				{
					gpuDta->GetGPUSetting()->SetWorkSize(amdDev->GetMaxWorkGroupSize());
				}

				gpuDta->SetDevice(amdDev);

				SetThreadConcurrency(gpuDta);

				std::stringstream csoptions;
				csoptions << "-D LOOKUP_GAP=" << gpuDta->GetGPUSetting()->GetLookupGap();
				csoptions << " -D CONCURRENT_THREADS=" << gpuDta->GetGPUSetting()->GetThreadConcurrency();
				csoptions << " -D WORKSIZE=" << gpuDta->GetGPUSetting()->GetWorkSize();
				csoptions << " -D VECTORS=" << gpuDta->GetGPUSetting()->GetVectors();
				csoptions << " -D WORKVEC=" << gpuDta->GetGPUSetting()->GetWorkSize() * gpuDta->GetGPUSetting()->GetVectors();

				std::string devInfo = amdDevices[devIndex].getInfo<CL_DEVICE_EXTENSIONS>();

				std::string devVersion = amdDevices[devIndex].getInfo<CL_DEVICE_VERSION>();
				boost::iterator_range<std::string::iterator> devVersionRange = boost::algorithm::ifind_first(devVersion, "OpenCL 1.0");
				if (devVersionRange.empty())
				{
					devVersionRange = boost::algorithm::ifind_first(devVersion, "OpenCL 1.1");
					if (devVersionRange.empty())
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
					allKernels[kernelIndex].getWorkGroupInfo(amdDevices[devIndex], CL_KERNEL_WORK_GROUP_SIZE, &wrkSize);

					size_t prefWrkSize;
					allKernels[kernelIndex].getWorkGroupInfo(amdDevices[devIndex], CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &prefWrkSize);

					size_t reqWGS[3];
					allKernels[kernelIndex].getWorkGroupInfo(amdDevices[devIndex], CL_KERNEL_COMPILE_WORK_GROUP_SIZE, &reqWGS);

					pKernel->SetName(kernelName);
					pKernel->SetKernel(allKernels[kernelIndex]);
					if (gpuDta->GetGPUSetting()->GetWorkSize() < wrkSize)
					{
						pKernel->SetWorksize(gpuDta->GetGPUSetting()->GetWorkSize());
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

				std::cout << "Adding GPU Data for OpenCL Device Named: " << gpuDta->GetDevice()->GetName() << std::endl;
				std::cout << std::endl;
				gpuDevices.push_back(gpuDta);
			}
		}

		boost::iterator_range<std::string::iterator> nvidiaRange = boost::algorithm::ifind_first(platformName, "NVIDIA");
		if (!nvidiaRange.empty())
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

	if(pGPU->GetGPUSetting()->GetThreadConcurrency() <= 0)
	{
		unsigned long long sixtyfours = clDevice->GetMaxMemoryAllocSize() / 131072ULL / 64ULL - 1ULL;
		pGPU->GetGPUSetting()->SetThreadConcurrency((int)(sixtyfours * 64));
		if (pGPU->GetGPUSetting()->GetShaders() > 0 && pGPU->GetGPUSetting()->GetThreadConcurrency() > pGPU->GetGPUSetting()->GetShaders()) 
		{
			pGPU->GetGPUSetting()->SetThreadConcurrency(pGPU->GetGPUSetting()->GetThreadConcurrency() - (pGPU->GetGPUSetting()->GetThreadConcurrency() % pGPU->GetGPUSetting()->GetShaders()));
			if (pGPU->GetGPUSetting()->GetThreadConcurrency() > (pGPU->GetGPUSetting()->GetShaders() * 5))
			{
				pGPU->GetGPUSetting()->SetThreadConcurrency(pGPU->GetGPUSetting()->GetShaders() * 5);
			}
		}
	}
}

void ReInitializeCLDevice(GPUData* pGPUData)
{
	CLDevice* pCLDevice = dynamic_cast<CLDevice*>(pGPUData->GetDevice());
	if (!pCLDevice)
	{
		return;
	}

	std::stringstream ssName;
	ssName << ".\\resources\\kernel\\" << pCLDevice->GetAlgorithm() << ".cl";

	std::ifstream programFile(ssName.str());
	std::string programString(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));

	

	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)pCLDevice->GetPlatform()(), 0 };
	cl_int status = -1;

	cl::Context amdContext(CL_DEVICE_TYPE_GPU, cps, NULL, NULL, &status);
	cl::Program amdProgram(amdContext, source);

	pCLDevice->SetContext(amdContext);

	cl::CommandQueue commQueue(pCLDevice->GetContext(), pCLDevice->GetDeviceID(), 0, &status);
	pCLDevice->SetCommandQueue(commQueue);

	if (pGPUData->GetGPUSetting()->GetLookupGap() <= 0)
	{
		pGPUData->GetGPUSetting()->SetLookupGap(2);
	}

	if (pGPUData->GetGPUSetting()->GetVectors() <= 0)
	{
		pGPUData->GetGPUSetting()->SetVectors(pCLDevice->GetPreferredVectorWidth());
	}

	if (pGPUData->GetGPUSetting()->GetWorkSize() <= 0)
	{
		pGPUData->GetGPUSetting()->SetWorkSize(pCLDevice->GetMaxWorkGroupSize());
	}
	else if (pGPUData->GetGPUSetting()->GetWorkSize() > pCLDevice->GetMaxWorkGroupSize())
	{
		pGPUData->GetGPUSetting()->SetWorkSize(pCLDevice->GetMaxWorkGroupSize());
	}
	
	pGPUData->SetDevice(pCLDevice);

	SetThreadConcurrency(pGPUData);

	std::stringstream csoptions;
	csoptions << "-D LOOKUP_GAP=" << pGPUData->GetGPUSetting()->GetLookupGap();
	csoptions << " -D CONCURRENT_THREADS=" << pGPUData->GetGPUSetting()->GetThreadConcurrency();
	csoptions << " -D WORKSIZE=" << pGPUData->GetGPUSetting()->GetWorkSize();
	csoptions << " -D VECTORS=" << pGPUData->GetGPUSetting()->GetVectors();
	csoptions << " -D WORKVEC=" << pGPUData->GetGPUSetting()->GetWorkSize() * pGPUData->GetGPUSetting()->GetVectors();

	std::string devInfo = pCLDevice->GetDeviceID().getInfo<CL_DEVICE_EXTENSIONS>();

	std::string devVersion = pCLDevice->GetDeviceID().getInfo<CL_DEVICE_VERSION>();
	boost::iterator_range<std::string::iterator> devVersionRange = boost::algorithm::ifind_first(devVersion, "OpenCL 1.0");
	if (devVersionRange.empty())
	{
		devVersionRange = boost::algorithm::ifind_first(devVersion, "OpenCL 1.1");
		if (devVersionRange.empty())
		{
			std::string name = pGPUData->GetDevice()->GetName();

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
		oneDevice.push_back(((CLDevice*)pGPUData->GetDevice())->GetDeviceID());

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
		allKernels[kernelIndex].getWorkGroupInfo(pCLDevice->GetDeviceID(), CL_KERNEL_WORK_GROUP_SIZE, &wrkSize);

		size_t prefWrkSize;
		allKernels[kernelIndex].getWorkGroupInfo(pCLDevice->GetDeviceID(), CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &prefWrkSize);

		size_t reqWGS[3];
		allKernels[kernelIndex].getWorkGroupInfo(pCLDevice->GetDeviceID(), CL_KERNEL_COMPILE_WORK_GROUP_SIZE, &reqWGS);

		pKernel->SetName(kernelName);
		pKernel->SetKernel(allKernels[kernelIndex]);
		if (pGPUData->GetGPUSetting()->GetWorkSize() < wrkSize)
		{
			pKernel->SetWorksize(pGPUData->GetGPUSetting()->GetWorkSize());
		}
		else
		{
			pKernel->SetWorksize(wrkSize);
		}
		pKernel->SetPrefWorkSize(prefWrkSize);
		pKernel->SetReqdWorkSizes(reqWGS);

		pCLDevice->SetKernel(kernelName, pKernel);
	}

	pGPUData->SetDevice(pCLDevice);
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
	pNewGPU->SetAdapterIndex(pMainGPUThread->GetAdapterIndex());
	pNewGPU->SetHashes(pMainGPUThread->GetHashes());
	pNewGPU->SetMaxHashes(pMainGPUThread->GetMaxHashes());

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
	csoptions << "-D LOOKUP_GAP=" << pNewGPU->GetGPUSetting()->GetLookupGap();
	csoptions << " -D CONCURRENT_THREADS=" << pNewGPU->GetGPUSetting()->GetThreadConcurrency();
	csoptions << " -D WORKSIZE=" << pNewGPU->GetGPUSetting()->GetWorkSize();
	csoptions << " -D VECTORS=" << pNewGPU->GetGPUSetting()->GetVectors();
	csoptions << " -D WORKVEC=" << pNewGPU->GetGPUSetting()->GetWorkSize() * pNewGPU->GetGPUSetting()->GetVectors();

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
		if (pNewGPU->GetGPUSetting()->GetWorkSize() < wrkSize)
		{
			pKernel->SetWorksize(pNewGPU->GetGPUSetting()->GetWorkSize());
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

std::vector<CLDevice*> GetCLGPUs(const std::string& szPlatform)
{
	std::vector<CLDevice*> foundGPUs;

	std::vector<cl::Platform> vecPlatforms;
	cl::Platform::get(&vecPlatforms);

	size_t platformSize = vecPlatforms.size();

	for (size_t index = 0; index < platformSize; ++index)
	{
		cl::Platform plat = vecPlatforms[index];

		std::string platformName = plat.getInfo<CL_PLATFORM_VENDOR>();

		boost::iterator_range<std::string::iterator> selectedRange = boost::algorithm::ifind_first(platformName, szPlatform);
		if (!selectedRange.empty())
		{
			std::vector<cl::Device> gpuDevices;
			plat.getDevices(CL_DEVICE_TYPE_GPU, &gpuDevices);

			for (size_t devIndex = 0; devIndex < gpuDevices.size(); ++devIndex)
			{
				cl::Device currDevice = gpuDevices[devIndex];

				CLDevice* gpuDev = new CLDevice(currDevice);
				gpuDev->SetPlatform(plat);
				gpuDev->SetDeviceIndex(devIndex);

				foundGPUs.push_back(gpuDev);
			}
		}
	}

	return foundGPUs;
}