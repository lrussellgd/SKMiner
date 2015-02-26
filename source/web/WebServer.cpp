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
			m_thTHREAD = NULL;
			m_pSocket = new LLP::Socket_t(new boost::asio::ip::tcp::socket(m_bstIOService));

			m_pAPIHandler = new APIHandler();

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

			m_connLock = new boost::mutex();
			m_bShutdown = false;
			m_pServerThread = new LLP::Thread_t(boost::thread(&WebServer::RunServer, this));
			m_pConnectionThread = new LLP::Thread_t(boost::thread(&WebServer::HandleConnections, this));
		}

		WebServer::~WebServer()
		{
			std::cout << "Web Server Closing!" << std::endl;

			if (m_thTHREAD)
			{
				m_thTHREAD->detach();
				delete(m_thTHREAD);
				m_thTHREAD = NULL;
			}

			if (m_pServerThread)
			{
				m_pServerThread->detach();
				delete(m_pServerThread);
				m_pServerThread = NULL;
			}

			if (m_pAcceptor)
			{
				delete(m_pAcceptor);
				m_pAcceptor = NULL;
			}

			if (m_pAPIHandler)
			{
				m_pAPIHandler->Shutdown();

				while (!m_pAPIHandler->GetDidShutdown()){};

				delete(m_pAPIHandler);
				m_pAPIHandler = NULL;
			}

			if (m_pConnectionThread)
			{
				m_pConnectionThread->detach();
				delete(m_pConnectionThread);
				m_pConnectionThread = NULL;
			}

			if (m_connLock)
			{
				m_connLock->unlock();
				delete(m_connLock);
				m_connLock = NULL;
			}

			if (m_pSocket)
			{
				delete(m_pSocket);
			}

			ClearConnections();
		}

		void WebServer::RunServer()
		{
			StartAccept();
		}

		void WebServer::Run()
		{
			if (m_thTHREAD)
			{
				m_thTHREAD->detach();
				delete(m_thTHREAD);
				m_thTHREAD = NULL;
			}

			m_thTHREAD = new boost::thread(&WebServer::StartServer, this);
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
				m_connLock->lock();
				{
					HttpConnection* pConn = new HttpConnection(std::move(m_pSocket), m_pAPIHandler);
					m_vecConnections.push_back(pConn);
					pConn->Start();
				}
				m_connLock->unlock();
			}

			StartAccept();
		}

		void WebServer::HandleStop()
		{
			m_pAcceptor->close();
			m_bShutdown = true;
			ClearConnections();
		}

		void WebServer::HandleConnections()
		{
			while (!m_bShutdown)
			{
				Sleep(10);
				m_connLock->lock();
				{
					std::vector<HttpConnection*> vecKeep;
					for (size_t index = 0; index < m_vecConnections.size(); ++index)
					{
						if (!m_vecConnections[index]->GetIsStopped())
						{
							vecKeep.push_back(m_vecConnections[index]);
						}
						else
						{
							delete(m_vecConnections[index]);
							m_vecConnections[index] = NULL;
						}
					}

					m_vecConnections.clear();

					for (size_t keepIndex = 0; keepIndex < vecKeep.size(); ++keepIndex)
					{
						m_vecConnections.push_back(vecKeep[keepIndex]);
					}

					vecKeep.clear();
				}
				m_connLock->unlock();

			}
		}

		void WebServer::ClearConnections()
		{
			m_connLock->lock();
			{
				for (size_t index = 0; index < m_vecConnections.size(); ++index)
				{
					delete(m_vecConnections[index]);
					m_vecConnections[index] = NULL;
				}

				m_vecConnections.clear();
			}
			m_connLock->unlock();
		}
	}
}