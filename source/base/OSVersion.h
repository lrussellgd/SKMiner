////////////////////////////////////////////////
// File:	OSVersion.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _OSVERSION_H_
#define _OSVERSION_H_


#include <cstdint>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
#include <assert.h>
#include <streambuf>
#include <mutex>
#include <array>
#include <vector>
#include <memory>
#include <thread>

#if defined(_WIN32)

	#include <SDKDDKVer.h>

	#define WIN32_LEAN_AND_MEAN 

	#if !defined(NOMINMAX)
		#define NOMINMAX
	#endif //NOMINMAX
	
	#include <windows.h>
	#include <objbase.h>
	#include <ShlObj.h>

	#include <winhttp.h>
	
	#include <Wincrypt.h>

	#ifdef UNICODE
		#define LOAD_LIBRARY LoadLibraryW
		typedef LPCWSTR _LOADSTR;
	#else
		#define LOAD_LIBRARY  LoadLibraryA
		typedef LPCSTR _LOADSTR;
	#endif

	#define LOAD_LIB(_loadStr,_flags) LOAD_LIBRARY(_LOADSTR(_loadStr))

	typedef int pid_t;
	typedef HINSTANCE INSTANCE_HANDLE;

	typedef int32_t int32;
	typedef uint32_t uint32;
	typedef int64_t	int64;
	typedef uint64_t uint64;

	static const std::string OS_VERSION = std::string("Windows");
	static const std::string APP_DATA_PATH = std::string(getenv("APPDATA")) + std::string("\\SKMiner");


#elif defined(__unix__)

	#include <dlfcn.h>

	#define LOAD_LIBRARY dlopen;

	typedef void* INSTANCE_HANDLE;
	typedef const char* _LOADSTR;

	#define LOAD_LIB(_loadStr,_flags) LOAD_LIBRARY(_loadStr, _flags)

	typedef int int32;
	typedef unsigned int uint32;
	typedef long long  int64;
	typedef unsigned long long  uint64;	

	static const std::string OS_VERSION = std::string("Linux");
	static const std::string APP_DATA_PATH = "~/.skminer";

#endif //_WIN32

	static const uint64 MAX_THREADS = std::numeric_limits<uint64>::max();

	template<typename Type>
	inline void* ToVoid(const Type& Data)
	{
		static long long llValue = 0;
		memcpy(&llValue, &Data, sizeof(Type));
		return (void*)llValue;
	}

	template<typename Type>
	inline Type FromVoid(void* pData)
	{
		static Type Value;
		memcpy(&Value, &pData, sizeof(Type));
		return Value;
	}

	static const std::string VERSION = "1.3";

#endif //_OSVERSION_H_