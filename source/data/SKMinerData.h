////////////////////////////////////////////////
// File:	SKMinerData.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _SKMINERDATA_H
#define _SKMINERDATA_H

#include "MinerData.h"
#include "../hash/uint1024.h"

class CBigNum;

class SKMinerData : public MinerData
{
private:

	CBigNum* m_pTarget;


public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	SKMinerData();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	SKMinerData(const SKMinerData& minerData);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	SKMinerData& operator=(const SKMinerData& minerData);
	
	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~SKMinerData();

	SKMinerData* Clone();
	SKMinerData* DeepCopy();

	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	CBigNum*	GetTarget()	const		{ return this->m_pTarget; }

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void	SetTarget(CBigNum* pTarget)	{ this->m_pTarget = pTarget; }
};

#endif //_SKMINERDATA_H