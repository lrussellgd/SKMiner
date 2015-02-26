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
#include "../base/Entity.h"

//class BaseGPU;
class GPUSetting;
class BaseComputeDevice;

class GPUData : public IPrototype, public Entity
{
private:

	bool m_bIsActive;
	bool m_bIsDisabled;

	int m_nID;
	unsigned int m_unHashes; 
	unsigned int m_unMaxHashes;
	unsigned int m_unHardwareErrors;
	int m_iAdapterIndex;

	GPUSetting*			m_pGPUSettings;
	BaseComputeDevice*	m_pDevice;
	
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
	const	int				GetAdapterIndex()			const			{	return this->m_iAdapterIndex;			}	
	const	unsigned int	GetHashes()					const			{	return this->m_unHashes;				}
	const	unsigned int	GetMaxHashes()				const			{	return this->m_unMaxHashes;				}
	const	unsigned int	GetHardwareErrors()			const			{	return this->m_unHardwareErrors;		}	
	GPUSetting*				GetGPUSetting()				const			{	return this->m_pGPUSettings;			}
	BaseComputeDevice*		GetDevice()					const			{	return this->m_pDevice;					}

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void					SetIsActive(const bool bIsActive)						{	this->m_bIsActive = bIsActive;					}
	void					SetIsDisabled(const bool bIsDisabled)					{	this->m_bIsDisabled = bIsDisabled;				}
	void					SetID(const int nID)									{	this->m_nID = nID;								}
	void					SetAdapterIndex(int iAdapterIndex)						{	this->m_iAdapterIndex = iAdapterIndex;			}
	void					SetHashes(unsigned int unHashes)						{	this->m_unHashes = unHashes;					}
	void					SetMaxHashes(const unsigned int unMaxHashes)			{	this->m_unMaxHashes = unMaxHashes;				}
	void					SetHardwareErrors(const unsigned int unHardwareErrors)	{	this->m_unHardwareErrors = unHardwareErrors;	}
	void					SetGPUSetting(GPUSetting* gpuSetting)					{	this->m_pGPUSettings = gpuSetting;				}
	void					SetDevice(BaseComputeDevice* pDevice)					{	this->m_pDevice = pDevice;						}
};

#endif //_GPUDATA_H_