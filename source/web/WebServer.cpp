////////////////////////////////////////////////
// File:	WebServer.cpp
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

#include "WebServer.h"
#include "APIHandler.h"
#include "HttpConnection.h"

#include <boost/bind.hpp>

namespace Http
{
	namespace Server
	{

		WebServer::WebServer(const std::string& szAddress, const std::string& szPort, const std::string& szDocRoot) : m_bstIOService(), m_bstSignalSet(m_bstIOService)
		{
			m_pAcceptor = new LLP::Listener_t(m_bstIOService);
			m_pConnectionManager = new ConnectionManager();
			m_pSocket = new LLP::Socket_t(new boost::asio::ip::tcp::socket(m_bstIOService));

			APIHandler* pAPIHandler = new APIHandler();
			m_pRequestHandler = new RequestHandler(pAPIHandler);

			m_bstSignalSet.add(SIGINT);
			m_bstSignalSet.add(SIGTERM);

			#if defined(SIGQUIT)
				m_bstSignalSet.add(SIGQUIT);
			#endif // defined(SIGQUIT)

			m_bstSignalSet.async_wait(boost::bind(&WebServer::HandleStop, this));

			boost::asio::ip::tcp::resolver bstResolver(m_bstIOService);
			boost::asio::ip::tcp::resolver::query bstQuery(szAddress, szPort);
			LLP::Endpoint_t bstEndPoint = *bstResolver.resolve(bstQuery);

			m_pAcceptor->open(bstEndPoint.protocol());
			m_pAcceptor->set_option(LLP::Listener_t::reuse_address(true));
			m_pAcceptor->bind(bstEndPoint);
			m_pAcceptor->listen();

			m_pEvents = Events::EventManager::GetInstance();

			StartAccept();
		}

		WebServer::~WebServer()
		{
			m_thTHREAD.detach();

			if (m_pAcceptor)
			{
				delete(m_pAcceptor);
				m_pAcceptor = NULL;
			}

			if (m_pRequestHandler)
			{
				delete(m_pRequestHandler);
				m_pRequestHandler = NULL;
			}
		}

		void WebServer::Run()
		{
			m_thTHREAD = boost::thread(&WebServer::StartServer, this);
		}

		void WebServer::StartServer()
		{
			m_bstIOService.run();
		}

		void WebServer::StartAccept()
		{
			m_pAcceptor->async_accept(*m_pSocket->get(),
				boost::bind(&WebServer::HandleAccept, this,
				boost::asio::placeholders::error));
		}

		void WebServer::HandleAccept(const boost::system::error_code& e)
		{
			if (!m_pAcceptor->is_open())
			{
				return;
			}

			if (!e)
			{
				m_pConnectionManager->Start(connection_ptr(new HttpConnection(std::move(m_pSocket), m_pConnectionManager, m_pRequestHandler)));
			}

			StartAccept();
		}

		void WebServer::HandleStop()
		{
			m_pAcceptor->close();
			m_pConnectionManager->StopAll();
		}
	}
}