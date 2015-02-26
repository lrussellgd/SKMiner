////////////////////////////////////////////////
// File:	RequestHandler.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
//			Based off of the boost http server example
//			http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/examples/cpp11_examples.html
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#ifndef _REQUESTHANDLER_H_
#define _REQUESTHANDLER_H_

#include "../base/Entity.h"

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>

#include "APIHandler.h"
#include "APIRequest.h"

namespace Http
{
	namespace Server
	{
		class Request;
		class Reply;

		class RequestHandler : private boost::noncopyable
		{

		private:

			/// The directory containing the files to be served.
			std::string m_szDocRoot;

			APIHandler* m_pAPIHandler;
			APIRequest* m_pAPIRequest;

			/// Perform URL-decoding on a string. Returns false if the encoding was
			/// invalid.
			static bool UrlDecode(const std::string& in, std::string& out);

		public:

			RequestHandler(const RequestHandler&) = delete;
			RequestHandler& operator=(const RequestHandler&) = delete;

			//////////////////////////////////////////////////////////////////////////////
			//Constructor
			///////////////////////////////////////////////////////////////////////////////
			explicit RequestHandler(APIHandler* pAPIHandler);

			void HandleRequest(const Request* pRequest, Reply* pReply);

			///////////////////////////////////////////////////////////////////////////////
			//Destructor
			///////////////////////////////////////////////////////////////////////////////
			~RequestHandler();
		};
	}
}


#endif //_REQUESTHANDLER_H_