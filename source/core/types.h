////////////////////////////////////////////////
// File:	types.h
//
// Author:	Videlicet
//
// Copyright:	2014-2015 Videlicet 
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef COINSHIELD_LLP_TYPES_H
#define COINSHIELD_LLP_TYPES_H

#include "util.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

namespace LLP
{
	/** Definitions for LLP Functions **/
	typedef boost::shared_ptr<boost::asio::ip::tcp::socket>		Socket_t;
	typedef boost::asio::ip::tcp::acceptor						Listener_t;
	typedef boost::asio::ip::tcp::endpoint						Endpoint_t;
	typedef boost::asio::io_service                             Service_t;
	typedef boost::thread                                       Thread_t;
	typedef boost::system::error_code                           Error_t;	
	typedef boost::asio::io_service::strand						Strand_t;
}

#endif
	
	