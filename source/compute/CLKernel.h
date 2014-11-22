////////////////////////////////////////////////
// File:	CLKernel.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _CLKERNEL_H_
#define _CLKERNEL_H_

#include <CL/cl.hpp>
#include <string>

#include "../base/IPrototype.h"
#include <map>

class CLMemory;

class CLKernel : public IPrototype
{
private:
	
	std::string m_szName;

	size_t m_unWorkSize;
	size_t m_unPrefWorkSize;
	size_t m_pReqdWorkGroupSizes[3];

	cl::Kernel m_clKernel;

	std::map<std::string,CLMemory*> m_mapMemBuffers;

	size_t m_stGlobalWorkGroupSize;
	size_t m_stLocalWorkGroupSize;

public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	CLKernel();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	CLKernel(const CLKernel& cKernel);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	CLKernel& operator=(const CLKernel& cKernel);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~CLKernel();

	CLKernel* Clone();
	CLKernel* DeepCopy();


	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const size_t&				GetWorksize()						const						{	return this->m_unWorkSize;						}
	const size_t&				GetPrefWorkSize()					const						{	return this->m_unPrefWorkSize;					}
	const size_t&				GetGlobalWorkGroupSize()			const						{	return this->m_stGlobalWorkGroupSize;			}		
	const size_t&				GetLocalWorkGroupSize()				const						{	return this->m_stLocalWorkGroupSize;			}
	const size_t*				GetReqdWorkGroupSizes()				const						{	return this->m_pReqdWorkGroupSizes;				}	
	const cl::Kernel&			GetKernel()							const						{	return this->m_clKernel;						}	
	const std::string&			GetName()							const						{	return this->m_szName;							}
	CLMemory*					GetMemoryBuffer(const std::string& bufferName)					{	return this->m_mapMemBuffers[bufferName];		}


	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void						SetKernel(const cl::Kernel& clKernel)										{	this->m_clKernel = clKernel;					}
	void						SetName(const std::string szName)											{	this->m_szName = szName;						}
	void						SetWorksize(const size_t& unWorksize)										{	this->m_unWorkSize = unWorksize;				}
	void						SetPrefWorkSize(const size_t& unPrefWorkSize)								{	this->m_unPrefWorkSize = unPrefWorkSize;		}
	void						SetReqdWorkSizes(const size_t* pReqdWorkSizes)								{   this->m_pReqdWorkGroupSizes[0] = pReqdWorkSizes[0]; this->m_pReqdWorkGroupSizes[1] = pReqdWorkSizes[1]; this->m_pReqdWorkGroupSizes[2] = pReqdWorkSizes[2]; }
	void						SetGlobalWorkGroupSize(const size_t& stGlobalSize)							{	this->m_stGlobalWorkGroupSize = stGlobalSize;	}
	void						SetLocalWorkGroupSize(const size_t& stLocalSize)							{	this->m_stLocalWorkGroupSize = stLocalSize;		}
	void						SetMemoryBuffer(const std::string& bufferName, CLMemory* memBuffer)			{	this->m_mapMemBuffers[bufferName] = memBuffer;	}
};


#endif //_CLKERNEL_H_
