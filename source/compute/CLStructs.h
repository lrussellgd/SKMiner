////////////////////////////////////////////////
// File:	CLStructs.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _CLSTRUCTS_H_
#define _CLSTRUCTS_H_

#define BEGIN(a)            ((char*)&(a))
#define END(a)              ((char*)&((&(a))[1]))

typedef struct Skein1024
{
	unsigned long long  skein[16];
	unsigned long long  block[16];
	unsigned long long  T[2];
	
	size_t blkCnt;
	size_t byteCnt;
	
} CL_SK1024;


typedef struct Keccak1024
{
	unsigned long long keccak[25];

} KC1024;




typedef struct SkeinData
{
	unsigned long long skein[16];
	size_t byteCnt;

} SKData;

typedef struct HashData
{
	unsigned long long hash[40];

} HshDta;


typedef struct XData
{
	unsigned long long X[16];

} XDta;


typedef struct IndiceData
{
	size_t index;

} IndDta;

typedef struct SkeinMessage
{
	unsigned char message[128];
	size_t msgOffset;
	size_t msgByteCnt;

} SKMessage;

typedef struct SkeinHeader
{
	size_t  hashBitLen;                    
	size_t  bCnt;                 
	unsigned long long T[2];
} SkeinHdr;

typedef struct Skein1024Ctx                    
{
	SkeinHdr h;                      
	unsigned long long  X[16];
	unsigned char b[128];

} SkeinCtx;

typedef struct CtxTData
{
	unsigned long long T[2];

} CtxT;

typedef struct KeccakChr
{
	unsigned char keccak[200];
	
} KcChr;

typedef struct SkeinChr
{
	unsigned char skein[128];

} SkChr;

typedef struct SkeinKW
{
	unsigned long long kw[40];

} kwData;

typedef struct OCLIndices
{
	int indices[16];
	int round;
	int loopNum;
	int inum;
	
} OCLInd;


typedef struct Indices
{
	int round;
	int subRound;

} RndInd;

#endif //_CLSTRUCTS_H_
