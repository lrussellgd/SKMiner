////////////////////////////////////////////////
// File:	Event.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////
#ifndef _EVENT_H_
#define _EVENT_H_

#include "../base/OSVersion.h"
#include <string>
#include <map>


class Event
{

public:

	enum EVENT_TYPE
	{
		NONE = 0,
		ADD_POOL,
		ENABLE_GPU,
		ENABLE_POOL,
		DISABLE_GPU,
		DISABLE_POOL,
		GET_COIN,
		GET_MINING_INFO,
		GET_GPU,
		GET_POOL,
		GET_SUMMARY,
		LOCK_STATS,
		REMOVE_POOL,
		RESTART_GPU,
		RESTART_MINER,
		RETURN_MINING_INFO,
		QUIT,
		SAVE_SETTINGS,
		SET_ADL_GPU_ENGINE_CLOCK,
		SET_ADL_GPU_FAN,
		SET_ADL_GPU_MEMORY_CLOCK,
		SET_ADL_GPU_POWERTUNE,
		SET_ADL_GPU_VOLTAGE,
		SET_DEBUG_MODE,
		SET_GPU_INTENSTY,
		SET_GPU_ENGINE_CLOCK,
		SET_GPU_FAN,
		SET_GPU_MEMORY_CLOCK,
		SET_GPU_POWERTUNE,
		SET_GPU_RAW_INTENSTY,
		SET_GPU_VOLTAGE,
		SET_GPU_X_INTENSTY,
		SET_POOL,
		SET_POOL_PRIORITY,
		SET_ZERO,
		SWITCH_POOL
	};

	struct tParam
	{
		void* pMemory;
		virtual ~tParam() = 0 { }
	};

	template<typename ParamType>
	struct tTemplatedParam : public tParam
	{
		tTemplatedParam(const ParamType& Data)
		{
			pMemory = new ParamType;
			*((ParamType*)pMemory) = Data;
		}
		virtual ~tTemplatedParam()
		{
			delete pMemory;
		}
	};



private:

	EVENT_TYPE m_enmEventType;
	void*						m_pParam[4];
	tParam*						m_pCustomParam[4];
	bool m_bDeallocateCustom;

public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	Event();

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	Event(const EVENT_TYPE& enmEventType, void* pA = NULL, void* pB = NULL, void* pC = NULL, void* pD = NULL, bool bDeallocateCustom = true);

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	Event(const Event& rEvent);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	Event& operator=(const Event& rEvent);

	bool operator==(const Event& rEvent);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	virtual ~Event();

	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const EVENT_TYPE& GetEventType()					const	{ return this->m_enmEventType; }

	template<typename DataType>
	DataType GetDerefParam(unsigned int unParam) const
	{
		return *reinterpret_cast<DataType*>(m_pParam[unParam]);
	}

	template<typename DataType>
	DataType GetParam(unsigned int unParam) const
	{
		return FromVoid<DataType>(m_pParam[unParam]);
	}

	template<typename DataType>
	DataType& GetCustomParam(unsigned int unParam) const
	{
		return *((DataType*)m_pCustomParam[unParam]->pMemory);
	}

	template<typename DataType>
	DataType GetCustomParamPointer(unsigned int unParam) const
	{
		return (DataType*)m_pCustomParam[unParam]->pMemory;
	}

	bool GetDeallocateCustom() const { return m_bDeallocateCustom; }

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void SetEventType(const EVENT_TYPE& enmEventType)			{ this->m_enmEventType = enmEventType; }
	void SetParam(unsigned int unParam, void* data)
	{
		m_pParam[unParam] = data;
	}

	template<typename ParamType>
	void SetCustomParam(unsigned int unParam, const ParamType& Data)
	{
		m_pCustomParam[unParam] = (tParam*) new tTemplatedParam<ParamType>(Data);
	}

	void SetDeallocateCustom(bool bDeallocateCustom)			{ m_bDeallocateCustom = bDeallocateCustom; }


	void ClearCustomParameters();
};

#endif //_EVENT_H_