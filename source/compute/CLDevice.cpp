////////////////////////////////////////////////
// File:	CLDevice.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "CLDevice.h"
#include "CLKernel.h"


///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
CLDevice::CLDevice() : BaseComputeDevice()
{
	this->m_unIsEndianLittle = this->m_unAddressBits = this->m_unMaxClockFrequency = this->m_unPreferredVectorWidth = 0;
	this->m_ullMemorySize = this->m_ullDeviceGlobalMemSize = 0;
	this->m_stComputeShaders = this->m_stMaxWorkGroupSize = 0;
}

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
CLDevice::CLDevice(cl::Device device)
{
	this->m_clDevice = device;

	this->m_szName = m_clDevice.getInfo<CL_DEVICE_NAME>();
	this->m_szVendorName = m_clDevice.getInfo<CL_DEVICE_VENDOR>();
	this->m_szOpenCLVersion = m_clDevice.getInfo<CL_DEVICE_VERSION>();
	this->m_szDriverVersion = m_clDevice.getInfo<CL_DRIVER_VERSION>();

	cl_uint addrBits = 0;
	m_clDevice.getInfo(CL_DEVICE_ADDRESS_BITS, &addrBits);
	this->m_unAddressBits = addrBits;

	unsigned int vecWidth = 0;
	m_clDevice.getInfo(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, &vecWidth);
	m_unPreferredVectorWidth = vecWidth;

	cl_uint maxClock = 0;
	m_clDevice.getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &maxClock);
	this->m_unMaxClockFrequency = maxClock;

	cl_uint maxComp = 0;
	m_clDevice.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &maxComp);
	this->m_stComputeShaders = maxComp * 64;

	cl_ulong maxMem = 0;
	m_clDevice.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &maxMem);
	this->m_ullDeviceGlobalMemSize = maxMem;

	cl_uint vendorId = 0;
	m_clDevice.getInfo(CL_DEVICE_VENDOR_ID, &vendorId);
	this->m_unVendorID = vendorId;

	cl_bool isLittle = 0;
	m_clDevice.getInfo(CL_DEVICE_ENDIAN_LITTLE, &isLittle);
	this->m_unIsEndianLittle = isLittle;

	size_t maxSize = 0;
	m_clDevice.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxSize);
	m_stMaxWorkGroupSize = maxSize;

	size_t maxMemSize = 0;
	m_clDevice.getInfo(CL_DEVICE_MAX_MEM_ALLOC_SIZE, &maxMemSize);
	m_ullMemorySize = maxMemSize;

}


///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
CLDevice::CLDevice(const CLDevice& device) : BaseComputeDevice()
{
	this->m_clDevice = device.GetDeviceID();
	this->m_szName = device.GetName().c_str();
	this->m_szVendorName = device.GetVendorName().c_str();
	this->m_szOpenCLVersion = device.GetOpenCLVersion().c_str();
	this->m_szDriverVersion = device.GetDriverVersion().c_str();
	this->m_unAddressBits = device.GetAddressBits();
	this->m_unMaxClockFrequency = device.GetMaxClockFrequency();
	this->m_stMaxWorkGroupSize = device.GetComputeShaders();
	this->m_ullMemorySize = device.GetMaxMemoryAllocSize();
	this->m_ullDeviceGlobalMemSize = device.GetDeviceGlobalMemSize();
	this->m_unVendorID = device.GetVendorID();
	this->m_unIsEndianLittle = device.GetIsLitteEndian();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
CLDevice& CLDevice::operator = (const CLDevice& device)
{
	this->m_clDevice = device.GetDeviceID();
	this->m_szName = device.GetName().c_str();
	this->m_szVendorName = device.GetVendorName().c_str();
	this->m_szOpenCLVersion = device.GetOpenCLVersion().c_str();
	this->m_szDriverVersion = device.GetDriverVersion().c_str();
	this->m_unAddressBits = device.GetAddressBits();
	this->m_unMaxClockFrequency = device.GetMaxClockFrequency();
	this->m_stMaxWorkGroupSize = device.GetComputeShaders();
	this->m_ullMemorySize = device.GetMaxMemoryAllocSize();
	this->m_ullDeviceGlobalMemSize = device.GetDeviceGlobalMemSize();
	this->m_unVendorID = device.GetVendorID();
	this->m_unIsEndianLittle = device.GetIsLitteEndian();

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
CLDevice::~CLDevice()
{
	for (auto iter = this->m_mapKernels.begin(); iter != this->m_mapKernels.end(); ++iter)
	{
		CLKernel* pKernel = iter->second;

		if (pKernel)
		{
			delete(pKernel);
			pKernel = NULL;
		}
	}

	clReleaseCommandQueue(this->m_commandQueue());
	clReleaseDevice(this->m_clDevice());

}


const unsigned int CLDevice::GetIsAvailable()
{
	cl_bool isAvailable = 0;
	m_clDevice.getInfo(CL_DEVICE_AVAILABLE, &isAvailable);

	return isAvailable;
}

CLDevice* CLDevice::Clone()
{
	CLDevice* pCLDevice = new CLDevice();

	pCLDevice->SetVendorID(this->m_unVendorID);
	pCLDevice->SetVendorName(this->m_szVendorName);
	pCLDevice->SetName(this->m_szName);
	pCLDevice->SetAlgorithm(this->m_szAlgorithm);

	pCLDevice->SetIsLitteEndian(this->m_unIsEndianLittle);
	pCLDevice->SetAddressBits(this->m_unAddressBits);
	pCLDevice->SetMaxClockFrequency(this->m_unMaxClockFrequency);
	pCLDevice->SetMaxMemoryAllocSize(this->m_ullMemorySize);
	pCLDevice->SetDeviceGlobalMemSize(this->m_ullDeviceGlobalMemSize);
	pCLDevice->SetComputeShaders(this->m_stComputeShaders);
	pCLDevice->SetPreferredVectorWidth(this->m_unPreferredVectorWidth);
	pCLDevice->SetMaxWorkGroupSize(this->m_stMaxWorkGroupSize);	
	pCLDevice->SetDeviceID(this->m_clDevice);
	pCLDevice->SetOpenCLVersion(this->m_szOpenCLVersion);
	pCLDevice->SetDriverVersion(this->m_szDriverVersion);
	pCLDevice->SetContext(this->m_clContext);
	pCLDevice->SetCommandQueue(this->m_commandQueue);

	for (auto iter = this->m_mapKernels.begin(); iter != this->m_mapKernels.end(); ++iter)
	{
		pCLDevice->SetKernel(iter->first, iter->second);
	}

	return pCLDevice;
}

CLDevice* CLDevice::DeepCopy()
{
	CLDevice* pCLDevice = new CLDevice();

	pCLDevice->SetVendorID(this->m_unVendorID);
	pCLDevice->SetVendorName(this->m_szVendorName);
	pCLDevice->SetName(this->m_szName);
	pCLDevice->SetAlgorithm(this->m_szAlgorithm);

	pCLDevice->SetIsLitteEndian(this->m_unIsEndianLittle);
	pCLDevice->SetAddressBits(this->m_unAddressBits);
	pCLDevice->SetMaxClockFrequency(this->m_unMaxClockFrequency);
	pCLDevice->SetMaxMemoryAllocSize(this->m_ullMemorySize);
	pCLDevice->SetDeviceGlobalMemSize(this->m_ullDeviceGlobalMemSize);
	pCLDevice->SetComputeShaders(this->m_stComputeShaders);
	pCLDevice->SetPreferredVectorWidth(this->m_unPreferredVectorWidth);
	pCLDevice->SetMaxWorkGroupSize(this->m_stMaxWorkGroupSize);
	pCLDevice->SetDeviceID(this->m_clDevice);
	pCLDevice->SetOpenCLVersion(this->m_szOpenCLVersion);
	pCLDevice->SetDriverVersion(this->m_szDriverVersion);
	pCLDevice->SetContext(this->m_clContext);
	pCLDevice->SetCommandQueue(this->m_commandQueue);

	for (auto iter = this->m_mapKernels.begin(); iter != this->m_mapKernels.end(); ++iter)
	{
		pCLDevice->SetKernel(iter->first, ((CLKernel*)iter->second)->DeepCopy());
	}

	return pCLDevice;
}