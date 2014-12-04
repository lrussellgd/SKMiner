////////////////////////////////////////////////
// File:	GPUData.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#ifndef _GPUDATA_H_
#define _GPUDATA_H_

#include <string>
#include "../base/IPrototype.h"

class BaseGPU;
class BaseComputeDevice;

class GPUData : public IPrototype
{
private:

	bool m_bIsActive;
	bool m_bIsDisabled;

	int m_nID;
	int m_nDeviceID;
	int m_nGPUEngine;
	int m_nMinEngine;
	int m_nGPUFan;
	int m_nMinFan;
	int m_nGPUMemclock;
	int m_nGPUMemDiff;
	int m_nGPUPowertune;
	int m_nGPUEngineExit;
	int m_nGPUMemclockExit;
	unsigned int m_unHashes; 
	unsigned int m_unMaxHashes;
	float m_fGPUVDDC;
	float m_fGPUTemp;

	size_t m_unWorkSize;

	std::string m_szName;
	std::string m_szDevicePath;

	BaseGPU* m_pGPU;
	BaseComputeDevice* m_pDevice;
	
public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	GPUData();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	GPUData(const GPUData& gpuData);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	GPUData& operator=(const GPUData& gpuData);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~GPUData();

	GPUData* Clone();
	GPUData* DeepCopy();


	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const	bool			GetIsActive()				const			{	return this->m_bIsActive;				}
	const	bool			GetIsDisabled()				const			{	return this->m_bIsDisabled;				}
	const	int				GetID()						const			{	return this->m_nID;						}
	const	int				GetDeviceID()				const			{	return this->m_nDeviceID;				}
	const	int				GetGPUEngine()				const			{	return this->m_nGPUEngine;				}
	const	int				GetMinEngine()				const			{	return this->m_nMinEngine;				}
	const	int				GetGPUFan()					const			{	return this->m_nGPUFan;					}
	const	int				GetMinFan()					const			{	return this->m_nMinFan;					}
	const	int				GetGPUMemclock()			const			{	return this->m_nGPUMemclock;			}
	const	int				GetGPUMemDiff()				const			{	return this->m_nGPUMemDiff;				}
	const	int				GetGPUPowerTune()			const			{	return this->m_nGPUPowertune;			}
	const	int				GetGPUEngineExit()			const			{	return this->m_nGPUEngineExit;			}
	const	int				GetGPUMemclockExit()		const			{	return this->m_nGPUMemclockExit;		}
	const	unsigned int	GetHashes()					const			{	return this->m_unHashes;				}
	const	unsigned int	GetMaxHashes()				const			{	return this->m_unMaxHashes;				}
	const	float			GetGPUVDDC()				const			{	return this->m_fGPUVDDC;				}
	const	float			GetGPUTemp()				const			{	return this->m_fGPUTemp;				}
	const	size_t&			GetWorkSize()				const			{	return this->m_unWorkSize;				}
	const	std::string&	GetName()					const			{	return this->m_szName;					}
	const	std::string&	GetDevicePath()				const			{	return this->m_szDevicePath;			}
	BaseGPU*				GetGPU()					const			{	return this->m_pGPU;					}
	BaseComputeDevice*		GetDevice()					const			{	return this->m_pDevice;					}

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void					SetIsActive(const bool bIsActive)						{	this->m_bIsActive = bIsActive;			}
	void					SetIsDisabled(const bool bIsDisabled)					{	this->m_bIsDisabled = bIsDisabled;		}
	void					SetID(const int nID)									{	this->m_nID = nID;						}
	void					SetDeviceID(const int nDeviceID)						{	this->m_nDeviceID = nDeviceID;			}
	void					SetGPUEngine(const int nGPUEngine)						{	this->m_nGPUEngine = nGPUEngine;		}
	void					SetMinEngine(const int nMinEngine)						{	this->m_nMinEngine = nMinEngine;		}
	void					SetGPUFan(const int nGPUFan)							{	this->m_nGPUFan = nGPUFan;				}
	void					SetMinFan(const int nMinFan)							{	this->m_nMinFan	= nMinFan;				}
	void					SetGPUMemclock(const int nGPUMemclock)					{	this->m_nGPUMemclock = nGPUMemclock;	}
	void					SetGPUMemDiff(const int nGPUMemDiff)					{	this->m_nGPUMemDiff = nGPUMemDiff;		}
	void					SetGPUPowerTune(const int nGPUPowerTune)				{	this->m_nGPUPowertune = nGPUPowerTune;	}
	void					SetGPUEngineExit(const int nEngineExit)					{	this->m_nGPUEngineExit = nEngineExit;	}
	void					SetGPUMemclockExit(const int nMemExit)					{	this->m_nGPUMemclockExit = nMemExit;	}
	void					SetHashes(unsigned int unHashes)						{	this->m_unHashes = unHashes;			}
	void					SetMaxHashes(const unsigned int unMaxHashes)			{	this->m_unMaxHashes = unMaxHashes;		}
	void					SetWorksize(const size_t& unWorkSize)					{	this->m_unWorkSize = unWorkSize;		}
	void					SetGPUTemp(const float fGPUTemp)						{	this->m_fGPUTemp = fGPUTemp;			}
	void					SetGPUVDDC(const float fGPUVDDC)						{	this->m_fGPUVDDC = fGPUVDDC;			}
	void					SetName(const std::string& szName)						{	this->m_szName = szName;				}
	void					SetDevicePath(const std::string& szDevPath)				{	this->m_szDevicePath = szDevPath;		}
	void					SetGPU(BaseGPU* pGPU)									{	this->m_pGPU = pGPU;					}
	void					SetDevice(BaseComputeDevice* pDevice)					{	this->m_pDevice = pDevice;				}
};

#endif //_GPUDATA_H_