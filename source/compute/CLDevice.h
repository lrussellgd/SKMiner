////////////////////////////////////////////////
// File:	CLDevice.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _CLDEVICE_H_
#define _CLDEVICE_H_

#include <CL/cl.hpp>
#include <string>
#include <map>

#include "BaseComputeDevice.h"

class CLKernel;

class CLDevice : public BaseComputeDevice
{
private:
		
	unsigned int m_unIsEndianLittle;
	unsigned int m_unAddressBits;
	unsigned int m_unMaxClockFrequency;	
	unsigned int m_unPreferredVectorWidth;

	unsigned long long m_ullMemorySize;
	unsigned long long m_ullDeviceGlobalMemSize;

	size_t m_stDeviceIndex;
	size_t m_stComputeShaders;
	size_t m_stMaxWorkGroupSize;

	cl::Device m_clDevice;
	
	std::string m_szOpenCLVersion;
	std::string m_szDriverVersion;

	cl::Platform m_clPlatform;
	cl::Context m_clContext;
	cl::CommandQueue m_commandQueue;
	std::map<std::string, CLKernel*> m_mapKernels;

public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	CLDevice();

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	CLDevice(cl::Device device);

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	CLDevice(const CLDevice& device);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	CLDevice& operator=(const CLDevice& device);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~CLDevice();

	CLDevice* Clone();
	CLDevice* DeepCopy();

	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const unsigned int			GetIsAvailable();	
	const unsigned int			GetIsLitteEndian()						const						{	return this->m_unIsEndianLittle;							}
	const unsigned int			GetAddressBits()						const						{	return this->m_unAddressBits;								}
	const unsigned int			GetMaxClockFrequency()					const						{	return this->m_unMaxClockFrequency;							}
	const unsigned int			GetPreferredVectorWidth()				const						{	return this->m_unPreferredVectorWidth;						}
	const unsigned long long	GetMaxMemoryAllocSize()					const						{	return this->m_ullMemorySize;								}
	const unsigned long long	GetDeviceGlobalMemSize()				const						{	return this->m_ullDeviceGlobalMemSize;						}
	const size_t&				GetComputeShaders()						const						{	return this->m_stComputeShaders;							}
	const size_t&				GetMaxWorkGroupSize()					const						{	return this->m_stMaxWorkGroupSize;							}
	const size_t&				GetDeviceIndex()						const						{	return this->m_stDeviceIndex;								}	
	const std::string&			GetOpenCLVersion()						const						{	return this->m_szOpenCLVersion;								}
	const std::string&			GetDriverVersion()						const						{	return this->m_szDriverVersion;								}
	const cl::Device&			GetDeviceID()							const						{	return this->m_clDevice;									}
	const cl::Platform&			GetPlatform()							const						{	return this->m_clPlatform;									}		
	const cl::Context&		    GetContext()							const		    		    {	return this->m_clContext;									}
	CLKernel*					GetKernel(const std::string& szKernel)								{   return this->m_mapKernels[szKernel];						}
	const std::map<std::string, CLKernel*> GetKernels()					const						{	return this->m_mapKernels;									}
	const cl::CommandQueue&		GetCommandQueue()						const						{	return this->m_commandQueue;								}

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void SetIsLitteEndian(const unsigned int unIsLittleEndian)										{	this->m_unIsEndianLittle = unIsLittleEndian;				}
	void SetAddressBits(const unsigned int unAddressBits)											{	this->m_unAddressBits = unAddressBits;						}
	void SetMaxClockFrequency(const unsigned int unMaxClockFrequency)								{	this->m_unMaxClockFrequency = unMaxClockFrequency;			}
	void SetPreferredVectorWidth(const unsigned int unPreferredVectorWidth)							{	this->m_unPreferredVectorWidth = unPreferredVectorWidth;	}
	void SetMaxMemoryAllocSize(const unsigned long long ullMaxMemoryAllocSize)						{	this->m_ullMemorySize = ullMaxMemoryAllocSize;				}
	void SetDeviceGlobalMemSize(const unsigned long long ullDeviceGlobalMemSize)					{	this->m_ullDeviceGlobalMemSize = ullDeviceGlobalMemSize;	}
	void SetComputeShaders(const size_t& shaders)													{	this->m_stComputeShaders = shaders;							}
	void SetMaxWorkGroupSize(const size_t& maxSize)													{	this->m_stMaxWorkGroupSize = maxSize;						}
	void SetDeviceIndex(const size_t& devIndex)														{	this->m_stDeviceIndex = devIndex;							}	
	void SetOpenCLVersion(const std::string& szCLVersion)											{	this->m_szOpenCLVersion = szCLVersion;						}
	void SetDriverVersion(const std::string& szDriverVersion)										{	this->m_szDriverVersion = szDriverVersion;					}
	void SetDeviceID(const cl::Device& device)														{	this->m_clDevice = device;									}
	void SetContext(const cl::Context& context)				    									{	this->m_clContext = context;								}
	void SetPlatform(const cl::Platform& platform)													{	this->m_clPlatform = platform;								}		
	void SetKernel(const std::string& szKernel, CLKernel* clKernel)									{	this->m_mapKernels[szKernel] = clKernel;					}
	void SetCommandQueue(const cl::CommandQueue& commQueue)											{	this->m_commandQueue = commQueue;							}
};

#endif //_CLDEVICE_H_