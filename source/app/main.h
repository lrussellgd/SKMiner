
#include "../core/util.h"

#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <istream>
#include <ostream>

#include <boost\array.hpp>
#include <boost\thread.hpp>
#include <boost\filesystem.hpp>
#include <boost\filesystem\path.hpp>
#include <boost\interprocess\sync\interprocess_recursive_mutex.hpp>
#include <boost\interprocess\sync\scoped_lock.hpp>
#include <boost\interprocess\sync\interprocess_semaphore.hpp>
#include <boost\interprocess\sync\lock_options.hpp>
#include <boost\date_time\gregorian\gregorian_types.hpp>
#include <boost\date_time\posix_time\posix_time_types.hpp>


#include "../hash/uint1024.h"
#include "../core/bignum.h"
#include "../hash/skein.h"
#include "../hash/KeccakHash.h"
#include "../compute/OpenCLEnvironment.h"

#define loop                for(;;)
#define BEGIN(a)            ((char*)&(a))
#define END(a)              ((char*)&((&(a))[1]))
#define SLEEP(a)            boost::this_thread::sleep(boost::posix_time::milliseconds(a));

void minerThread();
void openclThread();

extern unsigned int nTotalHashes;

#define minNum(a,b)            (((a) < (b)) ? (a) : (b))


//COpenCLEnvironment* pEnvironment;

std::string EncodeBase64(const unsigned char* pch, size_t len)
{
    static const char *pbase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string strRet="";
    strRet.reserve((len+2)/3*4);

    int mode=0, left=0;
    const unsigned char *pchEnd = pch+len;

    while (pch<pchEnd)
    {
        int enc = *(pch++);
        switch (mode)
        {
            case 0: // we have no bits
                strRet += pbase64[enc >> 2];
                left = (enc & 3) << 4;
                mode = 1;
                break;

            case 1: // we have two bits
                strRet += pbase64[left | (enc >> 4)];
                left = (enc & 15) << 2;
                mode = 2;
                break;

            case 2: // we have four bits
                strRet += pbase64[left | (enc >> 6)];
                strRet += pbase64[enc & 63];
                mode = 0;
                break;
        }
    }

    if (mode)
    {
        strRet += pbase64[left];
        strRet += '=';
        if (mode == 1)
            strRet += '=';
    }

    return strRet;
}

std::string EncodeBase64(const std::string& str)
{
    return EncodeBase64((const unsigned char*)str.c_str(), str.size());
}



uint64 GetLocalTimestamp()
{
	return time(NULL);
}

/*template<typename T1>
inline uint512 SK512(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1];
	
    uint512 skein;
	Skein1024_Ctxt_t ctx;
	Skein1024_Init(&ctx, 512);
	Skein1024_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
	Skein1024_Final(&ctx, (unsigned char *)&skein);
	
    uint512 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize_SHA3_512(&ctx_keccak);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 512);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

template<typename T1, typename T2>
inline uint512 SK512(const T1 p1begin, const T1 p1end,
                    const T2 p2begin, const T2 p2end)
{
	static unsigned char pblank[1];
	
    uint512 skein;
	Skein1024_Ctxt_t ctx;
	Skein1024_Init(&ctx, 512);
	Skein1024_Update(&ctx, (p1begin == p1end ? pblank : (unsigned char*)&p1begin[0]), (p1end - p1begin) * sizeof(p1begin[0]));
    Skein1024_Update(&ctx, (p2begin == p2end ? pblank : (unsigned char*)&p2begin[0]), (p2end - p2begin) * sizeof(p2begin[0]));
	Skein1024_Final(&ctx, (unsigned char *)&skein);
	
    uint512 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize_SHA3_512(&ctx_keccak);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 512);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

template<typename T1, typename T2, typename T3>
inline uint512 SK512(const T1 p1begin, const T1 p1end,
                    const T2 p2begin, const T2 p2end,
                    const T3 p3begin, const T3 p3end)
{
	static unsigned char pblank[1];
	
    uint512 skein;
	Skein1024_Ctxt_t ctx;
	Skein1024_Init(&ctx, 512);
	Skein1024_Update(&ctx, (p1begin == p1end ? pblank : (unsigned char*)&p1begin[0]), (p1end - p1begin) * sizeof(p1begin[0]));
    Skein1024_Update(&ctx, (p2begin == p2end ? pblank : (unsigned char*)&p2begin[0]), (p2end - p2begin) * sizeof(p2begin[0]));
    Skein1024_Update(&ctx, (p3begin == p3end ? pblank : (unsigned char*)&p3begin[0]), (p3end - p3begin) * sizeof(p3begin[0]));
	Skein1024_Final(&ctx, (unsigned char *)&skein);
	
    uint512 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize_SHA3_512(&ctx_keccak);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 512);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

template<typename T1>
inline uint1024 SK1024(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1];
	
    uint1024 skein;
	Skein1024_Ctxt_t ctx;
	Skein1024_Init(&ctx, 1024);
	Skein1024_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
	Skein1024_Final(&ctx, (unsigned char *)&skein);
	
	uint1024 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize(&ctx_keccak, 576, 1024, 1024, 0x05);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 1024);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
    return keccak;
}

class CBlock
{	
	public:
		int nVersion;
		unsigned int nNonce;
		unsigned long long ullTime;
		unsigned int nBits;
		uint512 hashMerkleRoot;
		
		std::vector<uint512> vtx; //for simulation purposes
		
		mutable std::vector<uint512> vMerkleTree;
		
		CBlock()
		{
			SetNull();
		}
		
		void SetNull()
		{
			nVersion = 1;
			nNonce = 0;
			ullTime = GetLocalTimestamp();
			nBits = 0;
			
			hashMerkleRoot = 0;
		}
		
		uint512 BuildMerkleTree() const
		{
			vMerkleTree.clear();
			for(int index = 0; index < vtx.size(); index++)
				vMerkleTree.push_back(vtx[index]);
				
			size_t j = 0;
			for (size_t nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
			{
				for (int i = 0; i < nSize; i += 2)
				{
					size_t i2 = minNum(i+1, nSize-1);
					vMerkleTree.push_back(SK512(BEGIN(vMerkleTree[j+i]),  END(vMerkleTree[j+i]),
											   BEGIN(vMerkleTree[j+i2]), END(vMerkleTree[j+i2])));
				}
				j += nSize;
			}
			return (vMerkleTree.empty() ? 0 : vMerkleTree.back());
		}
		
		uint1024 GetHash() const
		{
			return SK1024(BEGIN(nNonce), END(hashMerkleRoot));
		}
};*/

