////////////////////////////////////////////////
// File:	BaseGPU.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#ifndef _BASEGPU_H_
#define _BASEGPU_H_

#include <string>
#include "../base/IPrototype.h"

class GPUSetting;

class BaseGPU : public IPrototype
{

protected:

	bool					m_bIsDefFanValid;
	bool					m_bIsFanValid;
	bool					m_bIsManaged;
	
	int						m_nGPUID;
	int						m_nAdapterIndex;
	int						m_nBusNumber;		
	int						m_nLastEngine;
	int						m_nLastTemp;
	
	std::string				m_szAdapterName;

	BaseGPU*				m_pTwin;
	GPUSetting*				m_pGPUSettings;	

public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	BaseGPU();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	BaseGPU(const BaseGPU& baseGPU);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	BaseGPU& operator=(const BaseGPU& baseGPU);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~BaseGPU();

	BaseGPU* Clone();
	BaseGPU* DeepCopy();

	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const bool						GetIsDefFanValid()			const										{	return this->m_bIsDefFanValid;						}
	const bool						GetIsFanValid()				const										{	return this->m_bIsFanValid;							}
	const bool						GetIsManaged()				const										{	return this->m_bIsManaged;							}
	const int						GetAdapterIndex()			const										{	return this->m_nAdapterIndex;						}
	const int						GetBusNumber()				const										{	return this->m_nBusNumber;							}
	const int						GetLastEngine()				const										{	return this->m_nLastEngine;							}
	const int						GetLastTemp()				const										{	return this->m_nLastTemp;							}
	const int						GetGPUID()					const										{	return this->m_nGPUID;								}
	const std::string&				GetAdapterName()			const										{	return this->m_szAdapterName;						}
	GPUSetting*						GetGPUSetting()				const										{	return this->m_pGPUSettings;						}
	BaseGPU*						GetTwin()					const										{	return this->m_pTwin;								}

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void							SetIsDefFanValid(const bool bIsDefFanValid)								{	this->m_bIsDefFanValid = bIsDefFanValid;			}
	void							SetIsFanValid(const bool bIsFanValid)									{	this->m_bIsFanValid = bIsFanValid;					}
	void							SetIsManaged(const bool bIsManaged)										{	this->m_bIsManaged = bIsManaged;					} 
	void							SetAdapterIndex(const int nAdapterIndex)								{	this->m_nAdapterIndex = nAdapterIndex;				}
	void							SetBusNumber(const int nBusNumber)										{	this->m_nBusNumber = nBusNumber;					}
	void							SetLastEngine(const int nLastEngine)									{	this->m_nLastEngine = nLastEngine;					}
	void							SetLastTemp(const int nLastTemp)										{	this->m_nLastTemp = nLastTemp;						}
	void							SetGPUID(const int nGPUID)												{	this->m_nGPUID = nGPUID;							}
	void							SetAdapterName(const std::string& szAdapterName)						{	this->m_szAdapterName = szAdapterName;				}
	void							SetGPUSetting(GPUSetting* gpuSetting)									{	this->m_pGPUSettings = gpuSetting;					}
	void							SetTwin(BaseGPU* pTwin)													{	this->m_pTwin = pTwin;								}

};


#endif //_BASEGPU_H_