////////////////////////////////////////////////
// File:	WebServer.h
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

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "../core/types.h"

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "RequestHandler.h"
#include "../core/Event.h"
#include "../core/EventManager.h"
#include "ConnectionManager.h"

class APIHandler;

namespace Http
{
	namespace Server
	{
		class WebServer : private boost::noncopyable
		{

		private:

			void StartAccept();
			void HandleAccept(const boost::system::error_code& e);
			void HandleStop();

			LLP::Thread_t m_thTHREAD;

			Events::EventManager* m_pEvents;
			LLP::Service_t m_bstIOService;
			boost::asio::signal_set m_bstSignalSet;
			LLP::Listener_t* m_pAcceptor;
			LLP::Socket_t* m_pSocket;
			//std::vector<Session_t*> m_vecSessions;

			/// The connection manager which owns all live connections.
			ConnectionManager* m_pConnectionManager;

			//Session_t m_pNewSession;
			RequestHandler* m_pRequestHandler;

			std::vector<APIHandler*> m_vecAPIHandlers;

			/// Perform an asynchronous accept operation.
			void DoAccept();

			/// Wait for a request to stop the server.
			void DoAwaitStop();

			void StartServer();

		public:

			WebServer(const WebServer&) = delete;
			WebServer& operator=(const WebServer&) = delete;

			explicit WebServer(const std::string& szAddress, const std::string& szPort, const std::string& szDocRoot);

			~WebServer();

			void Run();
		};
	}
}

#endif //_WEBSERVER_H_