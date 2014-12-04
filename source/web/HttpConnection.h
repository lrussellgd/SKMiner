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

#include "../core/types.h"

#include <array>
#include <memory>
#include <boost/asio.hpp>

namespace Http
{
	namespace Server
	{
		class ConnectionManager;
		class RequestHandler;
		class RequestParser;
		class Reply;
		class Request;

		class HttpConnection : public std::enable_shared_from_this<HttpConnection>
		{

		private:

			/// Perform an asynchronous read operation.
			void DoRead();

			/// Perform an asynchronous write operation.
			void DoWrite();

			/// Socket for the connection.
			LLP::Socket_t* m_pSocket;

			/// The manager for this connection.
			ConnectionManager* m_pConnectionManager;

			/// The handler used to process the incoming request.
			RequestHandler* m_pRequestHandler;

			/// Buffer for incoming data.
			std::array<char, 8192> m_arrBuffer;

			/// The incoming request.
			Request* m_pRequest;

			/// The parser for the incoming request.
			RequestParser* m_pRequestParser;

			/// The reply to be sent back to the client.
			Reply* m_pReply;


		public:

			HttpConnection(const HttpConnection&) = delete;
			HttpConnection& operator=(const HttpConnection&) = delete;

			/// Construct a connection with the given socket.
			HttpConnection(LLP::Socket_t* pSocket,
				ConnectionManager* manager, RequestHandler* handler);

			~HttpConnection();

			/// Start the first asynchronous operation for the connection.
			void Start();

			/// Stop all asynchronous operations associated with the connection.
			void Stop();
		};

		typedef std::shared_ptr<HttpConnection> connection_ptr;
	}
}


#endif //_HTTPCONNECTION_H_