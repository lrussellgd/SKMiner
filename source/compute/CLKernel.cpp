////////////////////////////////////////////////
// File:	CLKernel.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "CLKernel.h"

#include "CLMemory.h"

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
CLKernel::CLKernel()
{
	this->m_unWorkSize = this->m_unPrefWorkSize = 0;
	this->m_stGlobalWorkGroupSize = this->m_stLocalWorkGroupSize = 0;
	this->m_pReqdWorkGroupSizes[0] = 0;
	this->m_pReqdWorkGroupSizes[1] = 0;
	this->m_pReqdWorkGroupSizes[2] = 0;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
CLKernel::CLKernel(const CLKernel& cKernel)
{
	this->m_szName = cKernel.GetName();

	this->m_unWorkSize = cKernel.GetWorksize();;
	this->m_unPrefWorkSize = cKernel.GetPrefWorkSize();

	this->m_pReqdWorkGroupSizes[0] = cKernel.GetReqdWorkGroupSizes()[0];
	this->m_pReqdWorkGroupSizes[1] = cKernel.GetReqdWorkGroupSizes()[1];
	this->m_pReqdWorkGroupSizes[2] = cKernel.GetReqdWorkGroupSizes()[2];

	this->m_clKernel = cKernel.GetKernel();

	this->m_stGlobalWorkGroupSize = cKernel.GetGlobalWorkGroupSize();
	this->m_stLocalWorkGroupSize = cKernel.GetLocalWorkGroupSize();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
CLKernel& CLKernel::operator=(const CLKernel& cKernel)
{
	this->m_szName = cKernel.GetName();

	this->m_unWorkSize = cKernel.GetWorksize();;
	this->m_unPrefWorkSize = cKernel.GetPrefWorkSize();

	this->m_pReqdWorkGroupSizes[0] = cKernel.GetReqdWorkGroupSizes()[0];
	this->m_pReqdWorkGroupSizes[1] = cKernel.GetReqdWorkGroupSizes()[1];
	this->m_pReqdWorkGroupSizes[2] = cKernel.GetReqdWorkGroupSizes()[2];

	this->m_clKernel = cKernel.GetKernel();

	this->m_stGlobalWorkGroupSize = cKernel.GetGlobalWorkGroupSize();
	this->m_stLocalWorkGroupSize = cKernel.GetLocalWorkGroupSize();

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
CLKernel::~CLKernel()
{
	for (auto iter = this->m_mapMemBuffers.begin(); iter != this->m_mapMemBuffers.end(); ++iter)
	{
		CLMemory* memBuffer = iter->second;
		if (memBuffer)
		{
			delete(memBuffer);
			memBuffer = NULL;
		}
	}
	this->m_mapMemBuffers.clear();

	clReleaseKernel(this->m_clKernel());
}

CLKernel* CLKernel::Clone()
{
	CLKernel* pCLKernel = new CLKernel();

	pCLKernel->SetName(this->m_szName);
	pCLKernel->SetWorksize(this->m_unWorkSize);
	pCLKernel->SetPrefWorkSize(this->m_unPrefWorkSize);
	pCLKernel->SetReqdWorkSizes(this->m_pReqdWorkGroupSizes);
	pCLKernel->SetKernel(this->m_clKernel);
	pCLKernel->SetGlobalWorkGroupSize(this->m_stGlobalWorkGroupSize);
	pCLKernel->SetLocalWorkGroupSize(this->m_stLocalWorkGroupSize);

	return pCLKernel;
}

CLKernel* CLKernel::DeepCopy()
{
	CLKernel* pCLKernel = new CLKernel();

	pCLKernel->SetName(this->m_szName);
	pCLKernel->SetWorksize(this->m_unWorkSize);
	pCLKernel->SetPrefWorkSize(this->m_unPrefWorkSize);
	pCLKernel->SetReqdWorkSizes(this->m_pReqdWorkGroupSizes);
	pCLKernel->SetKernel(this->m_clKernel);
	pCLKernel->SetGlobalWorkGroupSize(this->m_stGlobalWorkGroupSize);
	pCLKernel->SetLocalWorkGroupSize(this->m_stLocalWorkGroupSize);

	return pCLKernel;
}