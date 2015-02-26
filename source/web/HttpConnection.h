////////////////////////////////////////////////
// File:	HttpConnection.h
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

#ifndef _HTTPCONNECTION_H_
#define _HTTPCONNECTION_H_

#include "../base/Entity.h"

#include "../core/types.h"
#include "../core/Timer.h"

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

class APIHandler;

namespace Http
{
	namespace Server
	{
		class ConnectionManager;
		class RequestHandler;
		class RequestParser;
		class JsonRequestParser;
		class Reply;
		class Request;

		class HttpConnection //: public std::enable_shared_from_this<HttpConnection>
		{

		private:

			void StartRead();

			/// Perform read operation.
			void DoRead();

			/// Socket for the connection.
			LLP::Socket_t* m_pSocket;

			APIHandler* m_pAPIHandler;

			/// The reply to be sent back to the client.
			boost::mutex* m_clLock;
			boost::mutex* m_requestLock;
			boost::mutex* m_replyLock;
			boost::mutex* m_dataLock;

			boost::mutex* m_readLock;
			LLP::Thread_t* m_pReadThread;

			bool m_bStopped;

			static const int32 TIME_OUT = 5000;
			LLP::Timer m_tDataTimer;
			int32 m_n32LastTime;

			Reply* m_pReply;
			Request* m_pRequest;
			RequestParser* m_pRequestParser;
			JsonRequestParser* m_pJsonParser;
			RequestHandler* m_pRequestHandler;

			void ClearData();

		public:

			HttpConnection(const HttpConnection&) = delete;
			HttpConnection& operator=(const HttpConnection&) = delete;

			/// Construct a connection with the given socket.
			HttpConnection(LLP::Socket_t* pSocket, APIHandler* handler);

			~HttpConnection();

			/// Start the first asynchronous operation for the connection.
			void Start();

			/// Stop all asynchronous operations associated with the connection.
			void Stop();

			const bool GetIsStopped()  const { return this->m_bStopped; }
		};

		//typedef std::shared_ptr<HttpConnection> connection_ptr;
	}
}


#endif //_HTTPCONNECTION_H_