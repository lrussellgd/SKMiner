////////////////////////////////////////////////
// File:	GPUSetting.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _GPUSETTING_H_
#define _GPUSETTING_H_

#include <string>
#include "../base/IPrototype.h"

class GPUSetting : public IPrototype
{

private:

	bool	m_bIsAutoEngine;
	bool	m_bIsAutoFan;
	bool	m_bHasFanSpeed;
	bool	m_bIsEnabled;

	int		m_nEngineClock;
	int		m_nMinEngineSpeed;
	int		m_nMaxEngineSpeed;
	int		m_nMemclock;
	int		m_nPowerTune;
	int		m_nThreads;
	int		m_nShaders;
	int		m_nTargetFan;
	int		m_nTargetTemp;
	int		m_nOverheatTemp;
	int		m_nCutOffTemp;	
	int		m_nIntensity;
	int		m_nRawintensity;
	int		m_nXIntensity;
	int		m_nThreadConcurreny;
	int		m_nLookupGap;
	int		m_nWorkSize;
	int		m_nVectors;

	float	m_fVDDC;

public:

	///////////////////////////////////////////////////////////////////////////////
	//Constructor
	///////////////////////////////////////////////////////////////////////////////
	GPUSetting();

	///////////////////////////////////////////////////////////////////////////////
	//Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	GPUSetting(const GPUSetting& setting);

	///////////////////////////////////////////////////////////////////////////////
	//Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	GPUSetting& operator=(const GPUSetting& setting);

	///////////////////////////////////////////////////////////////////////////////
	//Destructor
	///////////////////////////////////////////////////////////////////////////////
	~GPUSetting();

	GPUSetting* Clone();
	GPUSetting* DeepCopy();

	
	///////////////////////////////////////////////////////////////////////////////
	//Accessors
	///////////////////////////////////////////////////////////////////////////////
	const bool				GetIsAutoEngine()		const	{	return this->m_bIsAutoEngine;				}
	const bool				GetIsAutoFan()			const	{	return this->m_bIsAutoFan;					}
	const bool				GetHasFanSpeed()		const	{	return this->m_bHasFanSpeed;				}
	const bool				GetIsEnabled()			const	{	return this->m_bIsEnabled;					}
	const int				GetEngineClock()		const	{	return this->m_nEngineClock;				}
	const int				GetMinEngineSpeed()		const	{	return this->m_nMinEngineSpeed;				}
	const int				GetMaxEngineSpeed()		const	{	return this->m_nMaxEngineSpeed;				}
	const int				GetMemclock()			const	{	return this->m_nMemclock;					}
	const int				GetPowerTune()			const	{	return this->m_nPowerTune;					}
	const int				GetThreads()			const	{	return this->m_nThreads;					}
	const int				GetShaders()			const	{	return this->m_nShaders;					}
	const int				GetTargetFan()			const	{	return this->m_nTargetFan;					}	
	const int				GetTargetTemp()			const	{	return this->m_nTargetTemp;					}
	const int				GetOverHeatTemp()		const	{	return this->m_nOverheatTemp;				}		
	const int				GetCutOffTemp()			const	{	return this->m_nCutOffTemp;					}
	const int				GetIntensity()			const	{	return this->m_nIntensity;					}
	const int				GetRawIntensity()		const	{	return this->m_nRawintensity;				}
	const int				GetXIntensity()			const	{	return this->m_nXIntensity;					}
	const int				GetThreadConcurrency()	const	{	return this->m_nThreadConcurreny;			}
	const int				GetLookupGap()			const	{	return this->m_nLookupGap;					}
	const int				GetWorkSize()			const	{	return this->m_nWorkSize;					}
	const int				GetVectors()			const	{	return this->m_nVectors;					}
	const float				GetVoltage()			const	{	return this->m_fVDDC;						}

	///////////////////////////////////////////////////////////////////////////////
	//Mutators
	///////////////////////////////////////////////////////////////////////////////
	void	SetIsAutoEngine(bool bIsAutoEngine)			{	this->m_bIsAutoEngine = bIsAutoEngine;		}
	void	SetIsAutoFan(bool bIsAutoFan)				{	this->m_bIsAutoFan = bIsAutoFan;			}
	void	SetHasFanSpeed(bool bHasFanSpeed)			{	this->m_bHasFanSpeed = bHasFanSpeed;		}
	void	SetIsEnabled(bool bIsEnabled)				{	this->m_bIsEnabled = bIsEnabled;			}
	void	SetEngineClock(int nEngineClock)			{	this->m_nEngineClock = nEngineClock;		}
	void	SetMinEngineSpeed(int nMinEngineSpeed)		{	this->m_nMinEngineSpeed = nMinEngineSpeed;	}
	void	SetMaxEngineSpeed(int nMaxEngineSpeed)		{	this->m_nMaxEngineSpeed = nMaxEngineSpeed;	}
	void	SetMemclock(int nMemclock)					{	this->m_nMemclock = nMemclock;				}
	void	SetPowerTune(int nPowerTune)				{	this->m_nPowerTune = nPowerTune;			}
	void	SetThreads(int nThreads)					{	this->m_nThreads = nThreads;				}	
	void	SetShaders(int nShaders)					{	this->m_nShaders = nShaders;				}
	void	SetTargetFan(int nTargetFan)				{	this->m_nTargetFan = nTargetFan;			}
	void	SetTargetTemp(int nTargetTemp)				{	this->m_nTargetTemp = nTargetTemp;			}
	void	SetOverHeatTemp(int nOverHeatTemp)			{	this->m_nOverheatTemp = nOverHeatTemp;		}
	void	SetCutOffTemp(int nCutOffTemp)				{	this->m_nCutOffTemp = nCutOffTemp;			}
	void	SetIntensity(int nIntensity)				{	this->m_nIntensity = nIntensity;			}
	void	SetRawIntensity(int nRawIntensity)			{	this->m_nRawintensity = nRawIntensity;		}
	void	SetXIntensity(int nXIntensity)				{	this->m_nXIntensity = nXIntensity;			}
	void	SetThreadConcurrency(int nTC)				{	this->m_nThreadConcurreny = nTC;			}
	void	SetLookupGap(int nLookupGap)				{	this->m_nLookupGap = nLookupGap;			}
	void	SetWorkSize(int nWorkSize)					{	this->m_nWorkSize = nWorkSize;				}
	void	SetVectors(int nVectors)					{	this->m_nVectors = nVectors;				}
	void	SetVoltage(float fVoltage)					{	this->m_fVDDC = fVoltage;					}
};


#endif //_GPUSETTING_H_