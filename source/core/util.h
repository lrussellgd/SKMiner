////////////////////////////////////////////////
// File:	util.h
//
// Author:	Videlicet
//			Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Videlicet 
//				2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#ifndef COINSHIELD_UTIL_H
#define COINSHIELD_UTIL_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include "../hash/uint1024.h"

typedef int pid_t; /* define for windows compatiblity */


#include <string>
#include <vector>
#include <stdio.h>
#include <istream>
#include <ostream>
#include <cstdio>

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#define loop                for(;;)
#define BEGIN(a)            ((char*)&(a))
#define END(a)              ((char*)&((&(a))[1]))
#define Sleep(a)            boost::this_thread::sleep(boost::posix_time::milliseconds(a));
#define minNum(a,b)            (((a) < (b)) ? (a) : (b))

#ifdef WIN32      
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX
#endif

#define MAX_THREADS 18446744073709551615

#endif