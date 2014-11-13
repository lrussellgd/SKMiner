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

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
CLKernel::CLKernel()
{
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