////////////////////////////////////////////////
// File:	ServerConnection.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _SERVERCONNECTION_H_
#define _SERVERCONNECTION_H_

#include "../core/types.h"
#include "../core/Miner.h"
#include "../threads/MinerThread.h"
#include "../base/Entity.h"

class GPUData;

class ServerConnection : public Entity
{
protected:

	bool m_bIsShutDown;
	bool m_bDidShutDown;
	bool m_bIsEnabled;
	int m_nThreads, m_nTimeout, m_nBestHeight, m_nNewBlocks;
	double m_dCurrentHashses;
	double m_dTotalRolling;
	LLP::Miner* m_pCLIENT;
	std::vector<MinerThread*> m_vecTHREADS;
	LLP::Thread_t m_thTHREAD;
	LLP::Timer    m_tTIMER;
	LLP::Timer    m_totalTIMER;
	std::string   m_szIP, m_szPORT;

	void DecayTime(double fadd, double fsecs, double& fRet);

public:

	ServerConnection();
	ServerConnection(const ServerConnection& serverConnection);
	ServerConnection& operator=(const ServerConnection& serverConnection);
	~ServerConnection();

	void ResetThreads();
	double Hashes();
	int GetTotalAccepted();
	int GetTotalRejected();
	int GetTotalHardwareErrors();

	const bool						GetIsShuttingDown()		const			{	return this->m_bIsShutDown;				}
	const bool						GetDidShutDown()		const			{	return this->m_bDidShutDown;			}
	const bool						GetIsEnabled()			const			{	return this->m_bIsEnabled;				}
	const int						GetThreads()			const			{	return this->m_nThreads;				}
	const int						GetTimeout()			const			{	return this->m_nTimeout;				}
	const int						GetBestHeight()			const			{	return this->m_nBestHeight;				}
	const int						GetNewBlocks()			const			{	return this->m_nNewBlocks;				}	
	const double					GetCurrentHashes()		const			{	return this->m_dCurrentHashses;			}	
	const double					GetTotalRolling()		const			{	return this->m_dTotalRolling;			}		
	LLP::Miner*						GetClient()				const			{	return this->m_pCLIENT;					}
	const std::vector<MinerThread*>	GetMiningThreads()		const			{	return this->m_vecTHREADS;				}
	const LLP::Thread_t&			GetThread()				const			{	return this->m_thTHREAD;				}
	const LLP::Timer&				GetTimer()				const			{	return this->m_tTIMER;					}
	LLP::Timer						GetTotalTimer()			const			{	return this->m_totalTIMER;				}	
	const std::string&				GetIP()					const			{	return this->m_szIP;					}
	const std::string&				GetPort()				const			{	return this->m_szPORT;					}

	void SetIsShuttingDown(const bool bIsShuttingDown)						{	this->m_bIsShutDown = bIsShuttingDown;	}
	void SetIsEnabled(const bool bIsEnabled)								{	this->m_bIsEnabled = bIsEnabled;		}
	void SetThreads(const int nThreads)										{	this->m_nThreads = nThreads;			}
	void SetTimeout(const int nTimeout)										{	this->m_nTimeout = nTimeout;			}
	void SetCurrentHashes(const double dHashes)								{	this->m_dCurrentHashses = dHashes;		}
	void SetClient(LLP::Miner* pClient)										{	this->m_pCLIENT = pClient;				}
	void SetMinerThreads(const std::vector<MinerThread*> vecMinerThreads)	{	this->m_vecTHREADS = vecMinerThreads;	}
	void SetTimer(const LLP::Timer& tTimer)									{	this->m_tTIMER = tTimer;				}
	void SetTotalTimer(const LLP::Timer& totalTimer)						{	this->m_totalTIMER = totalTimer;		}
	void SetIP(const std::string& szIP)										{	this->m_szIP = szIP;					}
	void SetPort(const std::string& szPORT)									{	this->m_szPORT = szPORT;				}

	virtual void ServerThread() = 0;
};

#endif //_SERVERCONNECTION_H_