////////////////////////////////////////////////
// File:	IPrototype.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _IPROTOTYPE_H_
#define _IPROTOTYPE_H_

class IPrototype
{
private:
		
	IPrototype(const IPrototype& proto) {}
	IPrototype& operator=(const IPrototype& proto) { return *this; }	

public:	

	IPrototype() {};
	~IPrototype() {};

	virtual IPrototype* Clone() = 0;
	virtual IPrototype* DeepCopy() = 0;

};


#endif
