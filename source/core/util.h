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

#include "../base/OSVersion.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/assign.hpp>
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define loop                for(;;)
#define BEGIN(a)            ((char*)&(a))
#define END(a)              ((char*)&((&(a))[1]))
#define Sleep(a)            boost::this_thread::sleep(boost::posix_time::milliseconds(a));
#define minNum(a,b)         (((a) < (b)) ? (a) : (b))

#endif