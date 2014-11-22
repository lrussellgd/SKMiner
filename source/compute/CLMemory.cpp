////////////////////////////////////////////////
// File:	CLMemory.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "CLMemory.h"

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
CLMemory::CLMemory()
{
	this->m_pEmptyBuffer = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
CLMemory::CLMemory(const CLMemory& memory)
{
	this->m_szBufferName = memory.GetBufferName();
	this->m_stBufferSize = memory.GetBufferSize();
	this->m_clMemBuffer = memory.GetBuffer();
	this->m_clmfType = memory.GetBufferType();
	this->m_pEmptyBuffer = memory.GetEmptyBuffer();
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
CLMemory& CLMemory::operator = (const CLMemory& memory)
{
	this->m_szBufferName = memory.GetBufferName();
	this->m_stBufferSize = memory.GetBufferSize();
	this->m_clMemBuffer = memory.GetBuffer();
	this->m_clmfType = memory.GetBufferType();
	this->m_pEmptyBuffer = memory.GetEmptyBuffer();

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
CLMemory::~CLMemory()
{
	clReleaseMemObject(this->m_clMemBuffer);

	if (m_pEmptyBuffer)
	{
		free(m_pEmptyBuffer);
		m_pEmptyBuffer = NULL;
	}
}

CLMemory* CLMemory::Clone()
{
	CLMemory* pMemory = new CLMemory();

	pMemory->SetBuffer(this->m_clMemBuffer);
	pMemory->SetBufferType(this->m_clmfType);
	pMemory->SetBufferName(this->m_szBufferName);
	pMemory->SetBufferSize(this->m_stBufferSize);
	pMemory->SetEmptyBuffer(this->m_pEmptyBuffer);

	return pMemory;
}

CLMemory* CLMemory::DeepCopy()
{
	CLMemory* pMemory = new CLMemory();

	pMemory->SetBuffer(this->m_clMemBuffer);
	pMemory->SetBufferType(this->m_clmfType);
	pMemory->SetBufferName(this->m_szBufferName);
	pMemory->SetBufferSize(this->m_stBufferSize);

	void* pEmpty = calloc(this->m_stBufferSize, 1);
	memcpy(pEmpty, this->m_pEmptyBuffer, this->m_stBufferSize);
	pMemory->SetEmptyBuffer(pEmpty);

	return pMemory;
}
